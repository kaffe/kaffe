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

struct sockaddr;
struct timeval;
struct fd_set;
struct stat;

typedef struct SystemCallInterface {

	int	(*_open)(const char*, int, int);
	ssize_t	(*_read)(int, void*, size_t);
	ssize_t	(*_write)(int, const void*, size_t);
	off_t	(*_lseek)(int, off_t, int);
	int	(*_close)(int);
	int	(*_fstat)(int, struct stat*);
	int	(*_stat)(const char*, struct stat*);

	int	(*_mkdir)(const char*, int);
	int	(*_rmdir)(const char*);
	int	(*_rename)(const char*, const char*);
	int	(*_remove)(const char*);

	int	(*_socket)(int, int, int);
	int	(*_connect)(int, struct sockaddr*, size_t);
	int	(*_accept)(int, struct sockaddr*, size_t*);
	ssize_t	(*_sockread)(int, void*, size_t);
	ssize_t	(*_recvfrom)(int,void*,size_t,int,struct sockaddr*,int*);
	ssize_t	(*_sockwrite)(int, const void*, size_t);
	ssize_t (*_sendto)(int,const void*,size_t,int,const struct sockaddr*,int);
	int	(*_setsockopt)(int, int, int, const void*, int);
	int	(*_getsockopt)(int, int, int, void*, int*);
	int	(*_getsockname)(int, struct sockaddr*, int*);
	int	(*_getpeername)(int, struct sockaddr*, int*);

	int	(*_select)(int, fd_set*, fd_set*, fd_set*, struct timeval*);
	int	(*_forkexec)(char**, char**, int[4]);
	int	(*_waitpid)(int, int*, int);
	int	(*_kill)(int, int);

} SystemCallInterface;

extern SystemCallInterface Kaffe_SystemCallInterface;

#if !defined(NOUNIXPROTOTYPES)
/*
 * Define some 'UNIX' like macros to make things simpler.
 */

#define	open(A,B,C)	(*Kaffe_SystemCallInterface._open)(A,B,C)
#define	read(A,B,C)	(*Kaffe_SystemCallInterface._read)(A,B,C)
#define	write(A,B,C)	(*Kaffe_SystemCallInterface._write)(A,B,C)
#define	lseek(A,B,C)	(*Kaffe_SystemCallInterface._lseek)(A,B,C)
#define	close(A)	(*Kaffe_SystemCallInterface._close)(A)
#define	fstat(A,B)	(*Kaffe_SystemCallInterface._fstat)(A,B)
#define	stat(A,B)	(*Kaffe_SystemCallInterface._stat)(A,B)

#define	mkdir(A,B)	(*Kaffe_SystemCallInterface._mkdir)(A,B)
#define	rmdir(A)	(*Kaffe_SystemCallInterface._rmdir)(A)
#define	rename(A,B)	(*Kaffe_SystemCallInterface._rename)(A,B)
#define	remove(A)	(*Kaffe_SystemCallInterface._remove)(A)

#define	socket(A,B,C)	(*Kaffe_SystemCallInterface._socket)(A,B,C)
#define	connect(A,B,C)	(*Kaffe_SystemCallInterface._connect)(A,B,C)
#define	accept(A,B,C)	(*Kaffe_SystemCallInterface._accept)(A,B,C)
#define	sockread(A,B,C)	(*Kaffe_SystemCallInterface._sockread)(A,B,C)
#define	recvfrom(A,B,C,D,E,F) \
			(*Kaffe_SystemCallInterface._recvfrom)(A,B,C,D,E,F)
#define	sockwrite(A,B,C) \
			(*Kaffe_SystemCallInterface._sockwrite)(A,B,C)
#define	sendto(A,B,C,D,E,F) \
			(*Kaffe_SystemCallInterface._sendto)(A,B,C,D,E,F)
#define	setsockopt(A,B,C,D,E) \
			(*Kaffe_SystemCallInterface._setsockopt)(A,B,C,D,E)
#define	getsockopt(A,B,C,D,E) \
			(*Kaffe_SystemCallInterface._getsockopt)(A,B,C,D,E)
#define	getsockname(A,B,C) \
			(*Kaffe_SystemCallInterface._getsockname)(A,B,C)
#define	getpeername(A,B,C) \
			(*Kaffe_SystemCallInterface._getpeername)(A,B,C)

#define	select(A,B,C,D,E) \
			(*Kaffe_SystemCallInterface._select)(A,B,C,D,E)

#define	waitpid(A,B,C)	(*Kaffe_SystemCallInterface._waitpid)(A,B,C)
#define	forkexec(A,B,C)	(*Kaffe_SystemCallInterface._forkexec)(A,B,C)
#define	kill(A,B)	(*Kaffe_SystemCallInterface._kill)(A,B)

#endif

#endif
