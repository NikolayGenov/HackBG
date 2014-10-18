#include <stdio.h>
#include <stdlib.h>

void copyFileTo(char* source, char* dest){
    FILE* fs = fopen(source, "r");
    FILE* fd = fopen(dest, "w");
    const int BufferSize = 1024;
    char buffer[BufferSize];
    size_t n_chars;
    if (fs && fd){
        while ((n_chars = fread(buffer, 1, BufferSize, fs)) > 0)
            if(fwrite(buffer, 1, n_chars, fd) != n_chars)
                printf("Something wrong with writing");
        if(n_chars == -1)
            printf("Something wrong with reading");
        fclose(fs);
        fclose(fd);
    }
}

int main(int argc, char* argv[]){
    char* source = argv[1];
    char* dest = argv[2];
    copyFileTo(source, dest);
    return 0;
}
