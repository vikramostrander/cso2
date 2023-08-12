CC = clang
CFLAGS = -g -O3 -pthread -Wall -Werror
# ASAN = Address SANitizer, see https://github.com/google/sanitizers/wiki/AddressSanitizer
CFLAGS_ASAN = $(CFLAGS) -fsanitize=address -fno-omit-frame-pointer
LDFLAGS = -lpthread
LDFLAGS_ASAN = $(LDFLAGS) -fsanitize=address

all: life life-asan 

%-asan.o: %.c life.h timing.h
	$(CC) -c $(CFLAGS_ASAN) -o $@ $<

%.o: %.c life.h
	$(CC) -c $(CFLAGS) -o $@ $<

life: main.o life.o timing.o life-parallel.o life-serial.o
	$(CC) $(LDFLAGS) -o $@ $^

life-asan: main-asan.o life-asan.o timing-asan.o life-parallel-asan.o life-serial-asan.o
	$(CC) $(LDFLAGS_ASAN) -o $@ $^

clean:
	rm -f *.o

distclean: clean
	rm -f life life-asan

.PHONY: all clean distclean
