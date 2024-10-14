#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <openssl/sha.h>

unsigned char const good_hash[SHA256_DIGEST_LENGTH] = {
    0x2e, 0x3d, 0x60, 0xbf, 0xed, 0x7a, 0x65, 0x01,
    0x21, 0xe1, 0xd7, 0x15, 0x3a, 0x21, 0x75, 0xc4,
    0xa8, 0x33, 0x7f, 0x01, 0x93, 0x47, 0xc5, 0x2d,
    0x20, 0xdc, 0x9a, 0x8c, 0xb5, 0x09, 0xe7, 0xa3,
};
char const salt[] = {
    0x64, 0x75, 0x63, 0x6b,
    0x64, 0x75, 0x63, 0x6b,
    0x64, 0x75, 0x63, 0x6b,
    0x64, 0x75, 0x63, 0x6b
};
unsigned char hash[SHA256_DIGEST_LENGTH];

void win()
{
    execl("/bin/get_flag", "get_flag", NULL);
    exit(1);
}

int main()
{
    char salted[sizeof(salt) + 40] = {0};
    char buf[0x40];
    long uid = 1; /* user */

    printf("\x1b[36mEnter password: \x1b[33m"); fflush(stdout);
    fgets(buf, sizeof(buf), stdin);

    memcpy(salted, salt, sizeof(salt));
    strncat(salted + sizeof(salt), buf, sizeof(salted) - sizeof(salt));

    SHA256((unsigned char *) salted, sizeof(salt) + strlen(buf), hash);

    if (!memcmp(hash, good_hash, sizeof(good_hash)))
        uid = 0; /* root */

    sleep(1);

    if (uid) {
        printf("\x1b[31mNope.\x1b[0m\n");
        exit(1);
    }

    printf("\x1b[32mHey root!\x1b[0m\n");

    printf("\x1b[36m> \x1b[33m"); fflush(stdout);
    gets(buf);
    printf("\x1b[0m");
    return 0;
}
