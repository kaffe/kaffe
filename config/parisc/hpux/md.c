/*
 * parisc/hpux/md.c
 * HP-UX PArisc specific functions.
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

   asm(".text
        .align 4
        .EXPORT parisc_threadswitch,ENTRY,PRIV_LEV=3,RTNVAL=GR
parisc_threadswitch
        .PROC
        .CALLINFO FRAME=192
        .ENTRY
        ldo 192(%r30),%r30
        stw %r2,-180(0,%r30)
        stw %r2,-176(0,%r30)
        stw %r3,-172(0,%r30)
        stw %r4,-168(0,%r30)
        stw %r5,-164(0,%r30)
        stw %r6,-160(0,%r30)
        stw %r7,-156(0,%r30)
        stw %r8,-152(0,%r30)
        stw %r9,-148(0,%r30)
        stw %r10,-144(0,%r30)
        stw %r11,-140(0,%r30)
        stw %r12,-136(0,%r30)
        stw %r13,-132(0,%r30)
        stw %r14,-128(0,%r30)
        stw %r15,-124(0,%r30)
        stw %r16,-120(0,%r30)
        stw %r17,-116(0,%r30)
        stw %r18,-112(0,%r30)
        ldo -104(%r30),%r1
        fstds,ma %fr12,8(0,%r1)
        fstds,ma %fr13,8(0,%r1)
        fstds,ma %fr14,8(0,%r1)
        fstds,ma %fr15,8(0,%r1)
        fstds,ma %fr16,8(0,%r1)
        fstds,ma %fr17,8(0,%r1)
        fstds,ma %fr18,8(0,%r1)
        fstds,ma %fr19,8(0,%r1)
        fstds,ma %fr20,8(0,%r1)
        fstds,ma %fr21,8(0,%r1)
        stw %r30,0(0,%r26)
        ldw 0(0,%r25),%r30
        ldw -172(0,%r30),%r3
        ldw -168(0,%r30),%r4
        ldw -164(0,%r30),%r5
        ldw -160(0,%r30),%r6
        ldw -156(0,%r30),%r7
        ldw -152(0,%r30),%r8
        ldw -148(0,%r30),%r9
        ldw -144(0,%r30),%r10
        ldw -140(0,%r30),%r11
        ldw -136(0,%r30),%r12
        ldw -132(0,%r30),%r13
        ldw -128(0,%r30),%r14
        ldw -124(0,%r30),%r15
        ldw -120(0,%r30),%r16
        ldw -116(0,%r30),%r17
        ldw -112(0,%r30),%r18
        ldo -104(%r30),%r1
        fldds,ma 8(0,%r1),%fr12
        fldds,ma 8(0,%r1),%fr13
        fldds,ma 8(0,%r1),%fr14
        fldds,ma 8(0,%r1),%fr15
        fldds,ma 8(0,%r1),%fr16
        fldds,ma 8(0,%r1),%fr17
        fldds,ma 8(0,%r1),%fr18
        fldds,ma 8(0,%r1),%fr19
        fldds,ma 8(0,%r1),%fr20
        fldds,ma 8(0,%r1),%fr21
        ldw -180(0,%r30),%r2
	comib,<> 0,%r2,__nodyncall
        nop
        ldw -176(0,%r30),%r22
        ldo -192(%r30),%r30
        b,n $$dyncall
__nodyncall:
        bv 0(%r2)
        ldo -192(%r30),%r30
        .EXIT
        .PROCEND");
