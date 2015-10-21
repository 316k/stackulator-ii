CC=gcc
CFLAGS=-Wall -g

build: *.c
	clear
	$(CC) -o stackulator stackulator.c another-brick-in-the.c $(CFLAGS)

clean:
	rm *.o *~

install:
	cp stackulator /usr/bin/stackulator
