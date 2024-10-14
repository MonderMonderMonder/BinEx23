#include <stdlib.h>
#include <stdio.h>

int main()
{
    char buf[0x20], *ptrs[0x10] = {0};
    size_t n;

    setbuf(stdout, NULL);

    if ('y' ^ *fgets(buf, sizeof(buf), stdin))
        exit(-1);

    printf("\x1b[34mWelcome\x1b[0m to the \x1b[36mhouse of Luke Skywalker\x1b[0m!\n");

    if (!fgets(malloc(0x100), 0x111, stdin))
        exit(-1);

    for (size_t i = 1; i < sizeof(ptrs) / sizeof(*ptrs); ++i) {

        printf("\x1b[35mdo. or do not. there is no try{}.\x1b[0m\n");
        if (!fgets(buf, sizeof(buf), stdin))
            exit(-1);

        n = *buf ? strtoull(buf + 1, NULL, 0) : 0;

        switch (*buf) {
        case 'w':
            if (n != fwrite(ptrs[i] = malloc(n), sizeof(**ptrs), n, stdout))
                exit(-1);
            break;
        case 'r':
            fgets(ptrs[i] = malloc(n), n, stdin);
            break;
        case 'f':
            free(ptrs[n]);
            ptrs[n] = NULL;
            break;
        }
    }
}
