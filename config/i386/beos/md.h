/*
 * i386/beos/md.h
 * BeOS i386 configuration information.
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#ifndef __i386_beos_md_h
#define __i386_beos_md_h

#include "i386/common.h"
#include "i386/threads.h"

/*
 * Redefine stack pointer offset.
 */
#undef	SP_OFFSET
#define	SP_OFFSET	4

/*
 * BeOS limits each thread stack to "about 256K".  Let's be conservative
 * and assume it's much less.
 */
#define MAINSTACKSIZE	(220*1024)

#if defined(TRANSLATOR)
#include "jit-md.h"
#endif

/*
 * BeOS R4's sockets implementation doesn't provide getsockopt()
 */
int getsockopt(int,int,int,void*,int*);

#endif
