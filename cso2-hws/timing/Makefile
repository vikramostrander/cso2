CC = gcc
CFLAGS = -Wall -pedantic -std=c11 -fsanitize=address -o2
LDFLAGS = -Wall -pedantic -fsanitize=address -lm

gettimings: timing.o
	$(CC) -o gettimings timing.o $(LDFLAGS)

timing.o: timing.c
	$(CC) $(CFLAGS) -o $@ -c $<
