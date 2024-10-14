#define __USE_GNU
#include <stdio.h>
#include <dlfcn.h>
#include <unistd.h>
#include <sys/mman.h>

struct libc_funcs {
	/* Function pointer magic, sorted by descending name length :) */
	char *(*canonicalize_file_name)(const char *);
	int (*mprotect)(void *, size_t, int);
	void (*setbuf)(FILE *, char *);
	void *(*calloc)(size_t, size_t);
	void *(*memcpy)(void *, void *, size_t);
	int (*read)(int, void *, size_t);
	int (*free)(void *);
	int (*puts)(const char *);
};


void *resolve(char *symname)
{
	void *hdl;
	void *sym;
	const char *err;

	hdl = dlopen("libc.so.6", RTLD_NOW);
	if (!hdl)
		goto fail;
	sym = dlsym(hdl, symname);
	err = dlerror();
	dlclose(hdl);

	if (err)
		goto fail;

	return sym;

fail:
	_exit(-1);
}

void lockdown(struct libc_funcs *fs)
{
	int i;
	if (fs->mprotect((void *)((long)resolve & ~0xfff), 0x1000, PROT_WRITE | PROT_EXEC) == -1)
		_exit(-1);
	for (i = 0; i < (void *)lockdown - (void *)resolve; i++)
		*((unsigned char *)(resolve) + i) = 0xc3;
	if (fs->mprotect((void *)((long)resolve & ~0xfff), 0x1000, PROT_READ | PROT_EXEC) == -1)
		_exit(-1);
}

void get_path(struct libc_funcs *fs, char **buf, size_t *len)
{
	char path[0x100];

	fs->puts("\x1b[36mPlease provide your un-canonicalized file path:\x1b[33m");

	*len = fs->read(0, path, 0x1000);
	for (char *p = path + *len - 1; p >= path && *p == '\n'; --p)
		*p = 0;

	/* Do not return a pointer to a local array */
	*buf = fs->calloc(*len, sizeof(*path));
	fs->memcpy(*buf, path, *len);
	return;
}

int main()
{
	struct libc_funcs fs;
	char *path = NULL;
	char *res;
	size_t len = 0;

	fs.canonicalize_file_name = resolve("canonicalize_file_name");
	fs.mprotect = resolve("mprotect");
	fs.setbuf = resolve("setbuf");
	fs.calloc = resolve("calloc");
	fs.memcpy = resolve("memcpy");
	fs.read = resolve("read");
	fs.free = resolve("free");
	fs.puts = resolve("puts");

	fs.setbuf(stdout, NULL);
	fs.puts("\x1b[35mWelcome to CAAS, canonicalization as a service!\x1b[0m");
	fs.puts("\x1b[31mNOTE: Due to hacking attempts, we shrinked down our PLT.\x1b[0m");
	fs.puts("\x1b[31mNOTE**2: Ongoing hacking activity forces us to lock down the resolver.\x1b[0m");

	lockdown(&fs);

	get_path(&fs, &path, &len);

	res = fs.canonicalize_file_name(path);
	if (!res) {
		fs.puts("\x1b[31mHTTP/1.1 404 Not Found.\x1b[0m");
		fs.free(path);
		return -1;
	}

	fs.puts("\x1b[32mHere is your canonicalized file path:\x1b[0m");
	fs.puts(res);

	fs.free(path);
	fs.free(res);

	return 0;
}
