/*
 * java.io.File.c
 *
 * Copyright (c) 1996, 1997, 1998, 1999
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#include "config.h"
#include "config-std.h"
#include "config-io.h"
#include "config-mem.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <native.h>
#include <jsyscall.h>
#ifdef HAVE_UTIME_H
#include <utime.h>
#endif
#include "defs.h"
#include "files.h"
#include "../../../include/system.h"
#include "java_io_File.h"
#include "../../../kaffe/kaffevm/support.h"
#include "../../../kaffe/kaffevm/stringSupport.h"

/*
 * Is named item a file?
 */
jbool
java_io_File_isFile0(struct Hjava_io_File* this)
{
	struct stat buf;
	char str[MAXPATHLEN];
	int r;

	stringJava2CBuf(unhand(this)->path, str, sizeof(str));

	r = KSTAT(str, &buf);
	if (r == 0 && S_ISREG(buf.st_mode)) {
		return (1);
	}
	else {
		return (0);
	}
}

/*
 * Is named item a directory?
 */
jbool
java_io_File_isDirectory0(struct Hjava_io_File* this)
{
	struct stat buf;
	char str[MAXPATHLEN];
	int r;

	stringJava2CBuf(unhand(this)->path, str, sizeof(str));

	r = KSTAT(str, &buf);
	if (r == 0 && S_ISDIR(buf.st_mode)) {
		return (1);
	}
	else {
		return (0);
	}
}

/*
 * Does named file exist?
 */
jbool
java_io_File_exists0(struct Hjava_io_File* this)
{
	struct stat buf;
	char str[MAXPATHLEN];

	stringJava2CBuf(unhand(this)->path, str, sizeof(str));

	/* A file exists if I can stat it */
	return (KSTAT(str, &buf) == 0);
}

/*
 * Last modified time on file.
 */
jlong
java_io_File_lastModified0(struct Hjava_io_File* this)
{
	struct stat buf;
	char str[MAXPATHLEN];
	int r;

	stringJava2CBuf(unhand(this)->path, str, sizeof(str));

	r = KSTAT(str, &buf);
	if (r != 0) {
		return ((jlong)0);
	}
	return ((jlong)buf.st_mtime * (jlong)1000);
}

/*
 * Can I write to this file?
 */
jbool
java_io_File_canWrite0(struct Hjava_io_File* this)
{
	char str[MAXPATHLEN];
	int r;

	stringJava2CBuf(unhand(this)->path, str, sizeof(str));
	/* XXX make part of jsyscall interface !? */
	r = access(str, W_OK);
	return (r < 0 ? 0 : 1);
}

/*
 * Can I read from this file.
 */
jbool
java_io_File_canRead0(struct Hjava_io_File* this)
{
	char str[MAXPATHLEN];
	int r;

	stringJava2CBuf(unhand(this)->path, str, sizeof(str));
	/* XXX make part of jsyscall interface !? */
	r = access(str, R_OK);
	return (r < 0 ? 0 : 1);
}

/*
 * Return length of file.
 */
jlong
java_io_File_length0(struct Hjava_io_File* this)
{
	struct stat buf;
	char str[MAXPATHLEN];
	int r;

	stringJava2CBuf(unhand(this)->path, str, sizeof(str));

	r = KSTAT(str, &buf);
	if (r != 0) {
		return ((jlong)0);
	}
	return ((jlong)buf.st_size);
}

/*
 * Create a directory.
 */
jbool
java_io_File_mkdir0(struct Hjava_io_File* this)
{
	char str[MAXPATHLEN];
	int r;

	stringJava2CBuf(unhand(this)->path, str, sizeof(str));
	r = KMKDIR(str, 0777);
	return (r != 0 ? 0 : 1);
}

/*
 * Rename a file.
 */
jbool
java_io_File_renameTo0(struct Hjava_io_File* this, struct Hjava_io_File* that)
{
	char str[MAXPATHLEN];
	char str2[MAXPATHLEN];
	int r;

	stringJava2CBuf(unhand(this)->path, str, sizeof(str));
	stringJava2CBuf(unhand(that)->path, str2, sizeof(str2));

	r = KRENAME(str, str2);
	return (r != 0 ? 0 : 1);
}

/*
 * Delete a file.
 */
jbool
java_io_File_delete0(struct Hjava_io_File* this)
{
	char str[MAXPATHLEN];
	int r;

	stringJava2CBuf(unhand(this)->path, str, sizeof(str));
	r = KREMOVE(str);
	return(r != 0 ? 0 : 1);
}

/*
 * Delete a directory.
 */
jbool
java_io_File_rmdir0(struct Hjava_io_File* this)
{
	char str[MAXPATHLEN];
	int r;

	stringJava2CBuf(unhand(this)->path, str, sizeof(str));
	r = KRMDIR(str);
	return(r != 0 ? 0 : 1);
}

/*
 * Get a directory listing.
 */
HArrayOfObject* /* [Ljava.lang.String; */
java_io_File_list0(struct Hjava_io_File* this)
{
#if defined(HAVE_DIRENT_H)
	char path[MAXPATHLEN];
	DIR* dir;
	struct dirent* entry;
	struct dentry {
		struct dentry* next;
		char name[1];
	};
	struct dentry* dirlist;
	struct dentry* mentry;
	HArrayOfObject* array;
	int count;
	int i;
	int oom = 0;

	stringJava2CBuf(unhand(this)->path, path, sizeof(path));

	/* XXX make part of jsyscall interface !? */
	dir = opendir(path);
	if (dir == 0) {
		return (0);
	}

	dirlist = 0;
	count = 0;
	/* XXX make part of jsyscall interface !? */
	while ((entry = readdir(dir)) != 0) {
		/* We skip '.' and '..' */
		if (strcmp(".", entry->d_name) == 0 ||
		    strcmp("..", entry->d_name) == 0) {
			continue;
		}
		mentry = KMALLOC(sizeof(struct dentry) + NAMLEN(entry));
		if (!mentry) {
			errorInfo info;

			while (dirlist) {
				mentry = dirlist;
				dirlist = dirlist->next;
				KFREE(mentry);
			}
			postOutOfMemory(&info);
			throwError(&info);
		}
		strcpy(mentry->name, entry->d_name);
		mentry->next = dirlist;
		dirlist = mentry;
		count++;
	}
	/* XXX make part of jsyscall interface !? */
	closedir(dir);

	array = (HArrayOfObject*)AllocObjectArray(count,
	    "Ljava/lang/String;", 0);
	/* XXX: This assert is a noop.  If AllocObjectArray throws an
	   exception, we leak. */
	assert(array != 0);
	for (i = 0; i < count; i++) {
		mentry = dirlist;
		dirlist = mentry->next;
		unhand_array(array)->body[i] =
			(Hjava_lang_Object*)stringC2Java(mentry->name);
		/* if allocation fails, continue freeing mentries in
		   this loop. */
		oom |= !unhand_array(array)->body[i];
		KFREE(mentry);
	}
	if (oom) {
		errorInfo info;
		postOutOfMemory(&info);
		throwError(&info);
	}

	return (array);
#else
	return (0);
#endif
}

/*
 * Translate the given pathname into the local canonized form.
 * For the moment we just return what we're given.
 */
struct Hjava_lang_String*
java_io_File_canonPath(struct Hjava_io_File* this, struct Hjava_lang_String* str)
{
	return (str);
}

/*
 * Is this filename absolute?
 */
jbool
java_io_File_isAbsolute(struct Hjava_io_File* this)
{
	char str[2];

	stringJava2CBuf(unhand(this)->path, str, sizeof(str));
	return (str[0] == file_separator[0]);
}

jboolean
java_io_File_createNewFile0(struct Hjava_io_File* this, jint mode)
{
	char str[MAXPATHLEN];
	int fd;
	int rc;

	stringJava2CBuf(unhand(this)->path, str, sizeof(str));

	rc = KOPEN(str, O_EXCL|O_WRONLY|O_CREAT, mode, &fd);
	switch (rc) {
	case 0:
		break;
	case EEXIST:
		return 0;
	default:
		SignalError("java.io.IOException", SYS_ERROR(rc));
	}
	rc = KCLOSE(fd);
	if (rc != 0)
		SignalError("java.io.IOException", SYS_ERROR(rc));
	return 1;
}

jboolean
java_io_File_setLastModified0(struct Hjava_io_File* this, jlong thetime)
{
#ifdef HAVE_UTIME_H
	char path[MAXPATHLEN];
	struct utimbuf ub;
#endif

	if (thetime < 0)
		SignalError("java.lang.IllegalArgumentException", "time < 0");
#ifdef HAVE_UTIME_H
	stringJava2CBuf(unhand(this)->path, path, sizeof(path));
	ub.actime = (time_t)(thetime / 1000);
	ub.modtime = ub.actime;
	return (utime(path, &ub) >= 0);
#else
	return 0;
#endif
}

jboolean
java_io_File_setReadOnly0(struct Hjava_io_File* this)
{
	struct stat buf;
	char str[MAXPATHLEN];
	int r;

	stringJava2CBuf(unhand(this)->path, str, sizeof(str));

	r = KSTAT(str, &buf);
	if (r != 0)
		return 0;

	r = chmod(str, buf.st_mode & ~(S_IWOTH|S_IWGRP|S_IWUSR));
	return (r == 0);
}

