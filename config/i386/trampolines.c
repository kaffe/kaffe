/*
 * i386/trampolines.c
 * i386 trampolines codes for various occasions.
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#if defined(TRAMPOLINE_FUNCTION)
/*
 * If we have an explit function defined then use that.
 */
TRAMPOLINE_FUNCTION()

#else
/*
 * Otherwise we'll try to construct one.
 */
#if !defined(START_ASM_FUNC)
#define	START_ASM_FUNC() ".text\n\t.align 4\n\t.globl "
#endif
#if !defined(END_ASM_FUNC)
#define	END_ASM_FUNC() ""
#endif
#if defined(HAVE_UNDERSCORED_C_NAMES)
#define	C_FUNC_NAME(FUNC) "_" #FUNC
#else
#define	C_FUNC_NAME(FUNC) #FUNC
#endif

asm(
	START_ASM_FUNC() C_FUNC_NAME(i386_do_fixup_trampoline) "\n"
C_FUNC_NAME(i386_do_fixup_trampoline) ":			\n \
	popl	%eax						\n \
	push	%ebp						\n \
	mov	%esp,%ebp					\n \
	push	%eax						\n \
	call	" C_FUNC_NAME(soft_fixup_trampoline) "		\n \
	leave							\n \
	jmp	*%eax"
	END_ASM_FUNC()
);

#if defined (HAVE_GCJ_SUPPORT)
/*
 * Warning: These trampolines are so twisted they may not
 * be legal in some states.  Basically, what we're doing here is rely 
 * on the way cross-shared library calls are made.
 */
asm(
	START_ASM_FUNC() C_FUNC_NAME(__kaffe_i386_gcj_fixup) "	\n"
C_FUNC_NAME(__kaffe_i386_gcj_fixup) ": \n"
"	mov	(%esp), %eax	# get return address \n"
"	add	-4(%eax), %eax	# add jump relative offset from previous instr. \n"
"				# this points at at jmp *$off(%ebx) instr. \n"
"	mov	2(%eax), %eax	# extract 'off' \n"
"	add	%ebx, %eax	# compute $off(%ebx) \n"
"	pushl	%eax		# pass as first argument \n"
"	call	" C_FUNC_NAME(gcj_fixup_trampoline) " # returns target \n"
"	addl	$4, %esp	# remove argument \n"
"	jmp	*%eax		# jump to target \n"
" \n"
	END_ASM_FUNC()
);

int 
arch_is_trampoline_frame(void *pc) 
{
	extern void (*f)(void) asm(C_FUNC_NAME(i386_do_fixup_trampoline));
	return ((char*)pc == ((char*)&f + 4));
}
#endif

#endif
