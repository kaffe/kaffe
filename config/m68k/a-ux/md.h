/*
 * m68k/aux/md.h
 * aux/m68k specific functions.
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#ifndef __m68k_netbsd_md_h
#define __m68k_netbsd_md_h

#include "m68k/threads.h"

#if defined(TRANSLATOR)
#include "jit-md.h"
#define INIT_MD()       init_md()
#endif

#endif
