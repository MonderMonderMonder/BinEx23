#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <openssl/sha.h>



#define DATASTRUCT_OVERHEAD sizeof(struct Data)
#define CKSUM_LENGTH SHA256_DIGEST_LENGTH

#define NUM_FIELDS 0x1000

struct Data
{
	unsigned char cksum[CKSUM_LENGTH];
	size_t length;
	char data[];
};


struct Data* fields[NUM_FIELDS];

unsigned long getlong() {
	char buf[0x20];
	char *tmp;
	unsigned long ret;
	ssize_t len;
	len = read(0, buf, sizeof(buf) - 1);
	if (len <= 0)
		exit(0);
	buf[len] = 0;
	ret = strtoul(buf, &tmp, 0);
	if (tmp == buf) {
		puts("Invalid Number");
		exit(1);
	}
	return ret;
}

void* salloc(size_t amount) {
	void* r = malloc(amount);
	if(!r)
		return r;
	memset(r, 0, amount);
	return r;
}

void compute_cksum(char* data, size_t length, unsigned char* cksum)
{
    SHA256((unsigned char *) data, length, cksum);
}

void assert_cksum(struct Data* data)
{
	unsigned char cksum[CKSUM_LENGTH];

	compute_cksum(data->data, data->length, cksum);

	int r = 0;
	for(int i = 0; i < CKSUM_LENGTH; ++i)
		r |= cksum[i] ^ data->cksum[i];

	if(r)
		_exit(0);
}

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

void store() {
	size_t amount;
	printf("How much would you like to store? ");
	amount = getlong();

	/* Check overflow */
	if(amount + DATASTRUCT_OVERHEAD < amount)
	{
		puts("Amount too high!");
		return;
	}

	size_t alloc_amount = amount + DATASTRUCT_OVERHEAD;

	printf("Where? ");
	size_t index;
	index = getlong();
	if(index >= NUM_FIELDS)
	{
		printf("Index out of bounds! Max is: %d\n", NUM_FIELDS);
		return;
	}

	if(fields[index])
	{
		puts("Index already taken!");
		return;
	}

	struct Data* ptr = fields[index] = salloc(alloc_amount);
	if(!ptr)
	{
		puts("Not enough memory...");
		return;
	}

	ptr->length = amount;

	puts("Gimme data...");

	read(0, ptr->data, amount);
	compute_cksum(ptr->data, ptr->length, ptr->cksum);
	assert_cksum(ptr);
	puts("Data received!");
}

void release() {
	unsigned long index;
	printf("Index: ");
	index = getlong();

	if (index >= NUM_FIELDS)
	{
		printf("Index out of bounds! Max is: %d\n", NUM_FIELDS);
		return;
	}

	if (fields[index] == NULL) {
		puts("Index already free!");
		return;
	}

	assert_cksum(fields[index]);
	free(fields[index]);
	fields[index] = NULL;
}

void edit() {
	unsigned long index;
	unsigned long len;
	printf("Index: ");
	index = getlong();
	if (index >= NUM_FIELDS || fields[index] == NULL)
	{
		puts("Index is not allocated!");
		return;
	}

	printf("Length: ");
	len = getlong();
	struct Data* ptr = fields[index];
	assert_cksum(ptr);
	ptr->length = len;

	puts("Gimme data...");

	read(STDIN_FILENO, ptr->data, len);
	compute_cksum(ptr->data, ptr->length, ptr->cksum);
	assert_cksum(ptr);
	puts("Data received!");
}

void doread () {
	unsigned long index;
	printf("Index: ");
	index = getlong();

	if (index >= NUM_FIELDS || fields[index] == NULL)
	{
		puts("Index is not allocated!");
		return;
	}

	struct Data* ptr = fields[index];
	write(1, ptr->data, ptr->length);
	puts("");
}




void print_menu() {
	puts("-------------------------------------------");
	puts(" \033[1;31m1. Store");
	puts(" \033[1;34m2. Edit");
	puts(" \033[1;36m3. Read");
	puts(" \033[1;32m4. Release\033[0;37m");
	puts("-------------------------------------------");
	printf("> ");
}



int main () {
	unsigned long choice;
	init();
	puts("Welcome to \033[1;30mR\033[0;37memote \033[1;31mS\033[0;37mtorage \033[1;32mA\033[0;37ms \033[1;33mA\033[0;37m \033[1;34mS\033[0;37mervice");
	while(1) {
		print_menu();
		choice = getlong();
		switch (choice) {
			default:
				puts("Invalid Choice");
				break;
			case 1:
				store();
				break;
			case 2:
				edit();
				break;
			case 3:
				doread();
				break;
			case 4:
				release();
				break;
		}
	}
}

