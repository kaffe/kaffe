/*
 * syscalls.c
 *
 * Copyright (c) 1996, 1997
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

#include "config.h"
#include "debug.h"
#include "config-std.h"
#include "config-mem.h"
#include "config-io.h"
#include "config-signal.h"
#include "gtypes.h"
#include "jsyscall.h"
#include "jsignal.h"
#include "lock-impl.h"

#include <errno.h>

#if defined(HAVE_SYS_TYPES_H)
#include <sys/types.h>
#endif /* defined(HAVE_SYS_TYPES_H) */

#if defined(HAVE_SYS_WAIT_H)
#include <sys/wait.h>
#endif

/*
 * various building blocks for timeout system call functions
 */
#define SET_DEADLINE(deadline, timeout) 		\
	if (timeout != NOTIMEOUT) {			\
		deadline = timeout + currentTime();	\
	}

#define BREAK_IF_LATE(deadline, timeout)		\
	if (timeout != NOTIMEOUT) {			\
		if (currentTime() >= deadline) {	\
			errno = ETIMEDOUT;		\
			break;				\
		}					\
	}

#define IGNORE_EINTR(r)					\
	if (r == -1 && errno == EINTR) {		\
		continue;				\
	}

#define SET_RETURN(r)					\
	if (r == -1) {					\
		r = errno;				\
	} 

#define SET_RETURN_OUT(r, out, ret)			\
	if (r == -1) {					\
		r = errno;				\
	} else {					\
		*out = ret;				\
		r = 0;					\
	}

static int
jthreadedOpen(const char* path, int flags, int mode, int *out)
{
        int r;

        /* Cygnus WinNT requires this */
        r = open(path, flags|O_BINARY, mode);
        if (r == -1) {
                r = errno;
        } else {
                *out = r;
                r = 0;
        }
        return (r);
}

static int
jthreadedClose(int fd)
{
	int rc = 0;

	if (close(fd) == -1) {
		rc = errno;
	}
	return (rc);
}

static int
jthreadedFStat(int fd, struct stat *sb)
{
	int rc = 0;

	if (fstat(fd, sb) == -1) {
		rc = errno;
	}
	return (rc);
}

static int
jthreadedStat(const char* path, struct stat *sb)
{
	int rc = 0;

	if (stat(path, sb) == -1) {
		rc = errno;
	}
	return (rc);
}

/*
 * Threaded read with no time out
 */
static int
jthreadedRead(int fd, void* buf, size_t len, ssize_t *out)
{
	ssize_t r = -1;

	r = read(fd, buf, len);
	SET_RETURN_OUT(r, out, r);
	return (r);
}

/*
 * The syscall interface as provided by the internal jthread system.
 */
SystemCallInterface Kaffe_SystemCallInterface = {
        jthreadedOpen,
        jthreadedRead,	
        jthreadedClose,
        jthreadedFStat,
        jthreadedStat,
};
