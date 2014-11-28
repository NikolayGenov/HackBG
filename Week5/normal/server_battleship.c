#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#include <pthread.h>
#define SIZE 10
#define NUMBER_OF_CLIENTS 2

char board[SIZE][SIZE];
enum state { FREE, SHIP, MISS, HIT };

pthread_mutex_t lock;

void generate_map() {

    int x;
    int y;
    // Vertical Ships
    // Vertical Ship 1
    x = rand() % 8;
    y = rand() % 8;
    if (x == SHIP) {
        x = rand() % 8;
    } else if (y == SHIP) {
        y = rand() % 8;
    }
    board[x][y] = SHIP;
    board[x][y + 1] = SHIP;
    board[x][y + 2] = SHIP;

    // Vertical Ship 2
    y = rand() % 8;
    y = rand() % 8;
    if (x == SHIP) {
        x = rand() % 8;
    } else if (y == SHIP) {
        y = rand() % 8;
    }
    board[x][y] = SHIP;
    board[x][y + 1] = SHIP;
    board[x][y + 2] = SHIP;
    // horizontal ships
    // Horizontal Ship 1
    x = rand() % 8;
    y = rand() % 8;
    if (x == SHIP) {
        x = rand() % 8;
    } else if (y == SHIP) {
        y = rand() % 8;
    }
    board[x][y] = SHIP;
    board[x + 1][y] = SHIP;
    board[x + 2][y] = SHIP;
    // Horizontal Ship 2
    x = rand() % 8;
    y = rand() % 8;
    if (x == SHIP) {
        x = rand() % 8;
    } else if (y == SHIP) {
        y = rand() % 8;
    }
    board[x][y] = SHIP;
    board[x + 1][y] = SHIP;
    board[x + 2][y] = SHIP;
}

void print_board() {
    int i, j, k, x, y;
    printf("  ");
    for (i = 0; i < 10; i++) {
        printf(" %d", i);
    }
    puts(" ");
    for (k = 0; k < 20; k++) {
        printf("-");
    }
    printf("\n");
    for (x = 0; x < 10; x++) {

        printf("%d| ", x);

        for (y = 0; y < 10; y++) {
            if (board[x][y] == FREE) {
                printf("  ");
            } else if (board[x][y] == SHIP) {
                printf("++");
            } else if (board[x][y] == HIT) {
                printf("**");
            } else if (board[x][y] == MISS) {
                printf("..");
            }
        }
        printf("\n");
    }
    printf("\n");
}

void clean_board() {
    int x, y;
    for (x = 0; x < 10; x++) {
        for (y = 0; y < 10; y++) {
            board[x][y] = FREE;
        }
    }
}

typedef struct {
    int x;
    int y;
    int val;
} read_struct;

void* change_table(void* arg) {

    int connfd = *(int*)arg;
    read_struct read_str;

    char buff[sizeof(read_struct)];
    int n;
    int error = 0;
    socklen_t len = sizeof(error);
    while (0 == getsockopt(connfd, SOL_SOCKET, SO_ERROR, &error, &len)) {
        if ((n = read(connfd, buff, sizeof(read_str))) > 0) {
            pthread_mutex_lock(&lock);

            memcpy((void*)&read_str, buff, sizeof(read_str));

            board[read_str.x][read_str.y] = read_str.val;

            print_board();

            pthread_mutex_unlock(&lock);
        }
    }

    return NULL;
}

int main(int argc, char* argv[]) {
    srand(time(NULL));
    clean_board();
    print_board();

    // Socket stuff
    int listenfd = 0, connfd = 0;
    struct sockaddr_in serv_addr;

    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    memset(&serv_addr, '0', sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(5000);

    bind(listenfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));

    listen(listenfd, SIZE);
    int i = 0;
    int err;
    pthread_t tid[NUMBER_OF_CLIENTS];

    if (pthread_mutex_init(&lock, NULL) != 0) {
        printf("\n mutex init failed\n");
        return 1;
    }

    while (i < NUMBER_OF_CLIENTS) {
        connfd = accept(listenfd, (struct sockaddr*)NULL, NULL);

        err = pthread_create(&(tid[i]), NULL, &change_table, &connfd);
        if (err != 0)
            printf("\ncan't create thread :[%s]", strerror(err));
        i++;
    }

    for (i = 0; i < NUMBER_OF_CLIENTS; ++i) {
        pthread_join(tid[i], NULL);
    }
}
