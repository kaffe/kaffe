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
#endif

/* Hack to configure AmigaOS */
#if defined(__amigaos__)
#define HAVE_DYN_UNDERSTORE 1
#endif  

/* Hack for Window's */ 
#if defined(__WIN32__) || defined(WIN32) || defined(_WIN32)
#undef	__WIN32__
#define __WIN32__ 1
#endif

/* Hack for NetBSD */
#if defined(__NetBSD__)
#define	HAVE_DYN_UNDERSTORE 1
#define	DEFAULT_LIBRARYPATH "/usr/local/lib:/usr/lib"
#endif

/* Hack to configure AIX */
#if defined(AIX)
#define  HAVE_SYS_SELECT_H 1
#endif

/* The 386 can do unaligned memory accesses */
#if defined(__i386__)
#define	HAVE_UNALIGNEDACCESS
#endif

/* The arm32, while being a little endian machine, uses a big endian
 * word order for its doubles.  The arm32 uses software floating point 
 * emulation.  We define this constant to declare that the word order
 * in a 64bit double is the opposite of the word order in a 64bit int.
 */
#if defined(arm32) || (defined(arm) && defined(linux))
#define DOUBLE_ORDER_OPPOSITE
#endif
