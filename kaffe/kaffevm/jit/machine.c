/* machine.c
 * Translate the Kaffe instruction set to the native one.
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#include "config.h"
#include "debug.h"
#include "config-std.h"
#include "config-mem.h"
#include "classMethod.h"
#include "gtypes.h"
#include "kaffe/jmalloc.h"
#include "bytecode.h"
#include "slots.h"
#include "registers.h"
#include "seq.h"
#include "gc.h"
#include "machine.h"
#include "basecode.h"
#include "icode.h"
#include "icode_internal.h"
#include "labels.h"
#include "constpool.h"
#include "codeproto.h"
#include "checks.h"
#include "access.h"
#include "object.h"
#include "constants.h"
#include "baseClasses.h"
#include "code.h"
#include "access.h"
#include "lookup.h"
#include "exception.h"
#include "errors.h"
#include "md.h"
#include "locks.h"
#include "code-analyse.h"
#include "external.h"
#include "soft.h"
#include "stringSupport.h"
#include "jni.h"
#include "thread.h"
#include "jthread.h"
#include "stats.h"
#include "support.h"
#include "native-wrapper.h"
#include "jvmpi_kaffe.h"

/*
 * Define information about this engine.
 */
char* engine_name = "Just-in-time";

iStaticLock	translatorlock;		/* lock to protect the variables below */
int stackno;
int maxStack;
int maxLocal;
int maxTemp;
int maxArgs;
int maxPush;
int isStatic;

int tmpslot;
int argcount = 0;		/* Function call argument count */
uint32 pc;
uint32 npc;

jitflags willcatch;

#define EXPLICIT_CHECK_NULL(_i, _s, _n)                       \
      cbranch_ref_const_ne((_s), NULL, reference_label(_i, _n)); \
      softcall_nullpointer();                                 \
      set_label(_i, _n)
/* Define CREATE_NULLPOINTER_CHECKS in md.h when your machine cannot use the
 * MMU for detecting null pointer accesses */
#if defined(CREATE_NULLPOINTER_CHECKS)
#define CHECK_NULL(_i, _s, _n)                                  \
    EXPLICIT_CHECK_NULL(_i, _s, _n)
#else
/*
 * If we rely on the MMU to catch null pointer accesses, we must spill
 * the registers to their home locations if that exception can be caught,
 * since the exception handler will load its registers from there.
 * For now, we use prepare_function_call() to do that. (Tim?)
 */
#define CHECK_NULL(_i, _s, _n)	\
	if (canCatch(NULLPOINTER)) {		\
		prepare_function_call();  	\
	}
#endif

/* For JIT3 compatibility */
#define check_array_store(a,b)		softcall_checkarraystore(a,b)
#define explicit_check_null(x,obj,y)	EXPLICIT_CHECK_NULL(x,obj,y)
#define check_null(x,obj,y)		CHECK_NULL(x,obj,y)
#define check_div(x,obj,y)
#define check_div_long(x,obj,y)

/* Unit in which code block is increased when overrun */
#define	ALLOCCODEBLOCKSZ	8192
/* Codeblock redzone - allows for safe overrun when generating instructions */
#define	CODEBLOCKREDZONE	256

nativecode* codeblock;
int codeblock_size;
static int code_generated;
static int bytecode_processed;
static int codeperbytecode;

int CODEPC;
Method *globalMethod;

#if defined(KAFFE_PROFILER)
int profFlag; /* flag to control profiling */
static void printProfilerStats(void);
#endif

struct {
	int time;
} jitStats;

static void generateInsnSequence(codeinfo* codeInfo);
static void checkCaughtExceptions(Method* meth, unsigned int pc);

void	endBlock(sequence*);
void	startBlock(sequence*);
void	endSubBlock(sequence*);
void	startSubBlock(sequence*);
void	cancelNoWriteback(void);
jlong	currentTime(void);

/*
 * By default, we comply with the Java spec and turn stack overflow checks
 * on.  Note that this involves a noticeable performance penalty.  If you
 * feel adventurous, undef this.
 */
#define CHECK_STACKOVERFLOW

#if defined(CHECK_STACKOVERFLOW)

static void 
checkStackOverflow(void)
{
	/* XXX fix this.  
	 * We should not have to access current just to do the stack check
	 */
	threadData *thread_data = KTHREAD(get_data)(KTHREAD(current)());

	if (KTHREAD(stackcheck)(thread_data->needOnStack)) {
		return;
	}
	
	if (thread_data->needOnStack == STACK_LOW) {
		dprintf(
			"Panic: unhandled StackOverflowError()\n");
		KAFFEVM_ABORT();
	}
	
	{
		Hjava_lang_Throwable *th;
		errorInfo einfo;

		thread_data->needOnStack = STACK_LOW;
		th = (Hjava_lang_Throwable *)newObjectChecked (javaLangStackOverflowError, &einfo);
		thread_data->needOnStack = STACK_HIGH;

		throwException(th);
	}
}
#endif /* CHECK_STACKOVERFLOW */

/*
 * Translate a method into native code.
 *
 * Return true if successful, false otherwise.
 */
bool
translate(Method* meth, errorInfo *einfo)
{
	jint low;
	jint high;
	jvalue tmpl;
	int idx;
	SlotInfo* tmp;
	SlotInfo* tmp2;
	SlotInfo* mtable;

	bytecode* base;
	unsigned int len;
	callInfo cinfo;
	fieldInfo finfo;
	Hjava_lang_Class* crinfo;
	bool success = true;

	nativeCodeInfo ncode;
	codeinfo* codeInfo;

	int64 tms = 0;
	int64 tme;


	static Method* jitting = NULL;	/* DEBUG */

	lockClass(meth->class);

	/* Must check the translation
	 * hasn't been done by someone else once we get it.
	 */
	if (METHOD_TRANSLATED(meth)) {
		goto done3;
	}

	if (Kaffe_JavaVMArgs.enableVerboseJIT) {
		tms = currentTime();
	}

DBG(MOREJIT,
	dprintf("asked to translate = %s.%s(%s)\n", 
	    meth->class->name->data, meth->name->data, METHOD_SIGD(meth));	
    );

	/* If this code block is native, then just set it up and return */
	if ((meth->accflags & ACC_NATIVE) != 0) {
		void *func = native(meth, einfo);

		if (func == NULL) {
			success = false;
			goto done3;
		}
		engine_create_wrapper(meth, func);
		KAFFEJIT_TO_NATIVE(meth);
		/* Note that this is a real function not a trampoline.  */
		if (meth->c.ncode.ncode_end == 0)
			meth->c.ncode.ncode_end = METHOD_NATIVECODE(meth);
		goto done3;
	}

	/* Scan the code and determine the basic blocks */
	success = analyzeMethod(meth, &codeInfo, einfo); 
	if (success == false) {
		goto done3;
	}

DBG(MOREJIT,
	dprintf("successfully verified = %s.%s(%s)\n", 
	    meth->class->name->data, meth->name->data, METHOD_SIGD(meth));	
    );

	/* Only one in the translator at once. */
	enterTranslator();
	startTiming(&jit_time, "jittime");

#if defined(KAFFE_PROFILER)
	if (profFlag) {
		static int init = 0;

		if (!init) {
			atexit(printProfilerStats);
			init = 1;
		}

		profiler_get_clicks(meth->jitClicks);
		meth->callsCount = 0;
		meth->totalClicks = 0;
		meth->totalChildrenClicks = 0;
		globalMethod = meth;
	}
#endif

DBG(MOREJIT,
	if (jitting) {
		dprintf("but still jitting = %s.%s(%s)\n", 
			jitting->class->name->data, jitting->name->data, 
			METHOD_SIGD(jitting));	
	}
    );
	/* start modifying global variables now */
	assert(jitting == 0 || !!!"reentered jitter");	/* DEBUG */
	jitting = meth;					/* DEBUG */

	maxLocal = meth->localsz;
	maxStack = meth->stacksz;
        maxArgs = sizeofSigMethod(meth, false);
	if (maxArgs == -1) {
		goto done2;
	}

	if (meth->accflags & ACC_STATIC) {
		isStatic = 1;
	}
	else {
		isStatic = 0;
		maxArgs += 1;
	}

	base = (bytecode*)meth->c.bcode.code;
	len = meth->c.bcode.codelen;

	/***************************************/
	/* Next reduce bytecode to native code */
	/***************************************/
	if (!(success = initInsnSequence(meth, codeperbytecode * len, meth->localsz,
					 meth->stacksz, einfo))) {
		goto done1;
	}

	start_basic_block();
	start_function();

#if defined(CHECK_STACKOVERFLOW)
	prepare_function_call();
	call_soft(checkStackOverflow);
	fixup_function_call();
#endif

	monitor_enter();
	if (IS_STARTOFBASICBLOCK(0)) {
		end_basic_block();
		start_basic_block();
	}

	for (pc = 0; pc < len; pc = npc) {

		assert(stackno <= maxStack+maxLocal);

		npc = pc + insnLen[base[pc]];

		/* Skip over the generation of any unreachable basic blocks */
		if (IS_UNREACHABLE(pc)) {
			while (npc < len && !IS_STARTOFBASICBLOCK(npc) && !IS_STARTOFEXCEPTION(npc)) {
				npc = npc + insnLen[base[npc]];
			}
DBG(JIT,		dprintf("unreachable basic block pc [%d:%d]\n", pc, npc - 1);   );
			if (IS_STARTOFBASICBLOCK(npc)) {
				end_basic_block();
				start_basic_block();
				stackno = STACKPOINTER(npc);
			}
			continue;
		}

DBG(JIT,	dprintf("pc = %d, npc = %d\n", pc, npc);	);

		/* Determine various exception conditions */
		checkCaughtExceptions(meth, pc);

		start_instruction();

		/* Note start of exception handling blocks */
		if (IS_STARTOFEXCEPTION(pc)) {
			stackno = meth->localsz + meth->stacksz - 1;
			start_exception_block();
		}

		switch (base[pc]) {
		default:
			dprintf("Unknown bytecode %d\n", base[pc]);
			postException(einfo, JAVA_LANG(VerifyError));
			success = false;
			goto done;
#include "kaffe.def"
		}

		/* Note maximum number of temp slots used and reset it */
		if (tmpslot > maxTemp) {
			maxTemp = tmpslot;
		}
		tmpslot = 0;

		cancelNoWriteback();

		if (IS_STARTOFBASICBLOCK(npc)) {
			end_basic_block();
			start_basic_block();
			stackno = STACKPOINTER(npc);
		}

		generateInsnSequence(codeInfo);
	}

	end_function();

	assert(maxTemp < MAXTEMPS);

	if (finishInsnSequence(codeInfo, &ncode, einfo) == false) {
		success = false;
		goto done;
	}
	installMethodCode(codeInfo, meth, &ncode);

done:
	tidyAnalyzeMethod(&codeInfo);

DBG(JIT,
	dprintf("Translated %s.%s%s (%s) %p\n", meth->class->name->data, 
		meth->name->data, METHOD_SIGD(meth), 
		isStatic ? "static" : "normal", METHOD_NATIVECODE(meth));	
    );

	if (Kaffe_JavaVMArgs.enableVerboseJIT) {
		tme = currentTime();
		jitStats.time += (tme - tms);
		printf("<JIT: %s.%s%s time %dms (%dms) @ %p>\n",
		       CLASS_CNAME(meth->class),
		       meth->name->data, METHOD_SIGD(meth),
		       (int)(tme - tms), jitStats.time,
		       METHOD_NATIVECODE(meth));
	}
done1:
#if defined(KAFFE_PROFILER)
	if (profFlag) {
		profiler_click_t end;

		profiler_get_clicks(end);
		meth->jitClicks = end - meth->jitClicks;
		globalMethod = NULL;
	}
#endif
done2:
	jitting = NULL;	/* DEBUG */
	stopTiming(&jit_time);
	leaveTranslator();
done3:
	unlockClass(meth->class);
	
#if defined(ENABLE_JVMPI)
	if (success && JVMPI_EVENT_ISENABLED(JVMPI_EVENT_COMPILED_METHOD_LOAD) )
	  {
	    JVMPI_Event ev;

	    jvmpiFillMethodLoad(&ev, meth);
	    jvmpiPostEvent(&ev);
	  }
#endif

	return (success);
}

#ifndef ALIGNMENT_OF_SIZE
#define ALIGNMENT_OF_SIZE(S)	(S)
#endif

/*
 * Generate the code.
 */
bool
finishInsnSequence(codeinfo* codeInfo, nativeCodeInfo* code, errorInfo *einfo)
{
#if defined(CALLTARGET_ALIGNMENT)
	unsigned int align = CALLTARGET_ALIGNMENT;
#else
	unsigned int align = 0;
#endif
#if defined(MD_JIT_EXCEPTION_INFO_LENGTH)
	int exc_len = MD_JIT_EXCEPTION_INFO_LENGTH;
#else
	int exc_len = 0;
#endif
	uintp const_align = sizeof(union _constpoolval) - 1;
	uint32 constlen;
	jitCodeHeader *jch;
	nativecode* methblock;

	/* Emit pending instructions */
	generateInsnSequence(codeInfo);

	/* Okay, put this into malloc'ed memory. We have to align the pool for
	 * some double-word aligned instructions. */
	constlen = KaffeJIT_getNumberOfConstants() * sizeof(union _constpoolval);
	/* Allocate some padding to align codebase if so desired 
	 */
	methblock = gc_malloc(sizeof(jitCodeHeader) + exc_len + constlen + const_align + CODEPC + (align ? (align - ALIGNMENT_OF_SIZE(sizeof(jdouble))) : 0), KGC_ALLOC_JITCODE);
	if (methblock == 0) {
		postOutOfMemory(einfo);
		return (false);
	}
	jch = (jitCodeHeader *)methblock;
	jch->pool = (void *)((char *)(jch + 1)) + exc_len;
	jch->pool = (void *)(((uintp)jch->pool + const_align) & ~const_align );
	jch->code_start = ((char *)jch->pool) + constlen;
	jch->code_len = CODEPC;
	/* align entry point if so desired */
	if (align != 0 && (unsigned long)jch->code_start % align != 0) {
		unsigned int pad = (align - (unsigned long)jch->code_start % align);
		
		assert(pad <= align - ALIGNMENT_OF_SIZE(sizeof(jdouble)));
		
		jch->code_start = (char*)jch->code_start + pad;
	}
	memcpy(jch->code_start, codeblock, CODEPC);
	addToCounter(&jitcodeblock, "jitmem-codeblock", 1,
		-(jlong)GCSIZEOF(codeblock));
	KFREE(codeblock);

	/* Establish any code constants */
	KaffeJIT_establishConstants(jch->pool);

	/* Link it */
	KaffeJIT_linkLabels(codeInfo, (uintp)jch->code_start);

	/* Note info on the compiled code for later installation */
	code->mem = methblock;
	code->memlen = exc_len + constlen + CODEPC;
	code->code = jch->code_start;
	code->codelen = CODEPC;
	
	return (true);
}

static int
getInsnPC(int pc, codeinfo* codeInfo, nativeCodeInfo* code) 
{
	int maxPC = codeInfo->codelen;

	while (pc < maxPC) {
		if (IS_STARTOFINSTRUCTION(pc)) {
			int insn = INSNPC(pc);
			if (insn!=-1) {
				return insn;
			}
		}
		pc++;
	}

	return code->codelen;
}
 
/*
 * Install the compiled code in the method.
 */
void
installMethodCode(codeinfo* codeInfo, Method* meth, nativeCodeInfo* code)
{
	unsigned int i;
	jexceptionEntry* e;
	jitCodeHeader *jch;
	void *tramp;

	/* Work out new estimate of code per bytecode */
	code_generated += code->memlen;
	bytecode_processed += meth->c.bcode.codelen;
	codeperbytecode = code_generated / bytecode_processed;

	tramp = METHOD_NATIVECODE(meth);

	/* install the jitted code */
	SET_METHOD_JITCODE(meth, code->code);

	meth->c.ncode.ncode_start = code->mem;
	meth->c.ncode.ncode_end = (char*)code->code + code->codelen;

	jch = (jitCodeHeader *)code->mem;
	jch->method = meth;
	
	/* Flush code out of cache */
	FLUSH_DCACHE(METHOD_NATIVECODE(meth), meth->c.ncode.ncode_end);

#if defined(MD_REGISTER_JIT_EXCEPTION_INFO)
	MD_REGISTER_JIT_EXCEPTION_INFO (meth->c.ncode.ncode_start,
					METHOD_NATIVECODE(meth),
					meth->c.ncode.ncode_end);
#endif

	/* Translate exception table and make it available */
	if (meth->exception_table != 0) {
		for (i = 0; i < meth->exception_table->length; i++) {
			/* Note that we cannot assume that the bytecode
			 * instructions that are at the start/end/handler
			 * pc index have a corresponding native pc.
			 * They only have one if they already emitted
			 * native instructions during translation.
			 * Jikes, for instance, likes to put a `nop' at
			 * the end_pc index of an exception handler range.
			 *
			 * If this happens, we simply find the next bytecode
			 * instruction that has a native pc and use it instead.
			 */
			e = &meth->exception_table->entry[i];
			e->start_pc = getInsnPC(e->start_pc, codeInfo, code) + (uintp)code->code;
			e->end_pc = getInsnPC(e->end_pc, codeInfo, code) + (uintp)code->code;
			e->handler_pc = getInsnPC(e->handler_pc, codeInfo, code) + (uintp)code->code;
			assert(e->start_pc <= e->end_pc);
		}
	}

	/* Translate line numbers table */
	if (meth->lines != 0) {
		for (i = 0; i < meth->lines->length; i++) {
			meth->lines->entry[i].start_pc = getInsnPC(meth->lines->entry[i].start_pc, codeInfo, code) + (uintp)code->code;
		}
	}
}

/*
 * Init instruction generation.
 */
bool
initInsnSequence(Method* meth, int codesize, int localsz, int stacksz,
		 struct _errorInfo *einfo)
{
	/* Clear various counters */
	tmpslot = 0;
	maxTemp = 0;
	maxPush = 0;  
	stackno = localsz + stacksz;
	maxTemp = MAXTEMPS - 1; /* XXX */ 
	npc = 0;

	initSeq();
	initRegisters();
	initSlots(stackno);
	KaffeJIT_resetLabels();
	KaffeJIT_resetConstants();

	localinfo = &slotinfo[0];
	tempinfo = &localinfo[stackno];

	/* Before generating code, try to guess how much space we'll need. */
	codeblock_size = codesize;
	if (codeblock_size < ALLOCCODEBLOCKSZ) {
		codeblock_size = ALLOCCODEBLOCKSZ;
	}
	codeblock = KMALLOC(codeblock_size + CODEBLOCKREDZONE);
	if (!codeblock) {
		postOutOfMemory(einfo);
		return false;
	}
	addToCounter(&jitcodeblock, "jitmem-codeblock", 1,
		     (jlong) GCSIZEOF(codeblock));
	CODEPC = 0;

	/* 
	 * add the method we're translating as the first entry to the constant pool,
	 * so that we can look it up pretty easily when creating stack traces.
	 */
	KaffeJIT_newConstant(CPref, meth);
	return true;
}

/*
 * Generate instructions from current set of sequences.
 */
static
void
generateInsnSequence(codeinfo* codeInfo)
{
	sequence* t;

	for (t = firstSeq; t != currSeq; t = t->next) {

		/* If we overrun the codeblock, reallocate and continue.  */
		if (CODEPC >= codeblock_size) {
			codeblock_size += ALLOCCODEBLOCKSZ;
			addToCounter(&jitcodeblock, "jitmem-codeblock", 0,
				    -(jlong)GCSIZEOF(codeblock));
			codeblock = KREALLOC(codeblock, codeblock_size + CODEBLOCKREDZONE);
			addToCounter(&jitcodeblock, "jitmem-codeblock", 0,
				     (jlong)GCSIZEOF(codeblock));
		}

		/* Generate sequences */
		(*(void (*)(sequence*, codeinfo*))(t->func))(t, codeInfo);
	}

	/* Reset */
	initSeq();
}

/*
 * Start a new instruction.
 */
void
startInsn(sequence* s, codeinfo* codeInfo)
{
	SET_INSNPC(const_int(2), CODEPC);
}

/*
 * Mark slot not to be written back.
 */
void
nowritebackSlot(sequence* s)
{
	seq_slot(s, 0)->modified |= rnowriteback;
}

/*
 * Start a new basic block.
 */
void
startBlock(sequence* s)
{
	startSubBlock(s);
}

/*
 * Start a new basic sub-block.
 */
void
startSubBlock(sequence* s UNUSED)
{
	int i;

	/* Invalidate all slots - don't use clobberRegister which will
	 * flush them - we do not want to do that even if they are dirty.
	 */
	for (i = maxslot - 1; i >= 0; i--) {
		if (slotinfo[i].regno != NOREG) {
			register_invalidate(slotinfo[i].regno);
			slot_invalidate(&slotinfo[i]);
		}
	}
}

/*
 * Fixup after a function call.
 */
void
fixupFunctionCall(sequence* s UNUSED)
{
	int i;

	/* Invalidate all slots - don't use clobberRegister which will
	 * flush them - we do not want to do that even if they are dirty.
	 */
	for (i = maxslot - 1; i >= 0; i--) {
		if (slotinfo[i].regno != NOREG && (KaffeVM_jitGetRegInfo()[slotinfo[i].regno].flags & Rnosaveoncall) == 0) {
			register_invalidate(slotinfo[i].regno);
			slot_invalidate(&slotinfo[i]);
		}
	}
}

/*
 * End a basic block.
 */
void
endBlock(sequence* s)
{
	endSubBlock(s);
}

/*
 * End a basic sub-block.
 */
void
endSubBlock(sequence* s)
{
	int stkno;
	int tmpslot;
	int i;

	/* Spill locals */
	for (i = 0; i < maxLocal; i++) {
		if ((localinfo[i].modified & rwrite) != 0 && localinfo[i].regno != NOREG) {
			if ((localinfo[i].modified & rnowriteback) == 0) {
				spill(&localinfo[i]);
				localinfo[i].modified = 0;
			}
			else {
				localinfo[i].modified &= ~rnowriteback;
			}
		}
	}

	/* Spill stack */
	stkno = const_int(1);
	for (i = stkno; i < maxStack+maxLocal; i++) {
		if ((localinfo[i].modified & rwrite) != 0 && localinfo[i].regno != NOREG) {
			if ((localinfo[i].modified & rnowriteback) == 0) {
				spill(&localinfo[i]);
				localinfo[i].modified = 0;
			}
			else {
				localinfo[i].modified &= ~rnowriteback;
			}
		}
	}

	/* Spill temps currently in use */
	tmpslot = const_int(2);
	for (i = 0; i < tmpslot; i++) {
		if ((tempinfo[i].modified & rwrite) != 0 && tempinfo[i].regno != NOREG) {
			if ((tempinfo[i].modified & rnowriteback) == 0) {
				spill(&tempinfo[i]);
				tempinfo[i].modified = 0;
			}
			else {
				tempinfo[i].modified &= ~rnowriteback;
			}
		}
	}
}

/*
 * Prepare register state for function call.
 */
void
prepareFunctionCall(sequence* s)
{
	int stkno;
	int tmpslot;
	int i;

	/* Spill locals */
	for (i = 0; i < maxLocal; i++) {
		if ((localinfo[i].modified & rwrite) != 0 && localinfo[i].regno != NOREG && (KaffeVM_jitGetRegInfo()[localinfo[i].regno].flags & Rnosaveoncall) == 0) {
			spill(&localinfo[i]);
			localinfo[i].modified = 0;
		}
	}

	/* Spill stack */
	stkno = const_int(1);
	for (i = stkno; i < maxStack+maxLocal; i++) {
		if ((localinfo[i].modified & rwrite) != 0 && localinfo[i].regno != NOREG && (KaffeVM_jitGetRegInfo()[localinfo[i].regno].flags & Rnosaveoncall) == 0) {
			spill(&localinfo[i]);
			localinfo[i].modified = 0;
		}
	}

	/* Spill temps currently in use */
	tmpslot = const_int(2);
	for (i = 0; i < tmpslot; i++) {
		if ((tempinfo[i].modified & rwrite) != 0 && tempinfo[i].regno != NOREG && (KaffeVM_jitGetRegInfo()[tempinfo[i].regno].flags & Rnosaveoncall) == 0) {
			spill(&tempinfo[i]);
			tempinfo[i].modified = 0;
		}
	}
}

/*
 * Sync register state back to memory (but don't clean anything).
 */
void
syncRegisters(sequence* s)
{
	int stkno;
	int tmpslot;
	int i;
	int old_ro;

	old_ro = KaffeVM_jitGetEnableReadonce();
	KaffeVM_jitSetEnableReadonce(0);

	/* Spill locals */
	for (i = 0; i < maxLocal; i++) {
		if ((localinfo[i].modified & rwrite) != 0 && localinfo[i].regno != NOREG && (KaffeVM_jitGetRegInfo()[localinfo[i].regno].flags & Rnosaveoncall) == 0) {
			spill(&localinfo[i]);
		}
	}

	/* Spill stack */
	stkno = const_int(1);
	for (i = stkno; i < maxLocal+maxStack; i++) {
		if ((localinfo[i].modified & rwrite) != 0 && localinfo[i].regno != NOREG && (KaffeVM_jitGetRegInfo()[localinfo[i].regno].flags & Rnosaveoncall) == 0) {
			spill(&localinfo[i]);
		}
	}

	/* Spill temps currently in use */
	tmpslot = const_int(2);
	for (i = 0; i < tmpslot; i++) {
		if ((tempinfo[i].modified & rwrite) != 0 && tempinfo[i].regno != NOREG && (KaffeVM_jitGetRegInfo()[tempinfo[i].regno].flags & Rnosaveoncall) == 0) {
			spill(&tempinfo[i]);
		}
	}

	KaffeVM_jitSetEnableReadonce(old_ro);
}

/*
 * Cancel any pending nowriteback flags.
 */
void
cancelNoWriteback(void)
{
	int i;

	for (i = stackno; i < maxLocal+maxStack; i++) {
		localinfo[i].modified &= ~rnowriteback;
	}
}

/*
 * check what synchronous exceptions are caught for a given instruction
 */
static
void 
checkCaughtExceptions(Method* meth, unsigned int pc)
{
	unsigned int i;

	willcatch.BADARRAYINDEX = false;
	willcatch.NULLPOINTER = false;

	if (meth->exception_table == 0) 
		return;

	/* Determine various exception conditions */
	for (i = 0; i < meth->exception_table->length; i++) {
		Hjava_lang_Class* etype;

		/* include only if exception handler range matches pc */
		if (meth->exception_table->entry[i].start_pc > pc ||
		    meth->exception_table->entry[i].end_pc <= pc)
			continue;

		etype = meth->exception_table->entry[i].catch_type;
		if (etype == 0) {
			willCatch(BADARRAYINDEX);
			willCatch(NULLPOINTER);
		}
		else {
			if (instanceof(javaLangArrayIndexOutOfBoundsException, etype)) {
				willCatch(BADARRAYINDEX);
			}
			if (instanceof(javaLangNullPointerException, etype)) {
				willCatch(NULLPOINTER);
			}
		}
	}
}

/*
 * return what engine we're using
 */
const char* 
getEngine(void)
{
	return "kaffe.jit";
}

void initEngine(void)
{
  initStaticLock(&translatorlock);
}

#if defined(KAFFE_PROFILER)
static jlong click_multiplier;
static profiler_click_t click_divisor;
static FILE *prof_output;

static int
profilerClassStat(Hjava_lang_Class *clazz, void *param UNUSED)
{
	Method *meth;
	int mindex;

	for (mindex = CLASS_NMETHODS(clazz), meth = clazz->methods; mindex-- > 0; meth++) {
		if (meth->callsCount == 0)
			continue;

		fprintf(prof_output,
			"%10d %10.6g %10.6g %10.6g %10.6g %s.%s%s\n",
			meth->callsCount,
			(click_multiplier * ((double)meth->totalClicks)) / click_divisor,
			(click_multiplier * ((double)(meth->totalClicks
						      - meth->totalChildrenClicks)))
			/ click_divisor,
			(click_multiplier * ((double)meth->totalChildrenClicks)) / click_divisor,
			(click_multiplier * ((double)meth->jitClicks)) / click_divisor,

			meth->class->name->data,
			meth->name->data,
			METHOD_SIGD(meth)
		       );
	}
	return 0;
}


static void
printProfilerStats(void)
{
	profiler_click_t start, end;
	jlong m_start, m_end;

	/* If the profFlag == 0, don't bother printing anything */
	if (profFlag == 0)
		return;

 	/* open file */
 	prof_output = fopen("prof.out", "w");
	if (prof_output == NULL) {
		return;
	}

	/* Need to figure out what profiler_click  _really_ is.
	 * Use currentTime() to guest how long is a second.  Call it before
	 * to don't count dynamic linker resolve time.  */
	m_start = currentTime();
	profiler_get_clicks(start);
	sleep (1);
	m_end = currentTime();
	profiler_get_clicks(end);

	click_multiplier = m_end - m_start;
	click_divisor = end - start;

	fprintf(prof_output,
		"# clockTick: %lld per 1/%lld seconds aka %lld per milliseconds\n",
		click_divisor,
		click_multiplier,
		click_divisor / click_multiplier);

 	fprintf(prof_output, "%10s %10s %10s %10s %10s %s\n",
 		"#    count", "total", "self", "children", "jit",
 		"method-name");

	/* Traverse through class hash table */
	walkClassPool(profilerClassStat, NULL);

	fclose(prof_output);
}
#endif
