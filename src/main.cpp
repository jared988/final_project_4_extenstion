#include <ledger.h>

int main(int argc, char* argv[]) {
  if (argc != 4) {
    cerr << "Usage: " << argv[0] << "<num_of_writer_threads> <num_of_reader_threads> <leader_file>\n" << endl;
    exit(-1);
  }

  int num_workers = atoi(argv[1]);
  int num_readers = atoi(argv[2]);
  InitBank(num_workers, num_readers, argv[3]);

  return 0;
}
