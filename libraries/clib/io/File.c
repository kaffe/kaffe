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
	int r;

	stringJava2CBuf(unhand(this)->path, str, sizeof(str));

	/* a file exists if I can stat it */
	r = KSTAT(str, &buf);
	if (r) {
		return (0);
	}
	else {
		return (1);
	}
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
		assert(mentry != 0);
		strcpy(mentry->name, entry->d_name);
		mentry->next = dirlist;
		dirlist = mentry;
		count++;
	}
	/* XXX make part of jsyscall interface !? */
	closedir(dir);

	array = (HArrayOfObject*)AllocObjectArray(count, "Ljava/lang/String");
	assert(array != 0);
	for (i = 0; i < count; i++) {
		mentry = dirlist;
		dirlist = mentry->next;
		unhand(array)->body[i] = (Hjava_lang_Object*)stringC2Java(mentry->name);
		KFREE(mentry);
	}

	return (array);
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

	if (str[0] == file_separator[0]) {
		return (1);
	}
	else {
		return (0);
	}
}
