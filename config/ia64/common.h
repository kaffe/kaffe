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

#define NEED_STACK_ALIGN
#define STACK_ALIGN(p)  ((((unsigned long)(p)) & 15) ^ (unsigned long)(p))

#if NEED_sysdepCallMethod
#include "sysdepCallMethod.h"
#endif

#endif
