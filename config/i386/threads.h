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

/**/
/* Thread handling */
/**/
#define	USE_INTERNAL_THREADS

/*
 * Set a default size for the stack.
 */
#define	THREADSTACKSIZE		(32 * 1024)

/*
 * Stack offset.
 * This is the offset into the setjmp buffer where the stack pointer is
 * stored.  This may be different with different OSes.
 */
#define	SP_OFFSET		4

#endif
