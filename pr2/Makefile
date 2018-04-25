CC=gcc
CFLAGS=-Wall -std=c99 -pedantic -O2 -Wextra
CENTER=center.c buffer.o

all: center clean

center: $(CENTER)
	$(CC) $(CENTER) $(CFLAGS) -o center

%.o: %.c %.h
	$(CC) $(CFLAGS) -c $<

clean:
	rm -rf *.o