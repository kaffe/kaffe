/*
 * ia64/common.h
 * Common IA-64 configuration information.
 *
 * Copyright (c) 2001
 *	MandrakeSoft.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

#ifndef __ia64_common_h
#define __ia64_common_h

#include "generic/gentypes.h"

#define NEED_STACK_ALIGN
#define STACK_ALIGN(p)  ((((unsigned long)(p)) & 15) ^ (unsigned long)(p))

#if defined(NEED_sysdepCallMethod)
#include "sysdepCallMethod.h"
#endif /* defined(NEED_sysdepCallMethod) */

/*
 * Do an atomic compare and exchange.  The address 'A' is checked
 * against value 'O' and if they match it's exchanged with value 'N'.
 * We return '1' if the exchange is sucessful, otherwise 0.
 */

#ifdef HAVE_IA64INTRIN_H
#include <ia64intrin.h>
#endif

#include "gccbuiltin.h"
#include "atomic.h"
#include "katomic.h"
#include "generic/comparexch.h"

#endif
