/*
 * ia64/threads.h
 * IA-64 threading information.
 *
 * Copyright (c) 2001
 *	MandrakeSoft.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

#ifndef __ia64_threads_h
#define __ia64_threads_h

/*
 * Set a default size for the stack.
 * Includes register stack size
 */
#define	THREADSTACKSIZE	(128 * 1024)

/*
 * Stack offset.
 * This is the offset into the setjmp buffer where the stack pointer is
 * stored.  This may be different with different OSes.
 */
#define	SP_OFFSET	0

#endif
