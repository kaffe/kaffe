/*
 * jsyscall.h
 * Define the Java-to-system call interface.
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file.
 */

#ifndef __jsyscall_h
#define __jsyscall_h

#include "config.h"

#if defined(HAVE_NETDB_H)
#include <netdb.h>
#endif /* defined(HAVE_NETDB_H) */

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
	int	(*_write)(int, const void*, size_t, ssize_t *);
	int	(*_lseek)(int, off_t, int, off_t *);
	int	(*_close)(int);
	int	(*_fstat)(int, struct stat *);
	int	(*_stat)(const char *, struct stat *);
	int	(*_ftruncate)(int, off_t);
	int	(*_fsync)(int);

	int	(*_mkdir)(const char *, int);
	int	(*_rmdir)(const char *);
	int	(*_rename)(const char *, const char *);
	int	(*_remove)(const char *);

	int	(*_socket)(int, int, int, int *);
	int	(*_connect)(int, struct sockaddr *, socklen_t, int timeout);
	int	(*_bind)(int, struct sockaddr *, socklen_t);
	int	(*_listen)(int, int);
	int	(*_accept)(int, struct sockaddr *, socklen_t*, int, int *);
	int	(*_sockread)(int, void*, size_t, int, ssize_t *);
	int	(*_recvfrom)(int, void *, size_t, int, struct sockaddr *, socklen_t *,
		int timeout, ssize_t *);
	int	(*_sockwrite)(int, const void *, size_t, ssize_t *);
	int	(*_sendto)(int, const void *, size_t, int, const struct sockaddr *,
		int, ssize_t *);
	int	(*_setsockopt)(int, int, int, const void *, int);
	int	(*_getsockopt)(int, int, int, void *, socklen_t *);
	int	(*_getsockname)(int, struct sockaddr *, socklen_t *);
	int	(*_getpeername)(int, struct sockaddr *, socklen_t *);
	int	(*_sockclose)(int);
	int	(*_gethostbyname)(const char *, struct hostent **);
	int	(*_gethostbyaddr)(const char *, size_t, int, struct hostent **);

	int	(*_select)(int, fd_set *, fd_set *, fd_set *, struct timeval *,
		int*);
	int	(*_forkexec)(char **, char **, int[4], int*, const char *);
	int	(*_waitpid)(int, int *, int, int *);
	int	(*_kill)(int, int);

	int	(*_mmap)(void **, size_t *, int, int fd, off_t *);
	int	(*_munmap)(void *, size_t);
	int	(*_msync)(void *, size_t);

	int	(*_pipecreate)(int *, int *);
	int	(*_piperead)(int, void *, size_t, int, ssize_t *);
	int	(*_pipewrite)(int, const void*, size_t, int, ssize_t *);
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
 * Write bytes to a file from a buffer in a 
 * platform-independant, thread-safe way.
 *
 * @param filedescriptor filedescriptor to write to
 * @param buffer buffer to write the bytes from
 * @param length number of bytes to be written
 * @param bytesWritten number of bytes actually written
 * 
 * @return 0 on success, or errno on failure.
 */
#define	KWRITE(filedescriptor, buffer, length, bytesWritten)	\
  (*Kaffe_SystemCallInterface._write)(filedescriptor, buffer, length, bytesWritten)

/**
 * Reposition read/write offset in a file in a 
 * platform-independant, thread-safe way.
 *
 * @param filedescriptor filedescriptor to stat
 * @param offset offset to set
 * @param whence how to set the offset
 * @param new_offset new value of read/write offset
 * 
 * @return 0 on success, or errno on failure.
 */
#define	KLSEEK(filedescriptor, offset, whence, new_offset) \
  (*Kaffe_SystemCallInterface._lseek)(filedescriptor, offset, whence, new_offset)

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

/**
 * FTruncate a file in a platform-independant, thread-safe way.
 *
 * @param filedescriptor filedescriptor to truncate
 * @param offset pffest at which the filedescriptor should be truncated
 * 
 * @return 0 on success, or errno on failure.
 */
#define KFTRUNCATE(filedescriptor,offset) \
  (*Kaffe_SystemCallInterface._ftruncate)(filedescriptor, offset)

/**
 * Synchrnoze a file in a platform-independant, thread-safe way.
 *
 * @param filedescriptor filedescriptor to synchronize
 * 
 * @return 0 on success, or errno on failure.
 */
#define KFSYNC(filedescriptor) \
  (*Kaffe_SystemCallInterface._fsync)(filedescriptor)

#define	KMKDIR(A,B)	(*Kaffe_SystemCallInterface._mkdir)(A,B)
#define	KRMDIR(A)	(*Kaffe_SystemCallInterface._rmdir)(A)
#define	KRENAME(A,B)	(*Kaffe_SystemCallInterface._rename)(A,B)
#define	KREMOVE(A)	(*Kaffe_SystemCallInterface._remove)(A)

#define	KSOCKET(A,B,C,D) \
			(*Kaffe_SystemCallInterface._socket)(A,B,C,D)
#define	KCONNECT(A,B,C,D) \
			(*Kaffe_SystemCallInterface._connect)(A,B,C,D)
#define	KBIND(A,B,C)	(*Kaffe_SystemCallInterface._bind)(A,B,C)
#define	KLISTEN(A,B)	(*Kaffe_SystemCallInterface._listen)(A,B)
#define	KACCEPT(A,B,C,D,E) \
			(*Kaffe_SystemCallInterface._accept)(A,B,C,D,E)
#define	KSOCKREAD(A,B,C,D,E) \
			(*Kaffe_SystemCallInterface._sockread)(A,B,C,D,E)
#define	KRECVFROM(A,B,C,D,E,F,G,H) \
			(*Kaffe_SystemCallInterface._recvfrom)(A,B,C,D,E,F,G,H)
#define	KSOCKWRITE(A,B,C,D) \
			(*Kaffe_SystemCallInterface._sockwrite)(A,B,C,D)
#define	KSENDTO(A,B,C,D,E,F,G) \
			(*Kaffe_SystemCallInterface._sendto)(A,B,C,D,E,F,G)
#define	KSETSOCKOPT(A,B,C,D,E) \
			(*Kaffe_SystemCallInterface._setsockopt)(A,B,C,D,E)
#define	KGETSOCKOPT(A,B,C,D,E) \
			(*Kaffe_SystemCallInterface._getsockopt)(A,B,C,D,E)
#define	KGETSOCKNAME(A,B,C) \
			(*Kaffe_SystemCallInterface._getsockname)(A,B,C)
#define	KGETPEERNAME(A,B,C) \
			(*Kaffe_SystemCallInterface._getpeername)(A,B,C)
#define KGETHOSTBYNAME(A,B) \
			(*Kaffe_SystemCallInterface._gethostbyname)(A,B)
#define KGETHOSTBYADDR(A,B,C,D) \
			(*Kaffe_SystemCallInterface._gethostbyaddr)(A,B,C,D)
#define KSOCKCLOSE(A)	(*Kaffe_SystemCallInterface._sockclose)(A)

#define	KSELECT(A,B,C,D,E,F) \
			(*Kaffe_SystemCallInterface._select)(A,B,C,D,E,F)

#define	KWAITPID(A,B,C,D) \
			(*Kaffe_SystemCallInterface._waitpid)(A,B,C,D)
#define	KFORKEXEC(A,B,C,D,E) \
			(*Kaffe_SystemCallInterface._forkexec)(A,B,C,D,E)
#define	KKILL(A,B)	(*Kaffe_SystemCallInterface._kill)(A,B)
#define KMMAP(A,B,C,D,E) (*Kaffe_SystemCallInterface._mmap)(A,B,C,D,E)
#define KMUNMAP(A,B)  (*Kaffe_SystemCallInterface._munmap)(A,B)
#define KMSYNC(A,B)  (*Kaffe_SystemCallInterface._msync)(A,B)

#define KPIPECREATE(A,B)   (*Kaffe_SystemCallInterface._pipecreate)(A,B)
#define KPIPEREAD(A,B,C,D,E) (*Kaffe_SystemCallInterface._piperead)(A,B,C,D,E)
#define KPIPEWRITE(A,B,C,D,E) (*Kaffe_SystemCallInterface._pipewrite)(A,B,C,D,E)

#define KAFFE_MMAP_READ 0
#define KAFFE_MMAP_WRITE 1
#define KAFFE_MMAP_PRIVATE 2

#define NOTIMEOUT (-1)

#endif
