/*
 * alpha/threads.h
 * Alpha threading information.
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#ifndef __alpha_threads_h
#define __alpha_threads_h

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

#define	SP_OFFSET		?

#endif
