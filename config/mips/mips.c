/*
 * mips/mips.c
 * MIPS specific functions.
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#define	TDBG(s)

#include "config.h"
#include "config-std.h"
#include "config-mem.h"
#include "gtypes.h"
#include "classMethod.h"
#include "object.h"
#include "support.h"
#include "../../kaffe/kaffevm/thread.h"

extern int maxArgs;
extern int isStatic;

/*
 * Keep track of the argument mapping (we need this because the MIPS
 * aligns it's call arguments.
 */
int totalMaxArgs = -1;
int* argMap;

/* This function is here primarily to fake the compiler into thinking
 * __mipsGetNextFrame isn't a leaf function (if it were then things would
 * break).
 */
static
void
endOfFrames(exceptionFrame* fm)
{
	fm->return_pc = 0;
	fm->return_frame = 0;
}

void
__mipsGetNextFrame(exceptionFrame* fm)
{
	int* spc;
	int* pc;
	int* fp;
	int* ppc;
	int* pfp;

	if (fm->return_frame == 0) {
here:;
		spc = &&here;
		asm("move %0,$fp" : "=r" (fp));
	}
	else {
		fp = (int*)fm->return_frame;
		spc = (int*)fm->return_pc;
	}

#if 0
	/* Walk backwards down the code looking for the beginning of the
	 * function.  This assumes standard calling convention without a
	 * stack pointer.  We don't currently use this but keep it around
	 * in case this changes.
	 */
	for (pc = spc;; pc--) {
		unsigned short high = (short)((*pc) >> 16);
		short low = (short)*pc;
		switch (high) {
		case 0x27bd:	/* addiu $sp,$sp,-i */
		case 0x23bd:	/* addi $sp,$sp,-i */
		case 0x67bd:	/* daddiu $sp,$sp,-i */
			TDBG(kprintf("Found at %p: framesize = %d\n", pc, low);)
			TDBG(kprintf(" sp = %p\n", fp);)
			pfp = fp - (low / sizeof(int));
			TDBG(kprintf(" prev fp = %p\n", pfp);)
			break;

		default:
			continue;
		}
		break;
	}
#endif
	/* Walk backwards down the code looking for where the return
	 * pc is stored.
	 */
	TDBG(kprintf("starting search at %p\n", spc);)
	for (pc = spc;; pc--) {
		unsigned short high = (unsigned short)((*pc) >> 16);
		short low = (short)*pc;
		switch (high) {
		case 0xafbf:	/* sw $ra,i($sp) */
			TDBG(kprintf(" &ra = %p\n", pc);)
			ppc = (int*)fp[low / sizeof(int)];
			TDBG(kprintf(" prev pc = %p\n", ppc);)
			break;

		case 0xafdf:	/* sw $ra,i($fp) */
			TDBG(kprintf(" &ra = %p\n", pc);)
			ppc = (int*)fp[low / sizeof(int)];
			TDBG(kprintf(" prev pc = %p\n", ppc);)
			/* This means we're in JIT code so the following
			 * instruction is the frame pointer store.
			 *	sw $i1 (really oldfp),i($fp)
			 */
			pc++;
			TDBG(kprintf("Found at %p: fp offset = %d\n", pc, low);)
			high = (unsigned short)((*pc) >> 16);
			assert(high == 0xafc1);
			low = (short)*pc;
			TDBG(kprintf(" fp = %p\n", fp);)
			pfp = (int*)fp[low / sizeof(int)];
			TDBG(kprintf(" prev fp = %p\n", pfp);)
			goto end;

		case 0x0000:	/* move $ra,$zero */
			if ((unsigned short)low == 0xf821) {
				ppc = 0;
				break;
			}
			continue;

		default:
			continue;
		}
		break;
	}

	/* Walk backwards down the code looking for where we stored the
	 * previous frame pointer.
	 */
	TDBG(kprintf("starting search at %p\n", spc);)
	for (pc = spc;; pc--) {
		unsigned short high = (unsigned short)((*pc) >> 16);
		short low = (short)*pc;
		switch (high) {
		case 0xafbe:	/* sw $fp,i($sp) */
			TDBG(kprintf("Found at %p: fp offset = %d\n", pc, low);)
			TDBG(kprintf(" fp = %p\n", fp);)
			pfp = (int*)fp[low / sizeof(int)];
			TDBG(kprintf(" prev fp = %p\n", pfp);)
			/* If previous frame pointer is back up the stack
			 * then we must have got to the end.
			 */
			if (pfp < fp) {
				pfp = 0;
			}
			break;

		default:
			continue;
		}
		break;
	}

end:;
	if (ppc == 0 || pfp == 0) {
		endOfFrames(fm);
	}
	else {
		fm->return_pc = (char*)ppc;
		fm->return_frame = (char*)pfp;
	}
}

/*
 * On the MIPS the arguments are aligned to their natural boundaries, so
 * we must build a mapping between arguments and offsets (we can't use the
 * usual simple one-to-one mapping).
 */
void
__mipsInitJit(Method* meth)
{
	int i;
	int j;
	int a;
	const char* sig;

	a = maxArgs;
#if defined(STACK_LIMIT)
	a++;
#endif
	if (a > totalMaxArgs) {
		totalMaxArgs = a;
		argMap = jrealloc(argMap, totalMaxArgs * sizeof(int));
	}

	i = 0;
	a = 0;

	if (!isStatic) {
		argMap[i] = a;
		i++;
		a++;
	}

	sig = meth->signature->data;
	for (sig++; *sig != ')'; sig++) {
		char ch = *sig;
		if (ch == 'D' || ch == 'J') {
			a += a % 2;
			argMap[i++] = a++;
			argMap[i++] = a++;
		}
		else {
			if (ch == '[') {
				while (*sig == '[') {
					sig++;
				}
				if (*sig == 'L') {
					sig = strchr(sig, ';');
				}
			}
			else if (ch == 'L') {
				sig = strchr(sig, ';');
			}
			argMap[i++] = a++;
		}
	}
#if defined(STACK_LIMIT)
	argMap[i++] = a++;
#endif
}
