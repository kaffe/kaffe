/*
 * mips/common.h
 * Common MIPS configuration information.
 *
 * Copyright (c) 1996, 1997, 1998, 1999
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 *
 * by Alexandre Oliva <oliva@dcc.unicamp.br>
 */

#ifndef __mips_common_h
#define __mips_common_h

#if NEED_sysdepCallMethod

#if _MIPS_SIM == _MIPS_SIM_NABI32

#define LONG_SYSDEP 1
#include "n32-sysdepCallMethod.h"
#undef LONG_SYSDEP
#include "n32-sysdepCallMethod.h"

#elif _MIPS_SIM == _MIPS_SIM_ABI32

#define LONG_SYSDEP 1
#include "o32-sysdepCallMethod.h"
#undef LONG_SYSDEP
#include "o32-sysdepCallMethod.h"

#else

#error "Calling convention not specified"

#endif

#endif /* NEED_sysdepCallMethod */

#endif
