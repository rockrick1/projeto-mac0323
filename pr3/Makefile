CC=gcc
CFLAGS=-Wall -std=c99 -pedantic -O2 #-Wextra
FREQ=freq.c stable.o

all: freq clean

freq: $(FREQ)
	$(CC) $(FREQ) $(CFLAGS) -o freq

%.o: %.c %.h
	$(CC) $(CFLAGS) -c $<

clean:
	rm -rf *.o