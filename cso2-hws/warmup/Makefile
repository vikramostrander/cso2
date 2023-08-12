CC = gcc
CFLAGS = -Wall -pedantic -std=c11 -fsanitize=address
LDFLAGS = -Wall -pedantic -fsanitize=address -lm

primes: main.o primes.o
	$(CC) -o primes main.o primes.o $(LDFLAGS)

.c.o: 
	$(CC) $(CFLAGS) -o $@ -c $<

main.o: main.c primes.h

primes.o: primes.c primes.h
