
#define _DEFAULT_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <unistd.h>
#include <sys/prctl.h>
#include <sys/fcntl.h>
#include <linux/seccomp.h>
#include <linux/filter.h>
#include <linux/audit.h>
#include <sys/syscall.h>

#define CHECK(B) do { if(!(B)) exit(1); } while(0)
#define FDMASK 0x3ff
#define BUFSZ 512

void fun(size_t len)
{
    char buf[BUFSZ];
    CHECK(0 <= read(fileno(stdin), buf, len));
}

int flagfd, outfd;

int main()
{
    int init_seccomp();

    char buf[8];
    int randfd;
    FILE *flagfp;

    setbuf(stdout, NULL);

    printf("\x1b[36mToday's special: Free info leaks!\x1b[0m ");
    printf("\x1b[35mstderr = %p.\x1b[0m\n", (void *) stderr);

    CHECK(0 <= (randfd = open("/dev/urandom", O_RDONLY)));
    CHECK(sizeof(flagfd) == read(randfd, &flagfd, sizeof(flagfd)));
    CHECK(sizeof(outfd) == read(randfd, &outfd, sizeof(outfd)));
    CHECK(!close(randfd));

    CHECK(flagfp = popen("get_flag", "r"));
    CHECK(0 > close(flagfd &= FDMASK) && 0 <= dup2(fileno(flagfp), flagfd));
    CHECK(0 > close(outfd &= FDMASK) && 0 <= dup2(fileno(stdout), outfd));

    printf("\x1b[36mEnter length: \x1b[33m"); fflush(stdout);
    CHECK(fgets(buf, sizeof(buf), stdin));
    printf("\x1b[0m");
    if((size_t) strtoull/*RTFM*/(buf, NULL, 0) >= BUFSZ)
        exit(1);

    CHECK(!pclose(flagfp));
    CHECK(!fclose(stdout));
    CHECK(!fclose(stderr));

    CHECK(!init_seccomp());

    fun(atoll(buf));
}

int init_seccomp()
{
#define ALLOW(NR) \
    BPF_JUMP(BPF_JMP + BPF_JEQ + BPF_K, (NR), 0, 1), \
    BPF_STMT(BPF_RET + BPF_K, SECCOMP_RET_ALLOW) \

    struct sock_filter filter[] = {
        BPF_STMT(BPF_LD + BPF_W + BPF_ABS, offsetof(struct seccomp_data, arch)),
        BPF_JUMP(BPF_JMP + BPF_JEQ + BPF_K, AUDIT_ARCH_X86_64, 1, 0),
        BPF_STMT(BPF_RET + BPF_K, SECCOMP_RET_KILL),

        BPF_STMT(BPF_LD + BPF_W + BPF_ABS, offsetof(struct seccomp_data, nr)),
        ALLOW(SYS_read),
        ALLOW(SYS_write),
        ALLOW(SYS_exit_group),

        BPF_STMT(BPF_RET + BPF_K, SECCOMP_RET_KILL),
    };
#undef ALLOW

    struct sock_fprog prog = {
        .len = sizeof(filter) / sizeof(*filter),
        .filter = filter,
    };

    return prctl(PR_SET_NO_NEW_PRIVS, 1, 0, 0, 0) || prctl(PR_SET_SECCOMP, SECCOMP_MODE_FILTER, &prog);
}

