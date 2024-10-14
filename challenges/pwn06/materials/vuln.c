#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <smmintrin.h>

#define BUFSIZE 0x20

void init() {
    setbuf(stdout, NULL);
    setbuf(stdin, NULL);
}

int main(int argc, char **argv) {
    char *buf;
    int i, num;
    uint32_t *array;
    uint32_t vala = 0;
    uint32_t valb = 0;
    FILE *f;

    init();
    buf = malloc(BUFSIZE);

    puts("Welcome to the unrigable(TM) random number generator");

    printf("How many random numbers are there? [0,255]: ");
    fgets(buf, BUFSIZE, stdin);
    i = atoi(buf);

    num = i & 0xFF;

    array = calloc(num + 1, sizeof(*array));
    
    f = fopen("/dev/urandom", "r");
    fread(array, sizeof(*array), num, f);
    fclose(f);
    
    for (int j = 0, i = 0; j < num; j++) {
        if (array[j] < 0x1337134D) i++;
        vala ^= array[j];
    }

    printf("Put in the number you want to manipulate: ");
    fgets(buf, BUFSIZE, stdin);
    array[i] = atoi(buf);

    for (int j = 0; j < num; j++) {
        valb ^= array[j];
    }

    printf("original #set bits: %u\nmanipulated #set bits: %u\n", _mm_popcnt_u32(vala), _mm_popcnt_u32(valb));
    puts("see, it is unrigable!");
    
    free(buf);
    free(array);

    return 0;
}
