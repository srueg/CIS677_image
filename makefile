CC=gcc
CFLAGS= -Wall -g -std=c99 -pthread -lm

ODIR=obj

image: *.c
	gcc -o $@ $^ $(CFLAGS)

.PHONY: clean

clean:
	rm -f image
	
