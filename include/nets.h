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

#endif /* __nets_h */

