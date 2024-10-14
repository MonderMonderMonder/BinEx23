#define _DEFAULT_SOURCE
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <pthread.h>

#define xmalloc malloc
#define xrealloc realloc
#define BUFSIZE 512

/* GNUCODE (slightly modified to remove some bloat and make it work with stdin */

#define xfree(p) do { free ((void *) (p)); p = NULL; } while (0)

/* Find the minimum or maximum of two provided values */
# define MIN(i, j) ((i) <= (j) ? (i) : (j))
# define MAX(i, j) ((i) >= (j) ? (i) : (j))

typedef const char *(*hunk_terminator_t) (const char *, const char *, int);
#define FD_READ_LINE_MAX 4096

/* Basic socket operations, mostly EINTR wrappers.  */

static int
sock_read (int fd, char *buf, int bufsize)
{
  int res;
  do
    res = read (fd, buf, bufsize);
  while (res == -1 && errno == EINTR);
  return res;
}

/* Read no more than BUFSIZE bytes of data from FD, storing them to
   BUF.  If TIMEOUT is non-zero, the operation aborts if no data is
   received after that many seconds.  If TIMEOUT is -1, the value of
   opt.timeout is used for TIMEOUT.  */

int
fd_read (int fd, char *buf, int bufsize, double timeout)
{
  /* redacted >>
  struct transport_info *info;
  LAZY_RETRIEVE_INFO (info);
  if (!poll_internal (fd, info, WAIT_FOR_READ, timeout))
    return -1;
  if (info && info->imp->reader)
    return info->imp->reader (fd, buf, bufsize, info->ctx);
  else
  << redacted */
    return sock_read (fd, buf, bufsize);
}

static const char *
line_terminator (const char *start /*_GL_UNUSED*/, const char *peeked, int peeklen)
{
  const char *p = memchr (peeked, '\n', peeklen);
  if (p)
    /* p+1 because the line must include '\n' */
    return p + 1;
  return NULL;
}

/* Read a hunk of data from FD, up until a terminator.  The hunk is
   limited by whatever the TERMINATOR callback chooses as its
   terminator.  For example, if terminator stops at newline, the hunk
   will consist of a line of data; if terminator stops at two
   newlines, it can be used to read the head of an HTTP response.
   Upon determining the boundary, the function returns the data (up to
   the terminator) in malloc-allocated storage.

   In case of read error, NULL is returned.  In case of EOF and no
   data read, NULL is returned and errno set to 0.  In case of having
   read some data, but encountering EOF before seeing the terminator,
   the data that has been read is returned, but it will (obviously)
   not contain the terminator.

   The TERMINATOR function is called with three arguments: the
   beginning of the data read so far, the beginning of the current
   block of peeked-at data, and the length of the current block.
   Depending on its needs, the function is free to choose whether to
   analyze all data or just the newly arrived data.  If TERMINATOR
   returns NULL, it means that the terminator has not been seen.
   Otherwise it should return a pointer to the charactre immediately
   following the terminator.

   The idea is to be able to read a line of input, or otherwise a hunk
   of text, such as the head of an HTTP request, without crossing the
   boundary, so that the next call to fd_read etc. reads the data
   after the hunk.  To achieve that, this function does the following:

   1. Peek at incoming data.

   2. Determine whether the peeked data, along with the previously
      read data, includes the terminator.

      2a. If yes, read the data until the end of the terminator, and
          exit.

      2b. If no, read the peeked data and goto 1.

   The function is careful to assume as little as possible about the
   implementation of peeking.  For example, every peek is followed by
   a read.  If the read returns a different amount of data, the
   process is retried until all data arrives safely.

   SIZEHINT is the buffer size sufficient to hold all the data in the
   typical case (it is used as the initial buffer size).  MAXSIZE is
   the maximum amount of memory this function is allowed to allocate,
   or 0 if no upper limit is to be enforced.

   This function should be used as a building block for other
   functions -- see fd_read_line as a simple example.  */

char *
fd_read_hunk (int fd, hunk_terminator_t terminator, long sizehint, long maxsize)
{
  long bufsize = sizehint;
  char *hunk = xmalloc (bufsize);
  int tail = 0;                 /* tail position in HUNK */

  assert (!maxsize || maxsize >= bufsize);

  while (1)
    {
      const char *end;
      int pklen, rdlen, remain;

      /* First, peek at the available data. */
	  /* peeking on stdin sadly does not work.
	  pklen = fd_peek (fd, hunk + tail, bufsize - 1 - tail, -1);
	  */
	  pklen = fd_read(fd, hunk + tail, 1, -1);
      if (pklen < 0)
        {
          xfree (hunk);
          return NULL;
        }
      end = terminator (hunk, hunk + tail, pklen);
      if (end)
        {
          /* The data contains the terminator: we'll drain the data up
             to the end of the terminator.  */
		  // remain = end - (hunk + tail);
		  remain = end - (hunk + tail + pklen);
          assert (remain >= 0);
          if (remain == 0)
            {
              /* No more data needs to be read. */
              hunk[tail] = '\0';
              return hunk;
            }
          if (bufsize - 1 < tail + remain)
            {
              bufsize = tail + remain + 1;
              hunk = xrealloc (hunk, bufsize);
            }
        }
      else
        /* No terminator: simply read the data we know is (or should
           be) available.  */
        remain = pklen;

      /* Now, read the data.  Note that we make no assumptions about
         how much data we'll get.  (Some TCP stacks are notorious for
         read returning less data than the previous MSG_PEEK.)  */

	  // we did not peek so we dont read now
	  // rdlen = fd_read (fd, hunk + tail, remain, 0);
	  rdlen = pklen;
      if (rdlen < 0)
        {
          xfree (hunk);
          return NULL;
        }
      tail += rdlen;
      hunk[tail] = '\0';

      if (rdlen == 0)
        {
          if (tail == 0)
            {
              /* EOF without anything having been read */
              xfree (hunk);
              errno = 0;
              return NULL;
            }
          else
            /* EOF seen: return the data we've read. */
            return hunk;
        }
      if (end && rdlen == remain)
        /* The terminator was seen and the remaining data drained --
           we got what we came for.  */
        return hunk;

      /* Keep looping until all the data arrives. */

      if (tail == bufsize - 1)
        {
          /* Double the buffer size, but refuse to allocate more than
             MAXSIZE bytes.  */
          if (maxsize && bufsize >= maxsize)
            {
              xfree (hunk);
              errno = ENOMEM;
              return NULL;
            }
          bufsize <<= 1;
          if (maxsize && bufsize > maxsize)
            bufsize = maxsize;
          hunk = xrealloc (hunk, bufsize);
        }
    }
}

/* Read one line from FD and return it.  The line is allocated using
   malloc, but is never larger than FD_READ_LINE_MAX.

   If an error occurs, or if no data can be read, NULL is returned.
   In the former case errno indicates the error condition, and in the
   latter case, errno is NULL.  */

char *
fd_read_line (int fd)
{
  return fd_read_hunk (fd, line_terminator, 128, FD_READ_LINE_MAX);
}


/* Pick an integer type large enough for file sizes, content lengths,
   and such.  Because today's files can be very large, it should be a
   signed integer at least 64 bits wide.  This can't be typedeffed to
   off_t because: a) off_t is always 32-bit on Windows, and b) we
   don't necessarily want to tie having a 64-bit type for internal
   calculations to having LFS support.  */

#ifdef WINDOWS
  /* nothing to do, see mswindows.h */
#elif SIZEOF_LONG >= 8
  /* long is large enough, so use it. */
  typedef long wgint;
# define SIZEOF_WGINT SIZEOF_LONG
#elif SIZEOF_LONG_LONG >= 8
  /* long long is large enough and available, use that */
  typedef long long wgint;
# define SIZEOF_WGINT SIZEOF_LONG_LONG
#elif HAVE_INT64_T
  typedef int64_t wgint;
# define SIZEOF_WGINT 8
#elif SIZEOF_OFF_T >= 8
  /* In case off_t is typedeffed to a large non-standard type that our
     tests don't find. */
  typedef off_t wgint;
# define SIZEOF_WGINT SIZEOF_OFF_T
#else
  /* Fall back to using long, which is always available and in most
     cases large enough. */
  typedef long wgint;
# define SIZEOF_WGINT SIZEOF_LONG
#endif

/* !GNUCODE */

char mot[8][50] = {"stronger", "faster", "better", "harder", "more", "now", "like it was a steak", "https://youtu.be/ErVZr2DEiss"};

void *motivate(void * dat) {
    char buf[BUFSIZE];
    char *line;
    wgint len, used, rdlen;

    memset(buf, 0, sizeof(buf));
    used = sprintf(buf, "Hack %s, ", mot[rand()%8]);

    printf("Welcome to \033[1;34mM\033[0;37motivational \033[1;35mS\033[0;37mentences \033[1;31ma\033[0;37ms \033[1;33ma\033[0;37m \033[1;32mS\033[0;37mervice (\033[1;34mM\033[1;35mS\033[1;31ma\033[1;33ma\033[1;32mS\033[0;37m)\nHow long is your name?\n");
    line = fd_read_line(fileno(stdin));
    len = strtol(line, NULL, 0);
    xfree(line);

    printf("What is your name?\n");
    rdlen = MIN (BUFSIZE - 1 - used, len);

    if (rdlen == 0) {
        line = fd_read_line(fileno(stdin));
        xfree(line);
        return NULL;
    }

    fd_read(fileno(stdin), buf + used, rdlen, 0);
    write(fileno(stdout), buf, strlen(buf));
    return NULL;
}


/*
 * New worker / manager thread design scheme
 * The worker does everything while the manager relaxes
 */
int main(int argc, char **argv) {
    pthread_t thr;
    void *retval;

    setbuf(stdin, NULL);
    setbuf(stdout, NULL);
    srand(time(NULL));

    if (pthread_create(&thr, NULL, motivate, NULL)) {
        printf("ERROR on pthread create\n");
        return 1;
    }
    pthread_join(thr, &retval);

    return 0;
}
