#include <stdio.h>
#include "netsim.h"

char instruction;
int seqnum;
int seqcount;
int last_received = -1;


void message(void *seq) {

    int seqnum = (int)seq;
    char msg[5];

    if(seqnum == 0) {
        msg[0] = 'G'^'E'^'T'^instruction;
        msg[1] = 'G'; msg[2] = 'E'; msg[3] = 'T';
        msg[4] = instruction;
    }
    else {
        last_received = seqnum;
        msg[0] = 'A'^'C'^'K'^seqnum;
        msg[1] = 'A'; msg[2] = 'C'; msg[3] = 'K';
        msg[4] = seqnum;
    }

    send(5, msg);

}

void recvd(size_t len, void* _data) {

    char *received = _data;
    seqnum = received[1];
    seqcount = received[2];

    if(seqnum == last_received) return;
    last_received = seqnum;

    char *data = _data;
    fwrite(data+3,1,len,stdout);
    fflush(stdout);

}

int main(int argc, char *argv[]) {

    // this code should work without modification
    if (argc != 2) {
        fprintf(stderr, "USAGE: %s n\n    where n is a number between 0 and 3\n", argv[0]);
        return 1;
    }

    instruction = argv[1][0];

    while(last_received == -1) {
        setTimeout(message, 1000, (void *)0);
        waitForAllTimeouts();
    }

    while(seqnum != seqcount) {
        setTimeout(message, 1000, (void *)seqnum);
        waitForAllTimeouts();
    }

}
