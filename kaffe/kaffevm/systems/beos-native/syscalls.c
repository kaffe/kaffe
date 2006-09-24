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

/*
 * Derived for BeOS R4 from oskit-pthreads/syscalls.c
 * Please report problems to alanlb@cs.vt.edu.
 */

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

#include <sys/wait.h>
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
beos_native_ftruncate(int fd, off_t new_size)
{
	return (ftruncate(fd, new_size) < 0) ? errno : 0;
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
beos_native_socket(int a, int b, int c, int *outsock)
{
        *outsock = socket(a, b, c); 
	return (*outsock < 0) ? errno : 0;
}

static int
beos_native_connect(int sock, struct sockaddr* addr, size_t len, int timeout)
{
	/* XXX implement timeout */
        int r = connect(sock, addr, len);
        /* annul EISCONN error --- is this really necessary or is this
	 * a java.net bug? 
	 */
        if (r < 0 && errno == EISCONN) {
                r = 0;
	}
        return (r);
}

static int
beos_native_bind(int sock, struct sockaddr* addr, size_t len)
{
        return (bind(sock, addr, len) < 0) ? errno : 0;
}

static int
beos_native_listen(int sock, int n)
{
	/* BUGFIX: BeOS doesn't default to 5 conns ala Unix if n == 0 */
	if (0 == n) n = 5;

        return (listen(sock, n) < 0) ? errno : 0;
}

static int
beos_native_accept(int sock, struct sockaddr* a, size_t *l, int timeout, 
	int* outsock)
{
	int cli_size;

	if (timeout > 0) {
		struct timeval tv;
		struct fd_set fds;
		int rc;

		tv.tv_sec = timeout / 1000;
		tv.tv_usec = (timeout % 1000) * 1000L;

		FD_ZERO(&fds);
		FD_SET(sock, &fds);

		rc = select(sock+1, &fds, NULL, NULL, &tv);
		if (rc < 0) {
			return errno;
		}
		else if (0 == rc) {
			errno = EINTR;
			return errno;
		}
	}

	cli_size = *l;
	*outsock = accept(sock, a, &cli_size);
	*l = cli_size;
	return (*outsock < 0) ? errno : 0;
}

static int
beos_native_sock_read(int sock, void* b, size_t l, int timeout, ssize_t *out)
{
	if (timeout > 0) {
		struct timeval tv;
		struct fd_set fds;
		int rc;

		tv.tv_sec = timeout / 1000;
		tv.tv_usec = (timeout % 1000) * 1000L;

		FD_ZERO(&fds);
		FD_SET(sock, &fds);

		rc = select(sock+1, &fds, NULL, NULL, &tv);
		if (rc < 0) {
			return errno;
		}
		else if (0 == rc) {
			errno = EINTR;
			return errno;
		}
	}

	*out = recv(sock, b, l, 0);
	return (*out < 0) ? errno : 0;
}

static int
beos_native_recvfrom(int sock, void* b, size_t c, int d, struct sockaddr* e, 
	int* f, int timeout, ssize_t *out)
{
	if (d & MSG_PEEK) {
		errno = B_UNSUPPORTED;
		*out = errno;
	}
	else {
		if (timeout > 0) {
			struct timeval tv;
			struct fd_set fds;
			int rc;

			tv.tv_sec = timeout / 1000;
			tv.tv_usec = (timeout % 1000) * 1000L;

			FD_ZERO(&fds);
			FD_SET(sock, &fds);

			rc = select(sock+1, &fds, NULL, NULL, &tv);
			if (rc < 0) {
				return errno;
			}
			else if (0 == rc) {
				errno = EINTR;
				return errno;
			}
		}

		*out = recvfrom(sock, b, c, d, e, f);
	}
	return (*out < 0) ? errno : 0;
}

static int
beos_native_sock_write(int f, const void* b, size_t l, ssize_t *out)
{
	*out = send(f, b, l, 0);
	return (*out < 0) ? errno : 0;
}

static int
beos_native_sendto(int a, const void* b, size_t c, int d, 
	const struct sockaddr* e, int f, ssize_t *out)
{
	*out = e ? sendto(a, b, c, d, e, f) : send(a, b, c, d);
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
	int rc = 0;

	/* XXX gethostbyname is not thread-safe! */
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
beos_native_gethostbyaddr(const char*n, int a, int b, struct hostent**out)
{
	int rc = 0;

	/* XXX gethostbyaddr is not thread-safe! */
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
beos_native_select(int a, fd_set* b, fd_set* c, fd_set* d, struct timeval* e,
	int* out)
{
	*out = select(a, b, c, d, e);
	return (*out < 0) ? errno : 0;
}

/* helper function for forkexec, close fd[0..n-1] */
static void
close_fds(int fd[], int n)
{
	int i = 0;
	while (i < n) {
		close(fd[i++]);
	}
}

static int
beos_native_forkexec(char *argv[], char *env[],
	int ioes[4], int *outpid, const char *dir)
{
/* Adapted from unix-jthreads/jthread.c */
/* these defines are indices in ioes */
#define IN_IN		0
#define IN_OUT		1
#define OUT_IN		2
#define OUT_OUT		3
#define ERR_IN		4
#define ERR_OUT		5
#define SYNC_IN		6
#define SYNC_OUT	7

	int fds[8];
	int nfd;		/* number of fds in `fds' that are valid */
	sigset_t nsig;
	char b[1];
	int pid, i, err;

	/* 
	 * we need execve() and fork() for this to work.  Don't bother if
	 * we don't have them.
	 */
#if !defined(HAVE_EXECVE) && !defined(HAVE_EXECVP)
	unimp("neither execve() nor execvp() not provided");
#endif
#if !defined(HAVE_FORK)
	unimp("fork() not provided");
#endif

DBG(JTHREAD,	
	{
		char **d = argv;
		dprintf("argv = [`%s ", *d++); 
		while (*d)
			dprintf(", `%s'", *d++);
		dprintf("]\n");
	}
    )
	/* Create the pipes to communicate with the child */
	/* Make sure fds get closed if we can't create all pipes */
	for (nfd = 0; nfd < 8; nfd += 2) {
		int e;
		err = pipe(fds + nfd);
		e = errno;
		if (err == -1) {
			close_fds(fds, nfd);
			return (e);
		}
	}

	/* 
	 * We must avoid that the child dies because of SIGVTALRM or
	 * other signals.  We disable interrupts before forking and then
	 * reenable signals in the child after we cleaned up.
	 */
	sigfillset(&nsig);
	sigprocmask(SIG_BLOCK, &nsig, 0);

	pid = fork();

	switch (pid) {
	case 0:
		/* Child */
		/* set all signals back to their default state */
		for (i = 0; i < NSIG; i++) {
			signal(i, SIG_DFL);
		}

		/* now reenable interrupts */
		sigprocmask(SIG_UNBLOCK, &nsig, 0);

		/* set stdin, stdout, and stderr up from the pipes */
		dup2(fds[IN_IN], 0);
		dup2(fds[OUT_OUT], 1);
		dup2(fds[ERR_OUT], 2);

		/* What is sync about anyhow?  Well my current guess is that
		 * the parent writes a single byte to it when it's ready to
		 * proceed.  So here I wait until I get it before doing
		 * anything.
		 */
		/* note that this is a blocking read */
		read(fds[SYNC_IN], b, sizeof(b));

		/* now close all pipe fds */
		close_fds(fds, 8);

		/* change working directory */
#if defined(HAVE_CHDIR)
		(void)chdir(dir);
#endif

		/*
		 * If no environment was given and we have execvp, we use it.
		 * If an environment was given, we use execve.
		 * This is roughly was the linux jdk seems to do.
		 */

		/* execute program */
#if defined(HAVE_EXECVP)
		if (env == NULL)
			execvp(argv[0], argv);
		else
#endif
			execve(argv[0], argv, env);
		break;

	case -1:
		/* Error */
		err = errno;
		/* Close all pipe fds */
		close_fds(fds, 8);
		sigprocmask(SIG_UNBLOCK, &nsig, 0);
		return (err);

	default:
		/* Parent */
		/* close the fds we won't need */
		close(fds[IN_IN]);
		close(fds[OUT_OUT]);
		close(fds[ERR_OUT]);
		close(fds[SYNC_IN]);

		/* copy and fix up the fds we do need */
		ioes[0] = fds[IN_OUT];
		ioes[1] = fds[OUT_IN];
		ioes[2] = fds[ERR_IN];
		ioes[3] = fds[SYNC_OUT];

		sigprocmask(SIG_UNBLOCK, &nsig, 0);
		*outpid = pid;
		return (0);
	}

	exit(-1);
	/* NEVER REACHED */	
}

static int
beos_native_waitpid(int a, int* b, int c, int* out)
{
	*out = waitpid(a, b, 0);
	return (*out < 0) ? errno : 0;
}

static int
beos_native_kill(int a, int b)
{
	return (send_signal(a, b) < 0) ? errno : 0;
}

static int
beos_native_mmap(void **memory, size_t *size, int mode, int fd, off_t *offset)
{
	return B_UNSUPPORTED;
}

static int
beos_native_munmap(void *memory, size_t size)
{
	return B_UNSUPPORTED;
}

SystemCallInterface Kaffe_SystemCallInterface = {
	beos_native_open,
	beos_native_read,
	beos_native_write, 
	beos_native_lseek,
	beos_native_close,
	beos_native_fstat,
	beos_native_stat,
	beos_native_ftruncate,
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
	beos_native_sock_write, 
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
	beos_native_mmap,
	beos_native_munmap
};
