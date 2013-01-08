CFLAGS=-std=c99 -pedantic -Wall
LDFLAGS=-lm

.PHONY: all clean
all: pie

pie: pie.c
	gcc ${CFLAGS} ${LDFLAGS} pie.c -o pie

clean:
	rm -f pie
