/*
 * alpha/netbsd1/md.h
 * NetBSD alpha configuration information.
 *
 * Copyright (c) 1996, 1997
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

#ifndef __alpha_netbsd1_md_h
#define __alpha_netbsd1_md_h

#include "alpha/common.h"
#include "alpha/threads.h"

#if defined(TRANSLATOR)
#include "jit-md.h"
#endif

/* Alpha's require a little initialisation for proper IEEE math.  */
extern void init_md(void);
#define INIT_MD()       init_md()

/*
 * Redefine stack pointer offset.
 */
#undef  SP_OFFSET
#define SP_OFFSET 34

#endif
