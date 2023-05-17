#ifndef _LEDGER_H
#define _LEDGER_H

#include <bank.h>

using namespace std;

#define D 0
#define W 1
#define T 2
#define C 3

const int SEED_RANDOM = 377;

struct Ledger {
	int from;
	int to;
	int amount;
  	int mode;
	int ledgerID;
};


void InitBank(int num_workers, int num_readers, char *filename);
void *load_ledger(void *infile_pointer);
void *worker(void *unused);

#endif
