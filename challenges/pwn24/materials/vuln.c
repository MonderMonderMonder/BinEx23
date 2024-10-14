#define _DEFAULT_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define KEYSZ 0x20
#define VALSZ 0x400

struct keyval
{
    char key[KEYSZ];
    size_t len;
    char val[];
};

struct keyval **kvs = NULL;
size_t cap = 0, num = 0;
char key[KEYSZ], val[VALSZ];

void nope()
{
    printf("\x1b[31mNope.\x1b[0m\n");
    exit(1);
}

size_t get(char const *what, char *buf, size_t len)
{
    size_t n;
    printf("\x1b[34m Enter %s: \x1b[33m", what);
    if(0 >= (n = read(0, buf, len - 1)))
        exit(-1);
    printf("\x1b[0m");
    buf[n] = 0;
    while(n && buf[n - 1] == '\n')
        buf[--n] = 0;
    return n;
}

#define getkey(BUF) get("key", (BUF), KEYSZ)
#define getval(BUF) get("value", (BUF), VALSZ)

/* linear search, f_ck yeah! */
struct keyval *lookup(char const *key, size_t *idx)
{
    for(size_t i = 0; i < num; ++i)
        if(!strcmp(key, kvs[i]->key))
            return kvs[idx ? (*idx = i) : i];
    return NULL;
}

void add()
{
    size_t len;
    getkey(key);
    if(lookup(key, NULL))
        nope();
    if(num++ >= cap)
        kvs = realloc(kvs, sizeof(*kvs) * (cap = num));

    len = getval(val);
    kvs[num - 1] = malloc(sizeof(**kvs) + len++);
    kvs[num - 1]->len = len;
    strcpy(kvs[num - 1]->key, key);
    strcpy(kvs[num - 1]->val, val);
}

void edit()
{
    size_t len;
    getkey(key);
    struct keyval *kv = lookup(key, NULL);
    if(!kv)
        nope();
    len = getval(val);
    if(len > kv->len)
        nope();
    strcpy(kv->val, val);
}

void print()
{
    getkey(key);
    struct keyval *kv = lookup(key, NULL);
    if(!kv)
        nope();
    printf("\x1b[32m%s\x1b[0m\n", kv->val);
}

void memzero(void *p, size_t n)
{
    static FILE *fp = NULL;
    if(!fp) {
        if(!(fp = fopen("/dev/zero", "r")))
            exit(-1);
    }
    if(n != fread(p, 1, n, fp))
        exit(-1);
}

void del(int safe)
{
    size_t idx;
    getkey(key);
    struct keyval *kv = lookup(key, &idx);
    if(!kv) {
        memzero(key, sizeof(key));
        return;
    }
    if(safe) {
        memzero(kv->key, sizeof(kv->key));
        memzero(kv->val, kv->len);
    }
    free(kv);
    memmove(kvs + idx, kvs + idx + 1, (--num - idx) * sizeof(*kvs));
}

int main()
{
    char cmd[2];

    setbuf(stdout, NULL);

    printf("\x1b[35mToday's special: Enjoy our key-value store's free\x1b[30m*\x1b[35m trial!\x1b[0m\n");
    printf("\x1b[34mWe promise to keep your data \x1b[30mun\x1b[34msafe. Trust us. Give us your files. Now. All of them!\x1b[0m\n");

    while(1) {
        printf("\x1b[36mCommand? \x1b[33m");
        if(0 >= read(0, cmd, sizeof(cmd)))
            exit(1);
        printf("\x1b[0m");
        switch(*cmd) {
        case 'a':
            add();
            break;
        case 'e':
            edit();
            break;
        case 'p':
            print();
            break;
        case 'd':
        case 'D':
            del(!(*cmd & ' '));
            break;
        default:
            /* fall through */
        case 'q':
            exit(0);
        }
    }
}
