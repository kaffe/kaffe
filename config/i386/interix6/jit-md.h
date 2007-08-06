/*
 * i386/i386/interix6/jit-md.h
 * Windows Vista Ultimate Interix 6 i386 JIT configuration information.
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * Copyright (c) 2007
 *	Kaffe.org contributors.  See ChangeLog for details.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#ifndef __i386_interix6_jit_md_h
#define __i386_interix6_jit_md_h

/**/
/* Include common information. */
/**/
#include "i386/jit.h"

/**/
/* Extra exception handling information. */
/**/

/*
 * No signal handler support yet!!
 */
#define	EXCEPTIONPROTO							\
	int sig

#define	EXCEPTIONFRAME(f, c)						\
	(f).retbp = 0;							\
	(f).retpc = 0

#endif
