
#define _DEFAULT_SOURCE
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <signal.h>

#define PORT 13713
#define CHECK(B) do { if (!(B)) exit(1); } while (0)

void get_str(int sock, char *dest, long long size)
{
    /* protip: avoid off-by-one errors by using a buffer twice the size we
     * actually need. because we're lazy. who cares anyway, it's 2017 */
    char buf[16] = { 0 };
    long long len, n;

    dprintf(sock, "\x1b[34mLength (max %lld): \x1b[33m", size);

    CHECK(0 <= (n = recv(sock, buf, 8, 0)));
    len = atoi(buf);

    if (len >= size) {
        dprintf(sock, "\x1b[31mNice try. Goodbye.\x1b[0m\n");
        exit(-1);
    }

    dprintf(sock, "\x1b[34mData: \x1b[33m");
    CHECK(0 <= (n = recv(sock, dest, len, 0)));
    if (dest[strlen(dest) - 1] == '\n')
        dest[strlen(dest) - 1] = 0;
}

long long get_num(int sock)
{
    char buf[0x80];
    get_str(sock, buf, sizeof(buf));
    return strtoull(buf, NULL, 0);
}

void handle(int sock)
{
    FILE *fp;
    char buf[0x80] = { 0 };
    long long r;

    dprintf(sock, "\x1b[36mWhat is your name?\x1b[33m\n");
    get_str(sock, buf, sizeof(buf));
    if (buf[strlen(buf) - 1] == '\n')
        buf[strlen(buf) - 1] = 0;

    CHECK(fp = fopen("/dev/urandom", "r"));
    CHECK(1 == fread(&r, sizeof(r), 1, fp));
    CHECK(!fclose(fp));

    /* protip: 2^64 is A LOT. */
    if (get_num(sock) != r) {
        dprintf(sock, "\x1b[31mNope! You are a failure, %.128s.\x1b[0m\n", buf);
        return;
    }

    dprintf(sock, "\x1b[32mCongratulations %.128s! You win!\n", buf);
    CHECK(fp = popen("get_flag", "r"));
    CHECK(fgets(buf, sizeof(buf), fp));
    dprintf(sock, "\x1b[36mHere is your prize: \x1b[32m%s\x1b[0m", buf);
}

int main()
{
    /* This function contains merely socket handling code and is not intended
     * to contain any vulnerabilities. */

    pid_t pid;
    int lsock, sock, one = 1;
    struct sockaddr_in6 addr = {
        .sin6_family = AF_INET6,
        .sin6_addr = in6addr_any,
        .sin6_port = htons(PORT),
    };
    socklen_t addr_len = sizeof(addr);

    /* ignore dying children */
    /* ^ protip: don't type things like this into $searchengine */
    struct sigaction sigchld = {
        .sa_handler = SIG_DFL,
        .sa_flags = SA_NOCLDWAIT,
    };
    CHECK(!sigaction(SIGCHLD, &sigchld, NULL));

    CHECK(0 <= (lsock = socket(AF_INET6, SOCK_STREAM, 0)));
    CHECK(!setsockopt(lsock, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(sock)));

    CHECK(0 <= bind(lsock, (struct sockaddr *) &addr, sizeof(addr)));
    CHECK(!listen(lsock, 0x10));

    while (1)
    {
        CHECK(0 <= (sock = accept(lsock, (struct sockaddr *) &addr, &addr_len)));
        CHECK(0 <= (pid = fork()));

        if (pid && !close(sock))
            continue;

        /* child */
        close(lsock);
        handle(sock);
        exit(0);
    }

    return 0;
}
