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
#define HAVE_DYN_UNDERSTORE 1
#undef	HAVE_SBRK
#endif

/* Hack to configure AmigaOS */
#if defined(__amigaos__)
#define HAVE_DYN_UNDERSTORE 1
#endif  

/* Hack for Windows */
#if defined(__WIN32__) || defined(WIN32) || defined(_WIN32)
#undef	__WIN32__
#define __WIN32__ 1
#endif
#if defined(__WIN32__)
#if defined(HAVE_WINSOCK2_H)
#include <winsock2.h>
#if _MSC_VER < 1200
#define HAVE_INT32 1
#endif
#endif
#if defined(__cplusplus)
#define HAVE_BOOL 1
#else
#define inline
#endif
#endif

/* Hack for NetBSD */
#if defined(__NetBSD__)
#define	HAVE_DYN_UNDERSTORE 1
#define	DEFAULT_LIBRARYPATH "/usr/local/lib:/usr/lib"
#endif

/* Hack to configure AIX */
#if defined(_AIX)
#define  HAVE_SYS_SELECT_H 1
#endif

/* Hack to configure BeOS R4 */
#if defined(__BEOS__)
#include <socket.h>
#if defined(MSG_PEEK)
#error Looks like BeOS finally supports MSG_PEEK.
#error Please remove the MSG_PEEK hack in config-hacks.h and syscalls.c.
#else
#define MSG_PEEK 0x2
#endif
#endif

/* The 386 can do unaligned memory accesses */
#if defined(__i386__)
#define	HAVE_UNALIGNEDACCESS
#endif

/* ARM processors, in little endian mode, use a big endian word order for
 * doubles.  We define this constant to declare that the word order in a
 * 64 bit double is the opposite of the word order in a 64 bit int.
 *
 * If you change this definition, also change strtod("-0.0") check
 * in configure.in.
 */
#if defined(__arm__)
#define DOUBLE_ORDER_OPPOSITE
#endif

