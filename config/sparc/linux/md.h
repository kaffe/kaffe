/*
 * sparc/linux/md.h
 * Linux sparc configuration information.
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#ifndef __sparc_linux_md_h
#define __sparc_linux_md_h

#include "sparc/common.h"
#include "sparc/threads.h"

#if defined(TRANSLATOR)
#include "jit-md.h"
#endif

/* Linux requires a little initialisation */
#define	INIT_MD()	init_md()

#endif
