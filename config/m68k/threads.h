/*
 * m68k/threads.h
 * m68k threading information.
 *
 * Copyright (c) 1996, 1997, 1998
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#ifndef __m68k_threads_h
#define __m68k_threads_h

/*
 * Set a default size for the stack.
 */
#define	THREADSTACKSIZE		(64 * 1024)

/*
 * Stack offset.
 * This is the offset into the setjmp buffer where the stack pointer is
 * stored.  This may be different with different OSes.
 */
/*	#define	SP_OFFSET		?	*/

#endif
