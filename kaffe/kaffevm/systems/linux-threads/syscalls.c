/*
 * syscalls.c
 * Definition of jsyscall interface.
 *
 * Copyright (c) 1996, 1997
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */
/*
 * Syscall definitions for linux-threads port.
 */
#include "config.h"
#include "debug.h"
#include "config-std.h"
#include "config-mem.h"
#include "config-io.h"
#include "config-net.h"
#include "config-signal.h"
#include "jthread.h"
#include "jsyscall.h"

#include <sys/wait.h>

static int
linux_thread_open(const char *path, int f, int m, int *outfd)
{
	*outfd = open(path, f, m);
	return (*outfd == -1) ? errno : 0;
}

static int
linux_thread_read(int fd, void* b, size_t l, ssize_t *out)
{
	*out = read(fd, b, l);
	return (*out == -1) ? errno : 0;
}

static int
linux_thread_write(int fd, const void* b, size_t l, ssize_t *out)
{
	*out = write(fd, b, l);
	return (*out == -1) ? errno : 0;
}

static int
linux_thread_lseek(int f, off_t o, int w, off_t *out)
{
	*out = lseek(f, o, w);
	return (*out == -1) ? errno : 0;
}

static int
linux_thread_close(int f)
{
	return (close(f) == -1) ? errno : 0;
}

static int
linux_thread_fstat(int f, struct stat *st)
{
	return (fstat(f, st) == -1) ? errno : 0;
}

static int
linux_thread_stat(const char *p, struct stat *st)
{
	return (stat(p, st) == -1) ? errno : 0;
}

static int
linux_thread_mkdir(const char *p, int m)
{
	return (mkdir(p, m) == -1) ? errno : 0;
}

static int
linux_thread_rmdir(const char *p)
{
	return (rmdir(p) == -1) ? errno : 0;
}

static int
linux_thread_rename(const char *p, const char *q)
{
	return (rename(p, q) == -1) ? errno : 0;
}

static int
linux_thread_remove(const char *p)
{
	return (remove(p) == -1) ? errno : 0;
}

static int
linux_thread_socket(int a, int b, int c, int *outfd)
{
        *outfd = socket(a, b, c); 
	return (*outfd == -1) ? errno : 0;
}

static int
linux_thread_connect(int fd, struct sockaddr* addr, size_t len, int timeout)
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
linux_thread_bind(int fd, struct sockaddr* addr, size_t len)
{
        return (bind(fd, addr, len) == -1) ? errno : 0;
}

static int
linux_thread_listen(int fd, int n)
{
        return (listen(fd, n) == -1) ? errno : 0;
}

static int
linux_thread_accept(int fd, struct sockaddr* a, size_t *l, int timeout, 
	int* outfd)
{
	/* XXX implement timeout!!! */
	*outfd = accept(fd, a, l);
	return (*outfd == -1) ? errno : 0;
}

static int
linux_thread_sock_read(int f, void* b, size_t l, int timeout, ssize_t *out)
{
	/* XXX implement timeout!!! */
	*out = read(f, b, l);
	return (*out == -1) ? errno : 0;
}

static int
linux_thread_recvfrom(int a, void* b, size_t c, int d, struct sockaddr* e, 
	int* f, int timeout, ssize_t *out)
{
	/* XXX implement timeout!!! */
	*out = recvfrom(a, b, c, d, e, f);
	return (*out == -1) ? errno : 0;
}

static int
linux_thread_sendto(int a, const void* b, size_t c, int d, 
	const struct sockaddr* e, int f, ssize_t *out)
{
	*out = sendto(a, b, c, d, e, f);
	return (*out == -1) ? errno : 0;
}

static int
linux_thread_setsockopt(int a, int b, int c, const void* d, int e)
{
        return (setsockopt(a, b, c, d, e) == -1) ? errno : 0;
}

static int
linux_thread_getsockopt(int a, int b, int c, void* d, int* e)
{
        return (getsockopt(a, b, c, d, e) == -1) ? errno : 0;
}

static int
linux_thread_getsockname(int a, struct sockaddr* b, int* c)
{
	return (getsockname(a, b, c) == -1) ? errno : 0;
}

static int
linux_thread_getpeername(int a, struct sockaddr* b, int* c)
{
        return (getpeername(a, b, c) == -1) ? errno : 0;
}

static int
linux_thread_gethostbyname(const char*n, struct hostent**out)
{
	*out = gethostbyname(n);
	return (*out == 0) ? h_errno : 0;
}

static int
linux_thread_gethostbyaddr(const char*n, int a, int b, struct hostent**out)
{
	*out = gethostbyaddr(n, a, b);
	return (*out == 0) ? h_errno : 0;
}

static int
linux_thread_select(int a, fd_set* b, fd_set* c, fd_set* d, struct timeval* e,
	int* out)
{
	*out = select(a, b, c, d, e);
	return (*out == -1) ? errno : 0;
}

static int
linux_thread_forkexec(char *argv[], char *env[], int fd[4], int *outpid)
{
	unimp("forkexec() not implemented in linux-threads system");
	return (EOPNOTSUPP);
}

static int
linux_thread_waitpid(int a, int* b, int c, int* out)
{
	*out = waitpid(a, b, c);
	return (*out == -1) ? errno : 0;	
}

static int
linux_thread_kill(int a, int b)
{
	return (kill(a, b) == -1) ? errno : 0;
}

SystemCallInterface Kaffe_SystemCallInterface = {
        linux_thread_open,
        linux_thread_read,
        linux_thread_write, 
        linux_thread_lseek,
        linux_thread_close,
        linux_thread_fstat,
        linux_thread_stat,
	linux_thread_mkdir,
        linux_thread_rmdir,
        linux_thread_rename,
        linux_thread_remove,
        linux_thread_socket,
        linux_thread_connect,
        linux_thread_bind,
        linux_thread_listen,
	linux_thread_accept, 
        linux_thread_sock_read,	
        linux_thread_recvfrom,
        linux_thread_write, 
        linux_thread_sendto,	
        linux_thread_setsockopt,
        linux_thread_getsockopt,
        linux_thread_getsockname, 
        linux_thread_getpeername,
        linux_thread_close,
	linux_thread_gethostbyname,
	linux_thread_gethostbyaddr,
        linux_thread_select,	
	linux_thread_forkexec,
        linux_thread_waitpid,
	linux_thread_kill,
};
