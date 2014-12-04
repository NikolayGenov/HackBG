#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/sendfile.h>

#define PORT_NUMBER 12000
#define remote_addrESS "127.0.0.1"
#define FILE_TO_SEND "file"

void error_msg_and_exit(const char* msg) {
    fprintf(stderr, "%s --> %s\n", msg, strerror(errno));
    exit(EXIT_FAILURE);
}

int make_socket(int port) {
    int sock;
    struct sockaddr_in remote_addr;

    sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock < 0) {
        error_msg_and_exit("Error creating socket");
    }

    memset(&remote_addr, 0, sizeof(remote_addr));
    remote_addr.sin_family = AF_INET;
    inet_pton(AF_INET, remote_addrESS, &(remote_addr.sin_addr));
    remote_addr.sin_port = htons(port);

    if (connect(sock, (struct sockaddr*)&remote_addr, sizeof(struct sockaddr)) < 0) {
        error_msg_and_exit("Error on connect");
    }

    return sock;
}

void send_file(int client_socket) {
    int fd;
    struct stat file_stat;
    off_t remain_data = 0ul;
    off_t sent_bytes = 0ul;
    off_t offset = 0ul;

    fd = open(FILE_TO_SEND, O_RDONLY);
    if (fd < 0) {
        error_msg_and_exit("Error openfing file");
    }

    if (fstat(fd, &file_stat) < 0) {
        error_msg_and_exit("Error fstat");
    }
    
    if (send(client_socket, (void*)&file_stat.st_size, sizeof(file_stat.st_size), 0) < 0) {
        error_msg_and_exit("Error on sending file size");
    }

    remain_data = file_stat.st_size;
    
    while (((sent_bytes = sendfile(client_socket, fd, (off_t*)&offset, BUFSIZ)) > 0) && (remain_data > 0)) {
        //fprintf(stdout, "%d bytes send, offset: %d and remaining = %d\n", sent_bytes, offset, remain_data);
    }
    close(fd);
}


int main(int argc, char** argv) {
    int client_socket = make_socket(PORT_NUMBER);

    send_file(client_socket);

    close(client_socket);

    return 0;
}
