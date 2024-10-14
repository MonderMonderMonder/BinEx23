#define _DEFAULT_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>

struct node {
    bool is_ptr;
    union {
        char *ptr;
        char imm[160];
    } data;
    struct node *next;
};

char linkin_park[] = "in the end, it doesn't even matter";
char buf[0x1000];

void get(char *p, size_t l, FILE *fp)
{
    size_t i;
    for (i = 0; i < l; ++i) {
        int c = getc(fp);
        if (c < 0 || c == '\n')
            break;
        p[i] = c;
    }
    p[i] = 0;
}

char flag[0x100];
void get_flag()
{
    FILE *fp;
    if (!(fp = popen("get_flag", "r")))
        exit(-1);
    get(flag, sizeof(flag), fp);
    pclose(fp);
}

int main()
{
    size_t n;
    char tmp[42];

    get_flag();

    setbuf(stdout, NULL);

    printf("\x1b[36mEnter your number: \x1b[33m"); fflush(stdout);
    get(tmp, 0x10, stdin);
    if (0x400 < (n = strtoull(tmp, NULL, 0))) exit(1);

    struct node *head = (void *) (buf + n);

    /* two elements is a linked list :-) */
    (head->next = (void *) ((char *) head + sizeof(*head)))->next = NULL;

    head->next->is_ptr = true;
    head->next->data.ptr = linkin_park;

    printf("\x1b[36mEnter your string: \x1b[33m"); fflush(stdout);
    head->is_ptr = false;
    get(head->data.imm, sizeof(head->data.imm), stdin);

    printf("\x1b[34m");
    for (struct node *cur = head; cur; cur = cur->next)
        puts(cur->is_ptr ? cur->data.ptr : cur->data.imm);
    printf("\x1b[0m");

    return 0;
}
