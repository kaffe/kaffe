/*
 * java.io.RandomAccessFile.c
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#include "config.h"
#include <stdio.h>
#include <assert.h>
#include "config-std.h"
#include "config-io.h"
#include "config-mem.h"
#include "defs.h"
#include "files.h"
#include "java_io_RandomAccessFile.h"
#include "java_io_FileDescriptor.h"
#include "jsyscall.h"
#include "../../../kaffe/kaffevm/support.h"
#include "../../../kaffe/kaffevm/stringSupport.h"

/*
 * Open a file for random access.
 */
void
java_io_RandomAccessFile_open(struct Hjava_io_RandomAccessFile* this, struct Hjava_lang_String* name, jbool rw)
{
	int fd;
	int rc;
	char str[MAXPATHLEN];

	stringJava2CBuf(name, str, sizeof(str));

	rc = KOPEN(str, (rw == 0 ? O_RDONLY : O_RDWR|O_CREAT)|O_BINARY, 0666, &fd);
	if (rc) {
		unhand(unhand(this)->fd)->fd = -1;
		SignalError("java.io.IOException", SYS_ERROR(rc));
	}
	unhand(unhand(this)->fd)->fd = fd;
}

/*
 * Return length of file.
 */
jlong
java_io_RandomAccessFile_length(struct Hjava_io_RandomAccessFile* this)
{
	struct stat buf;
	int r;

	r = KFSTAT(unhand(unhand(this)->fd)->fd, &buf);
	if (r) {
		SignalError("java.io.IOException", SYS_ERROR(r));
	}
	return (off_t2jlong(buf.st_size));
}

/*
 * Seek into file.
 */
void
java_io_RandomAccessFile_seek(struct Hjava_io_RandomAccessFile* this, jlong pos)
{
	int rc;
	off_t r;

	rc = KLSEEK(unhand(unhand(this)->fd)->fd, jlong2off_t(pos), SEEK_SET, &r);
	if (rc) {
		SignalError("java.io.IOException", SYS_ERROR(rc));
	}
}

/*
 * Read in bytes from file.
 */
jint
java_io_RandomAccessFile_readBytes(struct Hjava_io_RandomAccessFile* this, HArrayOfByte* bytes, jint off, jint len)
{
	ssize_t ret;
	int rc;

	/* Adjust length */
	len = (len < obj_length(bytes) - off ? len : obj_length(bytes) - off);

	rc = KREAD(unhand(unhand(this)->fd)->fd, &unhand_array(bytes)->body[off], len, &ret);
	if (rc) {
		SignalError("java.io.IOException", SYS_ERROR(rc));
	}
	return (ret > 0 ? ret : -1);
}

/*
 * Read a byte from file.
 */
jint
java_io_RandomAccessFile_read(struct Hjava_io_RandomAccessFile* this)
{
	ssize_t ret;
	int rc;
	unsigned char byte;

	rc = KREAD(unhand(unhand(this)->fd)->fd, &byte, 1, &ret);
	if (rc) {
		SignalError("java.io.IOException", SYS_ERROR(rc));
	}

	return (ret > 0 ? byte : -1);
}

/*
 * Write a byte to file.
 */
void
java_io_RandomAccessFile_write(struct Hjava_io_RandomAccessFile* this, jint data)
{
	jint ret;
	unsigned char byte;
	ssize_t bwritten;

	byte = data;

	ret = KWRITE(unhand(unhand(this)->fd)->fd, &byte, 1, &bwritten);
	if (ret) {
		SignalError("java.io.IOException", SYS_ERROR(ret));
	}
}

/*
 * Write a number of bytes to file.
 */
void
java_io_RandomAccessFile_writeBytes(struct Hjava_io_RandomAccessFile* this, HArrayOfByte* bytes, jint off, jint len)
{
	int r, fd;
	ssize_t nw;

	fd = unhand(unhand(this)->fd)->fd;
	while (len > 0) {
		r = KWRITE(fd, &unhand_array(bytes)->body[off], len, &nw);
		if (r) {
			SignalError("java.io.IOException", SYS_ERROR(r));
		}
		off += nw;
		len -= nw;
	}
}

/*
 * Get current file position.
 */
jlong
java_io_RandomAccessFile_getFilePointer(struct Hjava_io_RandomAccessFile* this)
{
	off_t r;
	int rc;

	rc = KLSEEK(unhand(unhand(this)->fd)->fd, 0, SEEK_CUR, &r);
	if (rc) {
		SignalError("java.io.IOException", SYS_ERROR(rc));
	}
	return (off_t2jlong(r));
}

/*
 * Close file.
 */
void
java_io_RandomAccessFile_close(struct Hjava_io_RandomAccessFile* this)
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
