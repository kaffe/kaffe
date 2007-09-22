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

#ifdef DIRECT_THREADING

#define	define_insn(code)	INSN_LBL_##code: \
				IDBG( dprintf("%03ld: %s\n", (long) pc, #code); );
#define	define_insn_alias(code)	INSN_LBL_##code: \
				IDBG( dprintf("%03ld: %s\n", (long) pc, #code); );
#define	define_wide_insn(code)	break;					\
				case code:				\
				IDBG( dprintf("%03ld: %s\n", (long) pc, #code); );

#define define_insn_fini { \
		pc = npc; \
		assert(npc < (uintp)(meth->c.bcode.codelen)); \
		vmExcept_setPC(mjbuf, pc); \
		npc = pc + insnLen[code[pc]]; \
		goto *insn_handlers[code[pc]]; \
	}
	
#define INTRP_SW_HEAD() goto *insn_handlers[code[pc]];

#define INTRP_SW_PROLOG() \
INSN_LBL_INVALID: { \
	CHDBG(dprintf("Unknown bytecode %d\n", code[pc]); ); \
		throwException(NEW_LANG_EXCEPTION(VerifyError)); \
		goto *insn_handlers[code[npc]]; }


#else
#define	define_insn(code)	case code:				\
				IDBG( dprintf("%03ld: %s\n", (long) pc, #code); );
#define	define_insn_alias(code)	case code:				\
				IDBG( dprintf("%03ld: %s\n", (long) pc, #code); );
#define	define_wide_insn(code)	break;					\
				case code:				\
				IDBG( dprintf("%03ld: %s\n", (long) pc, #code); );
#define define_insn_fini		break;

#define INTRP_SW_HEAD() switch(code[pc])

#define INTRP_SW_PROLOG() \
		default: \
			CHDBG(dprintf("Unknown bytecode %d\n", code[pc]); ); \
			throwException(NEW_LANG_EXCEPTION(VerifyError)); \
			break
#endif

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
	kaffeClassFlags methkflags;

	slots* lcl;
	slots* sp;
	uintp npc;

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
	methkflags = meth->kFlags;

#if defined(ENABLE_JVMPI)
	if (methaccflags & ACC_STATIC)
	  soft_enter_method(NULL, meth);
	else
	  soft_enter_method(((jvalue*)arg)[0].l, meth);
#endif

	if (methodIsNative(meth)) {
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
	if ((methkflags & KFLAG_VERIFIED) == 0) {
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
	  postException(&einfo,  JAVA_LANG(InternalError));
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

#ifdef DIRECT_THREADING

#define IH(name) &&INSN_LBL_##name
#define IH_INVALID &&INSN_LBL_INVALID

static const void *insn_handlers[] = {                                                  	/* opcodes */
    IH(NOP),		IH(ACONST_NULL),	IH(ICONST_M1), 		IH(ICONST_0),		/* 0-3 */
    IH(ICONST_1),	IH(ICONST_2),		IH(ICONST_3),		IH(ICONST_4),		/* 4-7 */
    IH(ICONST_5),	IH(LCONST_0),		IH(LCONST_1),		IH(FCONST_0),		/* 8-11 */
    IH(FCONST_1),	IH(FCONST_2),		IH(DCONST_0),		IH(DCONST_1),		/* 12-15 */
    IH(BIPUSH),		IH(SIPUSH),		IH(LDC1),		IH(LDC2),		/* 16-19 */
    IH(LDC2W),		IH(ILOAD),		IH(LLOAD),		IH(FLOAD),		/* 20-23 */
    IH(DLOAD),		IH(ALOAD),		IH(ILOAD_0),		IH(ILOAD_1),		/* 24-27 */
    IH(ILOAD_2),	IH(ILOAD_3),		IH(LLOAD_0),		IH(LLOAD_1),		/* 28-31 */
    IH(LLOAD_2),	IH(LLOAD_3),		IH(FLOAD_0),		IH(FLOAD_1),		/* 32-35 */
    IH(FLOAD_2),	IH(FLOAD_3),		IH(DLOAD_0),		IH(DLOAD_1),		/* 36-39 */
    IH(DLOAD_2),	IH(DLOAD_3),		IH(ALOAD_0),		IH(ALOAD_1),		/* 40-43 */
    IH(ALOAD_2),	IH(ALOAD_3),		IH(IALOAD),		IH(LALOAD),		/* 44-47 */
    IH(FALOAD),		IH(DALOAD),		IH(AALOAD),		IH(BALOAD),		/* 48-51 */
    IH(CALOAD),		IH(SALOAD),		IH(ISTORE),		IH(LSTORE),		/* 52-55 */
    IH(FSTORE),		IH(DSTORE),		IH(ASTORE),		IH(ISTORE_0),		/* 56-59 */
    IH(ISTORE_1),	IH(ISTORE_2),		IH(ISTORE_3),		IH(LSTORE_0),		/* 60-63 */
    IH(LSTORE_1),	IH(LSTORE_2),		IH(LSTORE_3),		IH(FSTORE_0),		/* 64-67 */
    IH(FSTORE_1),	IH(FSTORE_2),		IH(FSTORE_3),		IH(DSTORE_0),   	/* 68-71 */
    IH(DSTORE_1),	IH(DSTORE_2),		IH(DSTORE_3),		IH(ASTORE_0),   	/* 72-75 */
    IH(ASTORE_1),	IH(ASTORE_2),		IH(ASTORE_3),		IH(IASTORE),		/* 76-79 */
    IH(LASTORE),	IH(FASTORE),		IH(DASTORE),		IH(AASTORE),		/* 80-83 */
    IH(BASTORE),	IH(CASTORE),		IH(SASTORE),		IH(POP),		/* 84-87 */
    IH(POP2),		IH(DUP),		IH(DUP_X1),		IH(DUP_X2),		/* 88-91 */
    IH(DUP2),		IH(DUP2_X1),		IH(DUP2_X2),		IH(SWAP),		/* 92-95 */
    IH(IADD),		IH(LADD),		IH(FADD),		IH(DADD),		/* 96-99 */
    IH(ISUB),		IH(LSUB),		IH(FSUB),		IH(DSUB),		/* 100-103 */
    IH(IMUL),		IH(LMUL),		IH(FMUL),		IH(DMUL),		/* 104-107 */
    IH(IDIV),		IH(LDIV),		IH(FDIV),		IH(DDIV),		/* 108-111 */
    IH(IREM),		IH(LREM),		IH(FREM),		IH(DREM),		/* 112-115 */
    IH(INEG),		IH(LNEG),		IH(FNEG),		IH(DNEG),		/* 116-119 */
    IH(ISHL),		IH(LSHL),		IH(ISHR),		IH(LSHR),		/* 120-123 */
    IH(IUSHR),		IH(LUSHR),		IH(IAND),		IH(LAND),		/* 124-127 */
    IH(IOR),		IH(LOR),		IH(IXOR),		IH(LXOR),		/* 128-131 */
    IH(IINC),		IH(I2L),		IH(I2F),		IH(I2D),		/* 132-135 */
    IH(L2I),		IH(L2F),		IH(L2D),		IH(F2I),		/* 136-139 */
    IH(F2L),		IH(F2D),		IH(D2I),		IH(D2L),		/* 140-143 */
    IH(D2F),		IH(INT2BYTE),		IH(INT2CHAR),		IH(INT2SHORT),		/* 144-147 */
    IH(LCMP),		IH(FCMPL),		IH(FCMPG),		IH(DCMPL),		/* 148-151 */
    IH(DCMPG),		IH(IFEQ),		IH(IFNE),		IH(IFLT),		/* 152-155 */
    IH(IFGE),		IH(IFGT),		IH(IFLE),		IH(IF_ICMPEQ),		/* 156-159 */
    IH(IF_ICMPNE),	IH(IF_ICMPLT),		IH(IF_ICMPGE),		IH(IF_ICMPGT),		/* 160-163 */
    IH(IF_ICMPLE),	IH(IF_ACMPEQ),		IH(IF_ACMPNE),		IH(GOTO),		/* 164-167 */
    IH(JSR),		IH(RET),		IH(TABLESWITCH),	IH(LOOKUPSWITCH), 	/* 168-171 */
    IH(IRETURN),	IH(LRETURN),		IH(FRETURN),		IH(DRETURN),		/* 172-175 */
    IH(ARETURN),	IH(RETURN),		IH(GETSTATIC),		IH(PUTSTATIC),		/* 176-179 */
    IH(GETFIELD),	IH(PUTFIELD),		IH(INVOKEVIRTUAL),	IH(INVOKESPECIAL), 	/* 180-183 */
    IH(INVOKESTATIC),	IH(INVOKEINTERFACE),	IH_INVALID,		IH(NEW),		/* 184-187 */
    IH(NEWARRAY),	IH(ANEWARRAY),		IH(ARRAYLENGTH),	IH(ATHROW),		/* 188-191 */
    IH(CHECKCAST),	IH(INSTANCEOF),		IH(MONITORENTER),	IH(MONITOREXIT),	/* 192-195 */
    IH(WIDE),		IH(MULTIANEWARRAY),	IH(IFNULL),		IH(IFNONNULL),		/* 196-199 */
    IH(GOTO_W),		IH(JSR_W),		IH(BREAKPOINT),		IH_INVALID,		/* 200-203 */
    IH_INVALID,		IH_INVALID,		IH_INVALID,		IH_INVALID,		/* 204-207 */
    IH_INVALID,		IH_INVALID,		IH_INVALID,		IH_INVALID,		/* 208-211 */
    IH_INVALID,		IH_INVALID,		IH_INVALID,		IH_INVALID,		/* 212-215 */
    IH_INVALID,		IH_INVALID,		IH_INVALID,		IH_INVALID,		/* 216-219 */
    IH_INVALID,		IH_INVALID,		IH_INVALID,		IH_INVALID,		/* 220-223 */
    IH_INVALID,		IH_INVALID,		IH_INVALID,		IH_INVALID,		/* 224-227 */
    IH_INVALID,		IH_INVALID,		IH_INVALID,		IH_INVALID,		/* 228-231 */
    IH_INVALID,		IH_INVALID,		IH_INVALID,		IH_INVALID,		/* 232-235 */
    IH_INVALID,		IH_INVALID,		IH_INVALID,		IH_INVALID,		/* 236-239 */
    IH_INVALID,		IH_INVALID,		IH_INVALID,		IH_INVALID,		/* 240-243 */
    IH_INVALID,		IH_INVALID,		IH_INVALID,		IH_INVALID,		/* 244-247 */
    IH_INVALID,		IH_INVALID,		IH_INVALID,		IH_INVALID,		/* 248-251 */
    IH_INVALID,		IH_INVALID,		IH_INVALID,		IH_INVALID		/* 252-255 */
};
#undef IH
#endif

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

		assert(npc < (uintp)(meth->c.bcode.codelen));
		vmExcept_setPC(mjbuf, pc);
		npc = pc + insnLen[code[pc]];

		INTRP_SW_HEAD() {
		    INTRP_SW_PROLOG();
#include "kaffe.def"
		} // INTRP_SW_HEAD()
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
