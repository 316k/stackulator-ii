CC=gcc
CFLAGS=-Wall

build: *.c
	clear
	$(CC) -o stackulator stackulator.c another-brick-in-the.c $(CFLAGS)

clean:
	rm *.o *~
