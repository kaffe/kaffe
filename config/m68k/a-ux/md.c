/*
 * m68k/aux/md.c
 * A/UX specific functions.
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#ifdef TRANSLATOR
void
init_md(void)
{
	extern int jit_debug;
	if (getenv("KAFFE_JIT_DEBUG"))
		jit_debug = 1;
}
#endif
