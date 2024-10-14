
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
#include <sys/mman.h>
#include <string.h>

#define CHECK(B) do { if(!(B)) exit(1); } while(0)

char secret[0x100];
int init_seccomp();

void init() {
    int fd;
    // setting stdout and stdin to be unbuffered
    setvbuf(stdout,0,_IONBF,0);
    setvbuf(stdin,0,_IONBF,0);
    // load the secret
    fd = open("secret.txt", O_RDONLY);
    read(fd, &secret, sizeof(secret));
    close(fd);
}

int main()
{
    char *code;

    init();

    printf("Welcome to \033[1;32mRCEaaS\033[0m\n");
    
    code = mmap(0, 0x1000, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_PRIVATE | MAP_ANON, -1, 0);
    read(0, code, 0x1000);

    if (strcmp(secret, code)) {
        CHECK(!fclose(stdout));
        CHECK(!fclose(stderr));
        CHECK(!init_seccomp());
    } else
        printf("You seem trustworthy!\n");


    ((void (*)(void)) code) ();
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

