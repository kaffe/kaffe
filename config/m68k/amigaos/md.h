/*
 * m68k/amigaos/md.h
 * AmigaOS specific functions.
 *
 * Copyright (c) 1996 Matthias Hopf <mshopf@informatik.uni-erlangen.de>
 *
 * Permission granted for Tim Wilkinson to include this source in his
 * Kaffe system.
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#ifndef __m68k_amigaos_md_h
#define __m68k_amigaos_md_h

#include "m68k/threads.h"
#include "m68k/common.h"
#include "m68k/sysdepCallMethod.h"

/*	Define Stack Offset
*/
#undef	SP_OFFSET
#define	SP_OFFSET	2

#if defined(TRANSLATOR)
#include "jit-md.h"
#endif

/* We do additional things here */

extern void init_md(void);
#define INIT_MD()       md_init()

/* We cannot use the MMU for null pointer tests */
#define CREATE_NULLPOINTER_CHECKS

#endif
