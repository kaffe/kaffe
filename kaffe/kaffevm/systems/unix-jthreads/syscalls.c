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

/*
 * The syscall interface as provided by the internal jthread system.
 */
SystemCallInterface Kaffe_SystemCallInterface = {
        jthreadedOpen,
        jthreadedRead,	
        jthreadedWrite, 
        lseek,
        close,
        fstat,
        stat,
        (int(*)(const char*, int))mkdir,
        rmdir,
        rename,
        remove,
        jthreadedSocket,
        jthreadedConnect,
        jthreadedAccept, 
        jthreadedRead,	
        jthreadedRecvfrom,
        jthreadedWrite, 
        sendto,	
        setsockopt,	
        getsockopt,
        getsockname, 
        getpeername,
        select,	
        jthreadedForkExec,
        jthreadedWaitpid,
        kill
};
