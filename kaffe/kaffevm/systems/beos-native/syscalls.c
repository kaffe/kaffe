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

/* Derived for BeOS R4 from oskit-pthreads/syscalls.c */

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
beos_native_write(int fd, const void* b, size_t l, ssize_t *out)
{
	*out = write(fd, b, l);
	return (*out < 0) ? errno : 0;
}

static int
beos_native_lseek(int f, off_t o, int w, off_t *out)
{
	*out = lseek(f, o, w);
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

static int
beos_native_mkdir(const char *p, int m)
{
	return (mkdir(p, m) < 0) ? errno : 0;
}

static int
beos_native_rmdir(const char *p)
{
	return (rmdir(p) < 0) ? errno : 0;
}

static int
beos_native_rename(const char *p, const char *q)
{
	return (rename(p, q) < 0) ? errno : 0;
}

static int
beos_native_remove(const char *p)
{
	return (remove(p) < 0) ? errno : 0;
}

static int
beos_native_socket(int a, int b, int c, int *outfd)
{
        *outfd = socket(a, b, c); 
	return (*outfd < 0) ? errno : 0;
}

static int
beos_native_connect(int fd, struct sockaddr* addr, size_t len, int timeout)
{
	/* XXX implement timeout */
        int r = connect(fd, addr, len);
        /* annul EISCONN error --- is this really necessary or is this
	 * a java.net bug? 
	 */
        if (r < 0 && errno == EISCONN) {
                r = 0;
	}
        return (r);
}

static int
beos_native_bind(int fd, struct sockaddr* addr, size_t len)
{
        return (bind(fd, addr, len) < 0) ? errno : 0;
}

static int
beos_native_listen(int fd, int n)
{
        return (listen(fd, n) < 0) ? errno : 0;
}

static int
beos_native_accept(int fd, struct sockaddr* a, size_t *l, int timeout, 
	int* outfd)
{
	/* XXX implement timeout!!! */
	*outfd = accept(fd, a, l);
	return (*outfd < 0) ? errno : 0;
}

static int
beos_native_sock_read(int f, void* b, size_t l, int timeout, ssize_t *out)
{
	/* XXX implement timeout!!! */
	*out = read(f, b, l);
	return (*out < 0) ? errno : 0;
}

static int
beos_native_recvfrom(int a, void* b, size_t c, int d, struct sockaddr* e, 
	int* f, int timeout, ssize_t *out)
{
	if (d & MSG_PEEK) {
		errno = B_UNSUPPORTED;
		*out = errno;
	}
	else {
		/* XXX implement timeout!!! */
		*out = recvfrom(a, b, c, d, e, f);
	}
	return (*out < 0) ? errno : 0;
}

static int
beos_native_sendto(int a, const void* b, size_t c, int d, 
	const struct sockaddr* e, int f, ssize_t *out)
{
	*out = sendto(a, b, c, d, e, f);
	return (*out < 0) ? errno : 0;
}

static int
beos_native_setsockopt(int a, int b, int c, const void* d, int e)
{
        return (setsockopt(a, b, c, d, e) < 0) ? errno : 0;
}

static int
beos_native_getsockopt(int a, int b, int c, void* d, int* e)
{
        /* return (getsockopt(a, b, c, d, e) < 0) ? errno : 0; */
	errno = B_UNSUPPORTED;
	return errno;
}

static int
beos_native_getsockname(int a, struct sockaddr* b, int* c)
{
	return (getsockname(a, b, c) < 0) ? errno : 0;
}

static int
beos_native_getpeername(int a, struct sockaddr* b, int* c)
{
        return (getpeername(a, b, c) < 0) ? errno : 0;
}

static int
beos_native_sockclose(int f)
{
	return (closesocket(f) < 0) ? errno : 0;
}

static int
beos_native_gethostbyname(const char*n, struct hostent**out)
{
	*out = gethostbyname(n);
	return (*out == 0) ? h_errno : 0;
}

static int
beos_native_gethostbyaddr(const char*n, int a, int b, struct hostent**out)
{
	*out = gethostbyaddr(n, a, b);
	return (*out == 0) ? h_errno : 0;
}

static int
beos_native_select(int a, fd_set* b, fd_set* c, fd_set* d, struct timeval* e,
	int* out)
{
	*out = select(a, b, c, d, e);
	return (*out < 0) ? errno : 0;
}

static int
beos_native_forkexec(char *argv[], char *env[], int fd[4], int *outpid)
{
	unimp("forkexec() not implemented in beos-native system");
	return (B_UNSUPPORTED);
}

static int
beos_native_waitpid(int a, int* b, int c, int* out)
{
	unimp("waitpid() not implemented in beos-native system");
	return (B_UNSUPPORTED);
}

static int
beos_native_kill(int a, int b)
{
#if notyet
	return (kill(a, b) < 0) ? errno : 0;
#else
	unimp("kill() not implemented in beos-native system");
	return (B_UNSUPPORTED);
#endif
}

SystemCallInterface Kaffe_SystemCallInterface = {
	beos_native_open,
	beos_native_read,
	beos_native_write, 
	beos_native_lseek,
	beos_native_close,
	beos_native_fstat,
	beos_native_stat,
	beos_native_mkdir,
	beos_native_rmdir,
	beos_native_rename,
	beos_native_remove,
	beos_native_socket,
	beos_native_connect,
	beos_native_bind,
	beos_native_listen,
	beos_native_accept, 
	beos_native_sock_read,	
	beos_native_recvfrom,
	beos_native_write, 
	beos_native_sendto,	
	beos_native_setsockopt,
	beos_native_getsockopt,
	beos_native_getsockname, 
	beos_native_getpeername,
	beos_native_sockclose,
	beos_native_gethostbyname,
	beos_native_gethostbyaddr,
	beos_native_select,	
	beos_native_forkexec,
	beos_native_waitpid,
	beos_native_kill,
};
