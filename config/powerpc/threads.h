/*
 * powerpc/threads.h
 * powerpc threading information.
 *
 * Copyright (c) 1996, 1997, 1998
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#ifndef __powerpc_threads_h
#define __powerpc_threads_h

#define	THREADSTACKSIZE		(256 * 1024)

/*
 * Stack offset.
 * This is the offset into the setjmp buffer where the stack pointer is
 * stored.  This may be different with different OSes.
 */
#if defined(__linux__) && defined(__powerpc__)
#define	SP_OFFSET  0
#else
#define	SP_OFFSET  ?
#endif
#endif
