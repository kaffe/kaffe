/*
 * alpha/common.h
 * Common Alpha configuration information.
 *
 * Copyright (c) 1996, 1997, 1998, 1999
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 *
 * by Alexandre Oliva <oliva@dcc.unicamp.br>
 */

#ifndef __alpha_common_h
#define __alpha_common_h

#if NEED_sysdepCallMethod

#define LONG_SYSDEP 1
#include "sysdepCallMethod.h"
#undef LONG_SYSDEP
#include "sysdepCallMethod.h"

#endif /* NEED_sysdepCallMethod */

#endif
