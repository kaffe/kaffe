/*
 * s390/threads.h
 * s390 threading information (cloned from i386/threads.h by ROSSP).
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#ifndef __s390_threads_h
#define __s390_threads_h

/*
 * Set a default size for the stack.
 */
#if defined(INTERPRETER)
#define	THREADSTACKSIZE		(128 * 1024)
#else
#define	THREADSTACKSIZE		(64 * 1024)
#endif

/*
 * Stack offset.
 * This is the offset into the setjmp buffer where the stack pointer is
 * stored.  This may be different with different OSes.
 */
/* This value was computed by developers/sp_offset.c - ROSSP */
#define	SP_OFFSET		9

#endif
