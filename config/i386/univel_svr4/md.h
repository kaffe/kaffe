/*
 * i386/univel_svr4/md.h
 * FreeBSD i386 configuration information.
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#ifndef __i386_univel_svr4_md_h
#define __i386_univel_svr4_md_h

#include "i386/common.h"
#include "i386/threads.h"

#if defined(TRANSLATOR)
#include "jit-md.h"
#endif

/* NCR MP-RAS requires a little initialisation */
extern void init_md(void);
#define INIT_MD()	init_md()

#endif
