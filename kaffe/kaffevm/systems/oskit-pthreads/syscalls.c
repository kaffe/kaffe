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

#include <oskit/time.h>
#include <oskit/com/listener.h>
#include <oskit/dev/clock.h>
#include <oskit/dev/timer.h>

extern oskit_clock_t *oskit_system_clock;

#ifdef newer_than_990722
/* 
 * The oskit 990722 snapshot has a bug at the intersection of
 * oskit_create_threaded_listener and pthread_mutex_init.  Until we
 * (utah) release a fixed version of the oskit, disable the timeout
 * event listeners.
 */

/*
 * Interrupt a network system call.  It is OK for the timer to expire
 * just after the call returns, the SIG_INT handler does nothing.  The
 * timer will not fire during a subsequent call, since it is destroyed
 * on the way out.
 */
static oskit_error_t
interrupt_me(struct oskit_iunknown *me, void *_tid)
{
	pthread_kill((pthread_t) _tid, SIG_INT);
	return 0;
}

/*
 * Use oskit timers rather than machine timers, since there is an
 * unlimited supply of them.  We create and destroy a timer for every
 * call, is it worthwhile to cache a timer for each thread?
 */
static oskit_timer_t *
make_net_timer(int ms)
{
	oskit_timer_t *timer;
	oskit_listener_t *listener;
	oskit_itimerspec_t spec;
	/* XXX: oskit snapshot 990722 was missing a prototype */
	extern oskit_listener_t *oskit_create_threaded_listener
		(oskit_listener_callback_t, void *);

	memset(&spec, 0, sizeof(spec));
	spec.it_value.tv_sec = ms/1000;
	spec.it_value.tv_nsec = (ms % 1000) * 1000000;
	listener = oskit_create_threaded_listener(interrupt_me,
						  (void *) pthread_self());
	oskit_clock_createtimer(oskit_system_clock, &timer);
	oskit_timer_setlistener(timer, listener);
	oskit_listener_release(listener);
	oskit_timer_settime(timer, 0, &spec);
	return timer;
}

#define WITH_TIMEOUT(MS,EXP)			\
{						\
    oskit_timer_t *_timer = 0;			\
    int _r;					\
						\
    if (MS) _timer = make_net_timer(MS);	\
    _r = (EXP);					\
    if (_timer) oskit_timer_release(_timer);	\
    return _r;					\
}
#else
#define WITH_TIMEOUT(_,EXP) return EXP
#endif

/* return error code or 0 */   
#define ERR(EXP) (((EXP) < 0) ? errno : 0)

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
oskit_pthread_ftruncate(int fd, off_t new_size)
{
	return (ftruncate(fd, new_size) == -1) ? errno : 0;
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
	WITH_TIMEOUT(timeout, ERR(connect(fd, addr, len)));
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
	WITH_TIMEOUT(timeout, ERR(*outfd = accept(fd, a, l)));
}

static int
oskit_pthread_sock_read(int f, void* b, size_t l, int timeout, ssize_t *out)
{
	WITH_TIMEOUT(timeout, ERR(*out = read(f, b, l)));
}

static int
oskit_pthread_recvfrom(int a, void* b, size_t c, int d, struct sockaddr* e, 
	socklen_t* f, int timeout, ssize_t *out)
{
	WITH_TIMEOUT(timeout, ERR(*out = recvfrom(a, b, c, d, e, f)));
}

static int
oskit_pthread_sendto(int a, const void* b, size_t c, int d, 
	const struct sockaddr* e, int f, ssize_t *out)
{
	*out = e ? sendto(a, b, c, d, e, f) : send(a, b, c, d);
	return (*out == -1) ? errno : 0;
}

static int
oskit_pthread_setsockopt(int a, int b, int c, const void* d, int e)
{
        return (setsockopt(a, b, c, d, e) == -1) ? errno : 0;
}

static int
oskit_pthread_getsockopt(int a, int b, int c, void* d, socklen_t* e)
{
        return (getsockopt(a, b, c, d, e) == -1) ? errno : 0;
}

static int
oskit_pthread_getsockname(int a, struct sockaddr* b, socklen_t* c)
{
	return (getsockname(a, b, c) == -1) ? errno : 0;
}

static int
oskit_pthread_getpeername(int a, struct sockaddr* b, socklen_t* c)
{
        return (getpeername(a, b, c) == -1) ? errno : 0;
}

static int
oskit_pthread_gethostbyname(const char*n, struct hostent**out)
{
	int rc = 0;

	*out = gethostbyname(n);
	if (*out == 0) {
		rc = h_errno;
		if (rc == 0) {
			*out = (void*)-1;
			rc = errno;
		}
	}
	return (rc);
}

static int
oskit_pthread_gethostbyaddr(const char*n, int a, int b, struct hostent**out)
{
	int rc = 0;

	*out = gethostbyaddr(n, a, b);
	if (*out == 0) {
		rc = h_errno;
		if (rc == 0) {
			*out = (void*)-1;
			rc = errno;
		}
	}
	return (rc);
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
	unimp("forkexec() not implemented in OSKit");
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

static int
oskit_pthread_mmap(void **memory, size_t *size, )
{
	return (EOPNOTSUPP);
}

static int
oskit_pthread_munmap(void *memory, size_t size)
{
	return (EOPNOTSUPP);
}

SystemCallInterface Kaffe_SystemCallInterface = {
	oskit_pthread_open,
	oskit_pthread_read,
	oskit_pthread_write, 
	oskit_pthread_lseek,
	oskit_pthread_close,
	oskit_pthread_fstat,
	oskit_pthread_stat,
	oskit_pthread_ftruncate,
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
	oskit_pthread_mmap,
	oskit_pthread_munmap
};
