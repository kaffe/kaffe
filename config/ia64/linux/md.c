/*
 * ia64/linux/md.c
 * Linux IA-64 specific functions.
 *
 * Copyright (c) 2001
 *	MandrakeSoft.  All rights reserved.
 *
 * Copyright (C) 2000
 *  Silicon Graphics, Inc.  All Rights Reserved.
 *  IA64_context_{save,restore} functions from State Threads Library
 *
 * Copyright (c) 2001
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

#include "config.h"
#include <stdlib.h>
#include <string.h>

void
init_md(void)
{
#if defined(M_MMAP_MAX) && defined(HAVE_MALLOPT)
	mallopt(M_MMAP_MAX, 0);
#endif
}

/*
 * Portions created by SGI are Copyright (C) 2000 Silicon Graphics, Inc.
 * All Rights Reserved.
 *
 * The internal __jmp_buf layout is different from one used
 * by setjmp()/longjmp().
 *
 *       Offset   Description
 *       ------   -----------
 *       0x000    stack pointer (r12)
 *       0x008    gp (r1)
 *       0x010    caller's unat
 *       0x018    fpsr
 *       0x020    r4
 *       0x028    r5
 *       0x030    r6
 *       0x038    r7
 *       0x040    rp (b0)
 *       0x048    b1
 *       0x050    b2
 *       0x058    b3
 *       0x060    b4
 *       0x068    b5
 *       0x070    ar.pfs
 *       0x078    ar.lc
 *       0x080    pr
 *       0x088    ar.bsp
 *       0x090    ar.unat
 *       0x098    &__jmp_buf
 *       0x0a0    ar.rsc
 *       0x0a8    ar.rnat
 *       0x0b0     f2
 *       0x0c0     f3
 *       0x0d0     f4
 *       0x0e0     f5
 *       0x0f0    f16
 *       0x100    f17
 *       0x110    f18
 *       0x120    f19
 *       0x130    f20
 *       0x130    f21
 *       0x140    f22
 *       0x150    f23
 *       0x160    f24
 *       0x170    f25
 *       0x180    f26
 *       0x190    f27
 *       0x1a0    f28
 *       0x1b0    f29
 *       0x1c0    f30
 *       0x1d0    f31
 *
 * Note that the address of __jmp_buf is saved but not used: we assume
 * that the jmp_buf data structure is never moved around in memory.
 */

/*
 * Implemented according to "IA-64 Software Conventions and Runtime
 * Architecture Guide", Chapter 10: "Context Management".
 */

        /* IA64_context_save(__jmp_buf env) */
        asm(" \n"
"        .text \n"
"        .align 32 \n"
"        .global IA64_context_save \n"
"        .proc IA64_context_save \n"
"IA64_context_save: \n"
"        alloc r14 = ar.pfs,1,0,0,0 \n"
"        mov r16 = ar.unat \n"
"        ;; \n"
"        mov r17 = ar.fpsr \n"
"        mov r2 = in0 \n"
"        add r3 = 8,in0 \n"
"        ;; \n"
"        st8.spill.nta [r2] = sp,16    // r12 (sp) \n"
"        ;; \n"
"        st8.spill.nta [r3] = gp,16    // r1  (gp) \n"
"        ;; \n"
"        st8.nta [r2] = r16,16         // save caller's unat \n"
"        st8.nta [r3] = r17,16         // save fpsr \n"
"        add r8 = 0xb0,in0 \n"
"        ;;    \n"
"        st8.spill.nta [r2] = r4,16    // r4 \n"
"        ;; \n"
"        st8.spill.nta [r3] = r5,16    // r5 \n"
"        add r9 = 0xc0,in0 \n"
"        ;; \n"
"        stf.spill.nta [r8] = f2,32 \n"
"        stf.spill.nta [r9] = f3,32 \n"
"        mov r15 = rp \n"
"        ;; \n"
"        stf.spill.nta [r8] = f4,32 \n"
"        stf.spill.nta [r9] = f5,32 \n"
"        mov r17 = b1 \n"
"        ;; \n"
"        stf.spill.nta [r8] = f16,32 \n"
"        stf.spill.nta [r9] = f17,32 \n"
"        mov r18 = b2 \n"
"        ;; \n"
"        stf.spill.nta [r8] = f18,32 \n"
"        stf.spill.nta [r9] = f19,32 \n"
"        mov r19 = b3 \n"
"        ;; \n"
"        stf.spill.nta [r8] = f20,32 \n"
"        stf.spill.nta [r9] = f21,32 \n"
"        mov r20 = b4 \n"
"        ;; \n"
"        stf.spill.nta [r8] = f22,32 \n"
"        stf.spill.nta [r9] = f23,32 \n"
"        mov r21 = b5 \n"
"        ;; \n"
"        stf.spill.nta [r8] = f24,32 \n"
"        stf.spill.nta [r9] = f25,32 \n"
"        mov r22 = ar.lc \n"
"        ;; \n"
"        stf.spill.nta [r8] = f26,32 \n"
"        stf.spill.nta [r9] = f27,32 \n"
"        mov r24 = pr \n"
"        ;; \n"
"        stf.spill.nta [r8] = f28,32 \n"
"        stf.spill.nta [r9] = f29,32 \n"
"        ;; \n"
"        stf.spill.nta [r8] = f30 \n"
"        stf.spill.nta [r9] = f31 \n"
" \n"
"        st8.spill.nta [r2] = r6,16    // r6 \n"
"        ;; \n"
"        st8.spill.nta [r3] = r7,16    // r7 \n"
"        ;; \n"
"        mov r23 = ar.bsp \n"
"        mov r25 = ar.unat \n"
" \n"
"        st8.nta [r2] = r15,16         // b0 \n"
"        st8.nta [r3] = r17,16         // b1 \n"
"        ;; \n"
"        st8.nta [r2] = r18,16         // b2 \n"
"        st8.nta [r3] = r19,16         // b3 \n"
"        mov r26 = ar.rsc \n"
"        ;; \n"
"        st8.nta [r2] = r20,16         // b4 \n"
"        st8.nta [r3] = r21,16         // b5 \n"
"        ;; \n"
"        st8.nta [r2] = r14,16         // ar.pfs \n"
"        st8.nta [r3] = r22,16         // ar.lc \n"
"        ;; \n"
"        st8.nta [r2] = r24,16         // pr \n"
"        st8.nta [r3] = r23,16         // ar.bsp \n"
"        ;; \n"
"        st8.nta [r2] = r25,16         // ar.unat \n"
"        st8.nta [r3] = in0,16         // &__jmp_buf (just in case) \n"
"        ;; \n"
"        st8.nta [r2] = r26            // ar.rsc \n"
"        ;; \n"
"        flushrs                       // flush dirty regs to backing store \n"
"        ;; \n"
"        and r27 = ~0x3,r26            // clear ar.rsc.mode \n"
"        ;; \n"
"        mov ar.rsc = r27              // put RSE in enforced lazy mode \n"
"        ;; \n"
"        mov r28 = ar.rnat \n"
"        ;; \n"
"        st8.nta [r3] = r28            // ar.rnat \n"
"        mov ar.rsc = r26              // restore ar.rsc \n"
"        ;; \n"
"        mov r8 = 0 \n"
"        br.ret.sptk.few b0 \n"
"        .endp IA64_context_save \n"
"        \n");


/****************************************************************/

        /* IA64_context_restore(__jmp_buf env, int val) */
        asm(" \n"
"        .text \n"
"        .global IA64_context_restore \n"
"        .proc IA64_context_restore \n"
"IA64_context_restore: \n"
"        alloc r8 = ar.pfs,2,0,0,0 \n"
"        add r2 = 0x88,in0             // r2 <- &jmpbuf.ar_bsp \n"
"        mov r16 = ar.rsc \n"
"        ;; \n"
"        flushrs                       // flush dirty regs to backing store \n"
"        ;; \n"
"        and r17 = ~0x3,r16            // clear ar.rsc.mode \n"
"        ;; \n"
"        mov ar.rsc = r17              // put RSE in enforced lazy mode \n"
"        ;; \n"
"        invala                        // invalidate the ALAT \n"
"        ;; \n"
"        ld8 r23 = [r2],8              // r23 <- jmpbuf.ar_bsp \n"
"        ;; \n"
"        mov ar.bspstore = r23         // write BSPSTORE \n"
"        ld8 r25 = [r2],24             // r25 <- jmpbuf.ar_unat \n"
"        ;; \n"
"        ld8 r26 = [r2],-8             // r26 <- jmpbuf.ar_rnat \n"
"        ;; \n"
"        mov ar.rnat = r26             // write RNAT \n"
"        ld8 r27 = [r2]                // r27 <- jmpbuf.ar_rsc \n"
"        ;; \n"
"        mov ar.rsc = r27              // write RSE control \n"
"        mov r2 = in0 \n"
"        ;; \n"
"        mov ar.unat = r25             // write ar.unat \n"
"        add r3 = 8,in0 \n"
"        ;; \n"
"        ld8.fill.nta sp = [r2],16     // r12 (sp) \n"
"        ld8.fill.nta gp = [r3],16     // r1  (gp) \n"
"        ;; \n"
"        ld8.nta r16 = [r2],16         // caller's unat \n"
"        ld8.nta r17 = [r3],16         // fpsr \n"
"        ;; \n"
"        ld8.fill.nta r4 = [r2],16     // r4 \n"
"        ld8.fill.nta r5 = [r3],16     // r5 \n"
"        ;; \n"
"        ld8.fill.nta r6 = [r2],16     // r6 \n"
"        ld8.fill.nta r7 = [r3],16     // r7 \n"
"        ;; \n"
"        mov ar.unat = r16             // restore caller's unat \n"
"        mov ar.fpsr = r17             // restore fpsr \n"
"        ;; \n"
"        ld8.nta r16 = [r2],16         // b0 \n"
"        ld8.nta r17 = [r3],16         // b1 \n"
"        ;; \n"
"        ld8.nta r18 = [r2],16         // b2 \n"
"        ld8.nta r19 = [r3],16         // b3 \n"
"        ;; \n"
"        ld8.nta r20 = [r2],16         // b4 \n"
"        ld8.nta r21 = [r3],16         // b5 \n"
"        ;; \n"
"        ld8.nta r11 = [r2],16         // ar.pfs \n"
"        ld8.nta r22 = [r3],72         // ar.lc \n"
"        ;; \n"
"        ld8.nta r24 = [r2],48         // pr \n"
"        mov b0 = r16 \n"
"        ;; \n"
"        ldf.fill.nta f2 = [r2],32 \n"
"        ldf.fill.nta f3 = [r3],32 \n"
"        mov b1 = r17 \n"
"        ;; \n"
"        ldf.fill.nta f4 = [r2],32 \n"
"        ldf.fill.nta f5 = [r3],32 \n"
"        mov b2 = r18 \n"
"        ;; \n"
"        ldf.fill.nta f16 = [r2],32 \n"
"        ldf.fill.nta f17 = [r3],32 \n"
"        mov b3 = r19 \n"
"        ;; \n"
"        ldf.fill.nta f18 = [r2],32 \n"
"        ldf.fill.nta f19 = [r3],32 \n"
"        mov b4 = r20 \n"
"        ;; \n"
"        ldf.fill.nta f20 = [r2],32 \n"
"        ldf.fill.nta f21 = [r3],32 \n"
"        mov b5 = r21 \n"
"        ;; \n"
"        ldf.fill.nta f22 = [r2],32 \n"
"        ldf.fill.nta f23 = [r3],32 \n"
"        mov ar.lc = r22 \n"
"        ;; \n"
"        ldf.fill.nta f24 = [r2],32 \n"
"        ldf.fill.nta f25 = [r3],32 \n"
"        cmp.eq p6,p7 = 0,in1 \n"
"        ;; \n"
"        ldf.fill.nta f26 = [r2],32 \n"
"        ldf.fill.nta f27 = [r3],32 \n"
"        mov ar.pfs = r11 \n"
"        ;; \n"
"        ldf.fill.nta f28 = [r2],32 \n"
"        ldf.fill.nta f29 = [r3],32 \n"
"        ;; \n"
"        ldf.fill.nta f30 = [r2] \n"
"        ldf.fill.nta f31 = [r3] \n"
"(p6)    mov r8 = 1 \n"
"(p7)    mov r8 = in1 \n"
" \n"
"        mov pr = r24,-1 \n"
"        br.ret.sptk.few b0 \n"
"        .endp IA64_context_restore \n"
"        ");

/****************************************************************/
