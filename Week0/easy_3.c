#include <stdio.h>

int main(int argc, char* argv[])
{
    if (argc != 2) {
        puts("Wrong number of arguments!");
        return -1;
    }

    char str[256];
    scanf("%255s", str);

    FILE* fp = fopen(argv[1], "w");

    if (fp) {
        fprintf(fp, "%s", str);
        fclose(fp);
    }
    return 0;
}
