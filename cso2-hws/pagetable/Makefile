CC = gcc
CFLAGS = -Wall -pedantic -std=c11 -fsanitize=address
LDFLAGS = -Wall -pedantic -fsanitize=address -lm

libmlpt.a: pagetable.o
	ar rcs libmlpt.a pagetable.o

pagetable.o: pagetable.c config.h mlpt.h
	$(CC) $(CFLAGS) -o pagetable.o -c pagetable.c

.PHONY: all clean

all: libmlpt.a

clean:
	rm --force libmlpt.a pagetable.o