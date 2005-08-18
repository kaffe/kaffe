/*
 * config-std.h
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * Copyright (c) 2005
 *	Kaffe.org contributors, see ChangeLog for details.  All rights reserved.
 *
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */


#include "config.h"
#ifndef __config_std_h
#define __config_std_h

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "config.h"


#if !defined (__CYGWIN__)
#if !defined(HAVE_WINDOWS_H) && defined(HAVE_WINNT_H)
#include <winnt.h>
#endif
#if defined(HAVE_WINTYPES_H)
#include <wintypes.h>
#endif
#endif /* ! __CYGWIN__ */
#if defined(HAVE_BSD_LIBC_H)
#include <bsd/libc.h>
#endif
#if defined(HAVE_KERNEL_OS_H)
#include <kernel/OS.h>
#endif

#undef	__UNUSED__
#if defined(__GNUC__)
#define	__UNUSED__ __attribute__((__unused__))
#else
#define	__UNUSED__
#endif

/* SunOS has on_exit only */
#if !defined(HAVE_ATEXIT) && defined(HAVE_ON_EXIT)
#define atexit(x)       on_exit(x, 0)
#endif

#if defined(WITH_DMALLOC)
#  include <dmalloc.h>            
#endif

#endif
