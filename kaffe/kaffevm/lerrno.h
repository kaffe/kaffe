/* lerrno.h
 * Not all systems support all errnos - so define those which aren't used
 * to be harmless.
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#ifndef __lerrno_h
#define __lerrno_h

#include <errno.h>

#define	NOERRNO		100000

#if !defined(EINPROGRESS)
#define	EINPROGRESS	NOERRNO
#endif
#if !defined(EALREADY)
#define	EALREADY	NOERRNO
#endif
#if !defined(EWOULDBLOCK)
#define	EWOULDBLOCK	NOERRNO
#endif
#if !defined(EAGAIN)
#define	EAGAIN		NOERRNO
#endif
#if !defined(EINTR)
#define	EINTR		NOERRNO
#endif
#if !defined(EISCONN)
#define	EISCONN		NOERRNO
#endif
#if !defined(EOPNOTSUPP)
#define EOPNOTSUPP	NOERRNO
#endif

/* This is for NetBSD */ 
#if !defined(ENOTSUP)
#define ENOTSUP EOPNOTSUPP
#endif

#endif
