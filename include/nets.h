/*
 * nets.h
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#ifndef __nets_h
#define __nets_h

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "config-mem.h"
#include <errno.h>
#include <netdb.h>

/* some systems define this already as a macro, in which we leave it as is */
#ifndef H_ERRNO_DECLARED
extern int h_errno;
#endif /* h_errno */

#define	MAXHOSTNAME	128

#if defined(HAVE_STRERROR)
#define	SYS_ERROR(x)	strerror(x)
#else
extern char* sys_errlist[];
#define	SYS_ERROR(x)	sys_errlist[x]
#endif

#if defined(HAVE_HSTRERROR)
#define	SYS_HERROR(x)	hstrerror(x)
#else
#define	SYS_HERROR(x)	"Network error"
#endif

#if !defined(HAVE_INET_NTOP)
extern const char * inet_ntop(int af, const void * src, char * dst, size_t size);
#endif /* HAVE_INET_NTOP */

#if !defined(HAVE_INET_PTON)
extern int inet_pton(int af, const char * src, void * dst);
#endif /* HAVE_INET_PTON */

#endif /* __nets_h */

