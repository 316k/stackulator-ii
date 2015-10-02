CC=gcc
CFLAGS=-Wall

build: *.c
	clear
	$(CC) -o stackulator stackulator.c $(CFLAGS)

clean:
	rm *.o *~
