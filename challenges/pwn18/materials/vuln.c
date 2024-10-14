
#define _DEFAULT_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include <unistd.h>
#include <sys/prctl.h>
#include <sys/fcntl.h>
#include <linux/seccomp.h>
#include <linux/filter.h>
#include <linux/audit.h>
#include <sys/syscall.h>

#define CHECK(B) do { if(!(B)) exit(1); } while(0)

void fun()
{
    char buf[1];
    CHECK(0 <= read(fileno(stdin), buf, 0x400));
}

void more()
{
    char buf[0x400] = {0};
    fun();
}

int check_password()
{
    int fd, ret;
    static char pwd[0x100];
    char *inp;
    ssize_t n;
    size_t k = 0;

    CHECK(0 <= (fd = open("password.txt", O_RDONLY)));
    CHECK(0 <= (n = read(fd, pwd, sizeof(pwd) - 1)) && !(pwd[n] = 0));
    if(n && pwd[n - 1] == '\n')
        pwd[--n] = 0;
    CHECK(!close(fd));

    printf("\x1b[36mPassword: \x1b[33m"); fflush(stdout);
    CHECK(0 <= (n = getline(&inp, &k, stdin)));
    printf("\x1b[0m");
    if(n && inp[n - 1] == '\n')
        inp[--n] = 0;

    ret = !strcmp(inp, pwd);

    free(inp);
    return ret;
}

int main()
{
    int init_seccomp();

    setbuf(stdout, NULL);

    if(check_password())
        CHECK(!execl("/bin/get_flag", "get_flag", NULL));

    printf("\x1b[36mYesterday's special: Free info leaks!\x1b[0m ");
    printf("\x1b[35mstderr = %p.\x1b[0m\n", (void *) stderr);

    CHECK(!init_seccomp());

    more();
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
        ALLOW(SYS_clock_nanosleep),
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

