This final project is an extension of Project 4: Producer and Consumer

I implement the suggestions on the course website, and also added an extra command line argument to the application that takes in the number of reader threads.

The first suggestion implemented is: “Add a function CheckBalance() . Checking the balance should take priority over Deposite() (Hint: think about read/write locks)"

I implemented this by replacing the account locks that were mutexes to read/write locks. CheckBalance() takes the worker ID and account ID and prints to output that the worker workerID found the account accountID
to have the current balance: accounts[accountID].balance

This function uses a read lock to protect its critical section, allowing multiple threads to access the same lock in read mode simultaneously. The Deposit, Transfer, and Withdraw function protect their critical sections.

I added a new ledger mode C (3 in ledger file) indicating to pass the ledger to CheckBalance() in the worker thread. 

The second and third suggestion I combined as they are very similar with the difference of a bounded buffer: “Instead of the main thread reading all the lines in a file, have multiple reader threads.”
And, “Instead of a infinite Ledger list, create a bounded buffer where reader threads place Ledger objects and worker threads remove Ledger objects.

I first replaced the ledger list with a ledger vector so that it can be bounded. I set the bound to 16 maximum ledgers in the buffer.

I changed the load_ledger() function such that it takes a void* input file stream pointer so that multiple reader threads can read from the same file stream.

There are two conditionals: buffer_not_full and buffer_not_empty
-buffer_not_full is signalled by the worker thread to the reader thread to indicate it can place more ledgers.
-buffer_not_empty is signalled by the reader thread to the worker thread to indicate that it can process more ledgers.

Both the worker threads and reader threads use the same mutex that is buffer_lock to protect the critical sections accessing and modifying the buffer.

Run the application by ./bank_app <num_worker_threads> <num_reader_threads> <ledger_file>

There are 2 ledger files, ledger.txt and pressure_test.txt that can be used.

A further in depth explanation can be found in this youtube video containing a slideshow and demo’s of each implemtation: https://www.youtube.com/watch?v=_tlrxruIMyg&ab_channel=JaredVecchio

