#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

void print_pids(int pid) {
    printf("Own pid: %d, parent pid: %d, fork returned: %d\n", getpid(), getppid(), pid);
}

int main() {
    int n = 0, i = 0;
    char* s;
    puts("Enter 'forkN' where N is a number");
    scanf("fork%d", &n);
    pid_t pid;

    for (; i < n; ++i) {
        pid = fork();
        print_pids(pid);
    }
    /* wait for the second thread to finish */

    return 0;
}
