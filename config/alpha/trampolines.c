/*
 * alpha/trampolines.c
 * Alpha trampolines codes for various occasions.
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#if defined(NO_SHARED_VMLIBRARY)

#if defined(HAVE_UNDERSCORED_C_NAMES)

#else /* HAVE_UNDERSCORED_C_NAMES */

#endif /* HAVE_UNDERSCORED_C_NAMES */

#else

#if defined(HAVE_UNDERSCORED_C_NAMES)

#else /* HAVE_UNDERSCORED_C_NAMES */

static void __attribute__((unused))
silly_wrapper_2()
{
   asm(".align 3
	.global alpha_do_fixup_trampoline
alpha_do_fixup_trampoline:
	ldgp	$29,0($27)
	subq	$30,16*8,$30
	# Save register arguments
	stq	$26,0*8($30)
	stq	$15,1*8($30)
	stq	$16,2*8($30)
	stq	$17,3*8($30)
	stq	$18,4*8($30)
	stq	$19,5*8($30)
	stq	$20,6*8($30)
	stq	$21,7*8($30)
	stt	$f16,8*8($30)
	stt	$f17,9*8($30)
	stt	$f18,10*8($30)
	stt	$f19,11*8($30)
	stt	$f20,12*8($30)
	stt	$f21,13*8($30)
	# Load the method pointer
	ldq	$16,8($1)
	# Do the silly hook to the exception frame
	lda	$2,14*8-%1($30)
	ldq	$1,currentThread
	beq	$1,1f
	ldq	$2,threadContext
	ldl	$1,%0($1)
	s8addq	$1,$2,$2
	ldq	$2,0($2)
	stq	$30,%1($2)
1:	stq	$2,14*8($30)
	# Call C to do the fixup
	jsr	$26,fixupTrampoline
	# Unhook from the exception frame and restore arguments
	ldq	$1,14*8($30)
	mov	$0,$27
	ldq	$26,0*8($30)
	ldq	$16,2*8($30)
	ldq	$17,3*8($30)
	ldq	$18,4*8($30)
	ldq	$19,5*8($30)
	stq	$31,%1($1)
	ldq	$20,6*8($30)
	ldq	$21,7*8($30)
	ldt	$f16,8*8($30)
	ldt	$f17,9*8($30)
	ldt	$f18,10*8($30)
	ldt	$f19,11*8($30)
	ldt	$f20,12*8($30)
	ldt	$f21,13*8($30)
	addq	$30,16*8,$30
	jmp	$31,($27),0"
	: : "i"(offsetof(Hjava_lang_Thread, data[0].PrivateInfo)),
            "i"(offsetof(ctx, exceptPtr)));
}

#endif /* HAVE_UNDERSCORED_C_NAMES */

#endif
#endif
