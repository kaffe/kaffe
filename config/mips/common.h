/*
 * mips/common.h
 * Common MIPS configuration information.
 *
 * Copyright (c) 1996, 1997, 1998, 1999
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * Copyright (c) 2003
 *      Kaffe.org contributors. See ChangeLog for details.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 *
 * by Alexandre Oliva <oliva@dcc.unicamp.br>
 * and Edouard G. Parmelan <egp@free.fr>
 */

#ifndef __mips_common_h
#define __mips_common_h

#include "generic/gentypes.h"

/* The R5900 is the Mips Core in the PS2 (Playstation 2)
* It has most of the Mips III instructions set, some of the Mips IV 
* instructions, it lacks the Mips II "ll" and "sc" instructions and 
* it has 128 bit GP registers and 32 bit FPU registers. There is no 
* FPU Emulation present in the default kernel. Since no 64 bit
* FPU registers exist, doubles are passed in 2 GP registers.
*/
#ifdef _R5900
#define PS2LINUX
#undef HAVE_MIPSII_INSTRUCTIONS
#endif 

#include "gccbuiltin.h"

/* Atomicity is temporarily disabled because the compare_and_swap function 
 * do not return the right value. Use the generic one instead.
 */
#if defined(HAVE_MIPSII_INSTRUCTIONS)
#include "atomic.h"
#else

#include "generic/genatomic.h"

#endif /* defined(HAVE_MIPSII_INSTRUCTIONS) */

#include "generic/comparexch.h"
#include "katomic.h"
    
#endif
