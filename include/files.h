/*
 * files.h
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#ifndef __files_h
#define __files_h

#include "config.h"
#include "config-std.h"
#include "config-io.h"
#include <jtypes.h>
#include <errno.h>

#if defined(HAVE_STRING_H)
#include <string.h>
#endif

#if defined(HAVE_STRERROR)
#define	SYS_ERROR	(char*)strerror(errno)
#else
extern char* sys_errlist[];
#define	SYS_ERROR	sys_errlist[errno]
#endif

/* Define access() flags if not already defined */
#if !defined(W_OK)
#define	W_OK		2
#define	R_OK		4
#endif

/* If we don't have O_BINARY, define it to be 0 */
#if !defined(O_BINARY)
#define	O_BINARY	0
#endif

/* Convert jlong's to and from off_t's */
#define	jlong2off_t(j)		((off_t)(j))
#define	off_t2jlong(j)		((jlong)(j))

#endif
