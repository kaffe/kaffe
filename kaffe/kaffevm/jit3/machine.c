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

char* engine_name = "Just-in-time v3";
char* engine_version = KAFFEVERSION;

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
void	finishInsnSequence(nativeCodeInfo*);
static void generateInsnSequence(void);
static void installMethodCode(Method*, nativeCodeInfo*);
static void pushContext(void);
static void popContext(void);
static void nullCall(void);

jlong	currentTime(void);
Method*	findMethodFromPC(uintp);
label* newFakeCall(void*, uintp);
static void makeFakeCalls(void);


/*
 * Translate a method into native code.
 */
static
void
translate(Method* meth)
{
	int i;

	jint low;
	jint high;
	jvalue tmpl;
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

	int64 tms;
	int64 tme;

	int iLockRoot;

	static iLock* translatorlock;
	static bool reinvoke = false;

	// MSR_START( jit_time);

	/* Since we get here by calling the trampoline, we must be about
	 * to call this method - so make sure it's OK before generating
	 * code to avoid any extra initialization calls.
	 */
	processClass(meth->class, CSTATE_COMPLETE);

	/* Only one in the translator at once. Must check the translation
	 * hasn't been done by someone else once we get it.
	 */
	lockStaticMutex(&translatorlock);

	lockMutex(meth->class->centry);

	if (METHOD_TRANSLATED(meth)) {
		unlockMutex(meth->class->centry);
		unlockStaticMutex(&translatorlock);
		return;
	}

	if (Kaffe_JavaVMArgs[0].enableVerboseJIT) {
		tms = currentTime();
	}

// DBG( vm_jit_translate, ("callinfo = 0x%x\n", &cinfo));

	/* If this code block is native, then just set it up and return */
	if ((meth->accflags & ACC_NATIVE) != 0) {
		native(meth);
		KAFFEJIT_TO_NATIVE(meth);
		unlockMutex(meth->class->centry);
		unlockStaticMutex(&translatorlock);
		return;
	}

	/* Handle null calls specially */
	if (meth->bcode->codelen == 1 && meth->bcode->code[0] == RETURN) {
		SET_METHOD_NATIVECODE(meth, (nativecode*)nullCall);
		unlockMutex(meth->class->centry);
		unlockStaticMutex(&translatorlock);
		return;
	}

	/* Save current JIT context */
	pushContext();

	/* Scan the code and determine the basic blocks */
	verifyMethod(meth);

	assert(reinvoke == false);
	reinvoke = true;

	maxLocal = meth->localsz;
	maxStack = meth->stacksz;
	maxArgs = 0;
	for (i = 0; i < meth->sig->len; i++) {
		switch (meth->sig->elem[i]->data[0]) {
		case 'D':
		case 'J':
			maxArgs += 2;
			break;
		default:
			maxArgs += 1;
		}
	}

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

	base = (bytecode*)meth->bcode->code;
	len = meth->bcode->codelen;

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
	initInsnSequence(codeperbytecode * meth->bcode->codelen, meth->localsz, meth->stacksz);

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
		generateInsnSequence();
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
			kprintf("Unknown bytecode %d\n", base[pc]);
			unlockMutex(meth->class->centry);
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

SCHK(		sanityCheck();					)

		if (IS_STARTOFBASICBLOCK(npc)) {
			end_basic_block();
			generateInsnSequence();
			start_basic_block();
			stackno = STACKPOINTER(npc);
SCHK(			sanityCheck();				)
		}
	}

	finish_function();
	makeFakeCalls();

	assert(maxTemp < MAXTEMPS);

	finishInsnSequence(&ncode);
	installMethodCode(meth, &ncode);

	tidyVerifyMethod();

	reinvoke = false;
	/* Restore current JIT context */
	popContext();

// DBG( vm_jit_translate, ("Translating %s.%s%s (%s) %p\n", meth->class->name->data, meth->name->data, makeStrFromSignature(meth->sig), isStatic ? "static" : "normal", METHOD_NATIVECODE(meth)));

	if (Kaffe_JavaVMArgs[0].enableVerboseJIT) {
		tme = currentTime();
		jitStats.time += (int)(tme - tms);
		kprintf("<JIT: %s.%s%s time %dms (%dms) @ %p (%p)>\n",
		       CLASS_CNAME(meth->class),
		       meth->name->data, makeStrFromSignature(meth->sig),
		       (int)(tme - tms), jitStats.time,
		       METHOD_NATIVECODE(meth), meth);
	}

	unlockMutex(meth->class->centry);
	unlockStaticMutex(&translatorlock);

	// MSR_STOP( jit_time);
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
	nativeInfo* info;

	/* Emit pending instructions */
	generateInsnSequence();

	/* Okay, put this into malloc'ed memory */
	constlen = nConst * sizeof(union _constpoolval);
	methblock = KMALLOC(constlen + CODEPC + sizeof(nativeInfo));
	info = (nativeInfo*)(methblock + constlen);
	codebase = ((nativecode*)info) + sizeof(nativeInfo);
	memcpy(codebase, codeblock, CODEPC);
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

	/* Fill in the info block */
	info->ncode_start = methblock;
	info->ncode_end = codebase + CODEPC;
}

/*
 * Get the instruction offset corresponding to the given PC.
 * If the PC doesn't point at the start of a valid instruction,
 * look forward until we find one.
 */
static
int
getInsnPC(int pc)
{
	int res;

	for (;;) {
		res = INSNPC(pc);
		if (res != -1) {
			return (res);
		}
		pc++;
	}
}

/*
 * Install the compiled code in the method.
 */
static
void
installMethodCode(Method* meth, nativeCodeInfo* code)
{
	uint32 i;
	jexceptionEntry* e;

	/* Work out new estimate of code per bytecode */
	code_generated += code->memlen;
	bytecode_processed += meth->bcode->codelen;
	codeperbytecode = code_generated / bytecode_processed;

	GC_WRITE(meth, code->mem);
	SET_METHOD_NATIVECODE(meth, code->code);
#if 0
	meth->c.ncode.ncode_start = code->mem;
	meth->c.ncode.ncode_end = (void*)((uintp)code->code + code->codelen);
#endif
        // MSR_ADD(ncode_mem, code->codelen);

	/* Flush code out of cache */
	FLUSH_DCACHE(meth->ncode, meth->c.ncode.ncode_end);

	/* Translate exception table and make it available */
	if (meth->exception_table != 0) {
		for (i = 0; i < meth->exception_table->length; i++) {
			e = &meth->exception_table->entry[i];
			e->start_pc = getInsnPC(e->start_pc) + (uintp)code->code;
			e->end_pc = getInsnPC(e->end_pc) + (uintp)code->code;
			e->handler_pc = getInsnPC(e->handler_pc) + (uintp)code->code;
		}
	}

	/* Translate line numbers table */
	if (meth->lines != 0) {
		for (i = 0; i < meth->lines->length; i++) {
			meth->lines->entry[i].start_pc = getInsnPC(meth->lines->entry[i].start_pc) + (uintp)code->code;
		}
	}

	/* Make method active (so we can find if for exceptions) */
	makeMethodActive(meth);
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
generateInsnSequence(void)
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
startInsn(sequence* s)
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
 * Dispatch an exception.  These odd structures are necessary
 *  to emulate the old exception calling convention (we will
 *  deprecate at some point).
 */
static
void
EcallExceptionHandler(Method* meth, uintp handler, stackTraceInfo* frame)
{
	Hjava_lang_Object* eobj;
	Hjava_lang_Thread* ct;

	ct = getCurrentThread();

	eobj = (Hjava_lang_Object*)unhand(ct)->exceptObj;
	unhand(ct)->exceptObj = 0;

	CALL_KAFFE_EXCEPTION(frame->arg0, handler, eobj);
}

/* 
 * Trampolines come in here - do the translation and replace the trampoline.
 */
nativecode*
soft_fixup_trampoline(FIXUP_TRAMPOLINE_DECL)
{
	Method* meth;
	void** where;
	void* tramp;

	FIXUP_TRAMPOLINE_INIT;

	/* Find the trampoline so we can free it later if necessary */
	tramp = *where;

	/* If this class needs initializing, do it now.  */
	processClass(meth->class, CSTATE_COMPLETE);

	/* Generate code on demand.  */
	if (!METHOD_TRANSLATED(meth)) {
		translate(meth);
		// DBG( vm_jit, ("%x : %s:%s%s\n", METHOD_NATIVECODE(meth), meth->class->name->data, meth->name->data, makeStrFromSignature(meth->sig)));
		/* Free the trampoline */
		gc_free(tramp);
	}

	/* return (METHOD_NATIVECODE(meth)) doesn't work with egcs */
  	*where = METHOD_NATIVECODE(meth);
	return (*where);
}

/*
 * Include the machine specific trampoline functions.
 */
#include "trampolines.c"

static
void
EprocessExceptionMethod(stackTraceInfo* info, stackFrame* fm)
{
	if (info->meth != jniMethod) {
		info->object = (void*)FRAMEOBJECT(fm);
		info->arg0 = (uintp)fm->return_frame;
	}
	else {
		info->object = 0;
		info->arg0 = (uintp)unhand(getCurrentThread())->exceptPtr;
	}
}

/*
 * Call inter-machine method
 */
void
EcallInterEngine(callMethodInfo* call)
{
	sysdepCallMethod(call);
}

/*
 * Build an interface between the JIT and a JNI method.
 */
static
void
EsetupJNIcall(Method* xmeth, void* func)
{
	char buf[100];
	int i;
	methodSig* sig;
	int count;
	nativeCodeInfo ncode;
	SlotInfo* tmp;

	/* Convert the signature into a simple string of types, and
	 * count the size of the arguments too.
	 */
	sig = xmeth->sig;
	if (METHOD_IS_STATIC(xmeth)) {
		isStatic = 1;
		count = 0;
	}
	else {
		isStatic = 0;
		count = 1;
	}

	for (i = 0; i < sig->len; i++) {
		buf[i] = sig->elem[i]->data[0];
		switch (buf[i]) {
		case 'D':
		case 'J':
			count++;
			break;
		default:
			break;
		}
		count++;
	}

	/* Construct a wrapper to call the JNI method with the correct
	 * arguments.
	 */
	maxArgs = count;
	maxLocal = count;
	initInsnSequence(0, xmeth->localsz + !isStatic, 0);
	prologue(0);

	/* Start a JNI call */
	call_soft(startJNIcall);

#if defined(NEED_JNIREFS)
	/* Make the necesary JNI ref calls first */
	if (!METHOD_IS_STATIC(xmeth)) {
		pusharg_ref(local(0), 0);
		begin_func_sync();
		call_soft(addJNIref);
		popargs();
		end_func_sync();
	}
	j = i;
	jcount = count;
	while (j > 0) {
		j--;
		jcount--;
		if (buf[j] == '[' || buf[j] == 'L') {
			pusharg_ref(local(jcount), 0);
			begin_func_sync();
			call_soft(addJNIref);
			popargs();
			end_func_sync();
		}
		if (buf[j] == 'J' || buf[j] == 'D') {
			jcount--;
		}
	}
	start_sub_block();
#endif

	/* Add synchronisation if necessary */
	mon_enter(xmeth, local(0));

	/* Push the specified arguments */
	while (i > 0) {
		i--;
		count--;
		switch (buf[i]) {
		case '[':
		case 'L':
			pusharg_ref(local(count), count+1+isStatic);
			break;
		case 'I':
		case 'Z':
		case 'S':
		case 'B':
		case 'C':
			pusharg_int(local(count), count+1+isStatic);
			break;
		case 'F':
			pusharg_float(local(count), count+1+isStatic);
			break;
		case 'J':
			count--;
			pusharg_long(local(count), count+1+isStatic);
			break;
		case 'D':
			count--;
			pusharg_double(local(count), count+1+isStatic);
			break;
		default:
			ABORT();
		}
	}

	/* If static, push the class, else push the object */
	if (METHOD_IS_STATIC(xmeth)) {
		pusharg_ref_const(xmeth->class, 1);
	}
	else {
		pusharg_ref(local(0), 1);
	}

	/* Push the JNI info */
	pusharg_ref_const((void*)&Kaffe_JNIEnv, 0);

	/* Make the call */
	begin_func_sync();
	call_soft(func);
	popargs();
	end_func_sync();

	/* Determine return type */
	switch (sig->ret->data[0]) {
	case 'I':
	case 'Z':
	case 'S':
	case 'B':
	case 'C':
		slot_alloctmp(tmp);
		return_int(tmp);
		/* Remove synchronisation if necessary */
		if (xmeth->accflags & ACC_SYNCHRONISED) {
			mon_exit(xmeth, local(0));
		}
		begin_func_sync();
		call_soft(finishJNIcall);
		end_func_sync();
		returnarg_int(tmp);
		slot_freetmp(tmp);
		break;
	case 'F':
		slot_alloctmp(tmp);
		return_float(tmp);
		/* Remove synchronisation if necessary */
		if (xmeth->accflags & ACC_SYNCHRONISED) {
			mon_exit(xmeth, local(0));
		}
		begin_func_sync();
		call_soft(finishJNIcall);
		end_func_sync();
		returnarg_float(tmp);
		slot_freetmp(tmp);
		break;
	case 'J':
		slot_alloc2tmp(tmp);
		return_long(tmp);
		/* Remove synchronisation if necessary */
		if (xmeth->accflags & ACC_SYNCHRONISED) {
			mon_exit(xmeth, local(0));
		}
		begin_func_sync();
		call_soft(finishJNIcall);
		end_func_sync();
		returnarg_long(tmp);
		slot_free2tmp(tmp);
		break;
	case 'D':
		slot_alloc2tmp(tmp);
		return_double(tmp);
		/* Remove synchronisation if necessary */
		if (xmeth->accflags & ACC_SYNCHRONISED) {
			mon_exit(xmeth, local(0));
		}
		begin_func_sync();
		call_soft(finishJNIcall);
		end_func_sync();
		returnarg_double(tmp);
		slot_free2tmp(tmp);
		break;
	case 'V':
		/* Remove synchronisation if necessary */
		if (xmeth->accflags & ACC_SYNCHRONISED) {
			mon_exit(xmeth, local(0));
		}
		call_soft(finishJNIcall);
		break;
	case 'L':
	case '[':
		slot_alloctmp(tmp);
		return_ref(tmp);
		/* Remove synchronisation if necessary */
		if (xmeth->accflags & ACC_SYNCHRONISED) {
			mon_exit(xmeth, local(0));
		}
		begin_func_sync();
		call_soft(finishJNIcall);
		end_func_sync();
		returnarg_ref(tmp);
		slot_freetmp(tmp);
		break;
	default:
		ABORT();
	}

	epilogue(0);
	ret();

	/* Generate the code */
	if (tmpslot > maxTemp) {
		maxTemp = tmpslot;
	}
	finishInsnSequence(&ncode);

	/* Install it */
	SET_METHOD_NATIVECODE(xmeth, ncode.code);
#if 0
	xmeth->c.ncode.ncode_start = ncode.mem;
	xmeth->c.ncode.ncode_end = (void*)((uintp)ncode.code + ncode.codelen);
#endif
	xmeth->accflags |= ACC_JNI;
        // MSR_ADD(ncode_mem, ncode.codelen);

	/* Flush code out of cache */
	FLUSH_DCACHE(ncode.code, (void*)((uintp)ncode.code + ncode.codelen));
}

typedef struct JitContext {
	struct JitContext*	next;
	codeinfo*		codeInfo;
} JitContext;

static JitContext* lastContext;

/*
 * Push JIT context.
 */
static
void
pushContext(void)
{
	JitContext* nc;

	nc = KMALLOC(sizeof(JitContext));
	nc->codeInfo = codeInfo;

	nc->next = lastContext;
	lastContext = nc;

	codeInfo = 0;
}

static
void
popContext(void)
{
	JitContext* nc;

	nc = lastContext;
	lastContext = nc->next;

	codeInfo = nc->codeInfo;

	gc_free(nc);
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
