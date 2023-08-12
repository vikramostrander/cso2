#define _XOPEN_SOURCE 700

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <signal.h>

int mpid, pid;

void handler(int num) {
	if(num == SIGUSR1) { /* DO NOTHING */ }
	if(num == SIGUSR2) { kill(pid, SIGINT); }
}

__attribute__((noinline)) void empty() { /* DO NOTHING */ }

__attribute__((noinline)) long long nsecs() {
	struct timespec t;
	clock_gettime(CLOCK_MONOTONIC, &t);
	return t.tv_sec*1000000000 + t.tv_nsec;
}

int main(int argc, char *argv[]) {

	if(argc != 2) return 1;

	// SIGNAL HANDLER
	struct sigaction sa;
	sa.sa_handler = handler;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART;
	sigaction(SIGUSR1, &sa, NULL);
	sigaction(SIGUSR2, &sa, NULL);

	int value = atoi(argv[1]);
	long long time;
	long long avg = 0;
	int loops = 0;
	int control;

	// TIME NOTHING
	while(avg < 500000000) {
		time = nsecs();
		time = nsecs() - time;
		avg += time;
		loops++;
	}
	avg /= loops;

	control = avg;
	avg = 0;
	loops = 0;

	if(value == 1) {

		// TIME EMPTY
		while(avg < 500000000) {
			time = nsecs();
			empty();
			time = nsecs() - time;
			avg += time;
			loops++;
		}
		avg /= loops;

	}
	if(value == 2) {

		// TIME GETPID
		while(avg < 500000000) {
			time = nsecs();
			getpid();
			time = nsecs() - time;
			avg += time;
			loops++;
		}
		avg /= loops;

	}
	if(value == 3) {

		// TIME SHELL
		while(avg < 500000000) {
			time = nsecs();
			system("/bin/true");
			time = nsecs() - time;
			avg += time;
			loops++;
		}
		avg /= loops;

	}
	if(value == 4) {

		// TIME CURRENT PROCESS SIGNAL
		mpid = getpid();
		while(avg < 500000000) {
			time = nsecs();
			kill(mpid, SIGUSR1);	
			time = nsecs() - time;
			avg += time;
			loops++;
		}
		avg /= loops;

	}
	if(value == 5) {

		// TIME ANOTHER PROCESS SIGNAL
		mpid = getpid();
		char buffer[512];
		printf("%d\n", mpid);
		fgets(buffer, sizeof(buffer), stdin);
		pid = atoi(buffer);

		sigset_t set;
		sigemptyset(&set);
		sigaddset(&set, SIGINT);
		sigprocmask(SIG_BLOCK, &set, NULL);
		int sig;

		while(avg < 500000000) {
			time = nsecs();
			kill(pid, SIGUSR2);
			sigwait(&set, &sig);
			time = nsecs() - time;
			avg += time;
			loops++;
		}
		avg /= loops;
		
	}
	if(value == -1) {

		// OTHER PROCESS
		mpid = getpid();
		char buffer[512];
		printf("%d\n", mpid);
		fgets(buffer, sizeof(buffer), stdin);
		pid = atoi(buffer);

		for(;;) { /* WAIT FOR SIGNAL */ }

	}
	
	avg -= control;
	printf("%lld ns\n", avg);
	return 0;
}
