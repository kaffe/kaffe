/*
 * i386/linux/md.h
 * Linux i386 configuration information.
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#ifndef __ki386_linux_md_h
#define __ki386_linux_md_h

#include "i386/common.h"
#include "i386/threads.h"

#if defined(TRANSLATOR)
#include "jit-md.h"
#endif

/* Linux requires a little initialisation */
#define	INIT_MD()	init_md()

#endif
