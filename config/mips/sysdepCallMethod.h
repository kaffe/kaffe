/*
 * mips/sysdepCallMethod.h
 * Common MIPS configuration information.
 *
 * Copyright (c) 1996, 1997, 1998, 1999
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * Copyright (c) 2003
 *      Kaffe.org contributors. See ChangeLog for details.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 *
 * by Alexandre Oliva <oliva@dcc.unicamp.br>
 * and Edouard G. Parmelan <egp@free.fr>
 */

#ifndef __mips_sysdepCallMethod_h
#define __mips_sysdepCallMethod_h

#if defined(NEED_sysdepCallMethod)

#if defined(HAVE_SGIDEFS_H)
#include <sgidefs.h>
#endif /* defined(HAVE_SGIDEFS_H) */

#include "support.h"

#if !defined (_MIPS_SIM) || (_MIPS_SIM == _MIPS_SIM_ABI32)

#define LONG_SYSDEP 1
#include "o32-sysdepCallMethod.h"
#undef LONG_SYSDEP
#include "o32-sysdepCallMethod.h"

#elif (_MIPS_SIM == _MIPS_SIM_NABI32)

#define LONG_SYSDEP 1
#include "n32-sysdepCallMethod.h"
#undef LONG_SYSDEP
#include "n32-sysdepCallMethod.h"

#else

#error "Calling convention not specified"

#endif

#endif /* defined(NEED_sysdepCallMethod) */

#endif /* __mips_sysdepCallMethod_h */
