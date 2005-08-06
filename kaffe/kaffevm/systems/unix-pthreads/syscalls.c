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
#include "lock-impl.h"

#if defined(HAVE_SYS_WAIT_H)
#include <sys/wait.h>
#endif

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
			errno = ETIMEDOUT;		\
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

static 
int
waitForTimeout(int fd, int timeout){
	fd_set rset;
	struct timeval tv;
	int ret;

	FD_ZERO(&rset);
	FD_SET(fd,&rset);
	tv.tv_sec = timeout / 1000;
	tv.tv_usec = (timeout % 1000) * 1000;

	jthread_current()->interrupting = 0;
	if (timeout == NOTIMEOUT) 
		ret = select(fd+1,&rset,NULL,NULL,NULL);
	else	
		ret = select(fd+1,&rset,NULL,NULL,&tv);

	if (ret == 0) 
		errno = ETIMEDOUT;
	else if (ret == -1)
	{
		errno = EINTR;
		jthread_current()->interrupting = 1;
	}

	return (ret);
}

/* These two functions would need to be merged some time later.
 */
static
int waitForWritable(int fd, int timeout)
{
	fd_set wset;
	struct timeval tv;
	int ret;

	FD_ZERO(&wset);
        FD_SET(fd,&wset);
        tv.tv_sec = timeout / 1000;
	tv.tv_usec = (timeout % 1000) * 1000;

	jthread_current()->interrupting = 0;
        if (timeout == NOTIMEOUT)
		ret = select(fd+1,NULL,&wset,NULL,NULL);
	else
		ret = select(fd+1,NULL,&wset,NULL,&tv);

	if (ret == 0)
		errno = ETIMEDOUT;
	else if (ret == -1)
	{
		errno = EINTR;
		jthread_current()->interrupting = 1;
	}

	return (ret);
}

static
jlong
currentTime(void)
{
    struct timeval tm;
    gettimeofday(&tm, NULL);
    return (((jlong)tm.tv_sec * 1000L) + ((jlong)tm.tv_usec / 1000L));
}

/*
 * Threaded socket create.
 */
static int
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

static int
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
jthreadedBind(int fd, struct sockaddr *addr, socklen_t namelen)
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
jthreadedFTruncate(int fd, off_t new_size)
{
	int rc = 0;
	
	if (ftruncate(fd, new_size) == -1) {
		rc = errno;
	}
	return (rc);
}

static int
jthreadedFSync(int fd)
{
	int rc = 0;
	
	if (fsync(fd) == -1) {
		rc = errno;
	}
	return (rc);
}

static int
jthreadedMkdir(const char *path, int mode)
{
	int rc = 0;

	if (mkdir(path, (mode_t)mode) == -1) {
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

	*out = e ? sendto(a, b, c, d, e, (socklen_t)f) : send(a, b, c, d);
	if (*out == -1) {
		rc = errno;
	}
	return (rc);
}

static int
jthreadedSetSockOpt(int a, int b, int c, const void* d, int e)
{
	int rc = 0;

	if (setsockopt(a, b, c, d, (socklen_t)e) == -1) {
		rc = errno;
	}
	return (rc);
}

static int
jthreadedGetSockOpt(int a, int b, int c, void* d, socklen_t* e)
{
	int rc = 0;

	if (getsockopt(a, b, c, d, e) == -1) {
		rc = errno;
	}
	return (rc);
}

static int
jthreadedGetSockName(int a, struct sockaddr* b, socklen_t* c)
{
	int rc = 0;

	if (getsockname(a, b, c) == -1) {
		rc = errno;
	}
	return (rc);
}

static int
jthreadedGetPeerName(int a, struct sockaddr* b, socklen_t* c)
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
		if (rc == 0) {
			*out = (void*)-1;
			rc = errno;
		}
	}
	return (rc);
}

static int
jthreadedGetHostByAddr(const char *host, size_t l, int t, struct hostent** out)
{
	int rc = 0;

	/* NB: same comment as for jthreadedGetHostByName applies here */
	*out = gethostbyaddr(host, l, t);
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
static int
jthreadedConnect(int fd, struct sockaddr* addr, socklen_t len, int timeout)
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
static int
jthreadedAccept(int fd, struct sockaddr* addr, socklen_t* len, 
		int timeout, int* out)
{
	/* absolute time at which time out is reached */
	int r=-1, ret;	
	ret = waitForTimeout(fd,timeout);

	/* If result is 0, we had a timeout. 
	 * If it's not, let's try to accept.
	 */
	if (ret != 0) {
		r = accept(fd,addr,len);
		SET_RETURN_OUT(r, out, r)
		return (r);
	}
	else {
		errno = ETIMEDOUT;
		r = -1;
	}
	
	SET_RETURN_OUT(r, out, r)
	return (r);
}

/*
 * Threaded read with timeout
 */
static int
jthreadedTimedRead(int fd, void* buf, size_t len, int timeout, ssize_t *out)
{
	ssize_t r = -1;
	/* absolute time at which timeout is reached */
	int ret;
	
	ret = waitForTimeout(fd,timeout);
	if (ret > 0) {
		r = read(fd, buf, len);
	}
	
	SET_RETURN_OUT(r, out, r)
	return (r);
}

/*
 * Threaded write with timeout
 */
static int
jthreadedTimedWrite(int fd, const void* buf, size_t len, int timeout, ssize_t *out)
{
	ssize_t r = -1;
	/* absolute time at which timeout is reached */
	int ret;
	
	ret = waitForWritable(fd,timeout);
	if (ret > 0) {
		r = write(fd, buf, len);
	}
	
	SET_RETURN_OUT(r, out, r)
	return (r);
}

/*
 * Threaded read with no time out
 */
static int
jthreadedRead(int fd, void* buf, size_t len, ssize_t *out)
{
	ssize_t r = -1;

	r = read(fd, buf, len);
	SET_RETURN_OUT(r, out, r);
	return (r);
}

/*
 * Threaded write
 */
static int
jthreadedWrite(int fd, const void* buf, size_t len, ssize_t *out)
{
	ssize_t r = 1;
	const void* ptr;

	ptr = buf;

	while (len > 0 && r > 0) {
		r = (ssize_t)write(fd, ptr, len);
		if (r >= 0) {
			ptr = (void *)((uintp)ptr + r);
			len -= r;
			r = (uintp)ptr - (uintp)buf;
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
static int 
jthreadedRecvfrom(int fd, void* buf, size_t len, int flags, 
	struct sockaddr* from, socklen_t* fromlen, int timeout, ssize_t *out)
{
	int r;
	jlong deadline = 0;
	int poll_timeout;
	int blocking = jthread_is_blocking(fd);

	jthread_set_blocking(fd, 0);
	SET_DEADLINE(deadline, timeout)
	for (;;) {
		r = recvfrom(fd, buf, len, flags, from, fromlen);
		if (r >= 0 || !(errno == EWOULDBLOCK || errno == EINTR 
					|| errno == EAGAIN)) {
			break;
		}
		IGNORE_EINTR(r)
		if (timeout != NOTIMEOUT) {
		        poll_timeout = deadline - currentTime();
			if (poll_timeout > 0)
			  waitForTimeout(fd, poll_timeout);
		} else {
			waitForTimeout(fd, NOTIMEOUT);
		}
		BREAK_IF_LATE(deadline, timeout)
	}
	jthread_set_blocking(fd, blocking);
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

static int 
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
	sigset_t nsig, osig;
	char b[1];
	int pid, i, err;

	/* 
	 * we need execve() and fork() for this to work.  Don't bother if
	 * we don't have them.
	 */
#if !defined(HAVE_EXECVE) && !defined(HAVE_EXECVP)
	unimp("neither execve() nor execvp() provided");
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
	sigprocmask(SIG_BLOCK, &nsig, &osig);

	pid = fork();

	switch (pid) {
	case 0:
		/* Child */
		/* set all signals back to their default state */
		for (i = 0; i < NSIG; i++) {
			clearSignal(i);
		}

		/* now reenable interrupts */
		sigprocmask(SIG_UNBLOCK, &nsig, NULL);

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
		sigprocmask(SIG_UNBLOCK, &osig, NULL);
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

		sigprocmask(SIG_UNBLOCK, &osig, NULL);
		*outpid = pid;
		return (0);
	}

	exit(-1);
	/* NEVER REACHED */	
}

/* 
 * Wait for a child process.
 */
static int
jthreadedWaitpid(int wpid, int* status, int options, int *outpid)
{
#if defined(HAVE_WAITPID)
	int npid;
	sigset_t sigdata;

	KaffePThread_setBlockingCall(&sigdata);
	npid = waitpid(wpid, status, options);
	KaffePThread_clearBlockingCall(&sigdata);
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

static int
jthreadedMmap(void **memory, size_t *size, int mode, int fd, off_t *offset)
{
#if defined(HAVE_MMAP)
	size_t pages_sz;
	off_t pages_offset;
	int sysmode, sysflags;
	int rc = 0;

	pages_sz = (*size)/getpagesize();
	*size = (pages_sz+1)*getpagesize();
  
	pages_offset = (*offset)/getpagesize();
	*offset = pages_offset*getpagesize();

	switch (mode) {
		case KAFFE_MMAP_READ:
			sysflags = MAP_SHARED;
			sysmode = PROT_READ;
			break;
		case KAFFE_MMAP_WRITE:
			sysflags = MAP_SHARED;
			sysmode = PROT_WRITE | PROT_READ;
			break;
		case KAFFE_MMAP_PRIVATE:
			sysflags = MAP_PRIVATE;
			sysmode = PROT_WRITE | PROT_READ;
			break;
		default:
			return -EINVAL;
	}

	*memory = mmap(*memory, *size, sysmode, sysflags, fd, *offset);

	return (rc);
#else
	return (ENOTSUP);
#endif
}


static int
jthreadedMunmap(void *memory, size_t size)
{
#if defined(HAVE_MMAP)
	int rc = 0;
	
	if (munmap(memory, size) < 0) {
		rc = errno;
	}
	return (rc);
#else
	return (ENOTSUP);
#endif
}

static int
jthreadedMsync(void *memory, size_t size)
{
#if defined(HAVE_MMAP)
        int rc = 0;

        memory = (void *)(((size_t)memory/getpagesize()) * getpagesize());
        size += getpagesize();
        /* TODO: Try not to freeze the entire VM. */
        if (msync(memory, size, MS_SYNC | MS_INVALIDATE) < 0) {
            rc = errno;
        }

        return rc;
#else
        return (ENOTSUP);
#endif
}

static int
jthreadedPipeCreate(int *read_fd, int *write_fd)
{
	int pairs[2];

	assert(read_fd != NULL);
	assert(write_fd != NULL);

	if (pipe(pairs) < 0)
		return errno;

	*read_fd = pairs[0];
	*write_fd = pairs[1];

	return 0;
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
        jthreadedFTruncate,
        jthreadedFSync,
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
        jthreadedKill,
        jthreadedMmap,
        jthreadedMunmap,
	jthreadedMsync,
	jthreadedPipeCreate,
	jthreadedTimedRead,
	jthreadedTimedWrite
};
