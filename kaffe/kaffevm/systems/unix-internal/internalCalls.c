/*
 * internalCalls.c
 * Support for threaded ops which may block (read, write, connect, etc.).
 *
 * Copyright (c) 1996, 1997, 1998
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */


#define	DBG(s)

#include "config.h"
#include "config-std.h"
#include "config-io.h"
#include "config-mem.h"
#include "lerrno.h"
#include "object.h"
#include "thread.h"
#include "internal.h"
#include "md.h"
#include "locks.h"
#if defined(HAVE_SYS_WAIT_H)
#include <sys/wait.h>
#endif
#define NOUNIXPROTOTYPES
#include "jsyscall.h"

/*
 * We only need this stuff is we are using the internal thread system.
 */

/* Lock for waitpid process */
iLock waitlock;

/*
 * Threaded socket connect.
 */
static
int
threadedConnect(int fd, struct sockaddr* addr, size_t len)
{
	int r;

	do {
		r = connect(fd, addr, len);
	} while ((r < 0) && (errno == EINTR || errno == EALREADY));
	if (r < 0 && errno == EISCONN) {
		r = 0;
	}

	return (r);
}

/*
 * Threaded socket accept.
 */
static
int
threadedAccept(int fd, struct sockaddr* addr, size_t* len)
{
	int r;

	for (;;) {
		blockOnFile(fd, TH_ACCEPT);
		r = accept(fd, addr, len);
		if (r >= 0 || !(errno == EINPROGRESS || errno == EALREADY || errno == EWOULDBLOCK)) {
			break;
		}
	}
	return (r);
}

/*
 * Read but only if we can.
 */
static
ssize_t
threadedRead(int fd, void* buf, size_t len)
{
	ssize_t r;

	if (blockOnFile(fd, TH_READ) < 0) {
		return (-1);
	}
	for (;;) {
		r = read(fd, buf, len);
		if (r >= 0 || !(errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR)) {
			return (r);
		}
		if (blockOnFile(fd, TH_READ) < 0) {
			return (-1);
		}
	}
}

/*
 * Write but only if we can.
 */
static
ssize_t
threadedWrite(int fd, const void* buf, size_t len)
{
	ssize_t r;
	const void* ptr;

	ptr = buf;
	r = 1;

	while (len > 0 && r > 0) {
		if (blockOnFile(fd, TH_WRITE) < 0) {
			return (-1);
		}
		r = (ssize_t)write(fd, ptr, len);
		if (r >= 0) {
			ptr = (void*)((uint8*)ptr + r);
			len -= r;
		}
		else if (!(errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR)) {
			return (-1);
		}
		else {
			r = 1;
		}
	}
	return ((uint8*)ptr - (uint8*)buf);
}

/*
 * Recvfrom but only if we can.
 */
static
ssize_t 
threadedRecvfrom(int fd, void* buf, size_t len, int flags, struct sockaddr* from, int* fromlen)
{
	ssize_t r;
 
	if (blockOnFile(fd, TH_READ) < 0) {
		return (-1);
	}
	for (;;) {
		r = recvfrom(fd, buf, len, flags, from, fromlen);
		if (r >= 0 || !(errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR)) {
			return (r);
		}
		if (blockOnFile(fd, TH_READ) < 0) {
			return (-1);
		}
	}
}

/* 
 * Wait for a child process.
 */
static
int
threadedWaitpid(int wpid, int* status, int options)
{
#if defined(HAVE_WAITPID)
	int npid;

        lockStaticMutex(&waitlock); 
	for (;;) {
		npid = waitpid(wpid, status, options|WNOHANG);
		if (npid > 0) {
			break;
		}
		waitStaticCond(&waitlock, 0);
	}
        unlockStaticMutex(&waitlock);
	return (npid);
#else
	SignalError("java.lang.InternalError", "no waitpid() available");
#endif
}

int threadedFileDescriptor(int fd)
{
	return (fd);
}

SystemCallInterface Kaffe_SystemCallInterface = {

	threadedFileDescriptor,
	open,			/* warning because third argument is mode_t */
	threadedRead,
	threadedWrite,
	lseek,
	close,
	stat,

	mkdir,			/* warning because second argument is mode_t */
	rmdir,
	rename,
	remove,

	socket,
	threadedConnect,
	threadedAccept,
	threadedRead,
	threadedRecvfrom,
	threadedWrite,
	sendto,
	setsockopt,
	getsockopt,
	getsockname,
	getpeername,

	select,

	threadedWaitpid,
};

