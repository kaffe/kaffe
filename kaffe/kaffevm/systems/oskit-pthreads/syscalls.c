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
#include "jthread.h"
#include "jsyscall.h"

int
mkdir_with_int(const char *path, int m)
{
	return mkdir(path, m);
}

static int
my_connect(int fd, struct sockaddr* addr, size_t len)
{
        int r = connect(fd, addr, len);
        /* annul EALREADY error */
        if (r == -1 && errno == EALREADY)
                r = 0;
        return r;
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
my_kill(void)
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
        my_connect,
        (int (*)(int, struct sockaddr *, size_t*))accept, 
        read,	
        recvfrom,
        write, 
        sendto,	
        setsockopt,	
        getsockopt,
        getsockname, 
        getpeername,
        select,	
	my_forkexec,
        my_waitpid,
	my_kill,
};
