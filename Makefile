CC=gcc
CFLAGS=-Wall

build: *.c
	$(CC) -o stackulator *.c $(CFLAGS)

clean:
	rm *.o *~
