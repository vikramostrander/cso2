#include <stdio.h>
#include "primes.h"

int isprime(int n) {
	if(n == 0 || n == 1) return 0;
	for(int i = 2; i <= n/2; i++) {
		if(n % i == 0) return 0;
	}
	return 1;
}

int nextprime(int num) {
	num++;
	for(;;) {
		if(isprime(num) == 1) return num;
		num++; 
	}
}

void pprime(int num) {
	if(isprime(num) == 1) printf("%d is prime\n", num);
	else printf("%d is not prime, but %d is\n", num, nextprime(num));
}
