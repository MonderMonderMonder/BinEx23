#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <openssl/sha.h>

unsigned char hash[SHA256_DIGEST_LENGTH];
unsigned char const good_hash[SHA256_DIGEST_LENGTH] = {
    0xf1, 0x26, 0xd5, 0xd9, 0x48, 0x1b, 0x3b, 0x2b,
    0x2a, 0x03, 0xf0, 0xa7, 0x75, 0x4e, 0xf1, 0x7d,
    0x01, 0x3d, 0x4a, 0x67, 0x99, 0x7b, 0x0a, 0x48,
    0x93, 0x08, 0x98, 0x71, 0x26, 0xc0, 0x90, 0x3c,
};

int main()
{
    char buf[40] = { 0 };
    long is_admin = 0;

    printf("\x1b[36mEnter password: \x1b[33m"); fflush(stdout);
    scanf("%40s", buf);
    if(buf[strlen(buf) - 1] != '\n')
        strcat(buf, "\n");

    SHA256((unsigned char *) buf, strlen(buf), hash);

    if(!memcmp(hash, good_hash, sizeof(good_hash)))
        is_admin = 1;

    sleep(1);

    if(!is_admin) {
        printf("\x1b[31mNope.\x1b[0m\n");
        exit(1);
    }

    printf("\x1b[32mHey admin!\x1b[0m\n");

    printf("\x1b[36m> \x1b[33m"); fflush(stdout);
    scanf("%s", buf);
    printf("\x1b[0m");
    return 0;
}
