CC=gcc
CFLAGS=-Wall -std=c99 -pedantic -O2 #-Wextra
MACAS=macas.c stable.o list.o error.o buffer.o asmtypes.o optable.o parser.o mactypes.h opcodes.h

all: macas clean

macas: $(MACAS)
	$(CC) $(MACAS) $(CFLAGS)

%.o: %.c %.h
	$(CC) $(CFLAGS) -c $<

clean:
	rm -rf *.o *.gch