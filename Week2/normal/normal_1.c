#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int main(int argc, char* argv[])
{
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

    // Search for string
    char search_buffer[256], *search;
    puts("Enter the string you want to search for: ");
    scanf("%s", search_buffer);
    int search_len = strlen(search_buffer);
    search = malloc(search_len + 1);

    if (search == NULL) {
        fputs("Reading allocating memory\n", stderr);
        exit(1);
    }
    strncpy(search, search_buffer, search_len);
    char* search_ptr = strstr(file, search);

    if (search_ptr == NULL)
        puts("String not found");
    else
        puts("Found somewhere ");
    // TODO - search again

    fclose(fp);
    free(file);
    puts("Done ;)\n");

    return 0;
}
