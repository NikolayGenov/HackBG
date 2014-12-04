#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netinet/in.h>

#define PATH_MAX 50
#define NUMBER_OF_CONNECTIONS 5
#define PORT_NUMBER 12000
#define SERVER_ADDRESS "127.0.0.1"
#define FILENAME "file1"

void error_msg_and_exit(const char* msg) {
    fprintf(stderr, "%s --> %s\n", msg, strerror(errno));
    exit(EXIT_FAILURE);
}

int make_socket(int port) {
    int sock;
    struct sockaddr_in server_addr;

    sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock < 0) {
        error_msg_and_exit("Error creating socket");
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    inet_pton(AF_INET, SERVER_ADDRESS, &(server_addr.sin_addr));
    server_addr.sin_port = htons(port);

    if (bind(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        error_msg_and_exit("Error on bind");
    }

    return sock;
}

void receive_file(int server_socket) {
    int peer_socket;
    struct sockaddr_in peer_addr;
    socklen_t sock_len;

    char buffer[BUFSIZ];
    int received_fd;
    off_t file_size;
    off_t remain_data = 0ul;
    ssize_t len;

    peer_socket = accept(server_socket, (struct sockaddr*)&peer_addr, &sock_len);
    if (peer_socket < 0) {
        error_msg_and_exit("Error on accept");
    }

    //fprintf(stdout, "Accept peer --> %s\n", inet_ntoa(peer_addr.sin_addr));

    if (recv(peer_socket, (void*)&file_size, sizeof(file_size), 0) < 0) {
        error_msg_and_exit("Error on receiving size of the file");
    }

   //fprintf(stdout, "\nFile size : %lu\n", (unsigned long)file_size);

    received_fd = open(FILENAME, O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);

    if (received_fd < 0) {
        error_msg_and_exit("Failed to open file");
    }

    remain_data = file_size;

    while (((len = recv(peer_socket, buffer, BUFSIZ, 0)) > 0) && (remain_data > 0)) {
        write(received_fd, buffer, len);
        remain_data -= len;
    }
    if (len < 0) {
        error_msg_and_exit("Error on receiving size of the file");
    }

    //printf("Remaining size : %lu\n", remain_data);
    //fflush(stdout);

    close(received_fd);
}


int main(int argc, char** argv) {
    int server_socket = make_socket(PORT_NUMBER);

    if ((listen(server_socket, NUMBER_OF_CONNECTIONS)) < 0) {
        error_msg_and_exit("Error on listen");
    }

    while (1) { //Debug
        receive_file(server_socket);
    }

    close(server_socket);

    return 0;
}
