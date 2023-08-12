#include <stdio.h>
#include <stdlib.h>
#include "primes.h"

int main(int argc, char *argv[]) {
	if(argc > 1) {
		for(int i = 1; i < argc; i++) {
			pprime(atoi(argv[i]));
		}
	}
	else {
		char str[100];
		while(fgets(str, 99, stdin) != NULL) {
			pprime(atoi(str));
		}
	}	
}
