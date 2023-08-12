#include <stdio.h>
#include <sys/wait.h>
int my_system(const char *command);

int main(int argc, const char *argv[]) {
    int a1 = my_system("sleep 1; echo hi");
    int a2 = my_system("echo bye");
    int a3 = my_system("flibbertigibbet 23");
    printf("%d %d %d\n", 
        WEXITSTATUS(a1), WEXITSTATUS(a2), WEXITSTATUS(a3));
}