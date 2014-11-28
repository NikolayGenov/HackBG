#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

int c = 0;

void* fn_C() {
    int i;
    for (i = 0; i < 10; i++) {
        c++;
        printf(" %d", c);
    }
}

int main() {
    int rt1, rt2;
    pthread_t t1, t2;
    /* Create two threads */
    if ((rt1 = pthread_create(&t1, NULL, &fn_C, NULL)))
        printf("Thread creation failed: %d\n", rt1);
    if ((rt2 = pthread_create(&t2, NULL, &fn_C, NULL)))
        printf("Thread creation failed: %d\n", rt2);
    /* Wait for both threads to finish */
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    printf("\n");
    return 0;
}
