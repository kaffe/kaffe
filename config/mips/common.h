/*
 * mips/common.h
 * Common MIPS configuration information.
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#ifndef __mips_common_h
#define __mips_common_h

#include <stddef.h>

#if _MIPS_SIM == _MIPS_SIM_NABI32
#define sysdepCallMethod(CALL) mipsN32CallMethod(CALL)
#elif _MIPS_SIM == _MIPS_SIM_ABI32
#define sysdepCallMethod(CALL) mips32CallMethod(CALL)
#endif /* _MIPS_SIM == _MIPS_SIM_NABI32 */

#endif
