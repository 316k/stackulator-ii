CC=gcc
CFLAGS=-Wall

build: *.c
	clear
	$(CC) -o stackulator *.c $(CFLAGS)

clean:
	rm *.o *~
