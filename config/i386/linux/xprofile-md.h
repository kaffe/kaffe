/*
 * xprofile-md.h
 * Interface functions to the profiling code
 *
 * Copyright (c) 2001 University of Utah and the Flux Group.
 * All rights reserved.
 *
 * This file is licensed under the terms of the GNU Public License.  
 * See the file "license.terms" for restrictions on redistribution 
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 */

#ifndef __i386_linux_xprofile_md_h
#define __i386_linux_xprofile_md_h

/*
 * Unfortunately, I can't get linux to export a symbol to the gmonparam
 * structure (it doesn't link for some reason)...  So we can't get any early
 * recordings.
 */
static inline struct gmonparam *getGmonParam(void)
{
	return NULL;
}

#define _MCOUNT_DECL _mcount

#define _KAFFE_OVERRIDE_MCOUNT_DEF void _MCOUNT_DECL __P((uintp frompc, uintp selfpc))

#if (__GNUC__ < 2) || (__GNUC_MINOR__ < 9)
#define _KAFFE_OVERRIDE_MCOUNT \
void mcount(void) \
{ \
	uintp selfpc, frompc; \
\
	asm("movl 4(%%ebp),%0" : "=r" (selfpc)); \
	asm("movl (%%ebp),%0" : "=r" (frompc)); \
	frompc = ((uintp *)frompc)[1]; \
	_mcount(frompc, selfpc); \
}
#else
#define _KAFFE_OVERRIDE_MCOUNT \
void mcount(void) \
{ \
	uintp selfpc, frompc; \
\
	asm("movl (%%esp),%0" : "=r" (selfpc)); \
	asm("movl 4(%%ebp),%0" : "=r" (frompc)); \
	_mcount(frompc, selfpc); \
}
#endif

#endif
