#include <stdio.h>
#include <sys/wait.h>
int my_system(const char *command);

int main(int argc, const char *argv[]) {
    my_system("echo -n 'type something: ';"
        " read got;"
        " echo Thanks for typing \\\"\"$got\"\\\"");
}