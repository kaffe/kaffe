/*
 * config-signal.h
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */


#ifndef __config_signal_h
#define __config_signal_h

#include "config-std.h"
#include <signal.h>

/* This macro may not exist - define it if necessary */
#if !defined(HAVE_SIGEMPTYSET)
#define sigemptyset(s)          (*(s) = 0)
#endif
#if !defined(HAVE_SIGADDSET)
#define sigaddset(s, n)         (*(s) |= (1<<((n)-1)))
#endif

/* QUICK HACK - will make more configurable in future */
#if defined(NeXT)
typedef int sigset_t;
#endif

#if defined(HAVE_SIGPROCMASK)

/* Standard support - everything okay */

#elif defined(HAVE_SIGSETMASK)

/* We will clear all signals rather than just the ones we want.
 * This is okay because of how sigprocmask is used - but it's not a
 * general solition.
 */
#define	sigprocmask(op, nsig, osig)	sigsetmask(0)

#else

#define	sigprocmask(op, nsig, osig)
/* Signal support (sigprocmask or sigsetmask) not available */

#endif

#endif
