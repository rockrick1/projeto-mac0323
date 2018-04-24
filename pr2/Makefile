CC=gcc
CFLAGS=-Wall -pedantic -O2
CENTER=center.c buffer.o

all: center clean

center: $(CENTER)
	$(CC) $(CENTER) $(CFLAGS) -o center

%.o: %.c %.h
	$(CC) $(CFLAGS) -c $<

clean:
	rm -rf *.o