#define _POSIX_C_SOURCE 1
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <openssl/sha.h>

unsigned char const good_hash[SHA256_DIGEST_LENGTH] = {
    0x2a, 0x00, 0x72, 0xd7, 0x6a, 0xd1, 0x94, 0x4f,
    0xfc, 0x1c, 0x94, 0x6a, 0xc6, 0x44, 0xea, 0xb7,
    0x6f, 0x20, 0x29, 0x2c, 0xbe, 0xf3, 0x3c, 0x43,
    0x29, 0x4d, 0x0b, 0xf7, 0xf8, 0x72, 0xef, 0x07,
};

int main()
{
    char buf[0x30] = { 0 };
    unsigned char *hbuf = malloc(0x30);
    unsigned char *hash = malloc(SHA256_DIGEST_LENGTH);

    printf("\x1b[36mEnter username: \x1b[33m"); fflush(stdout);

    SHA256(hbuf, read(fileno(stdin), hbuf, 0x30), hash);

    sleep(1);

    if(strncmp((char *) hash, (char *) good_hash, sizeof(good_hash))) {
        printf("\x1b[31mNope.\x1b[0m\n");
        free(hash);
        free(hbuf);
        exit(1);
    }

    free(hash);
    memcpy(buf, hbuf, sizeof(buf));
    free(hbuf);

    printf("\x1b[32mHey %s\x1b[0m\n", buf);
    printf("Today's magic is \x1b[37m%p\x1b[33m.\n", buf);

    printf("\x1b[36m> \x1b[33m"); fflush(stdout);
    read(fileno(stdin), buf, 0x50);
    printf("\x1b[0m");
    return 0;
}
