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
#include "jni.h"

/*
 * Define information about this engine.
 */
char* engine_name = "Just-in-time";
char* engine_version = KVER;

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

/* Various exception related things */
extern Hjava_lang_Class javaLangArrayIndexOutOfBoundsException;
extern Hjava_lang_Class javaLangNullPointerException;

jitflags willcatch;

/* Define CREATE_NULLPOINTER_CHECKS in md.h when your machine cannot use the
 * MMU for detecting null pointer accesses */
#if defined(CREATE_NULLPOINTER_CHECKS)
#define CHECK_NULL(_i, _s, _n)                                  \
      cbranch_ref_const_ne((_s), 0, reference_label(_i, _n)); \
      softcall_nullpointer();                                 \
      set_label(_i, _n)
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

struct {
	int time;
} jitStats;

extern int enable_readonce;

static void generateInsnSequence(void);
static void checkCaughtExceptions(Method* meth, int pc);

void	endBlock(sequence*);
void	startBlock(sequence*);
void	endSubBlock(sequence*);
void	startSubBlock(sequence*);
void	cancelNoWriteback(void);
jlong	currentTime(void);

/*
 * Translate a method into native code.
 */
void
translate(Method* meth)
{
	jint low;
	jint high;
	jlong tmpl;
	int idx;
	SlotInfo* tmp;
	SlotInfo* tmp2;
	SlotInfo* mtable;
	char* str;

	bytecode* base;
	int len;
	callInfo cinfo;
	fieldInfo finfo;
	Hjava_lang_Class* crinfo;

	nativeCodeInfo ncode;

	int64 tms = 0;
	int64 tme;

	static iLock translatorlock;
	static bool init = false;

	if (init == false) {
		init = true;
		initStaticLock(&translatorlock);
	}

	/* Only one in the translator at once. Must check the translation
	 * hasn't been done by someone else once we get it.
	 */
	lockStaticMutex(&translatorlock);
	if (METHOD_TRANSLATED(meth)) {
		unlockStaticMutex(&translatorlock);
		return;
	}

	lockMutex(meth->class);

	if (Kaffe_JavaVMArgs[0].enableVerboseJIT) {
		tms = currentTime();
	}

DBG(MOREJIT,
	dprintf("callinfo = 0x%x\n", &cinfo);	
    )

	/* If this code block is native, then just set it up and return */
	if ((meth->accflags & ACC_NATIVE) != 0) {
		native(meth);
		KAFFEJIT_TO_NATIVE(meth);
		/* Note that this is a real function not a trampoline.  */
		meth->c.ncode.ncode_end = METHOD_NATIVECODE(meth);
		unlockMutex(meth->class);
		unlockStaticMutex(&translatorlock);
		return;
	}

	maxLocal = meth->localsz;
	maxStack = meth->stacksz;
	str = meth->signature->data;
        maxArgs = sizeofSig(&str, false);

	if (meth->accflags & ACC_STATIC) {
		isStatic = 1;
	}
	else {
		isStatic = 0;
		maxArgs += 1;
	}

	base = (bytecode*)meth->c.bcode.code;
	len = meth->c.bcode.codelen;

	/* Scan the code and determine the basic blocks */
	verifyMethod(meth);

	/***************************************/
	/* Next reduce bytecode to native code */
	/***************************************/

	initInsnSequence(codeperbytecode * len, meth->localsz, meth->stacksz);

	start_basic_block();
	start_function();
	monitor_enter();
	if (IS_STARTOFBASICBLOCK(0)) {
		end_basic_block();
		start_basic_block();
	}

	for (pc = 0; pc < len; pc = npc) {

		assert(stackno <= maxStack+maxLocal);

		npc = pc + insnLen[base[pc]];

DBG(JIT,	dprintf("pc = %d, npc = %d\n", pc, npc);	)

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
			fprintf(stderr, "Unknown bytecode %d\n", base[pc]);
			unlockStaticMutex(&translatorlock);
			throwException(VerifyError);
			break;
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

		generateInsnSequence();
	}

	finish_function();

	assert(maxTemp < MAXTEMPS);

	finishInsnSequence(&ncode);
	installMethodCode(meth, &ncode);

	tidyVerifyMethod();

DBG(JIT,
	dprintf("Translated %s.%s%s (%s) %p\n", meth->class->name->data, 
		meth->name->data, meth->signature->data, 
		isStatic ? "static" : "normal", meth->ncode);	
    )

	if (Kaffe_JavaVMArgs[0].enableVerboseJIT) {
		tme = currentTime();
		jitStats.time += (tme - tms);
		printf("<JIT: %s.%s%s time %dms (%dms) @ %p>\n",
		       CLASS_CNAME(meth->class),
		       meth->name->data, meth->signature->data,
		       (int)(tme - tms), jitStats.time,
		       METHOD_NATIVECODE(meth));
	}

	unlockMutex(meth->class);
	unlockStaticMutex(&translatorlock);
}

/*
 * Generate the code.
 */
void
finishInsnSequence(nativeCodeInfo* code)
{
	uint32 constlen;
	nativecode* methblock;
	nativecode* codebase;

	/* Emit pending instructions */
	generateInsnSequence();

	/* Okay, put this into malloc'ed memory */
	constlen = nConst * sizeof(union _constpoolval);
	methblock = gc_malloc(constlen + CODEPC, GC_ALLOC_JITCODE);
	codebase = methblock + constlen;
	memcpy(codebase, codeblock, CODEPC);
	gc_free(codeblock);

	/* Establish any code constants */
	establishConstants(methblock);

	/* Link it */
	linkLabels((uintp)codebase);

	/* Note info on the compiled code for later installation */
	code->mem = methblock;
	code->memlen = constlen + CODEPC;
	code->code = codebase;
	code->codelen = CODEPC;
}

/*
 * Install the compiled code in the method.
 */
void
installMethodCode(Method* meth, nativeCodeInfo* code)
{
	int i;
	jexceptionEntry* e;

	/* Work out new estimate of code per bytecode */
	code_generated += code->memlen;
	bytecode_processed += meth->c.bcode.codelen;
	codeperbytecode = code_generated / bytecode_processed;

	GC_WRITE(meth, code->mem);
	SET_METHOD_NATIVECODE(meth, code->code);
	meth->c.ncode.ncode_start = code->mem;
	meth->c.ncode.ncode_end = code->code + code->codelen;

	/* Flush code out of cache */
	FLUSH_DCACHE(meth->ncode, meth->c.ncode.ncode_end);

	/* Translate exception table and make it available */
	if (meth->exception_table != 0) {
		for (i = 0; i < meth->exception_table->length; i++) {
			e = &meth->exception_table->entry[i];
			e->start_pc = INSNPC(e->start_pc) + (uintp)code->code;
			e->end_pc = INSNPC(e->end_pc) + (uintp)code->code;
			e->handler_pc = INSNPC(e->handler_pc) + (uintp)code->code;
		}
	}

	/* Translate line numbers table */
	if (meth->lines != 0) {
		for (i = 0; i < meth->lines->length; i++) {
			meth->lines->entry[i].start_pc = INSNPC(meth->lines->entry[i].start_pc) + (uintp)code->code;
		}
	}
}

/*
 * Init instruction generation.
 */
void
initInsnSequence(int codesize, int localsz, int stacksz)
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
	resetLabels();

	localinfo = &slotinfo[0];
	tempinfo = &localinfo[stackno];

	/* Before generating code, try to guess how much space we'll need. */
	codeblock_size = codesize;
	if (codeblock_size < ALLOCCODEBLOCKSZ) {
		codeblock_size = ALLOCCODEBLOCKSZ;
	}
	codeblock = gc_malloc_fixed(codeblock_size + CODEBLOCKREDZONE);
	CODEPC = 0;
}

/*
 * Generate instructions from current set of sequences.
 */
static
void
generateInsnSequence(void)
{
	sequence* t;

	for (t = firstSeq; t != currSeq; t = t->next) {

		/* If we overrun the codeblock, reallocate and continue.  */
		if (CODEPC >= codeblock_size) {
			codeblock_size += ALLOCCODEBLOCKSZ;
			codeblock = gc_realloc_fixed(codeblock, codeblock_size + CODEBLOCKREDZONE);
		}

		/* Generate sequences */
		(*(t->func))(t);
	}

	/* Reset */
	initSeq();
}

/*
 * Start a new instruction.
 */
void
startInsn(sequence* s)
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
startSubBlock(sequence* s)
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
fixupFunctionCall(sequence* s)
{
	int i;

	/* Invalidate all slots - don't use clobberRegister which will
	 * flush them - we do not want to do that even if they are dirty.
	 */
	for (i = maxslot - 1; i >= 0; i--) {
		if (slotinfo[i].regno != NOREG && (reginfo[slotinfo[i].regno].flags & Rnosaveoncall) == 0) {
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
		if ((localinfo[i].modified & rwrite) != 0 && localinfo[i].regno != NOREG && (reginfo[localinfo[i].regno].flags & Rnosaveoncall) == 0) {
			spill(&localinfo[i]);
			localinfo[i].modified = 0;
		}
	}

	/* Spill stack */
	stkno = const_int(1);
	for (i = stkno; i < maxStack+maxLocal; i++) {
		if ((localinfo[i].modified & rwrite) != 0 && localinfo[i].regno != NOREG && (reginfo[localinfo[i].regno].flags & Rnosaveoncall) == 0) {
			spill(&localinfo[i]);
			localinfo[i].modified = 0;
		}
	}

	/* Spill temps currently in use */
	tmpslot = const_int(2);
	for (i = 0; i < tmpslot; i++) {
		if ((tempinfo[i].modified & rwrite) != 0 && tempinfo[i].regno != NOREG && (reginfo[tempinfo[i].regno].flags & Rnosaveoncall) == 0) {
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

	old_ro = enable_readonce;
	enable_readonce = 0;

	/* Spill locals */
	for (i = 0; i < maxLocal; i++) {
		if ((localinfo[i].modified & rwrite) != 0 && localinfo[i].regno != NOREG && (reginfo[localinfo[i].regno].flags & Rnosaveoncall) == 0) {
			spill(&localinfo[i]);
		}
	}

	/* Spill stack */
	stkno = const_int(1);
	for (i = stkno; i < maxLocal+maxStack; i++) {
		if ((localinfo[i].modified & rwrite) != 0 && localinfo[i].regno != NOREG && (reginfo[localinfo[i].regno].flags & Rnosaveoncall) == 0) {
			spill(&localinfo[i]);
		}
	}

	/* Spill temps currently in use */
	tmpslot = const_int(2);
	for (i = 0; i < tmpslot; i++) {
		if ((tempinfo[i].modified & rwrite) != 0 && tempinfo[i].regno != NOREG && (reginfo[tempinfo[i].regno].flags & Rnosaveoncall) == 0) {
			spill(&tempinfo[i]);
		}
	}

	enable_readonce = old_ro;
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
checkCaughtExceptions(Method* meth, int pc)
{
	int i;

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
			if (instanceof(&javaLangArrayIndexOutOfBoundsException, etype)) {
				willCatch(BADARRAYINDEX);
			}
			if (instanceof(&javaLangNullPointerException, etype)) {
				willCatch(NULLPOINTER);
			}
		}
	}
}
