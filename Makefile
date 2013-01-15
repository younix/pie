CFLAGS=-std=c99 -pedantic -Wall
LFLAGS=-lm
VERSION=1
DIR=pie-${VERSION}
CONTENT=pie.c pie.1 Makefile

.PHONY: all clean tar
all: pie

pie: pie.c
	gcc ${CFLAGS} ${LFLAGS} pie.c -o $@

tar: ${CONTENT}
	rm -rf ${DIR}
	mkdir -p ${DIR}
	cp ${CONTENT} ${DIR}
	tar czf ${DIR}.tar.gz ${DIR}

clean:
	rm -rf pie ${DIR}.tar.gz ${DIR}
