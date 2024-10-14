#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

int main()
{
    char buf[80];

    printf("\x1b[36mpls gief inpt: \x1b[33m"); fflush(stdout);
    fgets(buf, 0x100, stdin);
    printf("\x1b[0m");
    if(strlen(buf) >= sizeof(buf))
        exit(-1);

    return 0;
}
