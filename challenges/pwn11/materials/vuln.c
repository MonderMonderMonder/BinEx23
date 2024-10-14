#define _DEFAULT_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

void win()
{
    execl("/bin/get_flag", "get_flag", NULL);
    exit(1);
}

int main()
{
    char buf[0x100] = { 0 };
    ssize_t n;

    setbuf(stdout, NULL);

    printf("\x1b[36mHi, what's your name?\x1b[33m\n");
    if(0 >= (n = read(fileno(stdin), buf, sizeof(buf) - 1)))
        exit(1);

    for(char *p = buf + n - 1; p >= buf && *p == '\n'; --p)
        *p = 0;

    printf("\x1b[34mPwn harder, \x1b[0m");
    printf(buf);

    return 0;
}
