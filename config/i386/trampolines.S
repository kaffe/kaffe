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

#if defined(HAVE_CONFIG_H)
#include "config.h"
#endif
	
#if defined(TRAMPOLINE_FUNCTION)
/*
 * If we have an explit function defined then use that.
 */
TRAMPOLINE_FUNCTION()

#else
/*
 * Otherwise we'll try to construct one.
 */
#if defined(HAVE_UNDERSCORED_C_NAMES)
#define	C_FUNC_NAME(FUNC) _ ## FUNC
#else
#define	C_FUNC_NAME(FUNC) FUNC
#endif

     .text
     .align 4
     .globl C_FUNC_NAME(i386_do_fixup_trampoline)

     C_FUNC_NAME(i386_do_fixup_trampoline) :
        popl	%eax
	push	%ebp
	mov	%esp,%ebp
	push	%eax
	call	C_FUNC_NAME(soft_fixup_trampoline)
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
