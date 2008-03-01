/*
 * jsyscall.h
 * Define the Java-to-system call interface.
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * Copyright (c) 2008
 *      Kaffe.org contributors. See ChangeLog for details.
 * 
 * See the file "license.terms" for information on usage and redistribution 
 * of this file.
 */

#ifndef __jsyscall_h
#define __jsyscall_h

#include "config.h"

#include <stdint.h>

#if defined(HAVE_STDLIB_H)
#include <stdlib.h>
#endif /* HAVE_STDLIB_H */

#if defined(HAVE_SYS_TYPES_H)
#include <sys/types.h>
#endif /* defined(HAVE_SYS_TYPES_H) */


#if defined (HAVE_SYS_SOCKET_H)
#include <sys/socket.h>
#endif /* defined (HAVE_SYS_SOCKET_H) */

#if defined(HAVE_SYS_STAT_H)
#include <sys/stat.h>
#endif /* defined(HAVE_SYS_STAT_H) */

#if defined(HAVE_SYS_TIME_H)
#include <sys/time.h>
#endif /* defined(HAVE_SYS_TIME_H) */


#if defined(HAVE_UNISTD_H)
#include <unistd.h>
#endif /* HAVE_UNISTD_H */

/*
 * Thread-safe variants of several POSIXy calls.
 *
 * Output values, where given, are returned by reference as the last parameter.
 * All functions return zero on success, otherwise a non-zero value is
 * returned which corresponds to errno.
 */
typedef struct SystemCallInterface {

	int	(*_open)(const char *, int, int, int *);
	int	(*_read)(int, void *, size_t, ssize_t *);
	int	(*_close)(int);
	int	(*_fstat)(int, struct stat *);
	int	(*_stat)(const char *, struct stat *);
} SystemCallInterface;

extern SystemCallInterface Kaffe_SystemCallInterface;

/*
 * Define some convenience macros
 */

/**
 * Open a file in a platform-independant, thread-safe way, and
 * set the filedescriptor to the opened file's descriptor.
 *
 * @param filename name of file to open.
 * @param flags flags to pass to open
 * @param permissions permissions with which the file is to be opened
 * @param filedescriptor pointer to the filedescriptor to store the result in
 * 
 * @return 0 on success, or errno on failure.
 */
#define	KOPEN(filename, flags, permissions, filedescriptor)	\
  (*Kaffe_SystemCallInterface._open)(filename, flags, permissions, filedescriptor)

/**
 * Read bytes from a file into a buffer in a 
 * platform-independant, thread-safe way.
 *
 * @param filedescriptor filedescriptor to read from
 * @param buffer buffer to read the bytes into
 * @param length number of bytes to be read
 * @param bytesRead number of bytes actually read
 * 
 * @return 0 on success, or errno on failure.
 */
#define	KREAD(filedescriptor, buffer, length, bytesRead)	\
  (*Kaffe_SystemCallInterface._read)(filedescriptor, buffer, length, bytesRead)

/**
 * Close a file in a platform-independant, thread-safe way.
 *
 * @param filedescriptor filedescriptor to close
 * 
 * @return 0 on success, or errno on failure.
 */
#define	KCLOSE(filedescriptor)	\
  (*Kaffe_SystemCallInterface._close)(filedescriptor)

/**
 * FStat a file in a platform-independant, thread-safe way.
 *
 * @param filedescriptor filedescriptor to stat
 * @param stats return buffer
 * 
 * @return 0 on success, or errno on failure.
 */
#define	KFSTAT(filedescriptor, stats)	\
  (*Kaffe_SystemCallInterface._fstat)(filedescriptor, stats)
#define	KSTAT(A,B)	(*Kaffe_SystemCallInterface._stat)(A,B)

#define NOTIMEOUT (-1)

#endif
