#include <string.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/file.h>
#include <unistd.h>
#define LINE_SIZE 255
#define BUFFER_SIZE 2048

int number_of_lines_in_file(char* filename) {
    FILE* myfile = fopen(filename, "r");
    int ch, number_of_lines = 0;
    do {
        ch = fgetc(myfile);
        if (ch == '\n')
            number_of_lines++;
    } while (ch != EOF);

    if (ch != '\n' && number_of_lines != 0)
        number_of_lines++;

    fclose(myfile);
    return number_of_lines;
}

typedef struct {
    char logname[LINE_SIZE];
    int log_combiner_fd;
} log_arg_struct;

void* extract_log_data(void* arg) {
    log_arg_struct* args = (log_arg_struct*)arg;
    printf("Processing: %s\n", args->logname);
    int log_file = open(args->logname, O_RDONLY);
    int write_fd = args->log_combiner_fd;
    int bytes_read;
    char buffer[BUFFER_SIZE];
    while ((bytes_read = read(log_file, buffer, BUFFER_SIZE)) > 0) {
        flock(write_fd, LOCK_EX);
        write(write_fd, buffer, bytes_read);
        flock(write_fd, LOCK_UN);
    }

    close(log_file);
    printf("Work is done for %s\n", args->logname);
    return 0;
}
void process_file(char* read_log_file, int write_logs_fd, pthread_t* threads, log_arg_struct* log_strs) {
    FILE* read_logs = fopen(read_log_file, "r");
    int z = 5;
    char line[LINE_SIZE];
    int i = 0;
    while (fgets(line, sizeof(line), read_logs)) {
        line[strlen(line) - 1] = '\0';
        strncpy(log_strs[i].logname, line, LINE_SIZE);
        log_strs[i].log_combiner_fd = write_logs_fd;
        pthread_create(&threads[i], NULL, &extract_log_data, &log_strs[i]);
        ++i;
    }
    fclose(read_logs);
}

int main(int argc, char** argv) {
    if (argc != 3) {
        puts("Wrong number of arguments");
        puts("Please enter filename to where to read the logs, and where to "
             "write them");
        exit(1);
    }

    char* read_log_file = argv[1];
    char* write_log_file = argv[2];

    int write_logs_fd = open(write_log_file, O_WRONLY);
    int thread_count = number_of_lines_in_file(read_log_file);
    pthread_t* threads = malloc(thread_count * sizeof(pthread_t));

    log_arg_struct* log_strs = (log_arg_struct*)malloc(thread_count * sizeof(log_arg_struct));
    process_file(read_log_file, write_logs_fd, threads, log_strs);

    int i;
    for (i = 0; i < thread_count; ++i)
        pthread_join(threads[i], NULL);

    close(write_logs_fd);
    free(threads);
    free(log_strs);

    return 0;
}
