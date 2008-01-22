/*
 * syscalls.c
 * Definition of jsyscall interface for oskit-pthreads.
 *
 * Copyright (c) 1998, 1999 The University of Utah. All rights reserved.
 *
 * Copyright (c) 1996, 1997
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * Copyright (c) 2008
 *      Kaffe.org contributors. See ChangeLog for details.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 *
 * Contributed by the Flux Research Group at the University of Utah.
 * Author: Godmar Back
 */
/*
 * Syscall definitions for OSKit pthreads port.
 */

#include "config.h"
#include "debug.h"
#include "config-std.h"
#include "config-mem.h"
#include "config-net.h"
#include "config-io.h"
#include "config-signal.h"
#include "jthread.h"
#include "jsyscall.h"

#if defined(HAVE_SYS_TYPES_H)
#include <sys/types.h>
#endif /* defined(HAVE_SYS_TYPES_H) */

#include <oskit/time.h>
#include <oskit/com/listener.h>
#include <oskit/dev/clock.h>
#include <oskit/dev/timer.h>

static int
oskit_pthread_open(const char *path, int f, int m, int *outfd)
{
	*outfd = open(path, f, m);
	return (*outfd == -1) ? errno : 0;
}

static int
oskit_pthread_read(int fd, void* b, size_t l, ssize_t *out)
{
	*out = read(fd, b, l);
	return (*out == -1) ? errno : 0;
}

static int
oskit_pthread_close(int f)
{
	return (close(f) == -1) ? errno : 0;
}

static int
oskit_pthread_fstat(int f, struct stat *st)
{
	return (fstat(f, st) == -1) ? errno : 0;
}

static int
oskit_pthread_stat(const char *p, struct stat *st)
{
	return (stat(p, st) == -1) ? errno : 0;
}

SystemCallInterface Kaffe_SystemCallInterface = {
	oskit_pthread_open,
	oskit_pthread_read,
	oskit_pthread_close,
	oskit_pthread_fstat,
	oskit_pthread_stat,
};
