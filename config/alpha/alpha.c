/*
 * alpha/alpha.c
 * Alpha specific functions.
 *
 * Copyright (c) 2001
 *	Edouard G. Parmelan.  All rights reserved.
 *
 * Copyright (c) 2001
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

#include "gtypes.h"
#include "debug.h"
#include "../../kaffe/kaffevm/thread.h"

/* return register number that contains return address if function at
   pc is a frame-less procedure.  Else return -1. */
int
__alpha_ra (int *pc)
{
    int *p;
    int reg;

    for (p = pc; ; p++) {
	if ((*p & ~(0x1F << 16)) == 0x6be08001) {
	    /* ret $31,($REG),1 */
	    reg = (*p >> 16) & 0x1F;
	    return reg;
	}

	if ((*p & 0xffff0000) == ((0x8 << 26) | ((30 & 0x1f) << 21) | ((30 & 0x1f) << 16))) {
	    /* lda $sp,N($sp) */
	    return -1;
	}
    }
}


exceptionFrame *
__alpha_nextFrame (exceptionFrame *frame)
{
    int *pc;
    char *sp;
    char *fp;

    int *p;
    int reg;
    int framesize;
    int use_fp;

    if (frame->sp == 0) {
	/* first frame, retreive current $sp and $ra and compute
	   caller frame because this stack will be erased after this
	   function return */
	register char *_sp asm("30");
	register char *_fp asm("15");

    here: ;
	pc = &&here;
	sp = _sp;
	fp = _fp;

	DBG(STACKTRACE,
	    dprintf ("firstFrame pc %p sp %p fp %p\n", pc, sp, fp); );
    }
    else {
	pc = (int *)frame->pc;
	sp = frame->sp;
	fp = frame->fp;
	DBG(STACKTRACE,
	    dprintf ("nextFrame  pc %p sp %p fp %p\n", pc, sp, fp); );
    }

    if (!jthread_on_current_stack(sp)) {
	return NULL;
    }

    reg = 26;
    framesize = 0;
    use_fp = 0;

    /* First, walk forward and stop on these instructions:
       - ret 31,($REG),1 frame less procedure
       - lda $sp,-N($sp) __attribute__("noreturn")
    */
    /* Scan forward and stop on ret 31,($REG),1 or lda $sp,-N($sp) for
       __attribute__("noreturn") function.  */
    for (p = pc; ; p++) {

	if ((*p & ~(0x1F << 16)) == 0x6be08001) {
	    /* ret $31,($REG),1 */
	    if (framesize == 0) {
		/* frame-less procedure in call chain is not possible.
                   Handle __attribute__("noreturn") function followed
                   by a frame-less procedure as SignalErrorf().  */
		DBG(STACKTRACE,
		    dprintf ("frame-less procedure in call chain\n"); );
		goto scan_backward;
	    }

	    reg = (*p >> 16) & 0x1F;
	    DBG(STACKTRACE,
		dprintf ("ret $31,($%d),1 at pc %p\n", reg, p); );
	    goto extract_ldq;
	}

	if (*p == 0x45ef041e) {
	    /* mov $15,$30 */
	    /* this function use alloca() */
	    use_fp = 1;
	    DBG(STACKTRACE,
		dprintf ("mov $15,$30 at pc %p\n", p); );
	    continue;
	}

	if ((*p & 0xffff0000) == ((0x8 << 26) | ((30 & 0x1f) << 21) | ((30 & 0x1f) << 16))) {
	    /* lda $sp,N($sp) */
	    short low = *p & 0x0000ffff;

	    DBG(STACKTRACE,
		dprintf ("lda $sp,%d($sp) at %p\n", (int)low, p); );

	    if (low < 0) {
		/* lda $sp,-N($sp) */
		if (framesize == 0) {
		    /* pass end of function without finding epilogue
		       marker (ret instruction), look backward for
		       prologue.  Handle __attribute__("noreturn")
		       function followed by a frame procedure as
		       ThrowError().  */
		    goto scan_backward;
		}

		/* This also catch __attribute__("noreturn") function
                   but we have already catch stack frame.  */
		goto extract_ldq;
	    }
	    /* save framesize */
	    framesize = low;
	    continue;
	}
    }

 scan_backward:
    use_fp = 0;

    /* Was unable to find epilogue, scan backware to search framesize.  */
    for (p = pc; ; p--) {

	if ((*p & 0xffff0000) == ((0x8 << 26) | ((30 & 0x1f) << 21) | ((30 & 0x1f) << 16))) {
	    /* lda $sp,N($sp) */
	    short low = *p & 0x0000ffff;

	    DBG(STACKTRACE,
		dprintf ("lda $sp,%d($sp) at %p\n", (int)low, p); );

	    if (low < 0) {
		/* lda $sp,-N($sp) */
		framesize = -low;
		goto extract_stq;
	    }
	    else {
		/* lda $sp,N($sp) */
		/* scan _before_ start of function */
		return NULL;
	    }
	}
    }

 extract_stq:
    /* extract $REG and $fp from register saved area.  */
    for (; p < pc; p++) {
	if ((*p & 0xffff0000) == ((0x2d << 26) | ((reg & 0x1f) << 21) | ((30 & 0x1f) << 16))) {
	    /* stq $REG,N(sp) */
	    short low = *p & 0x0000ffff;

	    DBG(STACKTRACE,
		dprintf ("stq $%d,%d($sp) at %p\n", reg, low, p); );
	    frame->pc = (char*) *(long *) (sp + low);
	    continue;
	}

	if ((*p & 0xffff0000) == ((0x2d << 26) | ((15 & 0x1f) << 21) | ((30 & 0x1f) << 16))) {
	    /* stq $fp,N(sp) */
	    short low = *p & 0x0000ffff;

	    DBG(STACKTRACE,
		dprintf ("stq $fp,%d($sp) at %p\n", low, p); );
	    frame->fp = (char*) *(long *) (sp + low);
	    continue;
	}
    }
    goto end;

 extract_ldq:
    /* extract $REG and $fp from register saved area.  */
    if (use_fp)
	sp = fp;

    use_fp = 0;
    for (; p > pc; p--) {
	if ((*p & 0xffff0000) == ((0x29 << 26) | ((reg & 0x1f) << 21) | ((30 & 0x1f) << 16))) {
	    /* ldq $REG,N(sp) */
	    short low = *p & 0x0000ffff;

	    DBG(STACKTRACE,
		dprintf ("ldq $%d,%d($sp) at %p\n", reg, low, p); );
	    frame->pc = (char*) *(long *) (sp + low);
	    use_fp = 1;
	    continue;
	}

	if ((*p & 0xffff0000) == ((0x29 << 26) | ((15 & 0x1f) << 21) | ((30 & 0x1f) << 16))) {
	    /* ldq $fp,N(sp) */
	    short low = *p & 0x0000ffff;

	    DBG(STACKTRACE,
		dprintf ("ldq $fp,%d($sp) at %p\n", low, p); );
	    frame->fp = (char*) *(long *) (sp + low);
	    continue;
	}
    }
    /* special case for __divl() that don't reload t9 but save it.  */
    if (use_fp == 0)
	goto scan_backward;

 end:
    frame->sp = sp + framesize;
    return frame;
}
