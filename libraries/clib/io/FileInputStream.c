/*
 * java.io.FileInputStream.c
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#include "config.h"
#include "config-std.h"
#include "config-mem.h"
#include "config-io.h"
#include "files.h"
#include "defs.h"
#include "java_io_FileInputStream.h"
#include "java_io_FileDescriptor.h"
#include "jsyscall.h"
#include "../../../kaffe/kaffevm/support.h"
#include "../../../kaffe/kaffevm/stringSupport.h"

/*
 * Open a file for input.
 */
void
java_io_FileInputStream_open(struct Hjava_io_FileInputStream* this, struct Hjava_lang_String* name)
{
	char str[MAXPATHLEN];
	int fd;
	int rc;

	stringJava2CBuf(name, str, sizeof(str));
	rc = KOPEN(str, O_RDONLY|O_BINARY, 0, &fd);
	if (rc) {
		unhand(unhand(this)->fd)->fd = -1;
		SignalError("java.io.IOException", SYS_ERROR(rc));
	}
	unhand(unhand(this)->fd)->fd = fd;
}

/*
 * Close file.
 */
void
java_io_FileInputStream_close(struct Hjava_io_FileInputStream* this)
{
	int r;

	if (unhand(unhand(this)->fd)->fd >= 0) {
		r = KCLOSE(unhand(unhand(this)->fd)->fd);
		unhand(unhand(this)->fd)->fd = -1;
		if (r) {
			SignalError("java.io.IOException", SYS_ERROR(r));
		}
	}
}

/*
 * Read in bytes.
 */
jint
java_io_FileInputStream_readBytes(struct Hjava_io_FileInputStream* fh, HArrayOfByte* bytes, jint off, jint len)
{
	int rc;
	ssize_t ret;

	rc = KREAD(unhand(unhand(fh)->fd)->fd, &unhand_array(bytes)->body[off], len, &ret);
	if (rc == EINTR) {
		SignalError("java.io.InterruptedIOException", "");
	} else if (rc) {
		SignalError("java.io.IOException", SYS_ERROR(rc));
	}
	return (ret > 0 ? ret : -1);
}

/*
 * Read a single byte.
 */
jint
java_io_FileInputStream_read(struct Hjava_io_FileInputStream* fh)
{
	ssize_t ret;
	int rc;
	unsigned char byte;

	rc = KREAD(unhand(unhand(fh)->fd)->fd, &byte, 1, &ret);
	if (rc == EINTR) {
		SignalError("java.io.InterruptedIOException", "");
	} else if (rc) {
		SignalError("java.io.IOException", SYS_ERROR(rc));
	}
	return (ret > 0 ? byte : -1);
}

/*
 * Skip forward in stream.
 */
jlong
java_io_FileInputStream_skip(struct Hjava_io_FileInputStream* fh, jlong off)
{
	off_t orig;
	off_t ret;
	char buffer[100];
	ssize_t count;
	int rc;

	rc = KLSEEK(unhand(unhand(fh)->fd)->fd, (off_t)0, 1, &orig);
	if (rc) {
		SignalError("java.io.IOException", SYS_ERROR(rc));
	}
	rc = KLSEEK(unhand(unhand(fh)->fd)->fd, jlong2off_t(off), 1, &ret);
	if (rc == 0) {
		return (off_t2jlong(ret-orig));
	}

	/* Not seekable - try just reading. */
	ret = 0;
	while (off > 0) {
		int num = (off < 100) ? off : 100;

		rc = KREAD(unhand(unhand(fh)->fd)->fd, buffer, num, &count);
		if (rc == EINTR) {
			SignalError("java.io.InterruptedIOException", "");
		} else if (rc) {
			SignalError("java.io.IOException", SYS_ERROR(rc));
		}
		if (count == 0) {	/* Reached EOF. */
			break;
		}
		ret += count;
		off -= count;
	}
	return off_t2jlong(ret);
}

/*
 * Return the number of bytes available to read without blocking.
 */
jint
java_io_FileInputStream_available(struct Hjava_io_FileInputStream* fh)
{
	int r, nr;
	int fd = unhand(unhand(fh)->fd)->fd;
	off_t cur = 0;

	r = KLSEEK(fd, cur, SEEK_CUR, &cur);
	if (r == 0) {
		struct stat statbuf;
		if ((KFSTAT(fd, &statbuf) == 0) && S_ISREG(statbuf.st_mode)) {
			return (statbuf.st_size - cur);
		}
	}

	/* If lseek or fstat fail, try another mechanism... */

#if defined(HAVE_IOCTL) && defined(FIONREAD)
	/* XXX make part of jsyscall interface */
	r = ioctl(fd, FIONREAD, &nr);
	if (r >= 0 && nr != 0) {
		return (nr);
	} else
		/* FIONREAD may report 0 for files for which data is
                   available; maybe select will do... */
#endif
	{
		/* This uses select() to work out if we can read - but
		 * what happens at the end of file?  */
		static struct timeval tm = { 0, 0 };
		fd_set rd;

		FD_ZERO(&rd);
		FD_SET(fd, &rd);
		KSELECT(fd+1, &rd, NULL, NULL, &tm, &r);
		if (r == 1) {
			nr = 1;
		}
		else {
			nr = 0;
		}

		return (nr);
	}
}
