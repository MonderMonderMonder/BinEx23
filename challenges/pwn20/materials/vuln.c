#define _DEFAULT_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <dlfcn.h>

char *tmp;
struct stat st;

struct closure {
    int (*f)(char const *);
    char *s;
};

struct closure *call = NULL;
int main()
{
    char buf[80], cmd[0x100], *ptr = buf;
    long fd;

    setbuf(stdout, NULL);

    printf("\x1b[34mWelcome\x1b[0m to the \x1b[36mhouse of %s\x1b[0m!\n", "Polish vodka");

    while (1) {
        if (!fgets(cmd, sizeof(cmd), stdin))
            exit(-1);
        while (*cmd && cmd[strlen(cmd) - 1] == '\n')
            cmd[strlen(cmd) - 1] = 0;
        if (strstr(cmd, "proc"))
            exit(1);
        switch (*strtok(cmd, " ")) {
        case 'm':
            call = malloc(sizeof(call));
            call->f = dlsym(NULL, "puts");
            call->s = buf;
            break;
        case 'e':
            if (!fgets(buf, sizeof(buf), stdin))
                exit(-1);
            break;
        case 'f':
            free(ptr);
            break;
        case 'r':
            tmp = strtok(NULL, " ");
            if (lstat(tmp, &st))
                exit(-1);
            if (S_ISLNK(st.st_mode))
                exit(-1);
            if (0 > (fd = open(tmp, O_RDONLY)))
                exit(-1);
            if (0 > read(fd, buf, sizeof(buf)))
                exit(-1);
            if (0 > printf("%s\n", buf))
                exit(-1);
            fflush(stdout);
            break;
        case 'x':
            if (call)
                call->f(call->s);
            /* fall-through */
        case 'q':
            exit(0);
        }
    }
}
