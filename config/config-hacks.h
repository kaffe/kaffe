/*
 * config-hacks.h
 * Various hacks to the configuration for things that aren't
 *  automatically worked out.
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

/* Hacks to configure NeXT */
#if defined(NeXT)
#undef  HAVE_SYS_UTSNAME_H
#undef	HAVE_SBRK
#endif

/* Hack for Windows */
#if defined(__WIN32__) || defined(WIN32) || defined(_WIN32)
#undef	__WIN32__
#define __WIN32__ 1
#endif
#if defined(__WIN32__)
#if defined(HAVE_WINSOCK2_H)
#include <winsock2.h>
#endif
#endif

/* Hack to configure AIX */
#if defined(_AIX)
#define  HAVE_SYS_SELECT_H 1
#endif

/*
 * GCC before 3.0 does not support explicit branch optimization.
 */
#if !defined(__GNUC__) || (__GNUC__ < 3) || (__GNUC__ == 3 && __GNUC_MINOR__ == 0)
#define __builtin_expect(a,b) (a)

#include <stdlib.h>
#define __builtin_trap() abort()
#endif
