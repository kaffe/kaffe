/*
 * config.h
 *
 * Thread support using internal system.
 *
 * Copyright (c) 1996, 1997, 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 *
 * Written by Godmar Back <gback@cs.utah.edu> and
 *            Tim Wilkinson <tim@transvirtual.com>
 */

/*
 * Note: this file get included only when jthreads are used outside
 *       the context of the virtual machine. 
 *
 *       The virtual machines uses definitions from the config directory
 *	 for the specific processor.	
 */

#ifndef __config_jthreads_h
#define __config_jthreads_h

#include "config.h"

#include <assert.h>
#include <setjmp.h>
#include <sys/types.h>
#include <sys/signal.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <signal.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#define HAVE_SETITIMER	1
#define HAVE_WAITPID	1
#define THREADSTACKSIZE         (32 * 1024)

#if defined(__FreeBSD__)
/*
 * Stack offset.
 * This is the offset into the setjmp buffer where the stack pointer is
 * stored.  In FreeBSD, that's 2 - normally it's 4 for a i386
 * On NetBSD and OpenBSD it's 2 as well.
 */
#define SP_OFFSET               2
#define HAVE_SYS_FILIO_H 1

#elif defined(__linux__) && defined(mc68000)

#define SP_OFFSET		14

#elif defined(__linux__) && defined(i386)

#define SP_OFFSET		4

#elif defined(__svr4__) && defined(__sparc__)

/* Solaris */
#define SP_OFFSET		1
#define HAVE_SYS_FILIO_H 1

#elif defined(_AIX) && defined(_POWER)

/* AIX on IBM PowerPC */
#define SP_OFFSET		3
#include <sys/select.h>		/* another AIX oddity */

#elif defined(hpux)

/* HPUX */
#define STACK_GROWS_UP  	1
#define SP_OFFSET		1

#elif defined(hppa) && !defined(hpux)

/* HP-BSD - this is a Utah thing */
#define STACK_GROWS_UP  	1
#define SP_OFFSET		2

/* We will clear all signals rather than just the ones we want.
 * This is okay because of how sigprocmask is used - but it's not a
 * general solition.
 */
#define sigprocmask(op, nsig, osig)     sigsetmask(0)
typedef int sigset_t;

#elif defined(sgi) && defined(mips)

/* SGI running IRIX 6.2 */
#define SP_OFFSET		2
#define FP_OFFSET		13

#define SIGALTSTACK_NEEDS_END   1

#elif defined(arm32) && defined(__NetBSD__)

#define SP_OFFSET		23

#elif defined(arm) && defined(linux)

#define SP_OFFSET		20
#define FP_OFFSET		19

#else
#error Your system was not yet tested
#endif

#if HAVE_SYS_FILIO_H
#include <sys/filio.h>
#endif

#ifndef FD_COPY
#define FD_COPY(from, to)	memcpy(to, from, sizeof(*(from)))
#endif

/* define our own jlong and NOTIMEOUT */
typedef signed long long	jlong;
#define NOTIMEOUT	-1
#define true		1
#define false		0

#define catchSignal(s, h)	signal(s, h)  /* XXX use sigaction()? */

static jlong currentTime()
{
    struct timeval tm;
    gettimeofday(&tm, 0);
    return (((jlong)tm.tv_sec * 1000L) + ((jlong)tm.tv_usec / 1000L));
}

/* debug.h stuff */
#ifdef KAFFE_VMDEBUG
#define DBG(x, y)       	y
#define DBGEXPR(x, t, f)        t
#define DBGIF(x)        	x
#else
#define DBG(x, y)
#define DBGEXPR(x, t, f)        f
#define DBGIF(x)      
#endif

#endif /* __config_jthreads_h */
