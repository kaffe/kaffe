/*
 * defs.h
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#ifndef __defs_h
#define __defs_h

#define	MAXNUMLEN	64
#define	MAXLIBPATH	1024

#if !defined(MAXNAMELEN)
#define	MAXNAMELEN	1024
#endif

#if !defined(MAXPATHLEN)
#define	MAXPATHLEN	1024
#endif

#ifdef __cplusplus
#  define BEGIN_C_DECLS extern "C" {
#  define END_C_DECLS   }
#else /* !__cplusplus */
#  define BEGIN_C_DECLS
#  define END_C_DECLS
#endif /* __cplusplus */

#endif
