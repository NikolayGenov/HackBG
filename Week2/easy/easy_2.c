#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[])
{
    if (argc != 2) {
        fputs("Error - wrong number of argumets", stderr);
        exit(1);
    }
    char* name = argv[1];
    char* buffer = NULL;
    FILE* fp = fopen(name, "r");
    int i, flag = 1;

    if (fp == NULL) {
        fputs("Can't open input file \n", stderr);
        exit(1);
    }
    fseek(fp, 0, SEEK_SET);
    size_t chunk = 25, bytesRead, pos = 0, bufSize = 1;

    do {
        buffer = (char*)realloc(buffer, bufSize);
        bufSize += chunk;
        if (buffer == NULL) {
            fputs("Reading allocating memory\n", stderr);
            exit(1);
        }

        bytesRead = fread(buffer + pos, sizeof(char), chunk, fp);
        //  printf("%d\n",(int) pos);
        /* Do stuff here */

        for (i = pos; i < bytesRead + pos; ++i) {
            if (buffer[i] == '\n') {
                flag = 0;
                pos = i;
                break;
            }
        }
        if (flag) pos += bytesRead;
    } while (flag);

    buffer[pos] = '\0';
    printf("%s\n", buffer);
    free(buffer);

    fclose(fp);
    puts("Done ;)\n");

    return 0;
}
