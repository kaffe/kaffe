/*
 * superh/threads.h
 * Super-H threading information.
 *
 * Copyright (c) 2001
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#ifndef __superh_threads_h
#define __superh_threads_h

/**/
/* Thread handling */
/**/
#define	USE_INTERNAL_THREADS

/*
 * Set a default size for the stack.
 */
#if !defined(THREADSTACKSIZE)
#if defined(INTERPRETER)
#define	THREADSTACKSIZE		(128 * 1024)
#else
#define	THREADSTACKSIZE		(64 * 1024)
#endif
#endif

#define	FP_OFFSET		6
#define	SP_OFFSET		7

#endif
