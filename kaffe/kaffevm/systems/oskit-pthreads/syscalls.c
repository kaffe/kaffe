/*
 * syscalls.c
 * Definition of jsyscall interface for oskit-pthreads.
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
/*
 * Syscall definitions for OSKit pthreads port.
 */

#include "config.h"
#include "debug.h"
#include "config-std.h"
#include "config-mem.h"
#include "config-net.h"
#include "config-io.h"
#include "config-signal.h"
#include "jthread.h"
#include "jsyscall.h"

static int
oskit_pthread_open(const char *path, int f, int m, int *outfd)
{
	*outfd = open(path, f, m);
	return (*outfd == -1) ? errno : 0;
}

static int
oskit_pthread_read(int fd, void* b, size_t l, ssize_t *out)
{
	*out = read(fd, b, l);
	return (*out == -1) ? errno : 0;
}

static int
oskit_pthread_write(int fd, const void* b, size_t l, ssize_t *out)
{
	*out = write(fd, b, l);
	return (*out == -1) ? errno : 0;
}

static int
oskit_pthread_lseek(int f, off_t o, int w, off_t *out)
{
	*out = lseek(f, o, w);
	return (*out == -1) ? errno : 0;
}

static int
oskit_pthread_close(int f)
{
	return (close(f) == -1) ? errno : 0;
}

static int
oskit_pthread_fstat(int f, struct stat *st)
{
	return (fstat(f, st) == -1) ? errno : 0;
}

static int
oskit_pthread_stat(const char *p, struct stat *st)
{
	return (stat(p, st) == -1) ? errno : 0;
}

static int
oskit_pthread_mkdir(const char *p, int m)
{
	return (mkdir(p, m) == -1) ? errno : 0;
}

static int
oskit_pthread_rmdir(const char *p)
{
	return (rmdir(p) == -1) ? errno : 0;
}

static int
oskit_pthread_rename(const char *p, const char *q)
{
	return (rename(p, q) == -1) ? errno : 0;
}

static int
oskit_pthread_remove(const char *p)
{
	return (remove(p) == -1) ? errno : 0;
}

static int
oskit_pthread_socket(int a, int b, int c, int *outfd)
{
        *outfd = socket(a, b, c); 
	return (*outfd == -1) ? errno : 0;
}

static int
oskit_pthread_connect(int fd, struct sockaddr* addr, size_t len, int timeout)
{
	/* XXX implement timeout */
        int r = connect(fd, addr, len);
        /* annul EALREADY error --- is this really necessary or is this
	 * a java.net bug? 
	 */
        if (r == -1 && errno == EALREADY) {
                r = 0;
	}
        return (r);
}

static int
oskit_pthread_bind(int fd, struct sockaddr* addr, size_t len)
{
        return (bind(fd, addr, len) == -1) ? errno : 0;
}

static int
oskit_pthread_listen(int fd, int n)
{
        return (listen(fd, n) == -1) ? errno : 0;
}

static int
oskit_pthread_accept(int fd, struct sockaddr* a, size_t *l, int timeout, 
	int* outfd)
{
	/* XXX implement timeout!!! */
	*outfd = accept(fd, a, l);
	return (*outfd == -1) ? errno : 0;
}

static int
oskit_pthread_sock_read(int f, void* b, size_t l, int timeout, ssize_t *out)
{
	/* XXX implement timeout!!! */
	*out = read(f, b, l);
	return (*out == -1) ? errno : 0;
}

static int
oskit_pthread_recvfrom(int a, void* b, size_t c, int d, struct sockaddr* e, 
	int* f, int timeout, ssize_t *out)
{
	/* XXX implement timeout!!! */
	*out = recvfrom(a, b, c, d, e, f);
	return (*out == -1) ? errno : 0;
}

static int
oskit_pthread_sendto(int a, const void* b, size_t c, int d, 
	const struct sockaddr* e, int f, ssize_t *out)
{
	*out = sendto(a, b, c, d, e, f);
	return (*out == -1) ? errno : 0;
}

static int
oskit_pthread_setsockopt(int a, int b, int c, const void* d, int e)
{
        return (setsockopt(a, b, c, d, e) == -1) ? errno : 0;
}

static int
oskit_pthread_getsockopt(int a, int b, int c, void* d, int* e)
{
        return (getsockopt(a, b, c, d, e) == -1) ? errno : 0;
}

static int
oskit_pthread_getsockname(int a, struct sockaddr* b, int* c)
{
	return (getsockname(a, b, c) == -1) ? errno : 0;
}

static int
oskit_pthread_getpeername(int a, struct sockaddr* b, int* c)
{
        return (getpeername(a, b, c) == -1) ? errno : 0;
}

static int
oskit_pthread_gethostbyname(const char*n, struct hostent**out)
{
	*out = gethostbyname(n);
	return (*out == 0) ? h_errno : 0;
}

static int
oskit_pthread_gethostbyaddr(const char*n, int a, int b, struct hostent**out)
{
	*out = gethostbyaddr(n, a, b);
	return (*out == 0) ? h_errno : 0;
}

static int
oskit_pthread_select(int a, fd_set* b, fd_set* c, fd_set* d, struct timeval* e,
	int* out)
{
	*out = select(a, b, c, d, e);
	return (*out == -1) ? errno : 0;
}

static int
oskit_pthread_forkexec(char *argv[], char *env[], int fd[4], int *outpid)
{
	unimp("forkexec() not implemented in linux-threads system");
	return (EOPNOTSUPP);
}

static int
oskit_pthread_waitpid(int a, int* b, int c, int* out)
{
	unimp("waitpid() not implemented in OSKit");
	return (EOPNOTSUPP);
}

static int
oskit_pthread_kill(int a, int b)
{
#if notyet
	return (kill(a, b) == -1) ? errno : 0;
#else
	unimp("kill() not implemented in OSKit");
	return (EOPNOTSUPP);
#endif
}

SystemCallInterface Kaffe_SystemCallInterface = {
	oskit_pthread_open,
	oskit_pthread_read,
	oskit_pthread_write, 
	oskit_pthread_lseek,
	oskit_pthread_close,
	oskit_pthread_fstat,
	oskit_pthread_stat,
	oskit_pthread_mkdir,
	oskit_pthread_rmdir,
	oskit_pthread_rename,
	oskit_pthread_remove,
	oskit_pthread_socket,
	oskit_pthread_connect,
	oskit_pthread_bind,
	oskit_pthread_listen,
	oskit_pthread_accept, 
	oskit_pthread_sock_read,	
	oskit_pthread_recvfrom,
	oskit_pthread_write, 
	oskit_pthread_sendto,	
	oskit_pthread_setsockopt,
	oskit_pthread_getsockopt,
	oskit_pthread_getsockname, 
	oskit_pthread_getpeername,
	oskit_pthread_close,
	oskit_pthread_gethostbyname,
	oskit_pthread_gethostbyaddr,
	oskit_pthread_select,	
	oskit_pthread_forkexec,
	oskit_pthread_waitpid,
	oskit_pthread_kill,
};
