_Deps = bank.h ledger.h
_OBJ = bank.o ledger.o
_MOBJ = main.o

APPBIN = bank_app

IDIR = include
CC = g++
CFLAGS = -I$(IDIR) -Wall -Wextra -g -pthread
ODIR = obj
SDIR = src
LDIR = lib
LIBS = -lm
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))
MOBJ = $(patsubst %,$(ODIR)/%,$(_MOBJ))

$(ODIR)/%.o: $(SDIR)/%.cpp $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

all: $(APPBIN)

$(APPBIN): $(OBJ) $(MOBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

submission:
	find . -name "*~" -exec rm -rf {} \;

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o *~ core $(INCDIR)/*~
	rm -f $(APPBIN)
