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
#include "config-std.h"
#include "config-mem.h"
#include "config-io.h"
#include "config-signal.h"
#include "jthread.h"
#include "jsyscall.h"

#if defined(HAVE_SYS_TYPES_H)
#include <sys/types.h>
#endif /* defined(HAVE_SYS_TYPES_H) */

/*
 * This file contains thread-safe version of various POSIXy functions.
 * Other functions that use or interact with asynchronous I/O, are in jthread.c.
 */

static int
jthreadedClose(int fd)
{
	int rc = 0;

	jthread_spinon(0);
	if (close(fd) == -1) {
		rc = errno;
	}
	jthread_spinoff(0);
	return (rc);
}

static int
jthreadedFStat(int fd, struct stat *sb)
{
	int rc = 0;

	jthread_spinon(0);
	if (fstat(fd, sb) == -1) {
		rc = errno;
	}
	jthread_spinoff(0);
	return (rc);
}

static int
jthreadedStat(const char* path, struct stat *sb)
{
	int rc = 0;

	jthread_spinon(0);
	if (stat(path, sb) == -1) {
		rc = errno;
	}
	jthread_spinoff(0);
	return (rc);
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
