#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[])
{
    if (argc != 3) {
        puts("Error- wrong number of arguments - must be 2");
        exit(EXIT_FAILURE);
    }

    char* source = argv[1];
    char* dest = argv[2];

    if (rename(source, dest)) {
        puts("Error renaming file");
        exit(EXIT_FAILURE);
    }
}
