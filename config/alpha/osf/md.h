/*
 * alpha/osf/md.h
 * OSF/1 Alpha configuration information.
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#ifndef __alpha_osf_md_h
#define __alpha_osf_md_h

#include "alpha/common.h"
#include "alpha/threads.h"

#if defined(TRANSLATOR)
#include "jit-md.h"
#endif

#undef SP_OFFSET
#define SP_OFFSET 34

/* Alpha's require a little initialisation for proper IEEE math.  */
extern void init_md(void);
#define INIT_MD()       init_md()

#endif
