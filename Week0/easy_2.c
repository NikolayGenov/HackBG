#include <stdio.h>

int main()
{
    char str[256];
    scanf("%255s", str);

    printf("%s\n", str);

    return 0;
}
