/*
 * m68k/linux/md.h
 * Linux m68k configuration information.
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#ifndef __m68k_linux_md_h
#define __m68k_linux_md_h

#include "m68k/common.h"
#include "m68k/threads.h"

#if defined(TRANSLATOR)
#include "jit-md.h"
#endif

/* Linux requires a little initialisation */
#define	INIT_MD()	init_md()

#endif
