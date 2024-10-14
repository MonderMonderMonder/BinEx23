#define _GNU_SOURCE
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>

void highly_secure_strcat(char *a, char *b, unsigned short len)
{
    char result[len], may_the_canary_be_with_you[8];
    char *src = a, *dest = result;

    while(*src)
        *dest++ = *src++;
    src = b;
    while(*src)
        *dest++ = *src++;
    *dest = 0;

    printf("Result: \x1b[34m%s\x1b[0m\n", result);
}

void highly_secure_strcat_dispatcher(char *a, char *b, unsigned short len)
{
    highly_secure_strcat(a, b, len);
}

void (*highly_secure_strcat_factory(void))(char *, char *, unsigned short)
{
    return highly_secure_strcat_dispatcher;
}

char a[USHRT_MAX], b[USHRT_MAX];

int main()
{
    char buf[7];
    unsigned short na, nb;

    printf("Management always needs to know mission-critical information. The factory is here: %p\n", highly_secure_strcat_factory);
    /* ghetto load limiter */
    char *lmt = strdup("test $(ps -upwn1?|fgrep -v CMD|wc -l) -lt 42");
    uid_t uid = getuid();
    *strchr(lmt, '?') = (uid % 10) + 0x30;
    if(system(lmt))
        exit(1);

    setbuf(stdout, NULL);

    printf("\x1b[35mSCAAS\x1b[0m - \x1b[35mS\x1b[0mtring \x1b[35mC\x1b[0mat \x1b[35mA\x1b[0ms \x1b[35mA\x1b[0m \x1b[35mS\x1b[0mervice\n");

    printf("\x1b[36mEnter first length: \x1b[33m");
    fgets(buf, sizeof(buf), stdin);
    na = atoi(buf);

    printf("\x1b[36mEnter second length: \x1b[33m");
    fgets(buf, sizeof(buf), stdin);
    nb = atoi(buf);

    if(!na || !nb)
        exit(1);

    printf("\x1b[36mEnter first string: \x1b[33m");
    fgets(a, na, stdin);
    if(a[strlen(a) - 1] == '\n')
        a[strlen(a) - 1] = 0;

    printf("\x1b[36mEnter second string: \x1b[33m");
    fgets(b, nb, stdin);
    if(b[strlen(b) - 1] == '\n')
        b[strlen(b) - 1] = 0;

    printf("\x1b[0m");

    /* Actually, Clemens secretly holds an A* software engineering degree */
    auto void (*f)(char *, char *, unsigned short) = highly_secure_strcat_factory();
    f(a, b, na + nb - 1);

    return 0;
}
