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
        asm("
        .text
        .globl _i386_longjmp
        _i386_longjmp:
                movl         0x4(%esp), %eax

                /* restore edi */
                movl        0x4(%eax), %edi

                /* save rc in the edi slot */
                movl        0x8(%esp), %edx
                movl        %edx, 0x4(%eax)

                movl        (%eax), %ebp
                movl        0x8(%eax), %esp
                movl        0xc(%eax), %esi
                movl        0x10(%eax), %ebx

                /*add         $4, %esp*/

                /* put return addr on stack */
                movl        0x14(%eax), %edx
                movl        %edx, (%esp)

                /* restore edx and get rc in eax */
                movl        0x18(%eax), %edx
                pushl       %edx
                movl        0x4(%eax), %edx
                movl        %edx, %eax
                popl        %edx


                /*jmpl        0x14(%edx)*/

                /* should now return as if we're returning from setjmp */
                retl
        ");

        asm("
        .text
        .globl _i386_setjmp
        _i386_setjmp:
                movl        0x4(%esp), %eax

                /* save environment */
                movl        %ebp, (%eax)
                movl        %edi, 0x4(%eax)
                movl        %esp, 0x8(%eax)
                movl        %esi, 0xc(%eax)
                movl        %ebx, 0x10(%eax)
                movl        %edx, 0x18(%eax)

                /* get the return addr into the buffer while preserving the
                   value of the edx register.
                */
                pushl       %edx
                movl        0x4(%esp), %edx
                movl        %edx, 0x14(%eax)
                popl        %edx

                /* return 0 */
                subl        %eax, %eax
                retl
        ");





