/*
 * java.io.FileDescriptor.c
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "lib-license.terms" for information on usage and redistribution 
 * of this file. 
 */

#include "config.h"
#include "config-std.h"
#include "config-io.h"
#include "files.h"
#include "FileDescriptor.h"
#include "jsyscall.h"
#include "../../../kaffe/kaffevm/support.h"

/*
 * Initialise a file descriptor to the given file nr.
 */
struct Hjava_io_FileDescriptor*
java_io_FileDescriptor_initSystemFD(struct Hjava_io_FileDescriptor* this, jint i)
{
	unhand(this)->fd = i;
	return (this);
}

/*
 * Is this file descriptor valid ?
 */
jbool
java_io_FileDescriptor_valid(struct Hjava_io_FileDescriptor* this)
{
	if (unhand(this)->fd >= 0) {
		return (1);
	}
	else {
		return (0);
	}
}

/*
 * Synchronise this file descriptor with the real file system.
 */
void
java_io_FileDescriptor_sync(struct Hjava_io_FileDescriptor* this)
{
#if defined(HAVE_FSYNC)
	if (unhand(this)->fd >= 0) {
		int r = fsync(unhand(this)->fd);
		if (r < 0) {
			SignalError("java.io.SyncFailedException", SYS_ERROR);
		}
	}
#elif defined(HAVE_SYNC)
	/* Fallback on a full sync */
	sync();
#else
	/* Well we can't do anything can we? */
	SignalError("java.io.SyncFailedException", "no sync supported");
#endif
}
