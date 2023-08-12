#define _XOPEN_SOURCE 600

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/mman.h>
#include <fcntl.h>

int pid, mpid, size;
char inbox_name[512];
char outbox_name[512];
char *inbox;
char *outbox;

void handler(int num) {
	if(num == SIGINT) {
		//cleanup
		munmap(inbox, size);
		shm_unlink(inbox_name);
		exit(0);
	}
	if(num == SIGTERM) {
		//cleanup and send to other user
		kill(pid, SIGINT);
		kill(mpid, SIGINT);
		exit(0);
	}
	if(num == SIGUSR1) {
		//disply inbox
		fputs(inbox, stdout);
		fflush(stdout);
		inbox[0] = '\0';
	}
}

int main(int argc, char *argv[]) {


	
	//	SIGNAL HANDLING
	struct sigaction sa;
	sa.sa_handler = handler;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART;
	
	sigaction(SIGINT, &sa, NULL);
	sigaction(SIGTERM, &sa, NULL);
	sigaction(SIGUSR1, &sa, NULL);	



	//	OBTAINING PID
	char buffer[512];
	mpid = getpid();

	printf("%d\n", mpid);
	fgets(buffer, sizeof(buffer), stdin);

	pid = atoi(buffer);



	//	INITIALIZING INBOX
	snprintf(inbox_name, sizeof(inbox_name), "/%d-mmchat", mpid);
	int inbox_fd = shm_open(inbox_name, O_CREAT | O_RDWR, 0666);

	if(inbox_fd < 0) {
		printf("AN ERROR OCCURED CREATING INBOX FD");
		close(inbox_fd);
		kill(mpid, SIGTERM);
		return 1;
	}

	size = 4096;
	ftruncate(inbox_fd, size);

	inbox = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, inbox_fd, 0);
	if(inbox == (char*) MAP_FAILED) {
		printf("AN ERROR OCCURED CREATING INBOX MMAP");
		close(inbox_fd);
		kill(mpid, SIGTERM);
		return 1;
	}
	close(inbox_fd);
	printf("INBOX INITIALIZED\n");	


	//	INITIALIZING OUTBOX
	snprintf(outbox_name, sizeof(outbox_name), "/%d-mmchat", pid);
	int outbox_fd = shm_open(outbox_name, O_CREAT | O_RDWR, 0666);

	if(outbox_fd < 0) {
		printf("AN ERROR OCCURED CREATING OUTBOX FD");
		close(outbox_fd);
		kill(mpid, SIGTERM);
		return 1;
	}

	ftruncate(outbox_fd, size);

	outbox = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, outbox_fd, 0);
	if(outbox == (char*) MAP_FAILED) {
		printf("AN ERROR OCCURED CREATING OUTBOX MMAP");
		close(outbox_fd);
		kill(mpid, SIGTERM);
		return 1;
	}
	close(outbox_fd);
	printf("OUTBOX INITIALIZED\n");	



	//	HANDLING I/O
	while(!feof(stdin)) {
		fgets(outbox, sizeof(outbox), stdin);
		kill(pid, SIGUSR1);
		while(outbox[0]) { usleep(10000); }
	}

	kill(mpid, SIGTERM);
}
