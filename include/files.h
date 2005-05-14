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
#include "jni_md.h"
#include <errno.h>

#if defined(HAVE_STRING_H)
#include <string.h>
#endif

#if defined(HAVE_STRERROR)
#define	SYS_ERROR(x)	(char*)strerror(x)
#else
extern char* sys_errlist[];
#define	SYS_ERROR(x)	sys_errlist[x]
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

/* Convert jlong's to and from size_t's */
#define jlong2size_t(j)         ((size_t)(j))
#define size_t2jlong(j)         ((jlong)(j))

#endif
