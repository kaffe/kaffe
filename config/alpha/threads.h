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

/**/
/* Thread handling */
/**/
#define	USE_INTERNAL_THREADS

#define	THREADSTACKSIZE		(32 * 1024)

/*
 * Stack offset.
 * This is the offset into the setjmp buffer where the stack pointer is
 * stored.  This may be different with different OSes.
 */
#define	SP_OFFSET		?

#endif
