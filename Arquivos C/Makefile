CC=gcc
CFLAGS=-ansi -Wall -pedantic -O2
PR1=pr1.c

all: pr1 clean

pr1: $(PR1)
	$(CC) $(PR1) $(CFLAGS) -o pr1

%.o: %.c %.h
	$(CC) $(CFLAGS) -c $<

clean:
	rm -rf *.o