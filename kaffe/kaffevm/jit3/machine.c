/* machine.c
 * Translate the Kaffe instruction set to the native one.
 *
 * Copyright (c) 1996-1999
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#define SCHK(s) s
#define	SUSE(s)

#include "config.h"
#include "config-std.h"
#include "config-mem.h"
#include "gtypes.h"
#include "md.h"
#include "classMethod.h"
#include "bytecode.h"
#include "slots.h"
#include "registers.h"
#include "seq.h"
#include "gc.h"
#include "machine.h"
#include "basecode.h"
#include "icode.h"
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
#include "locks.h"
#include "code-analyse.h"
#include "external.h"
#include "soft.h"
#include "thread.h"
#include "itypes.h"
#include "code.h"
#include "stringSupport.h"
#include "debug.h"
#include "stats.h"

char* engine_name = "Just-in-time v3";
char* engine_version = KAFFEVERSION;

iLock translatorlock;           /* lock to protect the variables below */
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

#if defined(KAFFE_PROFILER)
int profFlag; /* flag to control profiling */
Method *globalMethod;
static void printProfilerStats(void);
#endif

/* Various exception related things */
extern Hjava_lang_Class javaLangArrayIndexOutOfBoundsException;
extern struct JNIEnv_ Kaffe_JNIEnv;
extern Method* jniMethod;
extern void startJNIcall(void);
extern void finishJNIcall(void);
extern void addJNIref(jref);
extern void removeJNIref(jref);

jitflags willcatch;

/* jit3 specific prototypes from icode.c */
void check_null (int x, SlotInfo* obj, int y);
void check_div (int x, SlotInfo* obj, int y);
void check_div_long (int x, SlotInfo* obj, int y);
void softcall_fakecall (label* from,label* to, void* func);
#define	CHECK_NULL check_null

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

void	initInsnSequence(int, int, int);
bool	finishInsnSequence(codeinfo*, nativeCodeInfo*, errorInfo*);
static void generateInsnSequence(codeinfo*);
void installMethodCode(codeinfo*, Method*, nativeCodeInfo*);
static void nullCall(void);

jlong	currentTime(void);
Method*	findMethodFromPC(uintp);
label* newFakeCall(void*, uintp);
static void makeFakeCalls(void);


/*
 * Translate a method into native code.
 */
void
translate(Method* meth, errorInfo *einfo)
{
	int i;

	jint low;
	jint high;
	jlong tmpl;
	int idx;
	SlotInfo* tmp;
	SlotInfo* tmp2;
	SlotInfo* mtable;

	bytecode* base;
	uint32 len;
	callInfo cinfo;
	fieldInfo finfo;
	Hjava_lang_Class* crinfo;

	nativeCodeInfo ncode;

	int64 tms = 0;
	int64 tme;

	static bool reinvoke = false;

	bool success = true;
	codeinfo* codeInfo;
	const char* str;

	// MSR_START( jit_time);

	/* Since we get here by calling the trampoline, we must be about
	 * to call this method - so make sure it's OK before generating
	 * code to avoid any extra initialization calls.
	 */
	processClass(meth->class, CSTATE_COMPLETE, einfo);

	/* Only one in the translator at once. Must check the translation
	 * hasn't been done by someone else once we get it.
	 */
	enterTranslator();

	lockMutex(meth->class->centry);

	if (METHOD_TRANSLATED(meth)) {
		goto done1;
	}

	if (Kaffe_JavaVMArgs[0].enableVerboseJIT) {
		tms = currentTime();
	}

// DBG( vm_jit_translate, ("callinfo = 0x%x\n", &cinfo));

	/* If this code block is native, then just set it up and return */
	if ((meth->accflags & ACC_NATIVE) != 0) {
		success = native(meth, einfo);
		if (success == true) {
			KAFFEJIT_TO_NATIVE(meth);
			/* Note that this is a real function not a trampoline.*/
			if (meth->c.ncode.ncode_end == 0) {
				meth->c.ncode.ncode_end = METHOD_NATIVECODE(meth);
			}
		}
		goto done2;
	}

	/* Handle null calls specially */
	if (meth->c.bcode.codelen == 1 && meth->c.bcode.code[0] == RETURN) {
		SET_METHOD_NATIVECODE(meth, (nativecode*)nullCall);
		goto done1;
	}

	/* Scan the code and determine the basic blocks */
        success = verifyMethod(meth, &codeInfo, einfo);
	if (success == false) {
		goto done2;
	}

	assert(reinvoke == false);
	reinvoke = true;

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

SUSE(
	printf("Method: %s.%s%s\n", CLASS_CNAME(meth->class), meth->name->data, makeStrFromSignature(meth->sig));
	for (i = 0; i < maxLocal; i++) {
		printf(" L%d: %2d", i, codeInfo->localuse[i].use);
	}
	printf("\n");
)

	base = (bytecode*)meth->c.bcode.code;
	len = meth->c.bcode.codelen;

	willcatch.ANY = false;
	willcatch.BADARRAYINDEX = false;

	/* Deterimine various exception conditions */
	if (meth->exception_table != 0) {
		willCatch(ANY);
		for (i = 0; i < (int)meth->exception_table->length; i++) {
			Hjava_lang_Class* etype;
			etype = meth->exception_table->entry[i].catch_type;
			if (etype == 0) {
				willCatch(BADARRAYINDEX);
			}
			else {
				if (instanceof(&javaLangArrayIndexOutOfBoundsException, etype)) {
					willCatch(BADARRAYINDEX);
				}
			}
		}
	}

	/*
	 * Initialise the translator.
	 */
	initInsnSequence(codeperbytecode * meth->c.bcode.codelen, meth->localsz, meth->stacksz);

	/***************************************/
	/* Next reduce bytecode to native code */
	/***************************************/

	pc = 0;
	start_function();
	check_stack_limit();
	if (Kaffe_JavaVMArgs[0].enableVerboseCall != 0) {
		softcall_trace(meth);
	}
	monitor_enter();
	if (IS_STARTOFBASICBLOCK(0)) {
		end_basic_block();
		generateInsnSequence(codeInfo);
		start_basic_block();
	}

	for (; pc < len; pc = npc) {

		assert(stackno <= maxStack+maxLocal);

		npc = pc + insnLen[base[pc]];

		start_instruction();

		/* Note start of exception handling blocks */
		if (IS_STARTOFEXCEPTION(pc)) {
			stackno = meth->localsz + meth->stacksz - 1;
			start_exception_block();
		}

		switch (base[pc]) {
		default:
// DBG(jit,		dprintf("Unknown bytecode %d\n", base[pc]); )
			postException(einfo, JAVA_LANG(VerifyError));
			goto done1;
			break;
#include "kaffe.def"
		}

		/* Note maximum number of temp slots used and reset it */
		if (tmpslot > maxTemp) {
			maxTemp = tmpslot;
		}
		tmpslot = 0;

SCHK(		sanityCheck();					)

		if (IS_STARTOFBASICBLOCK(npc)) {
			end_basic_block();
			generateInsnSequence(codeInfo);
			start_basic_block();
			stackno = STACKPOINTER(npc);
SCHK(			sanityCheck();				)
		}
	}

	finish_function();
	makeFakeCalls();

	assert(maxTemp < MAXTEMPS);

	finishInsnSequence(codeInfo, &ncode, einfo);
	installMethodCode(codeInfo, meth, &ncode);

done:;
	tidyVerifyMethod(codeInfo);

	reinvoke = false;

// DBG( vm_jit_translate, ("Translating %s.%s%s (%s) %p\n", meth->class->name->data, meth->name->data, makeStrFromSignature(meth->sig), isStatic ? "static" : "normal", METHOD_NATIVECODE(meth)));

	if (Kaffe_JavaVMArgs[0].enableVerboseJIT) {
		tme = currentTime();
		jitStats.time += (int)(tme - tms);
		printf("<JIT: %s.%s%s time %dms (%dms) @ %p (%p)>\n",
		       CLASS_CNAME(meth->class),
		       meth->name->data, meth->signature->data,
		       (int)(tme - tms), jitStats.time,
		       METHOD_NATIVECODE(meth), meth);
	}

done1:;
	unlockMutex(meth->class->centry);
done2:;
	leaveTranslator();

	// MSR_STOP( jit_time);
}


/*
 * Generate the code.
 */
bool
finishInsnSequence(codeinfo* codeInfo, nativeCodeInfo* code, errorInfo *einfo)
{
#if defined(CALLTARGET_ALIGNMENT)
	int align = CALLTARGET_ALIGNMENT;
#else
	int align = 0;
#endif
	uint32 constlen;
	nativecode* methblock;
	nativecode* codebase;

	/* Emit pending instructions */
	generateInsnSequence(codeInfo);

	/* Okay, put this into malloc'ed memory */
	constlen = nConst * sizeof(union _constpoolval);
	/* Allocate some padding to align codebase if so desired 
	 * NB: we assume the allocator returns at least 8-byte aligned 
	 * addresses.   XXX: this should really be gc_memalign
	 */  
	methblock = gc_malloc(constlen + CODEPC + (align ? (align - 8) : 0), GC_ALLOC_JITCODE);
	if (methblock == 0) {
		postOutOfMemory(einfo);
		return (false);
	}
	codebase = methblock + constlen;
	/* align entry point if so desired */
	if (align != 0 && (unsigned int)codebase % align != 0) {
		int pad = (align - (unsigned int)codebase % align);
		/* assert the allocator indeed returned 8 bytes aligned addrs */
		assert(pad <= align - 8);	
		codebase = (void*)codebase + pad;
	}
	memcpy(codebase, codeblock, CODEPC);
	addToCounter(&jitcodeblock, "jitmem-codeblock", 1,
		-(jlong)GCSIZEOF(codeblock));
	gc_free(codeblock);

	/* Establish any code constants */
	establishConstants(methblock);

	/* Link it */
	linkLabels(codeInfo, (uintp)codebase);

	/* Note info on the compiled code for later installation */
	code->mem = methblock;
	code->memlen = constlen + CODEPC;
	code->code = codebase;
	code->codelen = CODEPC;
	return (true);
}

/*
 * Install the compiled code in the method.
 */
void
installMethodCode(codeinfo* codeInfo, Method* meth, nativeCodeInfo* code)
{
	int i;
	jexceptionEntry* e;

	/* Work out new estimate of code per bytecode */
	code_generated += code->memlen;
	bytecode_processed += meth->c.bcode.codelen;
	codeperbytecode = code_generated / bytecode_processed;

	/* We must free the trampoline for <clinit> methods of interfaces 
	 * before overwriting it.
	 */
	if (CLASS_IS_INTERFACE(meth->class) && utf8ConstEqual(meth->name, init_name)) {
		KFREE(METHOD_NATIVECODE(meth));
	}
	SET_METHOD_NATIVECODE(meth, code->code);
	/* Free bytecode before replacing it with native code */
	if (meth->c.bcode.code != 0) {
		KFREE(meth->c.bcode.code);
	}
	meth->c.ncode.ncode_start = code->mem;
	meth->c.ncode.ncode_end = code->code + code->codelen;

	/* Flush code out of cache */
	FLUSH_DCACHE(meth->ncode, meth->c.ncode.ncode_end);

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
			int npc, epc;
			e = &meth->exception_table->entry[i];

			epc = e->start_pc;
			for (npc = INSNPC(epc); npc == -1; npc = INSNPC(++epc));
			e->start_pc = npc + (uintp)code->code;

			epc = e->end_pc;
			for (npc = INSNPC(epc); npc == -1; npc = INSNPC(++epc));
			e->end_pc = npc + (uintp)code->code;

			epc = e->handler_pc;
			for (npc = INSNPC(epc); npc == -1; npc = INSNPC(++epc));
			e->handler_pc = npc + (uintp)code->code;
			assert(e->start_pc <= e->end_pc);
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

	/* Before generating code, try to guess how much space we'll need. */
	codeblock_size = codesize;
	if (codeblock_size < ALLOCCODEBLOCKSZ) {
		codeblock_size = ALLOCCODEBLOCKSZ;
	}
	codeblock = KMALLOC(codeblock_size + CODEBLOCKREDZONE);
	CODEPC = 0;
}

/*
 * Generate instructions from current set of sequences.
 */
static
void
generateInsnSequence(codeinfo* codeInfo)
{
	sequence* t;
	int i;
	int m;

	for (t = firstSeq; t != currSeq; t = t->next) {

		/* If we overrun the codeblock, reallocate and continue.  */
		if (CODEPC >= codeblock_size) {
			codeblock_size += ALLOCCODEBLOCKSZ;
			codeblock = KREALLOC(codeblock, codeblock_size + CODEBLOCKREDZONE);
		}

SCHK(		sanityCheck();					)

		/* Generate sequences */
		assert(t->func != 0);
		if (t->refed != 0) {
			(*(t->func))(t);
		}
		else {
			/* printf("discard instruction\n"); */
		}

		/* Handle dead slots */
		m = t->lastuse;
		if (m != 0) {
			for (i = 0; m != 0; m = m >> 1, i++) {
				if ((m & 1) != 0) {
					assert(!isGlobal(t->u[i].slot));
					slot_invalidate(t->u[i].slot);
				}
			}
		}
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

void
doSpill(sequence* s)
{
	SlotData** mem;
	SlotData* sd;
	int type;
	int old_ro;

SCHK(	sanityCheck();						)

	type = s->u[2].value.i;

	old_ro = enable_readonce;
	if (type == SR_SYNC) {
		enable_readonce = 0;
	}

	/* Spill the registers */
	for (mem = s->u[1].smask; *mem != 0; mem++) {
		sd = *mem;

		/* Determine if we need to spill this slot at all */
		if ((sd->modified & rwrite) != 0 && sd->regno != NOREG) {
			switch (type) {
			case SR_BASIC:
			case SR_SUBBASIC:
				/* We only spill if it's not global */
				if (!isGlobal(sd)) {
					spillAndUpdate(sd, true);
				}
				break;

			case SR_SYNC:
				spillAndUpdate(sd, false);
				break;

			case SR_FUNCTION:
				if (calleeSave(sd->regno) == 0 || canCatch(ANY) != 0) {
					spillAndUpdate(sd, true);
				}
				break;

			default:
				ABORT();
			}
		}
	}

	/* If this isn't being done for a function call we can free the
	 * spill mask now.  Otherwise it will be freed by the function
	 * reloader.
	 */
	if (type != SR_FUNCTION) {
		gc_free(s->u[1].smask);
	}

	enable_readonce = old_ro;

SCHK(	sanityCheck();						)
}

void
doReload(sequence* s)
{
	SlotData* sd;
	SlotData** mem;
	int type;

SCHK(	sanityCheck();						)

	type = s->u[2].value.i;

	for (mem = s->u[1].smask; *mem != 0; mem++) {
		sd = *mem;
		if (sd->regno != NOREG && !isGlobal(sd)) {
			switch (type) {
			case SR_BASIC:
			case SR_SUBBASIC:
				slot_invalidate(sd);
				break;

			case SR_FUNCTION:
				if (calleeSave(sd->regno) == 0) {
					slot_invalidate(sd);
				}
				break;

			case SR_START:
			case SR_EXCEPTION:
				break;

			default:
				ABORT();
			}
		}
	}

	/* Deal with global reloading */
	for (mem = s->u[1].smask; *mem != 0; mem++) {
		sd = *mem;
		if (isGlobal(sd)) {
			switch (type) {
			case SR_BASIC:
			case SR_SUBBASIC:
				if (!isGlobalReadonly(sd)) {
					sd->modified = rwrite;
				}
				break;

			case SR_FUNCTION:
				break;

			case SR_START:
				if (isGlobalPreload(sd)) {
					reload(sd);
				}
				break;

			case SR_EXCEPTION:
				reload(sd);
				break;

			default:
				ABORT();
			}
		}
	}

	gc_free(s->u[1].smask);

SCHK(	sanityCheck();						)
}

/*
 * Create the spill/reload mask.
 * This contains a list of slot/global pairs indicating which slots are
 * active and should be spilled or reloaded now.  The global flag indicates
 * that at this point the slot is global (ie. carried between basic blocks)
 * so should be handled with care.
 */
SlotData**
createSpillMask(void)
{
	SlotData** mem;
	SlotData* d;
	int i;
	int c;

	/* Count the number of slots we need to remember.  Note: we
	 * include the number of potentially active temps.
	 */
	i = maxLocal + maxStack + tmpslot;
	c = 0;
	for (i--; i >= 0; i--) {
		d = slotinfo[i].slot;
		if (d->rseq != 0 || d->wseq != 0 || isGlobal(d)) {
			c++;
		}
	}
#if defined(STACK_LIMIT)
	d = stack_limit->slot;
	if (d->rseq != 0 || d->wseq != 0) {
		c++;
	}
#endif

	c++; /* Add null slot on the end */
	mem = KMALLOC(c * sizeof(SlotData*));

	i = maxLocal + maxStack + tmpslot;
	c = 0;
	for (i--; i >= 0; i--) {
		d = slotinfo[i].slot;
		if (d->rseq != 0 || d->wseq != 0 || isGlobal(d)) {
			mem[c++] = d;
		}
	}
#if defined(STACK_LIMIT)
	d = stack_limit->slot;
	if (d->rseq != 0 || d->wseq != 0) {
		mem[c++] = d;
	}
#endif

	return (mem);
}

/*
 * Alias one slot's register to reference another.
 */
void
slotAlias(sequence* s)
{
	int reg;
	SlotData* to;
	SlotData* from;
	int type;

SCHK(	sanityCheck();						)

	to = s->u[0].slot;
	type = s->u[1].value.i;
	from = s->u[2].slot;

	/* If this slot has a register we must invalidate it before we
	 * overwrite it.
	 */
	if (to->regno != NOREG) {
		/* If it has the register already then don't do anything */
		if (from->regno == to->regno) {
			return;
		}
		assert((reginfo[to->regno].type & Rglobal) == 0);
		slot_invalidate(to);
	}

	/* Get the register we're aliasing and attach the 'to' slot
	 * to it.
	 */
	reg = slotRegister(from, type, rread, NOREG);
	reginfo[reg].refs++;
	to->regno = reg;
	to->modified = rwrite;
	/* Prepend slot onto register use lists */
	to->rnext = reginfo[reg].slot;
	reginfo[reg].slot = to;

SCHK(	sanityCheck();						)
}

/*
 * Setup global registers
 */
void
setupGlobalRegisters(void)
{
#if defined(NR_GLOBALS)
	int i;
	int j;
	int k;
	localUse* lcl;
	int use;
	struct {
		localUse* lcl;
		SlotData* slot;
		boolean arg;
	} tmp[NR_GLOBALS];

	/* If we don't have any code info we can't do any global
	 * optimization
	 */
	if (codeInfo == 0) {
		return;
	}

	for (j = 0; j < NR_GLOBALS; j++) {
		tmp[j].lcl = 0;
		tmp[j].slot = 0;
	}

	for (i = 0; i < maxLocal; i++) {

		assert(localinfo[i].slot->global == GL_NOGLOBAL);

		lcl = &codeInfo->localuse[i];
		if (lcl->type != TINT && lcl->type != TOBJ) {
			continue;
		}

		/* Look for a global which has worse usage than this one */
		k = -1;
		use = 0x7FFFFFFF;
		for (j = 0; j < NR_GLOBALS && use > 0; j++) {
			if (tmp[j].slot == 0) {
				use = 0;
				k = j;
			}
			else if (tmp[j].lcl->use < use) {
				use = tmp[j].lcl->use;
				k = j;
			}
		}
		/* If we got a global and it's better, use it */
		if (lcl->use > use) {
			tmp[k].slot = localinfo[i].slot;
			tmp[k].lcl = lcl;
			tmp[k].arg = (i < maxArgs ? true : false);
		}
	}

	for (i = 0, j = 0; i < MAXREG && tmp[j].slot != 0; i++) {
		if ((reginfo[i].flags & Rglobal) != 0) {
			assert(tmp[j].slot->rnext == 0);
			assert(reginfo[i].slot == 0);
			reginfo[i].ctype = reginfo[i].type & (Rref|Rint);
			reginfo[i].type |= Rglobal;
			reginfo[i].refs++;
			reginfo[i].slot = tmp[j].slot;
			tmp[j].slot->regno = i;

			/* If this is an argument then pre-load */
			if (tmp[j].arg == true) {
				tmp[j].slot->global = GL_PRELOAD;
			}
			/* Otherwise there's no need */
			else {
				tmp[j].slot->global = GL_NOLOAD;
			}
			/* If this slot is never written note that the
			 * global is read-only.
			 */
			if (tmp[j].lcl->write == -1) {
				tmp[j].slot->global |= GL_RONLY;
			}
			j++;
		}
	}
#endif
}

/*
 * Build multi-dimensional array.
 *  This is sufficiently different from the standard
 *  to require our own routine.
 */
void*
jit_soft_multianewarray(Hjava_lang_Class* class, jint dims, ...)
{
	int array[16];
	Hjava_lang_Object* obj;
	jint arg;
	int i;
	int* arraydims;
	va_list ap;

        if (dims < 16) {
		arraydims = array;
	}
	else {
		arraydims = KCALLOC(dims+1, sizeof(int));
	}

	/* Extract the dimensions into an array */
	va_start(ap, dims);
	for (i = 0; i < dims; i++) {
		arg = va_arg(ap, jint);
		if (arg < 0) {
                        throwException(NegativeArraySizeException);
		}
		arraydims[i] = arg;
	}
	arraydims[i] = -1;
	va_end(ap);

	/* Mmm, okay now build the array using the wonders of recursion */
        obj = newMultiArray(class, arraydims);

	if (arraydims != array) {
		gc_free(arraydims);
	}

	/* Return the base object */
	return (obj);
}

/*
 * return what engine we're using
 */
char*
getEngine()
{
	return "kaffe.jit";
}

static
void
nullCall(void)
{
}

typedef struct _fakeCall {
	struct _fakeCall*	next;
	label*			from;
	label*			to;
	void*			func;
} fakeCall;

static fakeCall* firstFake;
static fakeCall* lastFake;
static fakeCall* currFake;

/*
 * Build a fake call to a function.
 *  A fake call has a return address which is different from where it's
 *  actually called from.
 */
label*
newFakeCall(void* func, uintp currpc)
{
	label* from;
	label* to;

	/* This is used to mark where I'm calling from */
	from = reference_code_label(currpc);

	/* This is where I'm calling to */
	to = newLabel();
	to->type = Linternal;
	to->at = 0;
	to->to = 0;
	to->from = 0;

	if (currFake == 0) {
		currFake = KMALLOC(sizeof(fakeCall));
		if (lastFake == 0) {
			firstFake = currFake;
		}
		else {
			lastFake->next = currFake;
		}
		lastFake = currFake;
	}

	currFake->from = from;
	currFake->to = to;
	currFake->func = func;

	currFake = currFake->next;

	return (to);
}

static
void
makeFakeCalls(void)
{
	fakeCall* c;

	for (c = firstFake; c != currFake; c = c->next) {
		softcall_fakecall(c->from, c->to, c->func);
	}

	currFake = firstFake;
}

#if defined(KAFFE_PROFILER)
static jlong click_multiplier;
static profiler_click_t click_divisor;

static int
profilerClassStat(Hjava_lang_Class *clazz, void *param)
{
	Method *meth;
	int mindex;

	for (mindex = clazz->nmethods, meth = clazz->methods; mindex-- > 0; meth++) {
		if (meth->callsCount == 0)
			continue;

		fprintf(stderr, "%10d %10.6g %10.6g %10.6g %s.%s%s\n",
				meth->callsCount,
				(click_multiplier * ((double)meth->totalClicks)) / click_divisor,
				(click_multiplier * ((double)meth->totalChildrenClicks)) / click_divisor,
				(click_multiplier * ((double)meth->jitClicks)) / click_divisor,

				meth->class->name->data,
				meth->name->data,
				meth->signature->data
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

	fprintf(stderr, "# clockTick: %lld per 1/%lld seconds aka %lld per milliseconds\n",
		click_divisor,
		click_multiplier,
		click_divisor / click_multiplier);

	fprintf(stderr, "# count\ttotal\tchildren\tjit\tmethod-name\n");

	/* Traverse through class hash table */
	walkClassPool(profilerClassStat, NULL);
}
#endif
