/*
 * m68k/netbsd1/md.h
 * netbsd/m68k specific functions.
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 *
 * Written by Kiyo Inaba <inaba@src.ricoh.co.jp>, 1998;
 */

#ifndef __m68k_netbsd1_md_h
#define __m68k_netbsd1_md_h

#include "m68k/common.h"
#include "m68k/threads.h"

/*
 * Redefine stack pointer offset.
 */
#undef  SP_OFFSET
#define SP_OFFSET       2

#if defined(TRANSLATOR)
#include "jit-md.h"
#endif

#endif
