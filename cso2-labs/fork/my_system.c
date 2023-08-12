#include <unistd.h>
//noreturn void _exit(int status);
#include <sys/wait.h>


int my_system(const char *command) {

    if(command == NULL) return 0;

    int wstatus;

    pid_t pid = getpid();
    pid_t child = fork();

    if (child > 0) {
        //  PARENT
        waitpid(child, &wstatus, 0);
        return wstatus;
    }

    else if (child == 0) {
        //  CHILD
        int exer = execl("/bin/sh", "sh", "-c", command, (char *) NULL);
        if (exer == -1) _exit(127);
        return 0;
    }

}