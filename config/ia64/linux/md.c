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
 * Copyright (C) 2000 Silicon Graphics, Inc.  All Rights Reserved.
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
        asm("
        .text
        .align 32
        .global IA64_context_save#
        .proc IA64_context_save#
IA64_context_save:
        alloc r14 = ar.pfs,1,0,0,0
        mov r16 = ar.unat
        ;;
        mov r17 = ar.fpsr
        mov r2 = in0
        add r3 = 8,in0
        ;;
        st8.spill.nta [r2] = sp,16    // r12 (sp)
        st8.spill.nta [r3] = gp,16    // r1  (gp)
        ;;
        st8.nta [r2] = r16,16         // save caller's unat
        st8.nta [r3] = r17,16         // save fpsr
        add r8 = 0xb0,in0
        ;;   
        st8.spill.nta [r2] = r4,16    // r4
        st8.spill.nta [r3] = r5,16    // r5
        add r9 = 0xc0,in0
        ;;
        stf.spill.nta [r8] = f2,32
        stf.spill.nta [r9] = f3,32
        mov r15 = rp
        ;;
        stf.spill.nta [r8] = f4,32
        stf.spill.nta [r9] = f5,32
        mov r17 = b1
        ;;
        stf.spill.nta [r8] = f16,32
        stf.spill.nta [r9] = f17,32
        mov r18 = b2
        ;;
        stf.spill.nta [r8] = f18,32
        stf.spill.nta [r9] = f19,32
        mov r19 = b3
        ;;
        stf.spill.nta [r8] = f20,32
        stf.spill.nta [r9] = f21,32
        mov r20 = b4
        ;;
        stf.spill.nta [r8] = f22,32
        stf.spill.nta [r9] = f23,32
        mov r21 = b5
        ;;
        stf.spill.nta [r8] = f24,32
        stf.spill.nta [r9] = f25,32
        mov r22 = ar.lc
        ;;
        stf.spill.nta [r8] = f26,32
        stf.spill.nta [r9] = f27,32
        mov r24 = pr
        ;;
        stf.spill.nta [r8] = f28,32
        stf.spill.nta [r9] = f29,32
        ;;
        stf.spill.nta [r8] = f30
        stf.spill.nta [r9] = f31
        st8.spill.nta [r2] = r6,16    // r6
        st8.spill.nta [r3] = r7,16    // r7
        ;;
        mov r23 = ar.bsp
        mov r25 = ar.unat
        st8.nta [r2] = r15,16         // b0
        st8.nta [r3] = r17,16         // b1
        ;;
        st8.nta [r2] = r18,16         // b2
        st8.nta [r3] = r19,16         // b3
        mov r26 = ar.rsc
        ;;
        st8.nta [r2] = r20,16         // b4
        st8.nta [r3] = r21,16         // b5
        ;;
        st8.nta [r2] = r14,16         // ar.pfs
        st8.nta [r3] = r22,16         // ar.lc
        ;;
        st8.nta [r2] = r24,16         // pr
        st8.nta [r3] = r23,16         // ar.bsp
        ;;
        st8.nta [r2] = r25,16         // ar.unat
        st8.nta [r3] = in0,16         // &__jmp_buf (just in case)
        ;;
        st8.nta [r2] = r26            // ar.rsc
        ;;
        flushrs                       // flush dirty regs to backing store
        ;;
        and r27 = ~0x3,r26            // clear ar.rsc.mode
        ;;
        mov ar.rsc = r27              // put RSE in enforced lazy mode
        ;;
        mov r28 = ar.rnat
        ;;
        st8.nta [r3] = r28            // ar.rnat
        mov ar.rsc = r26              // restore ar.rsc
        ;;
        mov r8 = 0
        br.ret.sptk.few b0
        .endp IA64_context_save#
        ");

        /* IA64_context_restore(__jmp_buf env, int val) */
        asm("
        .text
        .align 32
        .global IA64_context_restore#
        .proc IA64_context_restore#
IA64_context_restore:
        alloc r8 = ar.pfs,2,0,0,0
        add r2 = 0x88,in0             // r2 <- &jmpbuf.ar_bsp
        mov r16 = ar.rsc
        ;;
        flushrs                       // flush dirty regs to backing store
        ;;
        and r17 = ~0x3,r16            // clear ar.rsc.mode
        ;;
        mov ar.rsc = r17              // put RSE in enforced lazy mode
        ;;
        invala                        // invalidate the ALAT
        ;;
        ld8 r23 = [r2],8              // r23 <- jmpbuf.ar_bsp
        ;;
        mov ar.bspstore = r23         // write BSPSTORE
        ld8 r25 = [r2],24             // r25 <- jmpbuf.ar_unat
        ;;
        ld8 r26 = [r2],-8             // r26 <- jmpbuf.ar_rnat
        ;;
        mov ar.rnat = r26             // write RNAT
        ld8 r27 = [r2]                // r27 <- jmpbuf.ar_rsc
        ;;
        mov ar.rsc = r27              // write RSE control
        mov r2 = in0
        ;;
        mov ar.unat = r25             // write ar.unat
        add r3 = 8,in0
        ;;
        ld8.fill.nta sp = [r2],16     // r12 (sp)
        ld8.fill.nta gp = [r3],16     // r1  (gp)
        ;;
        ld8.nta r16 = [r2],16         // caller's unat
        ld8.nta r17 = [r3],16         // fpsr
        ;;
        ld8.fill.nta r4 = [r2],16     // r4
        ld8.fill.nta r5 = [r3],16     // r5
        ;;
        ld8.fill.nta r6 = [r2],16     // r6
        ld8.fill.nta r7 = [r3],16     // r7
        ;;
        mov ar.unat = r16             // restore caller's unat
        mov ar.fpsr = r17             // restore fpsr
        ;;
        ld8.nta r16 = [r2],16         // b0
        ld8.nta r17 = [r3],16         // b1
        ;;
        ld8.nta r18 = [r2],16         // b2
        ld8.nta r19 = [r3],16         // b3
        ;;
        ld8.nta r20 = [r2],16         // b4
        ld8.nta r21 = [r3],16         // b5
        ;;
        ld8.nta r11 = [r2],16         // ar.pfs
        ld8.nta r22 = [r3],72         // ar.lc
        ;;
        ld8.nta r24 = [r2],48         // pr
        mov b0 = r16
        ;;
        ldf.fill.nta f2 = [r2],32
        ldf.fill.nta f3 = [r3],32
        mov b1 = r17
        ;;
        ldf.fill.nta f4 = [r2],32
        ldf.fill.nta f5 = [r3],32
        mov b2 = r18
        ;;
        ldf.fill.nta f16 = [r2],32
        ldf.fill.nta f17 = [r3],32
        mov b3 = r19
        ;;
        ldf.fill.nta f18 = [r2],32
        ldf.fill.nta f19 = [r3],32
        mov b4 = r20
        ;;
        ldf.fill.nta f20 = [r2],32
        ldf.fill.nta f21 = [r3],32
        mov b5 = r21
        ;;
        ldf.fill.nta f22 = [r2],32
        ldf.fill.nta f23 = [r3],32
        mov ar.lc = r22
        ;;
        ldf.fill.nta f24 = [r2],32
        ldf.fill.nta f25 = [r3],32
        cmp.eq p6,p7 = 0,in1
        ;;
        ldf.fill.nta f26 = [r2],32
        ldf.fill.nta f27 = [r3],32
        mov ar.pfs = r11
        ;;
        ldf.fill.nta f28 = [r2],32
        ldf.fill.nta f29 = [r3],32
        ;;
        ldf.fill.nta f30 = [r2]
        ldf.fill.nta f31 = [r3]
(p6)    mov r8 = 1
(p7)    mov r8 = in1
        mov pr = r24,-1
        br.ret.sptk.few b0
        .endp IA64_context_restore#
        ");
