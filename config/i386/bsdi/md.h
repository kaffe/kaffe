/*
 * i386/bsdi/md.h
 * BSDI i386 configuration information.
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#ifndef __i386_bsdi_md_h
#define __i386_bsdi_md_h

#include "i386/common.h"
#include "i386/threads.h"

/*
 * Redefine stack pointer offset.
 */
#undef	SP_OFFSET
#define	SP_OFFSET	2

#if defined(TRANSLATOR)
#include "jit-md.h"
#endif


/*
        layout of i386_jump_buf.
	Note that sp must be in the 3rd position to be compatible with 
	native bsdi jump buffer.
                unsigned long Ebp;
                unsigned long Edi;
                unsigned long Esp;
                unsigned long Esi;
                unsigned long Ebx;
                unsigned long Eip;
                unsigned long Edx;
*/
typedef long i386_jmp_buf[7];
int i386_setjmp(i386_jmp_buf buf);
int i386_longjmp(i386_jmp_buf buf, int rc);

#undef JTHREAD_SETJMP
#define JTHREAD_SETJMP(buf)       i386_setjmp((buf))
#undef JTHREAD_LONGJMP
#define JTHREAD_LONGJMP(buf, val) i386_longjmp((buf), (val))
#undef JTHREAD_JMPBUF
#define JTHREAD_JMPBUF            i386_jmp_buf


#endif

