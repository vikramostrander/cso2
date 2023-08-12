#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/wait.h>

char *getoutput(const char *command) {

    if (command == NULL) return 0;

    int pipe_fd[2];
    pipe(pipe_fd);

    int wstatus;
    pid_t pid = getpid();
    pid_t child = fork();

    if (child > 0) {
        //  PARENT
        close(pipe_fd[1]);

        char *output;
        size_t length = 0;
        FILE *wrapper = fdopen(pipe_fd[0], "r");
        getdelim(&output, &length, '\0', wrapper);
        close(pipe_fd[0]);

        waitpid(child, &wstatus, 0);
        return output;
    }

    else if (child == 0) {
        //  CHILD
        dup2(pipe_fd[1], 1);
        close(pipe_fd[0]);
        close(pipe_fd[1]);
        int exer = execl("/bin/sh", "sh", "-c", command, (char *) NULL);
        if (exer == -1) _exit(127);
        return 0;
    }

}

char *parallelgetoutput(int count, const char **argv_base) {

    pid_t child_arr[count];

    int pipe_fd[2];
    pipe(pipe_fd);

    int wstatus;
    pid_t pid = getpid();

    int i = 0;
    pid_t child = -1;
    while (i < count) {
        child = fork();
        if(child == 0) break;
        child_arr[i] = child;
        ++i;
    }

    if (child > 0) {
        //  PARENT
        close(pipe_fd[1]);

        char *output;
        size_t length = 0;
        FILE *wrapper = fdopen(pipe_fd[0], "r");
        getdelim(&output, &length, '\0', wrapper);
        close(pipe_fd[0]);

        for (i = 0; i < count; ++i) {
            waitpid(child_arr[i], &wstatus, 0);
        }
        return output;
    }

    else if (child == 0) {
        //  CHILD
        dup2(pipe_fd[1], 1);
        close(pipe_fd[0]);
        close(pipe_fd[1]);

        int size = snprintf(NULL, 0, "%d", i);
        char str[size + 1];
        snprintf(str, size + 1, "%d", i);

        int index = 0;
        while (argv_base[index] != NULL) {
            ++index;
        }
        const char *argv[index + 2];
        for (int j = 0; j <= index; j++) {
            argv[j] = argv_base[j];
        }
        argv[index] = str;
        argv[index + 1] = NULL;

        argv_base[index] = str;

        int exer = execv(argv_base[0], (char*const*)argv);

        if (exer == -1) _exit(127);
        return 0;
    }

}

/*int main() {
    const char *argv_base[] = {"/bin/echo", "running", NULL};
    const char *output = parallelgetoutput(3, argv_base);
    printf("Text: [%s]\n", output);
}*/