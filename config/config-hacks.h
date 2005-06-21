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

/* On arm, word order of doubles is always big endian when
 * using FPA. When using VFP, word order of doubles follows
 * the word order of the memory system.
 *
 * Therefore, the word order of doubles is opposite to the
 * word order of jlongs when using a little endian arm
 * unless VFP is used.
 */
#if defined(__ARMEL__) && !defined(__VFP_FP__)
#define DOUBLE_ORDER_OPPOSITE
#endif

/*
 * GCC before 3.0 does not support explicit branch optimization.
 */
#if !defined(__GNUC__) || (__GNUC__ < 3) || (__GNUC__ == 3 && __GNUC_MINOR__ == 0)
#define __builtin_expect(a,b) (a)

#include <stdlib.h>
#define __builtin_trap() abort()
#endif
