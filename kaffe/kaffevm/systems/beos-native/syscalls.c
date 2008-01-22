/*
 * syscalls.c
 * Definition of jsyscall interface for beos-native
 *
 * Copyright (c) 1998, 1999 The University of Utah. All rights reserved.
 *
 * Copyright (c) 1996, 1997
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 *
 * Contributed by the Flux Research Group at the University of Utah.
 * Author: Godmar Back
 */

/*
 * Derived for BeOS R4 from oskit-pthreads/syscalls.c
 * Please report problems to alanlb@cs.vt.edu.
 */

/*
 * Syscall definitions for BeOS with native threads port.
 */

#include "config.h"
#include "debug.h"
#include "config-std.h"
#include "config-mem.h"
#include "config-net.h"
#include "config-io.h"
#include "config-signal.h"
#include "config-hacks.h"
#include "jthread.h"
#include "jsyscall.h"

#if defined(HAVE_SYS_TYPES_H)
#include <sys/types.h>
#endif /* defined(HAVE_SYS_TYPES_H) */

#include <sys/wait.h>
#include <stdlib.h>
#include <errno.h>

static int
beos_native_open(const char *path, int f, int m, int *outfd)
{
	*outfd = open(path, f, m);
	return (*outfd < 0) ? errno : 0;
}

static int
beos_native_read(int fd, void* b, size_t l, ssize_t *out)
{
	*out = read(fd, b, l);
	return (*out < 0) ? errno : 0;
}

static int
beos_native_close(int f)
{
	return (close(f) < 0) ? errno : 0;
}

static int
beos_native_fstat(int f, struct stat *st)
{
	return (fstat(f, st) < 0) ? errno : 0;
}

static int
beos_native_stat(const char *p, struct stat *st)
{
	return (stat(p, st) < 0) ? errno : 0;
}

SystemCallInterface Kaffe_SystemCallInterface = {
	beos_native_open,
	beos_native_read,
	beos_native_close,
	beos_native_fstat,
	beos_native_stat,
};
