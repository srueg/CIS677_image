CC=gcc
CFLAGS= -Wall -g -std=c99

ODIR=obj

image: *.c
	gcc -o $@ $^ $(CFLAGS)

.PHONY: clean

clean:
	rm -f image
	