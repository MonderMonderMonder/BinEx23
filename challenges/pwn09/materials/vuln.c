#define _DEFAULT_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

int main()
{
    char buf[0x100] = { 0 };
    char flag[0x100] = { 0 };
    FILE *fp;

    setbuf(stdout, NULL);

    if(!(fp = popen("get_flag", "r")))
        return -1;
    fgets(flag, sizeof(flag), fp);
    pclose(fp);

    printf("\x1b[36mHi, what's your name?\x1b[33m\n");
    fgets(buf, sizeof(buf), stdin);

    for(char *p = buf + strlen(buf) - 1; p >= buf && *p == '\n'; --p)
        *p = 0;

    printf("\x1b[34mPwn harder, ");
    printf(buf);
    printf("!\x1b[0m\n");

    return 0;
}
