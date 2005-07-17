/*
 * i386/linux/md.c
 * Linux i386 specific functions.
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#include "config.h"
#include "gtypes.h"
#include "md.h"

#include <malloc.h>
#include <unistd.h>
#include <stdio.h>
#if defined(HAVE_FEATURES_H)
#include <features.h>
#endif

#include "i386/fpu-set.h"

void
init_md(void)
{
#if defined(M_MMAP_MAX) && defined(HAVE_MALLOPT)
	mallopt(M_MMAP_MAX, 0);
#endif

	/* See http://www.srware.com/linux_numerics.txt */
        /*
	  This puts the X86 FPU in 64-bit precision mode.  The default
	  under Linux is to use 80-bit mode, which produces subtle
	  differences from FreeBSD and other systems, eg,
	  (int)(1000*atof("0.3")) is 300 in 64-bit mode, 299 in 80-bit
	  mode.
        */
	set_fpu_precision();
}

#if defined(__GLIBC__)
extern void * __libc_stack_end;

void *mdGetStackEnd(void)
{
  long sz = sysconf(_SC_PAGESIZE);
  
  return (void *)(((uintp)__libc_stack_end + sz - 1) & (-sz));
}
#endif
