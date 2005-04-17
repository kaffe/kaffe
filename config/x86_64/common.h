/*
 * x86_64/common.h
 * Common X86-64 configuration information.
 *
 * Copyright (c) 2002
 *	MandrakeSoft.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

#ifndef __x86_64_common_h
#define __x86_64_common_h

#include "generic/gentypes.h"

/* Stack must be aligned on 16-bytes boundary.  */
#define NEED_STACK_ALIGN
#define STACK_ALIGN(p)  ((((unsigned long)(p)) & 15) ^ (unsigned long)(p))

/* This define will cause callMethodV and callMethodA to avoid
   introducing unused slots after jlongs and jdoubles.  */
#ifndef NO_HOLES
# define NO_HOLES 1
#endif

/* This define will cause callMethodV and callMethodA to promote every
   integer type to a 64bit word, and every float to double, so that
   every value can be loaded as a single 64bit word.  It also causes
   float arguments to be marked as 'D'.  */
#ifndef PROMOTE_TO_64bits
# define PROMOTE_TO_64bits 1
#endif
 
#ifndef PROMOTE_jfloat2jdouble
# define PROMOTE_jfloat2jdouble 0
#endif

#if defined(NEED_sysdepCallMethod)
#include "sysdepCallMethod.h"
#endif /* defined(NEED_sysdepCallMethod) */

#if defined(__GNUC__)
#define KAFFE_PROFILER 1
#endif

#if defined(KAFFE_PROFILER)

/* Profiler clicks counter type.  */
typedef uint64 profiler_click_t;

/* ReaD the processor Time Stamp Counter.
 * This is a macro to help GCC optimization.
 * The rdtsc instruction load TSC to edx:eax aka A register.  */
#define profiler_get_clicks(COUNTER) \
        asm volatile ("rdtsc" : "=A" (COUNTER))

#endif

#include "gccbuiltin.h"
#include "atomic.h"
#include "katomic.h"
#include "generic/comparexch.h"

#endif
