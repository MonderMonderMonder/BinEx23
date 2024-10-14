#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

typedef struct list_s{
    struct list_s *next;
    unsigned long size;
    int values[];
} list;

list *head = NULL;

void init() {
    int fd;
    unsigned int rand_val;
    // setting stdout and stdin to be unbuffered
    setvbuf(stdout,0,_IONBF,0);
    setvbuf(stdin,0,_IONBF,0);
    // seeding the random number generator
    fd = open("/dev/urandom", O_RDONLY);
    read(fd, &rand_val, sizeof(rand_val));
    close(fd);
    srand(rand_val);
}

void print_menu() {
    puts("---------------------------");
    puts("  0. Add List");
    puts("  1. Edit List");
    puts("  2. Print List");
    puts("  3. Exit");
    puts("---------------------------");
    printf("> ");
}

int get_int() {
    char buf[0x40];

    if (!fgets(buf, sizeof(buf), stdin)) {
        puts("Error while reading");
        exit(1);
    }

    return atoi(buf);
}

void add_list() {
    int size;
    list *ele;

    printf("Number of Integers: ");
    size = get_int();
    if (size < 0) {
        puts("Invalid number of Integers!");
        exit(1);
    }

    ele = (list *) calloc(1, sizeof(list) + size * 4);
    ele->size = size;
    ele->next = head;
    head = ele;
}

void print_overfew() {
    list *tmp = head;
    for (int i = 0; tmp; i++, tmp = tmp->next)
        if (tmp->size > 9000)
            printf(" %3d) \033[1;31mIt's over nine thousand\033[0;37m\n", i);
        else
            printf(" %3d) %10lu Elements\n", i, tmp->size);
}

list *get_list() {
    int index;
    list *lst = head;

    printf("Lists: ");
    print_overfew();
    printf("> ");

    index = get_int();
    if (index < 0) {
        puts("Invalid index");
        exit(1);
    }

    for (; index > 0; index --) {
        if (lst->next == NULL) {
            puts("Invalid index");
            exit(1);
        }
        lst = lst->next;
    }

    return lst;
}

void edit_list() {
    int begin, end;
    list *lst;

    lst = get_list();
    if (!lst) {
        puts("List is empty.");
        return;
    }

    printf("begin [0..%ld]: ", lst->size);
    begin = get_int();
    printf("end [%d..%ld]: ", begin, lst->size);
    end = get_int();

    if (begin < 0 || end < begin || end > lst->size) {
        puts("Invalid range");
        exit(1);
    }

    for (; begin < end; begin ++) {
        lst->values[begin] = get_int();
    }
}

void print_int(int i) {
    if (i < 0)
        printf("\033[0;31m%#10x", i);
    else if (i < 0x31)
        printf("\033[1;36m%#10x", i);
    else if (i < 0x7f)
        printf("\033[1;35m%#10x", i);
    else if (i == 0x1337 || i == 0xdead || i == 0xbeef)
        printf("\033[1;32m%#10x", i);
    else
        printf("\033[0;39m%#10x", i);
}

void print_list() {
    int begin, end;
    list *lst;

    lst = get_list();
    if (!lst) {
        puts("List is empty.");
        return;
    }

    printf("begin [0..%ld]: ", lst->size);
    begin = get_int();
    printf("end [%d..%ld]: ", begin, lst->size);
    end = get_int();

    if (begin < 0 || end < begin || end > lst->size) {
        puts("Invalid range");
        exit(1);
    }

    for (; begin < end; begin ++) {
        print_int(lst->values[begin]);
    }
    puts("\033[0;39m");
}

int main() {
    int choice;
    puts("Integer Storage as a Service");
    init();

    while(1) {
        print_menu();
        choice = get_int();
        switch (choice) {
            case 0:
                add_list();
                break;
            case 1:
                edit_list();
                break;
            case 2:
                print_list();
                break;
            case 3:
                return 0;
                break;
            default:
                puts("Invalid choice");
                break;
        }
    }
}

