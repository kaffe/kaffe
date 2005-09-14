/*
 * i386/trampolines.S
 * i386 trampolines codes for various occasions.
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#include "kaffe/trampoline.h"
	
#if defined(TRAMPOLINE_FUNCTION)
/*
 * If we have an explit function defined then use that.
 */
TRAMPOLINE_FUNCTION()

#else
/*
 * Otherwise we'll try to construct one.
 */

     .text
     .align 4
     .globl C_FUNC_NAME(i386_do_fixup_trampoline)
/* Cygwin does not like the PIC declaration for some reason */
#if defined(__CYGWIN__)
     .def C_FUNC_NAME(i386_do_fixup_trampoline)
     .scl 3
     .type 32
     .endef
#else
     .type C_FUNC_NAME(i386_do_fixup_trampoline), @function
#endif

     C_FUNC_NAME(i386_do_fixup_trampoline) :
        popl	%eax
	push	%ebp
	mov	%esp,%ebp
#if defined(PIC)
	pushl	%ebx
	call	.L2
     .L2:
	popl	%ebx
	addl	$_GLOBAL_OFFSET_TABLE_+[.-.L2], %ebx
	subl	$4, %esp
	push	%eax
	call	C_FUNC_NAME(soft_fixup_trampoline@PLT)
	movl	-4(%ebp), %ebx
#else
	push	%eax
	call	C_FUNC_NAME(soft_fixup_trampoline)
#endif
	leave
	jmp	*%eax

#if defined (HAVE_GCJ_SUPPORT)
/*
 * Warning: These trampolines are so twisted they may not
 * be legal in some states.  Basically, what we're doing here is rely 
 * on the way cross-shared library calls are made.
 */

     .text
     .align 4
     .globl C_FUNC_NAME((__kaffe_i386_gcj_fixup)

     C_FUNC_NAME(__kaffe_i386_gcj_fixup) :

	mov	(%esp), %eax	# get return address
        add	-4(%eax), %eax	# add jump relative offset from previous instr.
				# this points at at jmp *$off(%ebx) instr.
	mov	2(%eax), %eax	# extract 'off'
	add	%ebx, %eax	# compute $off(%ebx) 
	pushl	%eax		# pass as first argument 
	call	C_FUNC_NAME(gcj_fixup_trampoline) # returns target
	addl	$4, %esp	# remove argument
	jmp	*%eax		# jump to target

#endif

#endif
