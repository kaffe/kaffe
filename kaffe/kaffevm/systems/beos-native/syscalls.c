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
#include "config-hacks.h"
#include "jthread.h"
#include "jsyscall.h"

int
mkdir_with_int(const char *path, int m)
{
	return mkdir(path, m);
}

static int
connect_with_size_t(int fd, struct sockaddr* addr, size_t len)
{
        return connect(fd, addr, len);
}

static ssize_t
beos_recvfrom(int sock, void* buf, size_t len, int flags,
              struct sockaddr* from, int* fromlen)
{
	if (flags & MSG_PEEK) {
		errno = B_UNSUPPORTED;
		return -1;
	}
	return recvfrom(sock, buf, len, flags, from, fromlen);
}

static int
setsockopt_with_int(int sock, int lev, int opt, const void* data, int size)
{
	return setsockopt(sock, lev, opt, data, size);
}

static int
my_waitpid(int wpid, int* status, int options)
{
	unimp("waitpid() not implemented");
	return -1;
}

static int
my_forkexec(char *argv[], char *env[], int fd[4])
{
	unimp("forkexec() not implemented");
	return -1;
}

static int
my_kill(int pid, int sig)
{
	unimp("kill() not implemented");
	return -1;
}

SystemCallInterface Kaffe_SystemCallInterface = {
        (int (*)(const char *, int, int))open,	
        read,	
        write, 
        lseek,
        close,
        fstat,
        stat,
        mkdir_with_int,		/* the real mkdir takes a mode_t */
        rmdir,
        rename,
        remove,
        socket,
        connect_with_size_t,
        (int (*)(int, struct sockaddr *, size_t*))accept, 
        read,	
        beos_recvfrom,
        write, 
        sendto,	
        setsockopt_with_int,	/* the BeOS version takes a uint size */
        getsockopt,
        getsockname, 
        getpeername,
	closesocket,
        select,	
	my_forkexec,
        my_waitpid,
	my_kill,
};
