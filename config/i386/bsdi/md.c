/*
 * i386/bsdi/md.c
 * BSDI i386 specific functions.
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */


#include "systems/unix-jthreads/jthread.h"
#include <stdio.h>

#define	GET_SP(E)	(((void**)(E))[SP_OFFSET])

/**
*	This function, and it's associated function i386_setjmp are a 
*	plain implementation of the standard C setjmp/longjmp functions.
*	These were first written for use on BSDI 2.1.
*	
*	These implementations are the best I can do with my meager 
*	assembler skills, they work but feel free to improve them.
*
*	@author  ted stockwell, early morning software
*/
        asm(" \n"
"        .text \n"
"        .globl _i386_longjmp \n"
"        _i386_longjmp: \n"
"                movl         0x4(%esp), %eax \n"
" \n"
"                /* restore edi */ \n"
"                movl        0x4(%eax), %edi \n"
" \n"
"                /* save rc in the edi slot */ \n"
"                movl        0x8(%esp), %edx \n"
"                movl        %edx, 0x4(%eax) \n"
" \n"
"                movl        (%eax), %ebp \n"
"                movl        0x8(%eax), %esp \n"
"                movl        0xc(%eax), %esi \n"
"                movl        0x10(%eax), %ebx \n"
" \n"
"                /*add         $4, %esp*/ \n"
" \n"
"                /* put return addr on stack */ \n"
"                movl        0x14(%eax), %edx \n"
"                movl        %edx, (%esp) \n"
" \n"
"                /* restore edx and get rc in eax */ \n"
"                movl        0x18(%eax), %edx \n"
"                pushl       %edx \n"
"                movl        0x4(%eax), %edx \n"
"                movl        %edx, %eax \n"
"                popl        %edx \n"
" \n"
" \n"
"                /*jmpl        0x14(%edx)*/ \n"
" \n"
"                /* should now return as if we're returning from setjmp */ \n"
"                retl \n"
"         \n");

        asm(" \n"
"        .text \n"
"        .globl _i386_setjmp \n"
"        _i386_setjmp: \n"
"                movl        0x4(%esp), %eax \n"
" \n"
"                /* save environment */ \n"
"                movl        %ebp, (%eax) \n"
"                movl        %edi, 0x4(%eax) \n"
"                movl        %esp, 0x8(%eax) \n"
"                movl        %esi, 0xc(%eax) \n"
"                movl        %ebx, 0x10(%eax) \n"
"                movl        %edx, 0x18(%eax) \n"
" \n"
"                /* get the return addr into the buffer while preserving the \n"
"                   value of the edx register. \n"
"                */ \n"
"                pushl       %edx \n"
"                movl        0x4(%esp), %edx \n"
"                movl        %edx, 0x14(%eax) \n"
"                popl        %edx \n"
" \n"
"                /* return 0 */ \n"
"                subl        %eax, %eax \n"
"                retl \n"
"         \n");





