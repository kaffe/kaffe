/*
 * machine.c
 * Java virtual machine interpreter.
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#include "debug.h"
#define	CDBG(s) 	DBG(INT_VMCALL, s)
#define	RDBG(s) 	DBG(INT_RETURN, s)
#define	NDBG(s) 	DBG(INT_NATIVE, s)
#define	IDBG(s)		DBG(INT_INSTR, s)
#define	CHDBG(s)	DBG(INT_CHECKS, s)

#include "config.h"
#include "config-std.h"
#include "config-math.h"
#include "config-mem.h"
#include "config-setjmp.h"
#include "classMethod.h"
#include "gtypes.h"
#include "bytecode.h"
#include "slots.h"
#include "icode.h"
#include "access.h"
#include "object.h"
#include "constants.h"
#include "gc.h"
#include "machine.h"
#include "lookup.h"
#include "code-analyse.h"
#include "soft.h"
#include "exception.h"
#include "external.h"
#include "baseClasses.h"
#include "thread.h"
#include "jthread.h"
#include "locks.h"
#include "checks.h"
#include "errors.h"
#include "md.h"

/*
 * Define information about this engine.
 */
char* engine_name = "Interpreter";
char* engine_version = KVER;

#define	define_insn(code)	break;					\
				case code:				\
				IDBG( dprintf("%03d: %s\n", pc, #code); )
#define	define_insn_alias(code)	case code:				\
				IDBG( dprintf("%03d: %s\n", pc, #code); )

/* Define CREATE_NULLPOINTER_CHECKS in md.h when your machine cannot use the
 * MMU for detecting null pointer accesses */
#if defined(CREATE_NULLPOINTER_CHECKS)
#define CHECK_NULL(_i, _s, _n)                                  \
      cbranch_ref_const_ne((_s), 0, reference_label(_i, _n)); \
      softcall_nullpointer();                                 \
      set_label(_i, _n)
#else
#define CHECK_NULL(_i, _s, _n)
#endif

#if defined(KAFFE_PROFILER)
int profFlag;			 /* flag to control profiling */
#endif

void runVirtualMachine(methods *meth, slots *lcl, slots *sp, uintp npc, slots *retval, volatile vmException *mjbuf, Hjava_lang_Thread *tid);

void
virtualMachine(methods*volatile meth, slots* volatile arg, slots* volatile retval, Hjava_lang_Thread* volatile tid)
{
	methods *volatile const vmeth = meth;
	Hjava_lang_Object* volatile mobj;
	vmException mjbuf;
	accessFlags methaccflags;

	slots* volatile lcl;
	slots* volatile sp;
	uintp volatile npc;

	int32 idx;
	jint low;

	errorInfo einfo;
	Hjava_lang_Throwable* overflow;
	jint *needOnStack;

	if (tid != 0)  {
		/* implement stack overflow check */
		needOnStack = &unhand(tid)->needOnStack;

		if (jthread_stackcheck(*needOnStack) == false) {
			overflow = (Hjava_lang_Throwable*)
				unhand(tid)->stackOverflowError;
			if (overflow != 0) {
				if (*needOnStack == STACK_LOW) {
					fprintf(stderr, 
					    "Panic: unhandled StackOverflowError()\n");
					ABORT();
				}
				*needOnStack = STACK_LOW;
				throwException(overflow);
			}
		}
	}

CDBG(	dprintf("Call: %s.%s%s.\n", meth->class->name->data, meth->name->data, METHOD_SIGD(meth)); )

	/* If this is native, then call the real function */
	methaccflags = meth->accflags;
	if (methaccflags & ACC_NATIVE) {
NDBG(		dprintf("Call to native %s.%s%s.\n", meth->class->name->data, meth->name->data, METHOD_SIGD(meth)); )
		if (methaccflags & ACC_STATIC) {
			callMethodA(meth, meth, 0, (jvalue*)arg, (jvalue*)retval, 1);
		}
		else {
			callMethodA(meth, meth, ((jvalue*)arg)[0].l, &((jvalue*)arg)[1], (jvalue*)retval, 1);
		}
		return;
	}

	/* Verify method if required */
	if ((methaccflags & ACC_VERIFIED) == 0) {
		codeinfo* codeInfo;
		bool success = verifyMethod(meth, &codeInfo, &einfo);
		tidyVerifyMethod(&codeInfo);
		if (success == false) {
			throwError(&einfo);
		}
	}

	/* Allocate stack space and locals. */
	lcl = alloca(sizeof(slots) * (meth->localsz + meth->stacksz));

	mobj = 0;
	npc = 0;

	/* If we have any exception handlers we must prepare to catch them.
	 * We also need to catch if we are synchronised (so we can release it).
	 */
	mjbuf.pc = 0;
	mjbuf.mobj = mobj;
	mjbuf.meth = meth;
	if (tid != NULL && unhand(tid)->PrivateInfo != 0) {
		mjbuf.prev = (vmException*)unhand(tid)->exceptPtr;
		unhand(tid)->exceptPtr = (struct Hkaffe_util_Ptr*)&mjbuf;
	}

	if (meth->exception_table != 0) {
		if (JTHREAD_SETJMP(mjbuf.jbuf) != 0) {
			meth = vmeth;
			unhand(tid)->exceptPtr = (struct Hkaffe_util_Ptr*)&mjbuf;
			npc = mjbuf.pc;
			sp = &lcl[meth->localsz];
			sp->v.taddr = (void*)unhand(tid)->exceptObj;
			unhand(tid)->exceptObj = 0;
			runVirtualMachine(meth, lcl, sp, npc, retval, &mjbuf, tid);
			goto end;
		}
	}

	/* Calculate number of arguments */
	idx = sizeofSigMethod(meth, false);
	if (idx == -1) {
		throwError(&einfo);
	}
	idx += (methaccflags & ACC_STATIC ? 0 : 1);

	/* Copy in the arguments */
	sp = lcl;
	for (low = 0; low < idx; low++) {
		*(sp++) = *(arg++);
	}

	/* Sync. if required */
	if (methaccflags & ACC_SYNCHRONISED) { 
		if (methaccflags & ACC_STATIC) {
			mobj = &meth->class->head;
		}
		else {
			mobj = (Hjava_lang_Object*)lcl[0].v.taddr;
		}
		/* this lock is safe for Thread.stop() */
		lockObject(mobj);
		/* We must store the object on which we synchronized in
		 * the mjbuf chain or else the exception handler routine
		 * won't find it.
		 */
		mjbuf.mobj = mobj;
	}       

	sp = &lcl[meth->localsz - 1];

	runVirtualMachine(meth, lcl, sp, npc, retval, &mjbuf, tid);

 end:
	/* Unsync. if required */
	if (mobj != 0) {
		unlockObject(mobj);
	}       
	if (tid != NULL && unhand(tid)->PrivateInfo != 0) {
		unhand(tid)->exceptPtr = (struct Hkaffe_util_Ptr*)mjbuf.prev;
	}

RDBG(	dprintf("Returning from method %s%s.\n", meth->name->data, METHOD_SIGD(meth)); )
}

void runVirtualMachine(methods *meth, slots *lcl, slots *sp, uintp npc, slots *retval, volatile vmException *mjbuf, Hjava_lang_Thread *tid) {
	bytecode *code = (bytecode*)meth->c.bcode.code;

	/* Misc machine variables */
	jlong lcc;
	jlong tmpl;
	slots tmp[1];
	slots tmp2[1];
	slots mtable[1];

	/* Variables used directly in the machine */
	int32 idx;
	jint low;
	jint high;

	/* Call, field and creation information */
	callInfo cinfo;
	fieldInfo finfo;
	Hjava_lang_Class* crinfo;
	errorInfo einfo;

	/* Finally we get to actually execute the machine */
	for (;;) {
		register uintp pc = npc;

		assert(npc < meth->c.bcode.codelen);
		mjbuf->pc = pc;
		npc = pc + insnLen[code[pc]];

		switch (code[pc]) {
		default:
			fprintf(stderr, "Unknown bytecode %d\n", code[pc]);
			throwException(NEW_LANG_EXCEPTION(VerifyError));
			break;
#include "kaffe.def"
		}
	}
 end:
	return;
}    

/*
 * say what engine we're using
 */
char* 
getEngine()
{
	return "kaffe.intr";
}
