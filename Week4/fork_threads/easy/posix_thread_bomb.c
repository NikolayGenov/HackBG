#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

void* fork_stuff() {
    pthread_t fork_thread, f2;

    pthread_create(&fork_thread, NULL, fork_stuff, 0);
    pthread_create(&f2, NULL, fork_stuff, 0);
    while (1) {
        pthread_join(fork_thread, NULL);
    }
}

int main() {

    fork_stuff();
    /* wait for the second thread to finish */

    return 0;
}
