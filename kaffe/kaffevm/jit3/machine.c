/* machine.c
 * Translate the Kaffe instruction set to the native one.
 *
 * Copyright (c) 1996-1999
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * Copyright (c) 2003, 2004
 *	Kaffe.org contributors. See ChangeLog for details. All rights reserved.
 *  
 * Cross-language profiling changes contributed by
 * the Flux Research Group, Department of Computer Science,
 * University of Utah, http://www.cs.utah.edu/flux/
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

#define SCHK(s)
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
#include "methodCache.h"
#include "support.h"
#include "xprofiler.h"
#if defined(KAFFE_FEEDBACK)
#include "feedback.h"
#endif
#include "debugFile.h"
#include "fileSections.h"
#include "mangle.h"
#include "jvmpi_kaffe.h"
#include "kaffe_jni.h"
#include "native-wrapper.h"
#include "stats.h"

const char* engine_name = "Just-in-time v3";

/**
 * top of the operand stack. localsz <= stackno < localsz+stacksz
 */
int stackno;
int maxStack;
int maxLocal;
int maxTemp;
int maxArgs;
int maxPush;
int isStatic;
codeinfo* codeInfo;

/**
 * index of first unused element of tempinfo (next available temp slot)
 */
int tmpslot;
int argcount = 0;		/* Function call argument count */
uint32 pc;
uint32 npc;

/* Various exception related things */
jitflags willcatch;

/* jit3 specific prototypes from icode.c */
void explicit_check_null (int x, SlotInfo* obj, int y);
void check_null (int x, SlotInfo* obj, int y);
void check_div (int x, SlotInfo* obj, int y);
void check_div_long (int x, SlotInfo* obj, int y);
void softcall_fakecall (label* from,label* to, void* func);

/* Unit in which code block is increased when overrun */
#define	ALLOCCODEBLOCKSZ	8192
/* Codeblock redzone - allows for safe overrun when generating instructions */
#define	CODEBLOCKREDZONE	256

static uint codeblock_size;
static int code_generated;
static int bytecode_processed;
static int codeperbytecode;
iStaticLock	translatorlock;

struct {
	int time;
} jitStats;

static jboolean generateInsnSequence(errorInfo*);

/**
 * Look for exception handlers that enclose the given PC in the given method.
 * If a match is found, it will set the appropriate willcatch flags.
 *
 * @param meth The method that may contain an exception handler.
 * @param pc The location within the method to look for a handler.
 */
static void checkCaughtExceptions(Method* meth, uint32 _pc);

void initFakeCalls(void);
static void makeFakeCalls(void);
static void relinkFakeCalls(void);

/* Desktop edition */
#include "debug.h"

Method *globalMethod;

#if defined(KAFFE_PROFILER)
int profFlag;

static void printProfilerStats(void);
#endif

/*
 * Translate a method into native code.
 *
 * Registers are allocated per basic block, using an LRU algorithm.
 * Contents of registers are spilled at the end of basic block,
 * depending on the edges in the CFG leaving the basic block:
 *
 * - If there is an edge from the basic block to an exception handler,
 *   local variables are spilled on the stack
 *
 * - If there is only one non-exception edge, and the target basic
 *   block is following the current block immediately, no spills are done
 *
 * - Otherwise, the local variables and the operand stack are spilled
 *   onto the stack
 */
jboolean
translate(Method* xmeth, errorInfo* einfo)
{
#if defined(KAFFE_VMDEBUG)
	int i;
#endif /* defined(KAFFE_VMDEBUG) */

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
	codeinfo* mycodeInfo;

	nativeCodeInfo ncode;

	int64 tms = 0;
	int64 tme;

	static bool reinvoke = false;

	jboolean success = true;

	lockClass(xmeth->class);

	if (METHOD_TRANSLATED(xmeth)) {
		goto done3;
	}

	/* If this code block is native, then just set it up and return */
	if ((xmeth->accflags & ACC_NATIVE) != 0) {
		void *func = native(xmeth, einfo);
		if (func != NULL) {
			engine_create_wrapper(xmeth, func);
			KAFFEJIT_TO_NATIVE(xmeth);
		} else {
			success = false;
		}
		goto done3;
	}

	/* Scan the code and determine the basic blocks */
	success = analyzeMethod(xmeth, &mycodeInfo, einfo);
	if (success == false) {
		goto done3;
	}

#if defined(KAFFE_FEEDBACK)
	if( kaffe_feedback_file )
		lockMutex(kaffe_feedback_file);
#endif
	/* Only one in the translator at once. Must check the translation
	 * hasn't been done by someone else once we get it.
	 */
	enterTranslator();

	startTiming(&fulljit, "JIT translation");

	if (Kaffe_JavaVMArgs.enableVerboseJIT) {
		tms = currentTime();
	}

DBG(MOREJIT,
    dprintf("callinfo = %p\n", &cinfo);
    );

#if defined(KAFFE_PROFILER)
	if (profFlag) {
		static int init = 0;

		if (!init) {
			atexit(printProfilerStats);
			init = 1;
		}

		profiler_get_clicks(xmeth->jitClicks);
		xmeth->callsCount = 0;
		xmeth->totalClicks = 0;
		xmeth->totalChildrenClicks = 0;
	}
#endif
	globalMethod = xmeth;

	codeInfo = mycodeInfo;

	/* Handle null calls specially */
	if (METHOD_BYTECODE_LEN(xmeth) == 1 && METHOD_BYTECODE_CODE(xmeth)[0] == RETURN) {
		SET_METHOD_NATIVECODE(xmeth, (nativecode*)soft_null_call);
		goto done;
	}

	assert(reinvoke == false);
	reinvoke = true;

	maxLocal = xmeth->localsz;
	maxStack = xmeth->stacksz;
        maxArgs = sizeofSigMethod(xmeth, false);
	if (maxArgs == -1) {
		goto done;
	}
	if (xmeth->accflags & ACC_STATIC) {
		isStatic = 1;
	}
	else {
		isStatic = 0;
		maxArgs += 1;
	}

DBG(MOREJIT,
	dprintf("Method: %s.%s%s\n", CLASS_CNAME(xmeth->class), xmeth->name->data, METHOD_SIGD(xmeth));
	for (i = 0; i < maxLocal; i++) {
		dprintf(" L%d: %2d", i, codeInfo->localuse[i].use);
	}
	dprintf("\n");
);

	base = (bytecode*)METHOD_BYTECODE_CODE(xmeth);
	len = METHOD_BYTECODE_LEN(xmeth);

	/*
	 * Initialise the translator.
	 */
	initFakeCalls();
	success = initInsnSequence(xmeth, codeperbytecode * METHOD_BYTECODE_LEN(xmeth), xmeth->localsz, xmeth->stacksz, einfo);
	if (success == false) {
		goto done;
	}

	/***************************************/
	/* Next reduce bytecode to native code */
	/***************************************/

	pc = 0;
	start_function();
	check_stack_limit();
	if (Kaffe_JavaVMArgs.enableVerboseCall != 0) {
		softcall_trace(xmeth);
	}
	monitor_enter();
	if (IS_STARTOFBASICBLOCK(0)) {
		end_basic_block();
		success = generateInsnSequence(einfo);
		if (success == false) {
			goto done;
		}
		start_basic_block();
	}

	for (; pc < len; pc = npc) {

		assert(stackno <= maxStack+maxLocal);
		assert(stackno >= 0);

		npc = pc + insnLen[base[pc]];

                /* Skip over the generation of any unreachable basic blocks */
                if (IS_UNREACHABLE(pc)) {
                        while (npc < len && !IS_STARTOFBASICBLOCK(npc) && !IS_STARTOFEXCEPTION(npc)) {
                                npc = npc + insnLen[base[npc]];
                        }
DBG(JIT,                dprintf("unreachable basic block pc [%d:%d]\n", pc, npc - 1);   );
                        if (IS_STARTOFBASICBLOCK(npc)) {
                                end_basic_block();
                                start_basic_block();
                                stackno = STACKPOINTER(npc);
                        }
                        continue;
                }

		/* Determine various exception conditions */
		checkCaughtExceptions(xmeth, pc);

		start_instruction();

		/* Note start of exception handling blocks */
		if (IS_STARTOFEXCEPTION(pc)) {
			stackno = xmeth->localsz + xmeth->stacksz - 1;
			start_exception_block();
		}

		switch (base[pc]) {
		default:
			printf("Unknown bytecode %d\n", base[pc]);
			leaveTranslator();
#if defined(KAFFE_FEEDBACK)
			if( kaffe_feedback_file )
				unlockMutex(kaffe_feedback_file);
#endif
			unlockClass(xmeth->class);
			postException(einfo, JAVA_LANG(VerifyError));
                        success = false;
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
			success = generateInsnSequence(einfo);
			if (success == false) {
				goto done;
			}
			start_basic_block();
			stackno = STACKPOINTER(npc);
SCHK(			sanityCheck();				)
		}
	}

	end_function();
	makeFakeCalls();

	assert(maxTemp < MAXTEMPS);

	if( finishInsnSequence(NULL, &ncode, einfo) )
	{
		installMethodCode(NULL, xmeth, &ncode);
	}
	else
	{
		success = false;
	}

done:;
	KaffeJIT3_resetLabels();
	KaffeJIT3_resetConstants();
	tidyAnalyzeMethod(&codeInfo);

	reinvoke = false;

#if defined(KAFFE_PROFILER)
	if (profFlag) {
		profiler_click_t end;

		profiler_get_clicks(end);
		xmeth->jitClicks = end - xmeth->jitClicks;
	}
#endif
	globalMethod = NULL;

DBG(MOREJIT,
    dprintf("Translating %s.%s%s (%s) %p\n",
	    xmeth->class->name->data,
	    xmeth->name->data,
	    METHOD_SIGD(xmeth),
	    isStatic ? "static" : "normal", METHOD_NATIVECODE(xmeth));
    );
	
	if (Kaffe_JavaVMArgs.enableVerboseJIT) {
		tme = currentTime();
		jitStats.time += (int)(tme - tms);
		printf("<JIT: %s.%s%s time %dms (%dms) @ %p (%p)>\n",
		       CLASS_CNAME(xmeth->class),
		       xmeth->name->data, METHOD_SIGD(xmeth),
		       (int)(tme - tms), jitStats.time,
		       METHOD_NATIVECODE(xmeth), xmeth);
	}

	stopTiming(&fulljit);

	leaveTranslator();
#if defined(KAFFE_FEEDBACK)
	if( kaffe_feedback_file )
		unlockMutex(kaffe_feedback_file);
#endif
done3:;
	unlockClass(xmeth->class);

#if defined(ENABLE_JVMPI)
	if (success && JVMPI_EVENT_ISENABLED(JVMPI_EVENT_COMPILED_METHOD_LOAD) )
	  {
	    JVMPI_Event ev;

	    jvmpiFillMethodLoad(&ev, xmeth);
	    jvmpiPostEvent(&ev);
	  }
#endif

	return (success);
}

/*
 * Generate the code.
 */
jboolean
finishInsnSequence(void* dummy UNUSED, nativeCodeInfo* code, errorInfo* einfo)
{
	uint32 constlen;
	jitCodeHeader *jch;
	nativecode* methblock;
	jboolean success;
	uintp const_align = sizeof(union _constpoolval) - 1;

	/* Emit pending instructions */
	success = generateInsnSequence(einfo);
	if (success == false) {
		return (false);
	}

	relinkFakeCalls();

	/* Okay, put this into malloc'ed memory. We allocate some more
	 * memory for alignment purpose. */
	constlen = KaffeJIT3_getNumberOfConstants() * sizeof(union _constpoolval); 
	methblock = gc_malloc(sizeof(jitCodeHeader) +
			      constlen + const_align +
			      CODEPC,
			      KGC_ALLOC_JITCODE);
	if (methblock == 0) {
		postOutOfMemory(einfo);
		return (false);
	}

	jch = (jitCodeHeader *)methblock;
	jch->pool = (void *)(jch + 1);
	jch->pool = (void *) ( ((unsigned long)(jch->pool) + const_align) & ~const_align);
	jch->code_start = ((nativecode *)jch->pool) + constlen;
	jch->code_len = CODEPC;
	memcpy(jch->code_start, codeblock, jch->code_len);
	gc_free(codeblock);

	/* Establish any code constants */
	KaffeJIT3_establishConstants(jch->pool);

	/* Link it */
	KaffeJIT3_linkLabels((uintp)jch->code_start);

	/* Note info on the compiled code for later installation */
	code->mem = methblock;
	code->memlen = constlen + CODEPC;
	code->code = jch->code_start;
	code->codelen = CODEPC;

	return (true);
}

/*
 * Get the instruction offset corresponding to the given PC.
 * If the PC doesn't point at the start of a valid instruction,
 * look forward until we find one. If we reach the end of the
 * method, we return code->codelen
 */
static
int
getInsnPC(int _pc, codeinfo* _codeInfo, nativeCodeInfo *code)
{
	int maxPc = _codeInfo->codelen;

	while (_pc < maxPc) {
		if (IS_STARTOFINSTRUCTION(_pc)) {
			int res = INSNPC(_pc);
			if (res != -1) {
				return (res);
			}
		}
		_pc ++;
	}
	return code->codelen;
}

/*
 * Install the compiled code in the method.
 * Returns true if successful
 */
void
installMethodCode(void* ignore UNUSED, Method* meth, nativeCodeInfo* code)
{
	uint32 i;
	jexceptionEntry* e;
	jitCodeHeader *jch;
	void *tramp;

#if defined(KAFFE_XPROFILER) || defined(KAFFE_XDEBUGGING)
	struct mangled_method *mm = 0;
#endif
#if defined(KAFFE_FEEDBACK)
	char *sym = 0;
#endif

	/* Work out new estimate of code per bytecode */
	code_generated += code->memlen;
	bytecode_processed += METHOD_BYTECODE_LEN(meth);

	/* When using GCJ, the only use for the translator may be to output
	 * JNI wrappers which have zero bytecode ;-); hence the test
	 */
	if (bytecode_processed > 0) {
		codeperbytecode = code_generated / bytecode_processed;
	}
	//KGC_WRITE(meth, code->mem);

	jch = (jitCodeHeader *)code->mem;
	jch->method = meth;

	tramp = METHOD_NATIVECODE(meth);

	SET_METHOD_JITCODE(meth, code->code);

	meth->c.ncode.ncode_start = code->mem;
	meth->c.ncode.ncode_end = (void*)((uintp)code->code + code->codelen);
	
#if defined(KAFFE_FEEDBACK)
	if( kaffe_feedback_file && !meth->class->loader )
	{
		sym = gc_malloc(strlen(CLASS_CNAME(meth->class)) +
				1 + /* '/' */
				strlen(meth->name->data) +
				strlen(METHOD_SIGD(meth)) +
				1,
				KGC_ALLOC_JITTEMP);
		sprintf(sym,
			"%s/%s%s",
			CLASS_CNAME(meth->class),
			meth->name->data,
			METHOD_SIGD(meth));
		feedbackJITMethod(sym, code->code, code->codelen, true);
		gc_free(sym);
	}
#endif
#if defined(KAFFE_XPROFILER) || defined(KAFFE_XDEBUGGING)
	if( (
#if defined(KAFFE_XPROFILER)
	     xProfFlag ||
#else
	     0 ||
#endif
#if defined(KAFFE_XDEBUGGING)
	     machine_debug_file
#else
	    0
#endif
	     ) &&
	    (mm = createMangledMethod()) )
	{
		mangleMethod(mm, meth);
	}
#endif
#if defined(KAFFE_XPROFILER)
	profileFunction(mm, code->code, code->codelen);
#endif

	/* Flush code out of cache */
#if defined(FLUSH_DCACHE)
	FLUSH_DCACHE(code->code, (void*)((uintp)code->code + code->codelen));
#endif

	/* Translate exception table and make it available */
	if (meth->exception_table != 0) {
		for (i = 0; i < meth->exception_table->length; i++) {
			e = &meth->exception_table->entry[i];
			e->start_pc = getInsnPC(e->start_pc, codeInfo, code) + (uintp)code->code;
			e->end_pc = getInsnPC(e->end_pc, codeInfo, code) + (uintp)code->code;
			e->handler_pc = getInsnPC(e->handler_pc, codeInfo, code) + (uintp)code->code;
			assert (e->start_pc <= e->end_pc);
		}
	}

	/* Translate line numbers table */
	if (meth->lines != 0) {
#if defined(KAFFE_XDEBUGGING)
		struct debug_file *df = machine_debug_file;

		/* Mark the start of this source file */
		char *sourcefile = meth->class->sourcefile;
		if (! sourcefile)
			sourcefile = "Unknown source file";
		addDebugInfo(df,
			     DIA_IncludeFile,
			     meth->class->name->data,
			     meth->class->packageLength,
			     sourcefile,
			     code->code,

			     DIA_Function,
			     meth, mm, meth->lines->entry[0].line_nr,
			     code->code, code->codelen,

			     DIA_DONE);
#endif
		for (i = 0; i < meth->lines->length; i++) {
			meth->lines->entry[i].start_pc = getInsnPC(meth->lines->entry[i].start_pc, codeInfo, code) + (uintp)code->code;
#if defined(KAFFE_XDEBUGGING)
			/* Add line debugging */
			addDebugInfo(df,
				     DIA_SourceLine,
				     meth->lines->entry[i].line_nr,
				     meth->lines->entry[i].start_pc -
				     (uintp)code->code,
				     DIA_DONE);
#endif
		}
		if( meth->lvars != NULL ) {
			localVariableEntry *lve;
			
			for (i = 0; i < meth->lvars->length; i++) {
				lve = &meth->lvars->entry[i];
				lve->start_pc =
					(uintp)code->code +
					getInsnPC(lve->start_pc,
						  codeInfo,
						  code);
#if defined(KAFFE_XDEBUGGING) && defined(SLOT2LOCALOFFSET)
				if( df != NULL ) {
					struct Hjava_lang_Class *cl;
					const char *vname, *vsig;
					errorInfo einfo;

					vname = CONST_UTF2CHAR(
						lve->name_index,
						CLASS_CONSTANTS(meth->class));
					vsig = CONST_UTF2CHAR(
						lve->descriptor_index,
						CLASS_CONSTANTS(meth->class));
					cl = getClassFromSignature(
						vsig,
						meth->class->loader,
						&einfo);
					if( cl == NULL )
					{
						discardErrorInfo(&einfo);
					}
					/* Add line debugging */
					addDebugInfo(df,
						     lve->index < maxArgs ?
						     DIA_Parameter :
						     DIA_LocalVariable,
						     vname,
						     cl,
						     local(lve->index)->slot->offset,
						     DIA_DONE);
				}
#endif
			}
#if 0
			addDebugInfo(df,
				     DIA_LeftBrace, lve->start_pc,
				     DIA_RightBrace, (lve->start_pc +
						      lve->length),
				     DIA_DONE);
#endif
		}
#if defined(KAFFE_XDEBUGGING)
		/*
		 * Mark the end of the function.  This needs to be here so that
		 * gdb doesn't get confused about the range of the function
		 * since that will be determined by the next debugging
		 * information that is added.
		 */
		addDebugInfo(df,
			     DIA_EndFunction, code->code + code->codelen,
			     DIA_DONE);
#endif
	}
	else
	{
#if defined(KAFFE_XDEBUGGING)
		/*
		 * No line debugging, but we'd like a symbol to show up anyways
		 */
                char *sourcefile = meth->class->sourcefile;
                if (! sourcefile)
                        sourcefile = "Unknown source file";
		addDebugInfo(
			machine_debug_file,
			DIA_IncludeFile,
			meth->class->name->data,
			meth->class->packageLength,
			sourcefile,
			code->code,
			
			DIA_Function, meth, mm, 0, code->code, code->codelen,
			DIA_EndFunction, code->code + code->codelen,
			DIA_DONE);
#endif
	}
#if defined(KAFFE_XPROFILER) || defined(KAFFE_XDEBUGGING)
	deleteMangledMethod(mm);
#endif
	
	/* record framesize for gcj unwinding information */
#if defined(LABEL_Lframe)
	LABEL_Lframe(&meth->framesize, /* unused */ 0, /* unused */ 0);
#endif
}

/*
 * Init instruction generation.
 */
jboolean
initInsnSequence(Method* meth, int codesize UNUSED, int localsz, int stacksz, errorInfo* einfo)
{
	/* Clear various counters */
	tmpslot = 0;
	maxTemp = 0;
	maxPush = 0;
	stackno = localsz + stacksz;
	npc = 0;

	/* Do any machine dependent JIT initialization */
#if defined(INIT_JIT_MD)
	INIT_JIT_MD(meth);
#endif

	initSeq();
	initRegisters();
	initSlots(stackno);

	/* Before generating code, try to guess how much space we'll need. */
	codeblock_size = ALLOCCODEBLOCKSZ;
	codeblock = gc_malloc(codeblock_size + CODEBLOCKREDZONE,
			      KGC_ALLOC_JIT_CODEBLOCK);
	if (codeblock == 0) {
		postOutOfMemory(einfo);
		return (false);
	}
	CODEPC = 0;
	
	return (true);
}

/*
 * Generate instructions from current set of sequences.
 */
static
jboolean
generateInsnSequence(errorInfo* einfo)
{
	sequence* t;
	int i;
	int m;

	for (t = firstSeq; t != currSeq; t = t->next) {

		/* If we overrun the codeblock, reallocate and continue.  */
		if (CODEPC >= codeblock_size) {
			nativecode *new_codeblock;
			
			codeblock_size += ALLOCCODEBLOCKSZ;
			new_codeblock = gc_realloc(codeblock,
						   codeblock_size +
						   CODEBLOCKREDZONE,
						   KGC_ALLOC_JIT_CODEBLOCK);
			if (new_codeblock == NULL) {
				gc_free(codeblock);
				codeblock = NULL;
				postOutOfMemory(einfo);
				return (false);
			} else {
				codeblock = new_codeblock;
			}
		}

SCHK(		sanityCheck();					);

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
					slot_kill_readonce(t->u[i].slot);
					slot_invalidate(t->u[i].slot);
				}
			}
		}
	}

	/* Reset */
	initSeq();

	return (true);
}

/*
 * check what synchronous exceptions are caught for a given instruction
 */
static
void 
checkCaughtExceptions(Method* meth, uint32 _pc)
{
	unsigned int i;

	willcatch.ANY = false;
	willcatch.BADARRAYINDEX = false;
	willcatch.NULLPOINTER = false;

	if (meth->exception_table == 0) 
		return;

	/* Determine various exception conditions */
	for (i = 0; i < meth->exception_table->length; i++) {
		Hjava_lang_Class* etype;

		/* include only if exception handler range matches pc */
		if (meth->exception_table->entry[i].start_pc > _pc ||
		    meth->exception_table->entry[i].end_pc <= _pc)
			continue;

		willCatch(ANY);
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
				if (calleeSave(sd->regno) == 0 || s->jflags.ANY != 0) {
					spillAndUpdate(sd, true);
				}
				break;

			default:
				KAFFEVM_ABORT();
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
				KAFFEVM_ABORT();
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
				KAFFEVM_ABORT();
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
	mem = gc_malloc(c * sizeof(SlotData*), KGC_ALLOC_JIT_SLOTS);

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

	if (reginfo[from->regno].flags & Rreadonce) {
		/*
		 * XXX Aggressively spill the floating point register on x86.
		 * Otherwise it might not get spilled out at all.
		 */
		spillAndUpdate(from, true);
	}
	
	/* If this slot has a register we must invalidate it before we
	 * overwrite it.
	 */
	if (to->regno != NOREG) {
		/* If it has the register already then don't do anything */
		if (from->regno == to->regno) {
			return;
		}
		assert(isGlobal(to) == 0);
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

void
setupArgumentRegisters(void)
{
#if defined(NR_ARGUMENTS)
	int args;
	int i;
	static sequence argseq[1];

	args = maxArgs;
	if (args > NR_ARGUMENTS) {
		args = NR_ARGUMENTS;
	}

	for (i = 0; i < args; i++) {
		writeslot(argseq, i, &localinfo[i], 1);
		localinfo[i].slot->modified = rwrite;
	}
#if defined(STACK_LIMIT)
	if (args < NR_ARGUMENTS) {
		writeslot(argseq, i, stack_limit, 1);
		stack_limit->slot->modified = rwrite;
	}
#endif
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
	errorInfo einfo;
	int array[16];
	Hjava_lang_Object* obj;
	jint arg;
	int i;
	int* arraydims;
	va_list ap;

        if (dims < 16-1) {
		arraydims = array;
	}
	else {
		arraydims = checkPtr(gc_calloc(dims+1, sizeof(int), KGC_ALLOC_JITTEMP));
	}

	/* Extract the dimensions into an array */
	va_start(ap, dims);
	for (i = 0; i < dims; i++) {
		arg = va_arg(ap, jint);
		if (arg < 0) {
			if (arraydims != array) {
				gc_free (arraydims);
			}
                        throwException(NegativeArraySizeException);
		}
		arraydims[i] = arg;
	}
	arraydims[i] = -1;
	va_end(ap);

	/* Mmm, okay now build the array using the wonders of recursion */
        obj = newMultiArrayChecked(class, arraydims, &einfo);

	if (arraydims != array) {
		gc_free(arraydims);
	}

	if (!obj) {
		throwError(&einfo);
	}

	/* Return the base object */
	return (obj);
}

/*
 * Include the machine specific trampoline functions.
 */
typedef struct _fakeCall {
	struct _fakeCall*	next;
	struct _fakeCall*	parent;
	label*			from;
	label*			to;
	void*			func;
} fakeCall;

/* XXX These are globally shared */
static fakeCall* firstFake;
static fakeCall** lastFake = &firstFake;
static fakeCall* fakePool;
static fakeCall* redundantFake;

void
initFakeCalls(void)
{
	*lastFake = redundantFake;
	redundantFake = NULL;
	fakePool = firstFake;
	firstFake = NULL;
	lastFake = &firstFake;
}

#if defined(HAVE_branch_and_link)
static
fakeCall *
findFakeCall(void *func)
{
	fakeCall *fc, *retval = NULL;

	for( fc = firstFake; fc && !retval; fc = fc->next )
	{
		if( fc->func == func )
			retval = fc;
	}
	return( retval );
}
#endif /* defined(HAVE_branch_and_link) */

/*
 * Build a fake call to a function.
 *  A fake call has a return address which is different from where it's
 *  actually called from.
 */
label*
newFakeCall(void* func, uintp currpc)
{
	fakeCall *fc;
	label* from;
	label* to;

	/* This is used to mark where I'm calling from */
	from = reference_code_label(currpc);

	/* This is where I'm calling to */
	to = KaffeJIT3_newLabel();
	to->type = Linternal;
	to->at = 0;
	to->to = 0;
	to->from = 0;

	/* XXX This isn't safe, but noone checks the return value :( */
	if( fakePool )
	{
		fc = fakePool;
		fakePool = fakePool->next;
		fc->next = NULL;
	}
	else
	{
		fc = KGC_malloc(main_collector,
				sizeof(fakeCall),
				KGC_ALLOC_JIT_FAKE_CALL);
	}
#if defined(HAVE_branch_and_link)
	fc->parent = findFakeCall(func);
#endif
	fc->from = from;
	fc->to = to;
	fc->func = func;
	if( fc->parent != NULL )
	{
		fc->next = redundantFake;
		redundantFake = fc;
	}
	else
	{
		*lastFake = fc;
		lastFake = &fc->next;
	}
	return (to);
}

static
void
makeFakeCalls(void)
{
	fakeCall* c;

	for (c = firstFake; c; c = c->next) {
		softcall_fakecall(c->from, c->to, c->func);
	}
}

static
void relinkFakeCalls(void)
{
	fakeCall *fc;
	
	for( fc = redundantFake; fc; fc = fc->next )
	{
		fc->to->to = fc->parent->to->to;
		fc->to->type = fc->parent->to->type;
		
		fc->from->at = fc->parent->from->at;
		fc->from->type = fc->parent->from->type;

		assert(fc->to->from != 0);
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

	for (mindex = CLASS_NMETHODS(clazz), meth = CLASS_METHODS(clazz); mindex-- > 0; meth++) {
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
