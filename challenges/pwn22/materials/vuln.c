#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

typedef struct note_s {
    char *text;
    unsigned long len;
    struct note_s *next;
} note;

note notes[100];
note *free_notes = notes;
note *back = &notes[100];

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

unsigned long readlong() {
    char buf[40];
    char *tmp;
    unsigned long ret;
    ssize_t len;
    len = read(0, buf, sizeof(buf) - 1);
    if (len <= 0)
        exit(0);
    buf[len] = 0;
    ret = strtoul(buf, &tmp, 0);
    if (tmp == buf) {
        puts("invalid");
        exit(1);
    }
    return ret;
}

void print_menu() {
    puts("-------------------------------------------");
    puts(" \033[1;31m1. allocate");
    puts(" \033[1;32m2. free");
    puts(" \033[1;34m3. fill");
    puts(" \033[1;36m4. dump\033[0;37m");
    puts("-------------------------------------------");
    printf("> ");
}

void allocate() {
    unsigned long size;
    printf("What size to allocate? ");
    size = readlong();
    // if you want to write more than a page you should write a book instead
    if (size > 0x1000) size = 0x1000;
    if (free_notes != back) {
        free_notes->text = calloc(1, size);

        if (free_notes->text == NULL)
            exit(1);

        free_notes->len = size;
        printf("Allocated note #%lu\n", (((uintptr_t) free_notes) - ((uintptr_t) notes)) / sizeof(note));

        if (free_notes->next == NULL)
            free_notes++;
        else
            free_notes = free_notes->next;
    }
}

void free_buf() {
    unsigned long index;
    printf("Index: ");
    index = readlong();

    if (index >= 100 || notes[index].text == NULL) {
        puts("invalid");
        exit(1);
    }

    free(notes[index].text);
    notes[index].text = NULL;
    notes[index].next = free_notes;
    free_notes = &notes[index];
}

void fill () {
    unsigned long index;
    unsigned long len;
    printf("Index: ");
    index = readlong();

    if (index > 100 || notes[index].text == NULL) {
        puts("invalid");
        exit(1);
    }

    printf("Length: ");
    len = readlong();
    read(0, notes[index].text, len);
}

void dump () {
    unsigned long index;
    printf("Index: ");
    index = readlong();

    if (index > 100 || notes[index].text == NULL) {
        puts("invalid");
        exit(1);
    }

    write(1, notes[index].text, notes[index].len);
    puts("");
}

int main () {
    unsigned long choice;
    init();
    puts("Welcome to \033[1;30mN\033[0;37mote \033[1;31mT\033[0;37making \033[1;32mA\033[0;37ms \033[1;33mA\033[0;37m \033[1;34mS\033[0;37mervice");
    while(1) {
        print_menu();
        choice = readlong();
        switch (choice) {
            default:
                puts("Invalid Choice");
                break;
            case 1:
                allocate();
                break;
            case 2:
                free_buf();
                break;
            case 3:
                fill();
                break;
            case 4:
                dump();
                break;
        }
    }
}
