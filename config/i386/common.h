/*
 * i386/common.h
 * Common i386 configuration information.
 *
 * Copyright (c) 1996, 1997, 1998, 1999
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * Copyright (c) 2003
 *      Kaffe.org contributors. See ChangeLog for details.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#ifndef __i386_h
#define __i386_h

#include "generic/gentypes.h"
#include "gtypes.h"

#if defined(NEED_sysdepCallMethod)
#include "sysdepCallMethod.h"
#endif /* defined(NEED_sysdepCallMethod) */

/* The 386 never aligns to more than a 4 byte boundary. */
#define	ALIGNMENT_OF_SIZE(S)	((S) < 4 ? (S) : 4)

#if defined(__GNUC__)
#define KAFFE_PROFILER 1
#endif

#if defined(KAFFE_PROFILER)

/* profiler clicks counter type.  */
typedef int64	profiler_click_t;

/* ReaD the processor Time Stamp Counter.
 * This is a macro to help GCC optimization.
 * The rdtsc instruction load TSC to edx:eax aka A register.  */
#define profiler_get_clicks(COUNTER)	\
	asm volatile (".byte 0xf; .byte 0x31" /* "rdtsc" */ : "=A" (COUNTER))

#endif

#ifndef KAFFE_PLUS_M_ASM_CONSTRAINT
# if __GNUC__ > 2 || (__GNUC__ == 2 && __GNUC_MINOR__ >= 8)
#  define KAFFE_PLUS_M_ASM_CONSTRAINT "+m"
# else
#  define KAFFE_PLUS_M_ASM_CONSTRAINT "=m"
/* Note: it should really be `+m', to indicate both input and output,
   but gcc 2.7 didn't accept it yet.  As long as we don't assign to
   *(A) ``just before'' COMPARE_AND_EXCHANGE, so that gcc could
   consider the assignment dead, it should be fine.  */
# endif
#endif

#include "gccbuiltin.h"
#include "atomic.h"
#include "katomic.h"
#include "generic/comparexch.h"

#endif
