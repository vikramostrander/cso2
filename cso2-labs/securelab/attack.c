#include "lab.h"
#include <string.h>

/* forward function for "attack 0" case.
   change this code to implement your attack
 */
void forward_attack_0(struct message *message) {
    if (strcmp(message->data, "PAY $1000 TO M") == 0) {
         strcpy(message->data, "PAY $10001000 TO M");
         message->data_size = strlen("PAY $10001000 TO M");
     }
     send_message(message);
}

/* forward function for "attack 1" case.
   change this code to implement your attack
 */
bool secondtime_1 = false;
void forward_attack_1(struct message *message) {
    if(secondtime_1) {
        send_message(message);
        return;
    }
    secondtime_1 = true;
    struct message *fake_message = new_message('A', 'B', "PAY $10001000 TO M", true, false);
    send_message(fake_message);
}

/* forward function for "attack 2" case.
   change this code to implement your attack
 */
bool secondtime_2 = false;
void forward_attack_2(struct message *message) {
    if(secondtime_2) {
        send_message(message);
        return;
    }
    secondtime_2 = true;
    send_message(message);
    send_message(message);
}

/* forward function for "attack 3" case.
   change this code to implement your attack
 */
void forward_attack_3(struct message *message) {
    send_message(message);
}

/* forward function for "attack 4" case.
   I did not intend this one to be possible. */
void forward_attack_4(struct message *message) {
    send_message(message);
}
