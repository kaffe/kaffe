/*
 * sparc/linux/md.c
 * Linux sparc specific functions.
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#include "config.h"
#include <malloc.h>

void
init_md(void)
{
#if defined(HAVE_MALLOPT)
	mallopt(M_MMAP_MAX, 0);
#endif
}

#include "systems/unix-jthreads/jthread.h"

#define	GET_SP(E)	(((void**)(E))[SP_OFFSET])

extern int __speciallongjmp(void*);
asm("
__speciallongjmp:
	mov %o0,%g1
	ta 3
	ld [%g1],%sp
	ld [%g1+4],%fp
	ld [%g1+8],%g1
	jmp %g1+8
	mov 1,%o0
");

void
sparcLinuxContextSwitch(jthread* from, jthread* to)
{
    if (setjmp(from->env) == 0) {
	from->restorePoint = GET_SP(from->env);
	__speciallongjmp(to->env);
    }
}
