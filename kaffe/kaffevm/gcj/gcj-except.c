/*
 * gcj-except.c
 *
 * Copyright (c) 1996, 1997, 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 *
 * Written by Godmar Back <gback@cs.utah.edu>
 */

#include "config.h"
#if defined(HAVE_GCJ_SUPPORT)

#include "config-std.h"
#include "config-setjmp.h"
#include "gtypes.h"
#include "classMethod.h"
#include "stackTrace.h"
#include "thread.h"
#include "gcj-except.h"
#include "stringSupport.h"
#include "lookup.h"
#include "soft.h"
#include "access.h"
#include "md.h"
#include "gcj.h"

/* from compat-include */
#include "gansidecl.h"
#include "eh-common.h"		
#include "frame.h"		

/* Language specific exception information */
typedef struct _gcjException {
        __eh_info               	eh_info;
        Hjava_lang_Class*       	eclass;
        Hjava_lang_Throwable*      	eobj;
} gcjException;

/* From libgcc2.c */
extern void (*__terminate_func)(void);
extern void __throw(void);
extern void **__get_eh_info(void);
extern short __get_eh_table_language (void *table);

/*
 * See if the exception info matches the given class.
 * This function is invoked by __throw as it tries to find a
 * matching exception handler.
 *
 * Compare _Jv_type_matcher in exception.cc
 */
static void *
gcjMatcher(gcjException* einfo, void* match_info, void* exception_table)
{
DBG(GCJ,
	fprintf(stderr, __FUNCTION__": match_info %p\n", match_info);
    )
	if (__get_eh_table_language (exception_table) != EH_LANG_Java) {
DBG(GCJ,	fprintf(stderr, __FUNCTION__":not java, ignored\n"); 
	)
		return (0);
	}

	if (match_info != 0) {
		errorInfo info;
		Hjava_lang_Class *kclass = gcjFindMatchClass(match_info, &info);

		if (kclass == 0) {
DBG(GCJ,		dprintf("%s: kclass is null\n", __FUNCTION__); )
			throwError(&info);	/* XXX does that work? */
		}

		assert(kclass);
DBG(GCJ,	dprintf("%s: matching class `%s'\n", __FUNCTION__, 
						    CLASS_CNAME(kclass)); 
	)
		if (!soft_instanceof(kclass, (Hjava_lang_Object*)einfo->eobj)) {
			return (0);
		}
	}
	/* else no match_info is given, must mean its always matches !? */

	return (einfo->eobj);
}

/* 
 * Return the exception object.
 *
 * gcc's exception system has a clumsy way of passing the exception 
 * object.
 * It is done out-of-band using thread-specific information
 * that's accessed in the exception handler.
 *
 * We must set this information before we throw.
 */
void*
_Jv_exception_info(void)
{
	void *obj;
	gcjException *einf = *(__get_eh_info());

	if (einf == 0) {
		fprintf(stderr, "Attempt to catch an exception "
				"before throwing one.  This is bad.\n");
		ABORT();
	}

	obj = einf->eobj;
	einf->eobj = 0;

	return (obj);
}

/*
 * GCJ code wants to throw an exception.  
 */
void
_Jv_Throw(void* obj)
{
	gcjException *einf;

	if (obj == 0) {
		obj = NullPointerException;
	}

	einf = *(__get_eh_info());

	/* Allocate on first invocation (for this thread) */
	if (einf == 0) {
		einf = /* XXX KMALLOC */ calloc(1, sizeof *einf);
		*(__get_eh_info()) = einf;
	}

	einf->eh_info.match_function = (__eh_matcher) gcjMatcher;
	einf->eh_info.language = EH_LANG_Java;
	einf->eh_info.version = 1;
	einf->eobj = (Hjava_lang_Throwable*)obj;
	einf->eclass = OBJECT_CLASS(&(einf->eobj)->base);

DBG(GCJ,
	dprintf("invoking __throw: obj@%p class=`%s'\n", 
			obj, CLASS_CNAME(einf->eclass));
    )

	/* send the exception off to gcc and pray */
	__throw();
}

void 
/* ARGUSED */
kenvThrowBadArrayIndex(int idx)
{
	_Jv_Throw(ArrayIndexOutOfBoundsException);
}


/*
 * This is what gcc's internal frame_state structure looks like
 * it is defined in frame.h
 *
 *
 *   typedef struct frame_state
 *    {
 *      void *cfa;
 *      void *eh_ptr;
 *      long cfa_offset;
 *      long args_size;
 *      long reg_or_offset[FIRST_PSEUDO_REGISTER+1];  
 *      unsigned short cfa_reg;
 *      unsigned short retaddr_column;
 *      char saved[FIRST_PSEUDO_REGISTER+1];
 *    } frame_state;
 *
 */
void
dumpFS(char *label, struct frame_state *s)
{
	int i;

	fprintf(stderr, "%s: frame_state %p\n", label, s);
	fprintf(stderr, "cfa = %p, eh_ptr = %p, "
			"cfa_offset = %ld, args_size = %ld\n",
			s->cfa, s->eh_ptr, s->cfa_offset, s->args_size);
	for (i = 0; i <= FIRST_PSEUDO_REGISTER; i++) {
		char *w = s->saved[i] == REG_SAVED_OFFSET ? "Off" :
			  s->saved[i] == REG_SAVED_REG ? "Reg" : "Und";
		if (s->saved[i] == 0) {	/* skip unsaved ones, they're not
					 * that interesting
					 */
			continue;
		}
		fprintf(stderr, "{%2d: %s %4ld}%c", i, w, s->reg_or_offset[i],
				((i+1) % 8 == 0) ? '\n' : ' ');
	}
	fprintf(stderr, "\ncfa_reg = %d, retaddr_column = %d\n---- eofs ----\n",
		s->cfa_reg, s->retaddr_column);
	fflush(stderr);
}

/*
 * This method is our hook into the exception handling system in eh.c
 *
 * NB: this method is invoked twice for each stack frame
 */

/* The original documentation for frame_state_for says: */
/* Called from __throw to find the registers to restore for a given
   PC_TARGET.  The caller should allocate a local variable of `struct
   frame_state' (declared in frame.h) and pass its address to STATE_IN.
   Returns NULL on failure, otherwise returns STATE_IN.  */

struct frame_state *
__external_frame_state_for(void *pc_target, void *callee_cfa, 
			   struct frame_state *state_in)
{
	int i, n;
	Method *meth;
	struct kaffe_frame_descriptor frame_desc[FIRST_PSEUDO_REGISTER];
	stackTraceInfo frame;
	gcjException *einf = *(__get_eh_info());

	frame.pc = (uintp)pc_target;
	frame.fp = (uintp)(*(void**)(callee_cfa + CFA_SAVED_OFFSET));
	frame.meth = (Method*)0;  /* not known, not needed, see stackTrace.h */
DBG(GCJ, 
	dprintf(__FUNCTION__": unwinding pc=%p fp=%p\n", frame.pc, frame.fp);
    )

	/* First attempt to deliver this exception to a handler in 
	 * kaffe's jitted code.  If that succeeds, we won't return.
	 * Otherwise, we'll have a pointer to the kaffe method that
	 * we need get our state from.
	 *
	 * unwindStackFrame includes actions to be done when unrolling 
	 * one stack frame in kaffe/jit, such as unlocking an object if 
	 * a method is synchronized 
	 */

	meth = unwindStackFrame(&frame, einf->eobj);

	if (!meth) {
DBG(GCJ,	dprintf("%s: no jit method @pc=%p\n", pc_target);	)
		return (0);
	}
	
DBG(GCJ,	
	dprintf(__FUNCTION__": %s.%s%s framesize is %d\n", 
		CLASS_CNAME(meth->class), meth->name->data, 
		PSIG_DATA(METHOD_PSIG(meth)), meth->framesize);
    )

	/*
	 * else tell gcj that there's no handler here and
	 * tell it how to unroll this frame.
	 */
	memset(state_in, 0, sizeof (*state_in));

	/* this is a constant that says where we store the return address */
	state_in->retaddr_column = DWARF_FRAME_RETURN_COLUMN;

	/* 
	 * Setting this to 0 means that this frame has no exception handler
	 * table, hence no handler at all.  gcc will accept that.
	 */
	state_in->eh_ptr = 0;	

	/* which register holds the canonical frame address?  Usually sp */
	state_in->cfa_reg = CFA_REGISTER;

	/* If we restore the sp, how much do we have to add to get to the
	 * canonical frame address.  See layout picture.
	 */
	state_in->cfa_offset = CFA_OFFSET;

	/*
	 * How many bytes did the caller have to push to invoke this method
	 * This is the n in "addl $n, esp" after a function call on the x86
	 */
	state_in->args_size = PSIG_NARGS(METHOD_PSIG(meth));
	if (!(meth->accflags & ACC_STATIC)) {
		state_in->args_size++;
	}
	state_in->args_size *= 4;

	/*
	 * For all callee-saved registers (including the cfa register and
	 * the return address register), fill in the offsets at which they're
	 * saved.  In a gcj program, this will be frame-specific information.
	 *
	 * In Kaffe with its eager strategy of saving those registers, it
	 * will be relatively constant.
	 */
	arch_get_frame_description(meth->framesize, frame_desc, &n);

	for (i = 0; i < n; i++) {
		int idx = frame_desc[i].idx;
		state_in->saved[idx] = REG_SAVED_OFFSET;
		state_in->reg_or_offset[idx] = frame_desc[i].offset;
	}
	return (state_in);
}

#endif
