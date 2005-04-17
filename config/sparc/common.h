/*
 * sparc/common.h
 * Common SPARC configuration information.
 *
 * Copyright (c) 1996, 1997, 1998, 1999
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 *
 * by Alexandre Oliva <oliva@dcc.unicamp.br>
 */

#ifndef __sparc_common_h
#define __sparc_common_h

#include "generic/gentypes.h"

#include "gccbuiltin.h"

/* Include the right compare_and_swap definition */
#if defined(__sparcv9)
#include "atomic-sparc32v9.h"
#else /* !defined(__sparcv9) */
#if defined(__arch64__)
#include "atomic-sparc64.h"
#else /* !defined(__arch64__) */
#include "atomic-sparc32.h"
#endif /* defined(__arch64__) */
#endif /* defined(__sparcv9) */

#include "katomic.h"

#if defined(NEED_sysdepCallMethod)

#define LONG_SYSDEP 1
#include "sysdepCallMethod.h"
#undef LONG_SYSDEP
#include "sysdepCallMethod.h"

#endif /* defined(NEED_sysdepCallMethod) */

#include "generic/comparexch.h"

#endif
