/*
 * kaffe/trampoline.h
 * Common trampoline macros.
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 * 
 * Copyright (c) 2005
 *      Kaffe.org developers. See ChangeLog for details.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#if !defined(KAFFE_TRAMPOLINE_H)
#define KAFFE_TRAMPOLINE_H

#if defined(HAVE_CONFIG_H)
#include "config.h"
#endif

#if !defined(C_FUNC_NAME)	
#if defined(HAVE_UNDERSCORED_C_NAMES)
#define	C_FUNC_NAME(FUNC) _ ## FUNC
#else
#define	C_FUNC_NAME(FUNC) FUNC
#endif
#endif 

#endif /* !defined(KAFFE_TRAMPOLINE_H) */
