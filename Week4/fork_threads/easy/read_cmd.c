#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
int main(int argc, char* argv[]) {

    pid_t pid = fork();
    printf("PID of child is %d: \n", pid);

    if (pid == 0) {
        execvp(argv[1], argv + 1);
        printf("Error in child: %d\n", errno);
        fflush(stdout);
        _exit(EXIT_FAILURE); // exec never returns
    }
}
