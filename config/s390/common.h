/*
 * s390/common.h (cloned from s390/common.h by ROSSP)
 * Common s390 configuration information.
 *
 * Copyright (c) 1996, 1997, 1998, 1999
 * Transvirtual Technologies, Inc.  All rights reserved.
 *
 * Copyright (c) 2003
 *      Kaffe.org contributors. See ChangeLog for details.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#ifndef __s390_h
#define __s390_h

#include "generic/gentypes.h"

/* The s390 gcc port aligns to the appropriate boundary: 1->1, 2->2, 4->4,
   and 8->8.  So alignment is pretty simple: */
#define  ALIGNMENT_OF_SIZE(S) (S)

#if defined(__GNUC__)
#define KAFFE_PROFILER 1
#endif

#if defined(KAFFE_PROFILER)

/* profiler clicks counter type.  */
typedef int64  profiler_click_t;

/* ReaD the processor Time Stamp Counter.
 * This is a macro to help GCC optimization. 
 * The rdtsc instruction load TSC to edx:eax aka A register.  */
#if 1 /* Not ready yet */
#define profiler_get_clicks(COUNTER)    \
   The s390 port does not have a profiler_get_clicks macro yet!
#else
#define profiler_get_clicks(COUNTER)    \
   asm volatile (".byte 0xf; .byte 0x31" /* "rdtsc" */ : "=A" (COUNTER))
#endif /* if 1 */

#endif

#include "gccbuiltin.h"
#include "atomic.h"
#include "katomic.h"
#include "generic/comparexch.h"

#endif /* __s390_h */
