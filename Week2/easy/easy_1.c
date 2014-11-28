#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[]) {

    if (argc != 2) {
        fputs("Error - wrong number of argumets", stderr);
        exit(1);
    }
    char* name = argv[1];
    char* file;
    FILE* fp = fopen(name, "r");

    if (fp == NULL) {
        fputs("Can't open input file \n", stderr);
        exit(1);
    }

    fseek(fp, 0, SEEK_END); // seek to the beginning of the file

    size_t sizeOfFile = ftell(fp), bytesRead;

    fseek(fp, 0, SEEK_SET); // seek to the beginning of the file

    file = (char*)malloc(sizeOfFile);

    if (file == NULL) {
        fputs("Reading allocating memory\n", stderr);
        exit(1);
    }

    bytesRead = fread(file, sizeof(char), sizeOfFile, fp);

    if (bytesRead != sizeOfFile) {
        fputs("Reading error\n", stderr);
        exit(1);
    }
    fclose(fp);
    free(file);
    puts("Done ;)\n");

    return 0;
}
