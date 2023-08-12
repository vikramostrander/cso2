CC = gcc
CFLAGS = -Wall -pedantic -std=c11 -fsanitize=address
LDFLAGS = -Wall -pedantic -fsanitize=address -lm

.c.o: 
	$(CC) $(CFLAGS) -o $@ -c $<

cheer.o: cheer.c say.h

grunt.o: grunt.c say.h

program.o: program.c say.h

libsay.a: cheer.o grunt.o
	ar rcs libsay.a cheer.o grunt.o

guesser: guesser.o libsay.a
	$(CC) -o guesser guesser.o libsay.a $(LDFLAGS)

.PHONY: all clean

all: guesser

clean: 
	rm --force guesser libsay.a program.o guesser.o grunt.o cheer.o

