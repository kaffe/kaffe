/*
 * java.io.FileOutputStream.c
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#include "config.h"
#include "config-std.h"
#include "config-io.h"
#include "config-mem.h"
#include "files.h"
#include "defs.h"
#include "java_io_FileOutputStream.h"
#include "java_io_FileDescriptor.h"
#include "jsyscall.h"
#include "../../../kaffe/kaffevm/support.h"
#include "../../../kaffe/kaffevm/stringSupport.h"

/*
 * Open a file for output.
 */
void
java_io_FileOutputStream_open(struct Hjava_io_FileOutputStream* fh, struct Hjava_lang_String* nm)
{
	int fd;
	int rc;
	char str[MAXPATHLEN];

	stringJava2CBuf(nm, str, sizeof(str));

	rc = KOPEN(str, O_WRONLY|O_CREAT|O_BINARY|O_TRUNC, 0666, &fd);
	if (rc) {
		unhand(unhand(fh)->fd)->fd = -1;
		SignalError("java.io.FileNotFoundException", SYS_ERROR(rc));
	}
	unhand(unhand(fh)->fd)->fd = fd;
}

/*
 * Open a file for appended output.
 */
void
java_io_FileOutputStream_openAppend(struct Hjava_io_FileOutputStream* fh, struct Hjava_lang_String* nm)
{
	int fd;
	int rc;
	char str[MAXPATHLEN];

	stringJava2CBuf(nm, str, sizeof(str));

	rc = KOPEN(str, O_WRONLY|O_CREAT|O_BINARY|O_APPEND, 0666, &fd);
	if (rc) {
		unhand(unhand(fh)->fd)->fd = -1;
		SignalError("java.io.FileNotFoundException", SYS_ERROR(rc));
	}
	unhand(unhand(fh)->fd)->fd = fd;
}

/*
 * Close a file.
 */
void
java_io_FileOutputStream_close(struct Hjava_io_FileOutputStream* fh)
{
	int r;

	if (unhand(unhand(fh)->fd)->fd >= 0) {
		r = KCLOSE(unhand(unhand(fh)->fd)->fd);
		unhand(unhand(fh)->fd)->fd = -1;
		if (r) {
			SignalError("java.io.IOException", SYS_ERROR(r));
		}
	}
}

/*
 * Write all the bytes in the buffer to a file.
 */
void
java_io_FileOutputStream_writeBytes(struct Hjava_io_FileOutputStream* this, HArrayOfByte* bytes, jint off, jint len)
{
	int r, fd;
	ssize_t nw;

	if (off < 0 || off + len > obj_length(bytes)) {
		SignalError("java.lang.IndexOutOfBoundsException", "");
	}
	fd = unhand(unhand(this)->fd)->fd;
	while (len > 0) {
		r = KWRITE(fd, &unhand_array(bytes)->body[off], len, &nw);
		if (r != 0) {
			SignalError("java.io.IOException", SYS_ERROR(r));
		}
		off += nw;
		len -= nw;
	}
}

/*
 * Write a byte to file.
 */
void
java_io_FileOutputStream_write(struct Hjava_io_FileOutputStream* fh, jint byte)
{
	int fd;
	int r;
	unsigned char b = byte;
	ssize_t bwritten;

	fd = unhand(unhand(fh)->fd)->fd;
	r = KWRITE(fd, &b, 1, &bwritten);
	if (r) {
		SignalError("java.io.IOException", SYS_ERROR(r));
	}
}
