/*
 * mips/irix5/md.h
 * Irix 5 MIPS configuration information.
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#ifndef __mips_irix5_md_h
#define __mips_irix5_md_h

#include "mips/threads.h"

#ifdef SP_OFFSET
#undef SP_OFFSET
#endif

#ifdef FP_OFFSET
#undef FP_OFFSET
#endif

/* Determined on a SGI running IRIX 6.2 */ 
#define SP_OFFSET               2
#define FP_OFFSET               13

#endif
