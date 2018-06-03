CC=gcc
CFLAGS=-Wall -std=c99 -pedantic -O2 #-Wextra
PARSE_TEST=parse_test.c stable.o list.o error.o buffer.o asmtypes.o optable.o parser.o mactypes.h opcodes.h

all: parse_test clean

parse_test: $(PARSE_TEST)
	$(CC) $(PARSE_TEST) $(CFLAGS)

%.o: %.c %.h
	$(CC) $(CFLAGS) -c $<

clean:
	rm -rf *.o *.gch