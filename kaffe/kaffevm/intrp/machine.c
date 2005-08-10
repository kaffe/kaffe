/*
 * machine.c
 * Java virtual machine interpreter.
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * Copyright (c) 2004
 *	Kaffe.org contributors, see ChangeLog for details.  All rights reserved.
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

#include "access.h"
#include "baseClasses.h"
#include "bytecode.h"
#include "checks.h"
#include "code-analyse.h"
#include "constants.h"
#include "classMethod.h"
#include "errors.h"
#include "exception.h"
#include "external.h"
#include "gc.h"
#include "gtypes.h"
#include "icode.h"
#include "jthread.h"
#include "locks.h"
#include "lookup.h"
#include "machine.h"
#include "md.h"
#include "object.h"
#include "slots.h"
#include "soft.h"
#include "support.h"
#include "thread.h"

/*
 * Define information about this engine.
 */
const char* engine_name = "Interpreter";

#define	define_insn(code)	break;					\
				case code:				\
				IDBG( dprintf("%03ld: %s\n", (long) pc, #code); );
#define	define_insn_alias(code)	case code:				\
				IDBG( dprintf("%03ld: %s\n", (long) pc, #code); );
#define	define_wide_insn(code)	break;					\
				case code:				\
				IDBG( dprintf("%03ld: %s\n", (long) pc, #code); );

#define EXPLICIT_CHECK_NULL(_i, _s, _n)                       \
      cbranch_ref_const_ne((_s), 0, reference_label(_i, _n)); \
      softcall_nullpointer();                                 \
      set_label(_i, _n)
/* Define CREATE_NULLPOINTER_CHECKS in md.h when your machine cannot use the
 * MMU for detecting null pointer accesses */
#if defined(CREATE_NULLPOINTER_CHECKS)
#define CHECK_NULL(_i, _s, _n)                                  \
    EXPLICIT_CHECK_NULL(_i, _s, _n)
#else
#define CHECK_NULL(_i, _s, _n)
#endif

/* For JIT3 compatibility */
#define check_array_store(a,b)		softcall_checkarraystore(a,b)
#define explicit_check_null(x,obj,y)	EXPLICIT_CHECK_NULL(x,obj,y)

#if defined(KAFFE_VMDEBUG)
#define check_null(x,obj,y)		EXPLICIT_CHECK_NULL(x,obj,y)
#else
#define check_null(x,obj,y)		CHECK_NULL(x,obj,y)
#endif
#define check_div(x,obj,y)
#define check_div_long(x,obj,y)


#if defined(KAFFE_PROFILER)
int profFlag;			 /* flag to control profiling */
#endif

void runVirtualMachine(methods *meth, slots *lcl, slots *sp, uintp npc, slots *retval, volatile VmExceptHandler *mjbuf, threadData *thread_data);

void
virtualMachine(methods*volatile meth, slots* volatile arg, slots* volatile retval, threadData* volatile thread_data)
{
	methods *volatile const vmeth = meth;
	Hjava_lang_Object* volatile mobj;
	VmExceptHandler mjbuf;
	accessFlags methaccflags;

	slots* volatile lcl;
	slots* volatile sp;
	uintp volatile npc;

	int32 idx;
	jint low;

	errorInfo einfo;

	/* implement stack overflow check */
	if (KTHREAD(stackcheck)(thread_data->needOnStack) == false) {
		if (thread_data->needOnStack == STACK_LOW) {
			CHDBG(
			    dprintf(
			    "Panic: unhandled StackOverflowError()\n");
			);
			KAFFEVM_ABORT();
		}
	
		{
			Hjava_lang_Throwable *th;
			errorInfo soeinfo;

			thread_data->needOnStack = STACK_LOW;
			th = (Hjava_lang_Throwable *)newObjectChecked (javaLangStackOverflowError, &soeinfo);
			thread_data->needOnStack = STACK_HIGH;
		
			throwException(th);
		}
	}

CDBG(	dprintf("Call: %s.%s%s.\n", meth->class->name->data, meth->name->data, METHOD_SIGD(meth)); );

	/* If this is native, then call the real function */
	methaccflags = meth->accflags;

#if defined(ENABLE_JVMPI)
	if (methaccflags & ACC_STATIC)
	  soft_enter_method(NULL, meth);
	else
	  soft_enter_method(((jvalue*)arg)[0].l, meth);
#endif

	if (methaccflags & ACC_NATIVE) {
NDBG(		dprintf("Call to native %s.%s%s.\n", meth->class->name->data, meth->name->data, METHOD_SIGD(meth)); );
		if (methaccflags & ACC_STATIC) {
			KaffeVM_callMethodA(meth, meth, 0, (jvalue*)arg, (jvalue*)retval, 1);
		}
		else {
			KaffeVM_callMethodA(meth, meth, ((jvalue*)arg)[0].l, &((jvalue*)arg)[1], (jvalue*)retval, 1);
		}

#if defined(ENABLE_JVMPI)
		soft_exit_method(meth);
#endif

		return;
	}

	/* Analyze method if required */
	if ((methaccflags & ACC_VERIFIED) == 0) {
		codeinfo* codeInfo;
		bool success = analyzeMethod(meth, &codeInfo, &einfo);
		tidyAnalyzeMethod(&codeInfo);
		if (success == false) {
			throwError(&einfo);
		}
	}

	/* Allocate stack space and locals. */
	lcl = alloca(sizeof(slots) * (meth->localsz + meth->stacksz));

#if defined(KAFFE_VMDEBUG)
	{
	    int32 *p = (int32 *) &lcl[meth->localsz + meth->stacksz];
	    while (p-- > (int32*)lcl)
		*p = UNINITIALIZED_STACK_SLOT;
	}
#endif

	mobj = 0;
	npc = 0;

	/* If we have any exception handlers we must prepare to catch them.
	 * We also need to catch if we are synchronised (so we can release it).
	 */
	setupExceptionHandling(&mjbuf, meth, mobj, thread_data);

	if (meth->exception_table != 0) {
		if (JTHREAD_SETJMP(mjbuf.jbuf) != 0) {
			meth = vmeth;
			thread_data->exceptPtr = &mjbuf;
			npc = vmExcept_getPC(&mjbuf);
			sp = &lcl[meth->localsz];
#if defined(KAFFE_VMDEBUG)
			{
			    int32 *p = (int32 *) &lcl[meth->localsz + meth->stacksz];
			    while (p-- > (int32*)sp)
				*p = 0xdeadbeef;
			}
#endif
			sp->v.taddr = (void*)thread_data->exceptObj;
			thread_data->exceptObj = 0;
			runVirtualMachine(meth, lcl, sp, npc, retval, &mjbuf, thread_data);
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
		locks_internal_lockMutex(&mobj->lock, 0);

		/*
		 * We must store the object on which we synchronized
		 * in the mjbuf chain for the exception handler
		 * routine to find it (and unlock it when unwinding).
		 */
		vmExcept_setSyncObj(&mjbuf, mobj);
	}

	sp = &lcl[meth->localsz - 1];

	runVirtualMachine(meth, lcl, sp, npc, retval, &mjbuf, thread_data);

 end:
	/* Unsync. if required */
	if (mobj != 0) {
		locks_internal_unlockMutex(&mobj->lock, NULL); 
	}

	cleanupExceptionHandling(&mjbuf, thread_data);


#if defined(ENABLE_JVMPI)
	soft_exit_method(meth);
#endif

RDBG(	dprintf("Returning from method %s%s.\n", meth->name->data, METHOD_SIGD(meth)); );
}

void runVirtualMachine(methods *meth, slots *lcl, slots *sp, uintp npc, slots *retval, volatile VmExceptHandler *mjbuf, threadData *thread_data) {
	bytecode *code = (bytecode*)meth->c.bcode.code;

	/* Misc machine variables */
	jvalue tmpl;
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
		vmExcept_setPC(mjbuf, pc);
		npc = pc + insnLen[code[pc]];

		switch (code[pc]) {
		default:
			CHDBG(dprintf("Unknown bytecode %d\n", code[pc]); );
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
const char*
getEngine(void)
{
	return "kaffe.intr";
}

void initEngine(void)
{
}

static inline void 
setFrame(VmExceptHandler* eh,
	 struct _jmethodID* meth,
	 struct Hjava_lang_Object* syncobj)
{
	assert(eh);
	assert(meth);
	
	eh->meth = meth;
	eh->frame.intrp.pc = 0;
	eh->frame.intrp.syncobj = syncobj;
}

void
setupExceptionHandling(VmExceptHandler* eh,
		       struct _jmethodID* meth,
		       struct Hjava_lang_Object* syncobj,
		       threadData*	thread_data)
{
  setFrame(eh, meth, syncobj);

  eh->prev = thread_data->exceptPtr;
  thread_data->exceptPtr = eh;
}

void
cleanupExceptionHandling(VmExceptHandler* eh,
			 threadData* thread_data) 
{
  thread_data->exceptPtr = eh->prev;
}
