
#include <ledger.h>
#include <vector>
using namespace std;

pthread_mutex_t buffer_lock;//Lock for critical sections accessing ledger buffer

pthread_cond_t buffer_not_full = PTHREAD_COND_INITIALIZER;//signal for worker->reader when there is space in buffer 
pthread_cond_t buffer_not_empty = PTHREAD_COND_INITIALIZER;//signal for reader->worker when there are ledgers to process
int buffer_size = 0;//global variable for various base cases in reader/worker threads. Tracks current buffer size and is set as a flag when ledger file exhausted

int ledger_id = 0;

vector<struct Ledger> buffer;//Ledger buffer that can be bounded 

Bank *bank;

/**
 * @brief creates a new bank object and sets up workers
 * 
 * Requirements:
 *  - Create a new Bank object class with 10 accounts.
 *  - Load the ledger into a list
 *  - Set up the worker threads.  
 *  
 * @param num_workers 
 * @param filename 
 */
void InitBank(int num_workers, int num_readers, char *filename) {
  pthread_mutex_init(&buffer_lock, NULL);
  
  //Bound the ledger buffer to only hold 16 Ledger's at a time
  buffer.reserve(16);

  //Create new Bank Object with 10 accounts  
  bank = new Bank(10);
  bank->print_account();

  ifstream infile(filename);//Input file stream of ledger file

  //Run the threads that work to read a ledger file, write to buffer
  pthread_t load_array[num_readers];
  for(int i = 0; i < num_readers; ++i){
    pthread_create(&load_array[i], NULL, load_ledger, static_cast<void *>(&infile));
  }
 
  pthread_t thread_array[num_workers];
  int worker_ids[num_workers];
  int *worker_id_pointer = worker_ids;
  
  //Run the threads that work to read from buffer, and execute ledger
  for(int i = 0; i < num_workers; ++i){
    worker_ids[i] = i;
    pthread_create(&thread_array[i], NULL, worker, worker_id_pointer);
    worker_id_pointer++;
  }
  
  //joing all worker's before printing out final output
  for(int i = 0; i < num_workers; ++i){    
      pthread_join(thread_array[i], NULL);
      }

  
  bank->print_account();

  //clean lock and bank objects
  pthread_mutex_destroy(&buffer_lock);
  delete bank;
}

/**
 * @brief Parse a ledger file and store each line into a list
 * 
 * @param file stream pointer
 * NOTICE: I changed this function to support multi-threading, so it takes a void* file_pointer so that multiple
 * threads can modify a single file stream (Ledger file stream)
 */
void* load_ledger(void *infile_pointer){
  //cast the paramter to ifstream pointer
  ifstream *infile = static_cast<ifstream*>(infile_pointer);
  int f, t, a, m;

  /**
   *In this while loop, I have buffer locks to prevent race conditions, and base case conditionals
   *in order to break out of the while loop and send the proper signals to worker functions.  
   */
  while(true){
        
    pthread_mutex_lock(&buffer_lock);

    //If the buffer is full and not empty, wait for the buffer to not be full
    if(buffer_size >= 16 && buffer_size != -1){
      while(buffer_size >= 16 && buffer_size != -1){
	pthread_cond_wait(&buffer_not_full, &buffer_lock);}
    }
    
    //If ledger file has no more ledgers (notice I set buffer_size to -1 when this is the case) then unlock the buffer lock and exit
    if(buffer_size == -1) {
      pthread_mutex_unlock(&buffer_lock);
      break;
    }

    //if the buffer has no more ledgers, set buffer_size to -1 to act as a flag and signal to worker to finish working
    if(!((*infile) >> f >> t >> a >> m)){
      buffer_size = -1;
      pthread_cond_signal(&buffer_not_empty);
      pthread_mutex_unlock(&buffer_lock);
      break;}

    //create new ledger and set fields 
    struct Ledger l;
    l.from = f;
    l.to = t;
    l.amount = a;
    l.mode = m;
    l.ledgerID = ledger_id;
    ledger_id++; 

    //add ledger to vector
    buffer.push_back(l);

    //increment global buffer size variable
    buffer_size++;
   
    pthread_cond_signal(&buffer_not_empty);//signal to worker thread that a ledger has been placed into the buffer
    pthread_mutex_unlock(&buffer_lock);
    }
}

/**
 * @brief Remove items from the list and execute the instruction.
 * 
 * @param workerID 
 * @return void* 
 */
void* worker(void *workerID){
  
    
    int id = *(int*) workerID;
    while(true){
      pthread_mutex_lock(&buffer_lock);
      
      //if the buffer is empty, wait for a ledger to be placed
      while(buffer_size == 0){
        pthread_cond_wait(&buffer_not_empty, &buffer_lock);
      }
      
      //if the buffer is empty and all ledger's have been placed, then this thread has nothing to do and exit
      if(buffer_size == -1 && buffer.size() == 0){
	pthread_mutex_unlock(&buffer_lock);
	break;
      }
          
       //read from buffer
      struct Ledger l = buffer[0];
      buffer.erase(buffer.begin());
      if(buffer_size != -1){buffer_size--;}
     
      //signal that the buffer is not full
      pthread_cond_signal(&buffer_not_full);
      pthread_mutex_unlock(&buffer_lock);

          
      //process ledgers:       
      if(l.mode == D){bank->deposit(id, l.ledgerID, l.from, l.amount);}
      else if(l.mode == T){bank->transfer(id, l.ledgerID, l.from, l.to, l.amount);}
      else if(l.mode == W){bank->withdraw(id, l.ledgerID, l.from, l.amount);}
      else if(l.mode == C){bank->CheckBalance(id, l.from);}
      else{cout << "Invalid Ledger #" << l.ledgerID << endl;}
    }
 
}


