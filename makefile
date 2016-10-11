CC=gcc
CFLAGS= -Wall -Wno-misleading-indentation -g -std=c99 -pthread -lm

ODIR=obj

image: *.c
	gcc -o $@ $^ $(CFLAGS)

.PHONY: clean

clean:
	rm -f image
	
