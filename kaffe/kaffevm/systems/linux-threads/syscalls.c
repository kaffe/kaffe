/*
 * syscalls.c
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
#include "config-signal.h"
#include "jthread.h"
#include "jsyscall.h"

#include <sys/wait.h>

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
my_forkexec(char *argv[], char *env[], int fd[4])
{
	unimp("forkexec() not implemented");
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
        (int (*)(const char *, int))mkdir,
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
        waitpid,
	kill,
};
