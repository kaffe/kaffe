/*
 * i386/threads.h
 * i386 threading information.
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#ifndef __i386_threads_h
#define __i386_threads_h

/*
 * Set a default size for the stack.
 */
#if defined(INTERPRETER)
#define	THREADSTACKSIZE		(512 * 1024)
#else
#define	THREADSTACKSIZE		(256 * 1024)
#endif

/*
 * Stack offset.
 * This is the offset into the setjmp buffer where the stack pointer is
 * stored.  This may be different with different OSes.
 */
#define	SP_OFFSET		4

#endif
