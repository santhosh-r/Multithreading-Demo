CC=gcc
CFLAGS=-I -Wall -lpthread
DEPS=queue.h
cons: proc
	$(CC) -o cons src/cons.c $(CFLAGS)
proc:
	$(CC) -o proc src/proc.c src/queue.c $(CFLAGS)
clean:
	rm *.output proc cons