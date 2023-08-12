#include "lab.h"

#include <string.h>     // for memset
#include <stdio.h>      // for printf

/* When complete, this function should fill in "buffer"
 * with a length-character \0-termianted string such that
 * check_passphrase(buffer) is true.
 *
 * The implementation we supplied tries the guesses of
 * 'a', 'b', and 'c' and prints out how long it takes
 * to check each of them.
 *
 * To do so, your implementation should rely on timing
 * how long check_passphrase takes, most likely by using
 * "measure_once" wrapper function.
 *
 * (Your implementation may not examine the memory in which
 *  the passphrase is stored in another way.)
 */
void find_passphrase(char *buffer, int length) {

    
    int result;
    int guess;

    long time;
    long avg;
    long max;

    while(1) {
        memset(buffer, '\0', length);
        for (int index = 0; index < length; ++index) {

            max = 0;
            guess = -1;

            for (int letter = 0; letter < 26; ++letter) {

                buffer[index] = (char)('a' + letter);
                avg = 0;

                for (int i = 0; i < 3; ++i) {
                    time = measure_once(&result, buffer, check_passphrase);
                    if(result == 1) return;
                    avg += time;
                }

                if(avg > max) {
                    max = avg;
                    guess = letter;
                }

            }

            buffer[index] = (char)('a' + guess);

        }
    }
}
