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
#include "config-std.h"
#include "config-mem.h"
#include "config-io.h"
#include "config-signal.h"
#include "config-net.h"
#include "jthread.h"
#include "jsyscall.h"
#include "nets.h"

/*
 * This file contains thread-safe version of various POSIXy functions.
 * Other functions that use or interact with asynchronous I/O, are in jthread.c.
 */

static int
jthreadedClose(int fd)
{
	int rc = 0;

	jthread_spinon(0);
	if (close(fd) == -1) {
		rc = errno;
	}
	jthread_spinoff(0);
	return (rc);
}

static int
jthreadedListen(int fd, int l)
{
	int rc = 0;

	jthread_spinon(0);
	if (listen(fd, l) == -1) {
		rc = errno;
	}
	jthread_spinoff(0);
	return (rc);
}

static int
jthreadedKill(int pid, int sig)
{
	int rc = 0;

	jthread_spinon(0);
	if (kill(pid, sig) == -1) {
		rc = errno;
	}
	jthread_spinoff(0);
	return (rc);
}

static int
jthreadedBind(int fd, struct sockaddr *addr, int namelen)
{
	int rc = 0;

	jthread_spinon(0);
	if (bind(fd, addr, (size_t)namelen) == -1) {
		rc = errno;
	}
	jthread_spinoff(0);
	return (rc);
}

static int
jthreadedLSeek(int fd, off_t offset, int whence, off_t *out)
{
	int rc = 0;

	jthread_spinon(0);
	*out = lseek(fd, offset, whence);
	if (*out == -1) {
		rc = errno;
	}
	jthread_spinoff(0);
	return (rc);
}

static int
jthreadedFStat(int fd, struct stat *sb)
{
	int rc = 0;

	jthread_spinon(0);
	if (fstat(fd, sb) == -1) {
		rc = errno;
	}
	jthread_spinoff(0);
	return (rc);
}

static int
jthreadedStat(const char* path, struct stat *sb)
{
	int rc = 0;

	jthread_spinon(0);
	if (stat(path, sb) == -1) {
		rc = errno;
	}
	jthread_spinoff(0);
	return (rc);
}

static int
jthreadedFTruncate(int fd, off_t new_size)
{
#ifdef HAVE_FTRUNCATE
	int rc = 0;
	jthread_spinon(0);
	if (ftruncate(fd, new_size) == -1) {
		rc = errno;
	}
	jthread_spinoff(0);
	return (rc);
#else
	return ENOTSUP;
#endif
}

static int
jthreadedFSync(int fd)
{
#ifdef HAVE_FSYNC
	int rc = 0;
	jthread_spinon(0);
	if (fsync(fd) == -1) {
		rc = errno;
	}
	jthread_spinoff(0);
	return (rc);
#else
	return ENOTSUP;
#endif
}

static int
jthreadedMkdir(const char *path, int mode)
{
	int rc = 0;

	jthread_spinon(0);
	if (mkdir(path, (unsigned)mode) == -1) {
		rc = errno;
	}
	jthread_spinoff(0);
	return (rc);
}

static int
jthreadedRmdir(const char *path)
{
	int rc = 0;

	jthread_spinon(0);
	if (rmdir(path) == -1) {
		rc = errno;
	}
	jthread_spinoff(0);
	return (rc);
}

static int
jthreadedRename(const char *path1, const char *path2)
{
	int rc = 0;

	jthread_spinon(0);
	if (rename(path1, path2) == -1) {
		rc = errno;
	}
	jthread_spinoff(0);
	return (rc);
}

static int
jthreadedRemove(const char *entry)
{
	int rc = 0;

	jthread_spinon(0);
	if (remove(entry) == -1) {
		rc = errno;
	}
	jthread_spinoff(0);
	return (rc);
}

static int     
jthreadedSendto(int a, const void* b, size_t c, int d, const struct sockaddr* e,
		size_t f, ssize_t *out)
{
	int rc = 0;

	jthread_spinon(0);
	*out = e ? sendto(a, b, c, d, e, f) : send(a, b, c, d);
	if (*out == -1) {
		rc = errno;
	}
	jthread_spinoff(0);
	return (rc);
}

static int
jthreadedSetSockOpt(int a, int b, int c, const void* d, int e)
{
	int rc = 0;

	jthread_spinon(0);
	if (setsockopt(a, b, c, d, (unsigned)e) == -1) {
		rc = errno;
	}
	jthread_spinoff(0);
	return (rc);
}

static int
jthreadedGetSockOpt(int a, int b, int c, void* d, socklen_t* e)
{
	int rc = 0;

	jthread_spinon(0);
	if (getsockopt(a, b, c, d, e) == -1) {
		rc = errno;
	}
	jthread_spinoff(0);
	return (rc);
}

static int
jthreadedGetSockName(int a, struct sockaddr* b, socklen_t* c)
{
	int rc = 0;

	jthread_spinon(0);
	if (getsockname(a, b, c) == -1) {
		rc = errno;
	}
	jthread_spinoff(0);
	return (rc);
}

static int
jthreadedGetPeerName(int a, struct sockaddr* b, socklen_t* c)
{
	int rc = 0;

	jthread_spinon(0);
	if (getpeername(a, b, c) == -1) {
		rc = errno;
	}
	jthread_spinoff(0);
	return (rc);
}

static int
jthreadedGetHostByName(const char *host, struct hostent** out)
{
	int rc = 0;

	jthread_spinon(0);
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
	jthread_spinoff(0);
	return (rc);
}

static int
jthreadedGetHostByAddr(const char *host, int l, int t, struct hostent** out)
{
	int rc = 0;

	jthread_spinon(0);
	/* NB: same comment as for jthreadedGetHostByName applies here */
	*out = gethostbyaddr(host, (unsigned)l, t);
	if (*out == 0) {
		rc = h_errno;
		if (rc == 0) {
			*out = (void*)-1;
			rc = errno;
		}
	}
	jthread_spinoff(0);
	return (rc);
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
			sysflags = MAP_PRIVATE;
			sysmode = PROT_READ;
			break;
		case KAFFE_MMAP_WRITE:
			sysflags = MAP_SHARED;
			sysmode = PROT_READ | PROT_WRITE;
			break;
		case KAFFE_MMAP_PRIVATE:
			sysflags = MAP_PRIVATE;
			sysmode = PROT_WRITE | PROT_READ;
			break;
		default:
			return EINVAL;
	}

	jthread_spinon(0);
	*memory = mmap(*memory, *size, sysmode, sysflags, fd, *offset);
	if (*memory == NULL)
		rc = errno;

	jthread_spinoff(0);
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
	
	jthread_spinon(0);
	if (munmap(memory, size) < 0) {
		rc = errno;
	}
	jthread_spinoff(0);
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

	jthread_spinon(0);
	memory = (void *)(((size_t)memory/getpagesize()) * getpagesize());
	size += getpagesize();
	/* TODO: Try not to freeze the entire VM. */
	if (msync(memory, size, MS_SYNC | MS_INVALIDATE) < 0) {
		rc = errno;
	}
	jthread_spinoff(0);

	return rc;
#else
	return (ENOTSUP);
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
