/*
 * syscalls.c
 *
 * Copyright (c) 1996, 1997
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

#include "config.h"
#include "debug.h"
#include "config-std.h"
#include "config-mem.h"
#include "config-io.h"
#include "config-signal.h"
#include "config-net.h"
#include "gtypes.h"
#include "jsyscall.h"
#include "jsignal.h"
#include "nets.h"
#if defined(HAVE_SYS_WAIT_H)
#include <sys/wait.h>
#endif

#define NOTIMEOUT	0

/*
 * We currently don't support exec'ing external processes.  While that
 * is easy to do, it unclear how we can wait for the process to exit since
 * it must be done from the same thread that created it.  This might not
 * be the case.
 * It's not clear how best to achieve this - TIM 10/17/99
 */
#undef	HAVE_FORK
#undef	HAVE_WAITPID

/*
 * various building blocks for timeout system call functions
 */
#define SET_DEADLINE(deadline, timeout) 		\
	if (timeout != NOTIMEOUT) {			\
		deadline = timeout + currentTime();	\
	}

#define BREAK_IF_LATE(deadline, timeout)		\
	if (timeout != NOTIMEOUT) {			\
		if (currentTime() >= deadline) {	\
			errno = EINTR;			\
			break;				\
		}					\
	}

#define IGNORE_EINTR(r)					\
	if (r == -1 && errno == EINTR) {		\
		continue;				\
	}

#define SET_RETURN(r)					\
	if (r == -1) {					\
		r = errno;				\
	} 

#define SET_RETURN_OUT(r, out, ret)			\
	if (r == -1) {					\
		r = errno;				\
	} else {					\
		*out = ret;				\
		r = 0;					\
	}

#define CALL_BLOCK_ON_FILE(A, B, C)				\
	if (blockOnFile(A, B, C)) {				\
		/* interrupted via jthread_interrupt() */ 	\
		errno = EINTR; 					\
		break;						\
	}

static
jlong
currentTime(void)
{
    struct timeval tm;
    gettimeofday(&tm, 0);
    return (((jlong)tm.tv_sec * 1000L) + ((jlong)tm.tv_usec / 1000L));
}

/*
 * Threaded socket create.
 */
int
jthreadedSocket(int af, int type, int proto, int *out)
{
        int r;

        r = socket(af, type, proto);
        if (r == -1) {
                r = errno;
        } else {
                *out = r;
                r = 0;
        }
        return (r);
}

int
jthreadedOpen(const char* path, int flags, int mode, int *out)
{
        int r;

        /* Cygnus WinNT requires this */
        r = open(path, flags|O_BINARY, mode);
        if (r == -1) {
                r = errno;
        } else {
                *out = r;
                r = 0;
        }
        return (r);
}

static int
jthreadedClose(int fd)
{
	int rc = 0;

	if (close(fd) == -1) {
		rc = errno;
	}
	return (rc);
}

static int
jthreadedListen(int fd, int l)
{
	int rc = 0;

	if (listen(fd, l) == -1) {
		rc = errno;
	}
	return (rc);
}

static int
jthreadedKill(int pid, int sig)
{
	int rc = 0;

	if (kill(pid, sig) == -1) {
		rc = errno;
	}
	return (rc);
}

static int
jthreadedBind(int fd, struct sockaddr *addr, int namelen)
{
	int rc = 0;

	if (bind(fd, addr, namelen) == -1) {
		rc = errno;
	}
	return (rc);
}

static int
jthreadedLSeek(int fd, off_t offset, int whence, off_t *out)
{
	int rc = 0;

	*out = lseek(fd, offset, whence);
	if (*out == -1) {
		rc = errno;
	}
	return (rc);
}

static int
jthreadedFStat(int fd, struct stat *sb)
{
	int rc = 0;

	if (fstat(fd, sb) == -1) {
		rc = errno;
	}
	return (rc);
}

static int
jthreadedStat(const char* path, struct stat *sb)
{
	int rc = 0;

	if (stat(path, sb) == -1) {
		rc = errno;
	}
	return (rc);
}

static int
jthreadedMkdir(const char *path, int mode)
{
	int rc = 0;

	if (mkdir(path, mode) == -1) {
		rc = errno;
	}
	return (rc);
}

static int
jthreadedRmdir(const char *path)
{
	int rc = 0;

	if (rmdir(path) == -1) {
		rc = errno;
	}
	return (rc);
}

static int
jthreadedRename(const char *path1, const char *path2)
{
	int rc = 0;

	if (rename(path1, path2) == -1) {
		rc = errno;
	}
	return (rc);
}

static int
jthreadedRemove(const char *entry)
{
	int rc = 0;

	if (remove(entry) == -1) {
		rc = errno;
	}
	return (rc);
}

static int     
jthreadedSendto(int a, const void* b, size_t c, int d, const struct sockaddr* e,
		int f, ssize_t *out)
{
	int rc = 0;

	if ((*out = sendto(a, b, c, d, e, f)) == -1) {
		rc = errno;
	}
	return (rc);
}

static int
jthreadedSetSockOpt(int a, int b, int c, const void* d, int e)
{
	int rc = 0;

	if (setsockopt(a, b, c, d, e) == -1) {
		rc = errno;
	}
	return (rc);
}

static int
jthreadedGetSockOpt(int a, int b, int c, void* d, int* e)
{
	int rc = 0;

	if (getsockopt(a, b, c, d, e) == -1) {
		rc = errno;
	}
	return (rc);
}

static int
jthreadedGetSockName(int a, struct sockaddr* b, int* c)
{
	int rc = 0;

	if (getsockname(a, b, c) == -1) {
		rc = errno;
	}
	return (rc);
}

static int
jthreadedGetPeerName(int a, struct sockaddr* b, int* c)
{
	int rc = 0;

	if (getpeername(a, b, c) == -1) {
		rc = errno;
	}
	return (rc);
}

static int
jthreadedGetHostByName(const char *host, struct hostent** out)
{
	int rc = 0;

	/* NB: this will block the whole process while we're looking up
	 * a name.  However, gethostbyname is extremely async-signal-unsafe,
	 * and hence we have no other choice.  Now I understand why old 
	 * Netscapes blocked when looking up a name.  
	 *
	 * In a UNIXy system, a possible solution is to spawn a process
	 * for lookups, which I think is what NS eventually did.
	 */
	*out = gethostbyname(host);
	if (*out == 0) {
		rc = h_errno;
	}
	return (rc);
}

static int
jthreadedGetHostByAddr(const char *host, int l, int t, struct hostent** out)
{
	int rc = 0;

	/* NB: same comment as for jthreadedGetHostByName applies here */
	*out = gethostbyaddr(host, l, t);
	if (*out == 0) {
		rc = h_errno;
	}
	return (rc);
}

static int
jthreadedSelect(int a, fd_set* b, fd_set* c, fd_set* d, 
		struct timeval* e, int* out)
{
	int rc = 0;

	if ((*out = select(a, b, c, d, e)) == -1) {
		rc = errno;
	}
	return (rc);
}

/*
 * Threaded socket connect.
 */
int
jthreadedConnect(int fd, struct sockaddr* addr, int len, int timeout)
{
	int r;
	jlong deadline = 0;
	int inProgress = 0;

	SET_DEADLINE(deadline, timeout)
	for (;;) {
		r = connect(fd, addr, len);
		if (r == 0 || !(errno == EINPROGRESS 
				|| errno == EINTR || errno == EISCONN)) {
			break;	/* success or real error */
		}
		if (r == -1 && errno == EISCONN) {
			/* On Solaris 2.5, after getting EINPROGRESS
			   from a non-blocking connect request, we
			   won't ever get success.  When we're waken
			   up, we'll either get EISCONN, which should
			   be taken as success, or a real failure.
			   However, we can't map EISCONN to success
			   inconditionally, because attempting to
			   connect the same socket again should raise
			   an exception.

			   Mapping EISCONN to success might lead to
			   false positives if connect fails and
			   another thread succeeds to connect this
			   socket before this one is waken up.  Let's
			   just hope it doesn't happen for now.  */
			if (inProgress) {
				r = 0;
			}
			break;
		} else if (r == -1 && errno == EINPROGRESS) {
			inProgress = 1;
		}
		IGNORE_EINTR(r)
		BREAK_IF_LATE(deadline, timeout)
	}
	SET_RETURN(r)
	return (r);
}

/*
 * Threaded socket accept.
 */
int
jthreadedAccept(int fd, struct sockaddr* addr, int* len, 
		int timeout, int* out)
{
	/* absolute time at which time out is reached */
	jlong deadline = 0;
	int r;

	SET_DEADLINE(deadline, timeout)
	for (;;) {
		r = accept(fd, addr, len);
		if (r >= 0 || !(errno == EWOULDBLOCK || errno == EINTR 
				|| errno == EAGAIN)) {
			break;	/* success or real error */
		}
		IGNORE_EINTR(r)
		BREAK_IF_LATE(deadline, timeout)
	}
	SET_RETURN_OUT(r, out, r)
	return (r);
}

/*
 * Threaded read with timeout
 */
int
jthreadedTimedRead(int fd, void* buf, size_t len, int timeout, ssize_t *out)
{
	ssize_t r = -1;
	/* absolute time at which timeout is reached */
	jlong deadline = 0;

	assert(timeout >= 0);
	SET_DEADLINE(deadline, timeout)
	for (;;) {
		r = read(fd, buf, len);
		if (r >= 0 || !(errno == EWOULDBLOCK || errno == EINTR 
				|| errno == EAGAIN)) {
			break;	/* real error or success */
		}
		IGNORE_EINTR(r)
		BREAK_IF_LATE(deadline, timeout)
	}
	SET_RETURN_OUT(r, out, r)
	return (r);
}

/*
 * Threaded read with no time out
 */
int
jthreadedRead(int fd, void* buf, size_t len, ssize_t *out)
{
	return (jthreadedTimedRead(fd, buf, len, NOTIMEOUT, out));
}

/*
 * Threaded write
 */
int
jthreadedWrite(int fd, const void* buf, size_t len, ssize_t *out)
{
	ssize_t r = 1;
	const void* ptr;

	ptr = buf;

	while (len > 0 && r > 0) {
		r = (ssize_t)write(fd, ptr, len);
		if (r >= 0) {
			ptr += r;
			len -= r;
			r = ptr - buf;
			continue;
		}
		if (errno == EINTR) {
			/* ignore */
			r = 1;
			continue;
		}
		if (!(errno == EWOULDBLOCK || errno == EAGAIN)) {
			/* real error */
			break;
		}
		r = 1;
	}
	SET_RETURN_OUT(r, out, r)
	return (r); 
}

/*
 * Threaded recvfrom 
 */
int 
jthreadedRecvfrom(int fd, void* buf, size_t len, int flags, 
	struct sockaddr* from, int* fromlen, int timeout, ssize_t *out)
{
	int r;
	jlong deadline = 0;
 
	SET_DEADLINE(deadline, timeout)
	for (;;) {
		r = recvfrom(fd, buf, len, flags, from, fromlen);
		if (r >= 0 || !(errno == EWOULDBLOCK || errno == EINTR 
					|| errno == EAGAIN)) {
			break;
		}
		IGNORE_EINTR(r)
		BREAK_IF_LATE(deadline, timeout)
	}
	SET_RETURN_OUT(r, out, r)
	return (r);
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

int 
jthreadedForkExec(char **argv, char **arge,
	int ioes[4], int *outpid, const char *dir)
{
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
			clearSignal(i);
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
		if (arge == NULL)
			execvp(argv[0], argv);
		else
#endif
			execve(argv[0], argv, arge);
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

/* 
 * Wait for a child process.
 */
int
jthreadedWaitpid(int wpid, int* status, int options, int *outpid)
{
#if defined(HAVE_WAITPID)
	int npid;

	npid = waitpid(wpid, status, options);
	if (npid > 0) {
		*outpid = npid;
		return (0);
	}
	else {
		return (-1);
	}
#else
	return (-1);
#endif
}

/*
 * The syscall interface as provided by the internal jthread system.
 */
SystemCallInterface Kaffe_SystemCallInterface = {
        jthreadedOpen,
        jthreadedRead,	
        jthreadedWrite, 
        jthreadedLSeek,
        jthreadedClose,
        jthreadedFStat,
        jthreadedStat,
        jthreadedMkdir,
        jthreadedRmdir,
        jthreadedRename,
        jthreadedRemove,
        jthreadedSocket,
        jthreadedConnect,
        jthreadedBind,
        jthreadedListen,
        jthreadedAccept, 
        jthreadedTimedRead,	
        jthreadedRecvfrom,
        jthreadedWrite, 
        jthreadedSendto,	
        jthreadedSetSockOpt,	
        jthreadedGetSockOpt,
        jthreadedGetSockName, 
        jthreadedGetPeerName, 
        jthreadedClose,
        jthreadedGetHostByName,
        jthreadedGetHostByAddr,
        jthreadedSelect,	
        jthreadedForkExec,
        jthreadedWaitpid,
        jthreadedKill
};
