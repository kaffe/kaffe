/* code-analyse.c
 * Analyse a method's bytecodes.
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#define	CDBG(s)
#define	DBG(s)
#define	FDBG(s)
#define	VDBG(s) s

#include "config.h"
#include "config-std.h"
#include "gtypes.h"
#include "bytecode.h"
#include "object.h"
#include "constants.h"
#include "access.h"
#include "classMethod.h"
#include "code-analyse.h"
#include "lookup.h"
#include "exception.h"
#include "icode.h"
#include "itypes.h"
#include "locks.h"
#include "md.h"
#include "gc.h"

codeinfo* codeInfo;

const uint8 insnLen[256] = {
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
	2, 3, 2, 3, 3, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
	1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
	1, 1, 1, 1, 3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
	1, 1, 1, 1, 1, 1, 1, 1, 1, 3, 3, 3, 3, 3, 3, 3, 
	3, 3, 3, 3, 3, 3, 3, 3, 3, 2, 0, 0, 1, 1, 1, 1, 
	1, 1, 3, 3, 3, 3, 3, 3, 3, 5, 1, 3, 2, 3, 1, 1, 
	3, 3, 1, 1, 1, 4, 3, 3, 5, 5, 1, 1, 1, 1, 1, 1, 
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
};

static void mergeFrame(int, int, frameElement*, Method*);
static bool verifyBasicBlock(Method*, int32);
/*
 * Note that verifyMethod and tidyVerifyMethod must be atomic since 
 * they both rely on codeInfo.  Therefore, we grab the vlock in verifyMethod
 * and release it in tidyVerifyMethod.
 *
 * Note that technically, vlock is only needed for the interpreter since
 * the translator has an supersetting translator lock.
 */
static iLock vlock;

void
verifyMethod(Method* meth)
{
	int32 pc;
	int32 tabpc;
	int32 idx;
	int32 sp;
	int32 lcl;
	char* sig;
	perPCInfo* bhead;
	perPCInfo* btail;
	perPCInfo* bcurr;
	bool rerun;
	bool failed;
	bool wide;
	static bool init = false;

	/* Initialise on first use */
	if (init == false) {
		init = true;
		initStaticLock(&vlock);
	}

	lockStaticMutex(&vlock);

	/* We don't need to do this twice */
	meth->accflags |= ACC_VERIFIED;

CDBG(	dprintf("verifyMethod: %s.%s, codeInfo = 0x%x\n", 
	meth->class->name->data, meth->name->data, codeInfo);)

	assert (codeInfo == 0 && " Attempt to reenter verifier!");
	codeInfo = gc_malloc_fixed(sizeof(codeinfo) + (meth->c.bcode.codelen * sizeof(perPCInfo)));

CDBG(	dprintf(__FUNCTION__"codeInfo = 0x%x\n", codeInfo);		)

	/* Allocate code info. block */
	codeInfo->localsz = meth->localsz;
	codeInfo->stacksz = meth->stacksz;
	codeInfo->codelen = meth->c.bcode.codelen;

	/* First basic block becomes head of block chain */
	SET_NEEDVERIFY(0);
	bhead = &codeInfo->perPC[0];
	btail = bhead;

	/* Scan the code and mark the beginning of basic blocks */
	wide = false;
	for (pc = 0; pc < codeInfo->codelen;) {
		SET_STARTOFINSTRUCTION(pc);
		switch (INSN(pc)) {
		case IFEQ:	case IFNE:	case IFLT:
		case IFGE:	case IFGT:	case IFLE:
		case IF_ICMPEQ: case IF_ICMPNE:
		case IF_ICMPLT: case IF_ICMPGE:
		case IF_ICMPGT: case IF_ICMPLE:
		case IF_ACMPEQ: case IF_ACMPNE:
		case IFNULL:	case IFNONNULL:
			tabpc = pc + WORD(pc+1);
			SET_STARTOFBASICBLOCK(tabpc);
			SET_JUMPFLOW(pc, tabpc);
			pc = pc + INSNLEN(pc);
			SET_STARTOFBASICBLOCK(pc);
			SET_NORMALFLOW(pc);
			break;
		case GOTO:
			tabpc = pc + WORD(pc+1);
			SET_STARTOFBASICBLOCK(tabpc);
			SET_JUMPFLOW(pc, tabpc);
			pc = pc + INSNLEN(pc);
			break;
		case GOTO_W:
			tabpc = pc + DWORD(pc+1);
			SET_STARTOFBASICBLOCK(tabpc);
			SET_JUMPFLOW(pc, tabpc);
			pc = pc + INSNLEN(pc);
			break;
		case JSR:
			tabpc = pc + WORD(pc+1);
			SET_STARTOFBASICBLOCK(tabpc);
			SET_JUMPFLOW(pc, tabpc);
			pc = pc + INSNLEN(pc);
			SET_STARTOFBASICBLOCK(pc);
			SET_NORMALFLOW(pc);
			break;
		case JSR_W:
			tabpc = pc + DWORD(pc+1);
			SET_STARTOFBASICBLOCK(tabpc);
			SET_JUMPFLOW(pc, tabpc);
			pc = pc + INSNLEN(pc);
			SET_STARTOFBASICBLOCK(pc);
			SET_NORMALFLOW(pc);
			break;
		case TABLESWITCH:
			tabpc = (pc + 4) & -4;
			idx = DWORD(tabpc+8)-DWORD(tabpc+4)+1;
			for (; idx > 0; idx--) {
				SET_STARTOFBASICBLOCK(pc+DWORD(tabpc+idx*4+8));
				SET_JUMPFLOW(pc, pc+DWORD(tabpc+idx*4+8));
			}
			SET_STARTOFBASICBLOCK(pc+DWORD(tabpc));
			SET_JUMPFLOW(pc, pc+DWORD(tabpc));
			pc = tabpc + (DWORD(tabpc+8)-DWORD(tabpc+4)+1+3) * 4;
			break;
		case LOOKUPSWITCH:
			tabpc = (pc + 4) & -4;
			idx = DWORD(tabpc+4);
			for (; idx > 0; idx--) {
				SET_STARTOFBASICBLOCK(pc+DWORD(tabpc+idx*8+4));
				SET_JUMPFLOW(pc, pc+DWORD(tabpc+idx*8+4));
			}
			SET_STARTOFBASICBLOCK(pc+DWORD(tabpc));
			SET_JUMPFLOW(pc, pc+DWORD(tabpc));
			pc = tabpc + (DWORD(tabpc+4)+1) * 8;
			break;
		case IRETURN:	case LRETURN:	case ARETURN:
		case FRETURN:	case DRETURN:	case RETURN:
		case ATHROW:	case RET:
			pc = pc + INSNLEN(pc);
			break;
		case WIDE:
			wide = true;
			pc = pc + INSNLEN(pc);
			SET_NORMALFLOW(pc);
			break;
		case ILOAD:	case LLOAD:	case FLOAD:
		case DLOAD:	case ALOAD:
		case ISTORE:	case LSTORE:	case FSTORE:
		case DSTORE:	case ASTORE:
			pc = pc + INSNLEN(pc);
			if (wide == true) {
				wide = false;
				pc += 1;
			}
			SET_NORMALFLOW(pc);
			break;
		case IINC:
			pc = pc + INSNLEN(pc);
			if (wide == true) {
				wide = false;
				pc += 2;
			}
			SET_NORMALFLOW(pc);
			break;
		default:
			/* The default */
			pc = pc + INSNLEN(pc);
			SET_NORMALFLOW(pc);
			break;
		}
	}

	/* Setup exception info. */
	sp = meth->localsz + meth->stacksz - 1;
	if (meth->exception_table != 0) {
		for (lcl = 0; lcl < meth->exception_table->length; lcl++) {
			pc = meth->exception_table->entry[lcl].handler_pc;
			SET_STARTOFEXCEPTION(pc);
			SET_STACKPOINTER(pc, sp);
			SET_NEWFRAME(pc);
			STACKINIT(0, TOBJ);
			ATTACH_NEW_BASICBLOCK(pc);
		}
	}

	/* Mark the various starting states.  These include the main
	 * entry point plus all the exception entry points, their arguments
	 * and stack values.
	 */
	pc = 0;
	SET_STACKPOINTER(pc, meth->localsz + meth->stacksz);
	SET_NEWFRAME(pc);

	/* Parse the method signature to setup the inital locals
	 */
	idx = 0;
	if ((meth->accflags & ACC_STATIC) == 0) {
		LOCALINIT(0, TOBJ);
		idx++;
	}
	sig = meth->signature->data;
	assert(sig[0] == '(');
	sig++;
	while (sig[0] != ')') {
		switch (sig[0]) {
		case '[':
			LOCALINIT(idx, TOBJ);
			idx += 1;
			while (sig[0] == '[') {
				sig++;
			}
			if (sig[0] == 'L') {
				while (sig[0] != ';') {
					sig++;
				}
			}
			sig++;
			break;
		case 'L':
			LOCALINIT(idx, TOBJ);
			idx += 1;
			while (sig[0] != ';') {
				sig++;
			}
			sig++;
			break;
		case 'I':
		case 'Z':
		case 'S':
		case 'B':
		case 'C':
			LOCALINIT(idx, TINT);
			idx += 1;
			sig++;
			break;
		case 'J':
			LOCALINIT(idx, TLONG);
			LOCALINIT(idx+1, TVOID);
			idx += 2;
			sig++;
			break;
		case 'F':
			LOCALINIT(idx, TFLOAT);
			idx += 1;
			sig++;
			break;
		case 'D':
			LOCALINIT(idx, TDOUBLE);
			LOCALINIT(idx+1, TVOID);
			idx += 2;
			sig++;
			break;
		default:
			assert("Signature character unknown" == 0);
		}
	}

	/* Scan out list of basic blocks.  Unfortunately they're not in
	 * precise order so we have to do this until they're all done.
	 */
	do {
		rerun = false;
		for (bcurr = bhead; bcurr != NULL; bcurr = bcurr->nextBB) {
			pc = bcurr - codeInfo->perPC;
			if (IS_NEEDVERIFY(pc)) {
				failed = verifyBasicBlock(meth, pc);
#ifdef notyet
				if (failed) {
					unlockStaticMutex(&vlock);
					throwException(VerifyError);
				}
#endif
				rerun = true;
			}
		}
	} while (rerun == true);

	/* Check we've processed each block at least once */
	for (bcurr = bhead; bcurr != NULL; bcurr = bcurr->nextBB) {
		assert((bcurr->flags & FLAG_DONEVERIFY) != 0);
	}
}

static
bool
verifyBasicBlock(Method* meth, int32 pc)
{
	int32 tabpc;
	int32 idx;
	int32 sp;
	int32 lcl;
	int32 lclw;
	int32 lclww;
	int32 opc;
	callInfo call;
	fieldInfo finfo;
	Hjava_lang_Class* type;
	char* sig;
	Hjava_lang_Class* class;
	frameElement* activeFrame;
	bool wide;
	bool failed;

	opc = pc;
	assert(pc == 0 || IS_STARTOFBASICBLOCK(pc) || IS_STARTOFEXCEPTION(pc));
	assert(IS_STACKPOINTERSET(pc));

	SET_DONEVERIFY(pc);

	/* Get stack pointer */
	sp = STACKPOINTER(pc);

	/* Allocate frame to hold type data */
	activeFrame = ALLOCFRAME();
	assert(activeFrame != 0);
	FRAMELOAD(pc);

	/* Process basic block until we get to the beginning of a new one */
	wide = false;
	failed = false;
	do {

		if (sp < meth->localsz || sp > meth->localsz + meth->stacksz) {
			failed = true;
			VDBG(printf("sp out of range: %d <%d> %d\n", meth->localsz, sp, meth->localsz + meth->stacksz);)
			break;
		}

		/* If we're in an exception block, merge locals into
		 * the handler.
		 */
		if (meth->exception_table != 0) {
			for (idx = 0; idx < meth->exception_table->length; idx++) {
				if (pc >= meth->exception_table->entry[idx].start_pc && pc < meth->exception_table->entry[idx].end_pc) {
					FRAMEMERGE_LOCALS(meth->exception_table->entry[idx].handler_pc);
				}
			}
		}

		/* Retrieve a potential local slot */
		lcl = BYTE(pc+1);
		lclw = WORD(pc+1);
		lclww = DWORD(pc+1);

DBG(		printf("%d: %d\n", pc, INSN(pc));		)

		switch (INSN(pc)) {
		case NOP:
			INCPC(1);
			break;

		case ACONST_NULL:
			STKPUSH(1);
			STACKOUT(0, TOBJ);
			INCPC(1);
			break;

		case ICONST_M1:
		case ICONST_0:
		case ICONST_1:
		case ICONST_2:
		case ICONST_3:
		case ICONST_4:
		case ICONST_5:
			STKPUSH(1);
			lcl = INSN(pc) - ICONST_0;
			STACKOUT_CONST(0, TINT, lcl);
			INCPC(1);
			break;

		case BIPUSH:
			STKPUSH(1);
			STACKOUT_CONST(0, TINT, lcl);
			INCPC(2);
			break;

		case SIPUSH:
			STKPUSH(1);
			STACKOUT_CONST(0, TINT, lclw);
			INCPC(3);
			break;

		case LCONST_0:
		case LCONST_1:
			STKPUSH(2);
			STACKOUT(0, TLONG);
			STACKOUT(1, TVOID);
			INCPC(1);
			break;

		case FCONST_0:
		case FCONST_1:
		case FCONST_2:
			STKPUSH(1);
			STACKOUT(0, TFLOAT);
			INCPC(1);
			break;

		case DCONST_0:
		case DCONST_1:
			STKPUSH(2);
			STACKOUT(0, TDOUBLE);
			STACKOUT(1, TVOID);
			INCPC(1);
			break;

		case LDC1:
			STKPUSH(1);
			lcl = (uint8)lcl;
			CONSTANTTYPE(type, lcl);
			STACKOUT(0, type);
			INCPC(2);
			break;

		case LDC2:
			STKPUSH(1);
			CONSTANTTYPE(type, (uint16)lclw);
			STACKOUT(0, type);
			INCPC(3);
			break;

		case LDC2W:
			STKPUSH(2);
			CONSTANTTYPE(type, (uint16)lclw);
			STACKOUT(0, type);
			STACKOUT(1, TVOID);
			INCPC(3);
			break;

		case ILOAD_0:
		case ILOAD_1:
		case ILOAD_2:
		case ILOAD_3:
			lcl = INSN(pc) - ILOAD_0;
			STKPUSH(1);
			STACKOUT_LOCAL(0, TINT, lcl);
			INCPC(1);
			break;

		case ILOAD:
			if (wide) {
				wide = false;
				STKPUSH(1);
				STACKOUT_LOCAL(0, TINT, lclw);
				INCPC(3);
			}
			else {
				STKPUSH(1);
				STACKOUT_LOCAL(0, TINT, lcl);
				INCPC(2);
			}
			break;

		case LLOAD_0:
		case LLOAD_1:
		case LLOAD_2:
		case LLOAD_3:
			lcl = INSN(pc) - LLOAD_0;
			STKPUSH(2);
			STACKOUT_LOCAL(0, TLONG, lcl);
			STACKOUT_LOCAL(1, TVOID, lcl+1);
			INCPC(1);
			break;

		case LLOAD:
			if (wide) {
				wide = false;
				STKPUSH(2);
				STACKOUT_LOCAL(0, TLONG, lclw);
				STACKOUT_LOCAL(1, TVOID, lclw+1);
				INCPC(3);
			}
			else {
				STKPUSH(2);
				STACKOUT_LOCAL(0, TLONG, lcl);
				STACKOUT_LOCAL(1, TVOID, lcl+1);
				INCPC(2);
			}
			break;

		case FLOAD_0:
		case FLOAD_1:
		case FLOAD_2:
		case FLOAD_3:
			lcl = INSN(pc) - FLOAD_0;
			STKPUSH(1);
			STACKOUT_LOCAL(0, TFLOAT, lcl);
			INCPC(1);
			break;

		case FLOAD:
			if (wide) {
				wide = false;
				STKPUSH(1);
				STACKOUT_LOCAL(0, TFLOAT, lclw);
				INCPC(3);
			}
			else {
				STKPUSH(1);
				STACKOUT_LOCAL(0, TFLOAT, lcl);
				INCPC(2);
			}
			break;

		case DLOAD_0:
		case DLOAD_1:
		case DLOAD_2:
		case DLOAD_3:
			lcl = INSN(pc) - DLOAD_0;
			STKPUSH(2);
			STACKOUT_LOCAL(0, TDOUBLE, lcl);
			STACKOUT_LOCAL(1, TVOID, lcl+1);
			INCPC(1);
			break;

		case DLOAD:
			if (wide) {
				wide = false;
				STKPUSH(2);
				STACKOUT_LOCAL(0, TDOUBLE, lclw);
				STACKOUT_LOCAL(1, TVOID, lclw+1);
				INCPC(3);
			}
			else {
				STKPUSH(2);
				STACKOUT_LOCAL(0, TDOUBLE, lcl);
				STACKOUT_LOCAL(1, TVOID, lcl+1);
				INCPC(2);
			}
			break;

		case ALOAD_0:
		case ALOAD_1:
		case ALOAD_2:
		case ALOAD_3:
			lcl = INSN(pc) - ALOAD_0;
			STKPUSH(1);
			STACKOUT_LOCAL(0, TOBJ, lcl);
			INCPC(1);
			break;

		case ALOAD:
			if (wide) {
				wide = false;
				STKPUSH(1);
				STACKOUT_LOCAL(0, TOBJ, lclw);
				INCPC(3);
			}
			else {
				STKPUSH(1);
				STACKOUT_LOCAL(0, TOBJ, lcl);
				INCPC(2);
			}
			break;

		case IALOAD:
		case BALOAD:
		case CALOAD:
		case SALOAD:
			STACKIN(1, TOBJ);
			STACKIN(0, TINT);
			STKPOP(1);
			STACKOUT(0, TINT);
			INCPC(1);
			break;

		case LALOAD:
			STACKIN(1, TOBJ);
			STACKIN(0, TINT);
			STACKOUT(0, TLONG);
			STACKOUT(1, TVOID);
			INCPC(1);
			break;

		case FALOAD:
			STACKIN(1, TOBJ);
			STACKIN(0, TINT);
			STKPOP(1);
			STACKOUT(0, TFLOAT);
			INCPC(1);
			break;

		case DALOAD:
			STACKIN(1, TOBJ);
			STACKIN(0, TINT);
			STACKOUT(0, TDOUBLE);
			STACKOUT(1, TVOID);
			INCPC(1);
			break;

		case AALOAD:
			STACKIN(1, TOBJ);
			STACKIN(0, TINT);
			STKPOP(1);
			STACKOUT(0, TOBJ);
			INCPC(1);
			break;

		case ISTORE_0:
		case ISTORE_1:
		case ISTORE_2:
		case ISTORE_3:
			lcl = INSN(pc) - ISTORE_0;
			LOCALOUT_STACK(lcl, TINT, 0);
			STKPOP(1);
			INCPC(1);
			break;

		case ISTORE:
			if (wide) {
				LOCALOUT_STACK(lclw, TINT, 0);
				INCPC(1);
				wide = false;
			}
			else {
				LOCALOUT_STACK(lcl, TINT, 0);
			}
			STKPOP(1);
			INCPC(2);
			break;

		case LSTORE_0:
		case LSTORE_1:
		case LSTORE_2:
		case LSTORE_3:
			lcl = INSN(pc) - LSTORE_0;
			LOCALOUT_STACK(lcl, TLONG, 0);
			LOCALOUT_STACK(lcl+1, TVOID, 1);
			STKPOP(2);
			INCPC(1);
			break;

		case LSTORE:
			if (wide) {
				LOCALOUT_STACK(lclw, TLONG, 0);
				LOCALOUT_STACK(lclw+1, TVOID, 1);
				INCPC(1);
				wide = false;
			}
			else {
				LOCALOUT_STACK(lcl, TLONG, 0);
				LOCALOUT_STACK(lcl+1, TVOID, 1);
			}
			STKPOP(2);
			INCPC(2);
			break;

		case FSTORE_0:
		case FSTORE_1:
		case FSTORE_2:
		case FSTORE_3:
			lcl = INSN(pc) - FSTORE_0;
			STACKIN(0, TFLOAT);
			LOCALOUT(lcl, TFLOAT);
			STKPOP(1);
			INCPC(1);
			break;

		case FSTORE:
			STACKIN(0, TFLOAT);
			if (wide) {
				LOCALOUT(lclw, TFLOAT);
				INCPC(1);
				wide = false;
			}
			else {
				LOCALOUT(lcl, TFLOAT);
			}
			STKPOP(1);
			INCPC(2);
			break;

		case DSTORE_0:
		case DSTORE_1:
		case DSTORE_2:
		case DSTORE_3:
			lcl = INSN(pc) - DSTORE_0;
			STACKIN(0, TDOUBLE);
			STACKIN(1, TVOID);
			LOCALOUT(lcl, TDOUBLE);
			LOCALOUT(lcl+1, TVOID);
			STKPOP(2);
			INCPC(1);
			break;

		case DSTORE:
			STACKIN(0, TDOUBLE);
			STACKIN(1, TVOID);
			if (wide) {
				LOCALOUT(lclw, TDOUBLE);
				LOCALOUT(lclw+1, TVOID);
				INCPC(1);
				wide = false;
			}
			else {
				LOCALOUT(lcl, TDOUBLE);
				LOCALOUT(lcl+1, TVOID);
			}
			STKPOP(2);
			INCPC(2);
			break;

		case ASTORE_0:
		case ASTORE_1:
		case ASTORE_2:
		case ASTORE_3:
			lcl = INSN(pc) - ASTORE_0;
			LOCALOUT_STACK(lcl, TOBJ, 0);
			STKPOP(1);
			INCPC(1);
			break;

		case ASTORE:
			if (wide) {
				LOCALOUT_STACK(lclw, TOBJ, 0);
				INCPC(1);
				wide = false;
			}
			else {
				LOCALOUT_STACK(lcl, TOBJ, 0);
			}
			STKPOP(1);
			INCPC(2);
			break;

		case BASTORE:
			STACKIN(0, TINT);
			STACKIN(1, TINT);
			STACKIN(2, TOBJ);
			STKPOP(3);
			INCPC(1);
			break;

		case IASTORE:
			STACKIN(0, TINT);
			STACKIN(1, TINT);
			STACKIN(2, TOBJ);
			STKPOP(3);
			INCPC(1);
			break;

		case CASTORE:
		case SASTORE:
			STACKIN(0, TINT);
			STACKIN(1, TINT);
			STACKIN(2, TOBJ);
			STKPOP(3);
			INCPC(1);
			break;

		case LASTORE:
			STACKIN(3, TOBJ);
			STACKIN(2, TINT);
			STACKIN(0, TLONG);
			STACKIN(1, TVOID);
			STKPOP(4);
			INCPC(1);
			break;

		case FASTORE:
			STACKIN(2, TOBJ);
			STACKIN(1, TINT);
			STACKIN(0, TFLOAT);
			STKPOP(3);
			INCPC(1);
			break;

		case DASTORE:
			STACKIN(3, TOBJ);
			STACKIN(2, TINT);
			STACKIN(0, TDOUBLE);
			STACKIN(1, TVOID);
			STKPOP(4);
			INCPC(1);
			break;

		case AASTORE:
			STACKIN(2, TOBJ);
			STACKIN(1, TINT);
			STACKIN(0, TOBJ);
			STKPOP(3);
			INCPC(1);
			break;

		case POP:
			STKPOP(1);
			INCPC(1);
			break;

		case POP2:
			STKPOP(2);
			INCPC(1);
			break;

		case DUP:
			STKPUSH(1);
			STACKCOPY(1, 0);
			INCPC(1);
			break;

		case DUP_X1:
			STKPUSH(1);
			STACKCOPY(1, 0);
			STACKCOPY(2, 1);
			STACKCOPY(0, 2);
			INCPC(1);
			break;

		case DUP_X2:
			STKPUSH(1);
			STACKCOPY(1, 0);
			STACKCOPY(2, 1);
			STACKCOPY(3, 2);
			STACKCOPY(0, 3);
			INCPC(1);
			break;

		case DUP2:
			STKPUSH(2);
			STACKCOPY(2, 0);
			STACKCOPY(3, 1);
			STACKCOPY(2, 2);
			STACKCOPY(3, 3);
			INCPC(1);
			break;

		case DUP2_X1:
			STKPUSH(2);
			STACKCOPY(2, 0);
			STACKCOPY(3, 1);
			STACKCOPY(4, 2);
			STACKCOPY(0, 3);
			STACKCOPY(1, 4);
			INCPC(1);
			break;

		case DUP2_X2:
			STKPUSH(2);
			STACKCOPY(2, 0);
			STACKCOPY(3, 1);
			STACKCOPY(4, 2);
			STACKCOPY(5, 3);
			STACKCOPY(0, 4);
			STACKCOPY(1, 5);
			INCPC(1);
			break;

		case SWAP:
			STACKSWAP(0, 1);
			INCPC(1);
			break;

		case IAND:
			STACKIN(0, TINT);
			STACKIN(1, TINT);
			STKPOP(1);
			STACKOUT(0, TINT);
			INCPC(1);
			break;

		case IOR:
			STACKIN(0, TINT);
			STACKIN(1, TINT);
			STKPOP(1);
			STACKOUT(0, TINT);
			INCPC(1);
			break;

		case IXOR:
			STACKIN(0, TINT);
			STACKIN(1, TINT);
			STKPOP(1);
			STACKOUT(0, TINT);
			INCPC(1);
			break;

		case IADD:
			STACKIN(0, TINT);
			STACKIN(1, TINT);
			STKPOP(1);
			STACKOUT(0, TINT);
			INCPC(1);
			break;

		case IMUL:
			STACKIN(0, TINT);
			STACKIN(1, TINT);
			STKPOP(1);
			STACKOUT(0, TINT);
			INCPC(1);
			break;

		case ISUB:
			STACKIN(0, TINT);
			STACKIN(1, TINT);
			STKPOP(1);
			STACKOUT(0, TINT);
			INCPC(1);
			break;

		case IDIV:
			STACKIN(0, TINT);
			STACKIN(1, TINT);
			STKPOP(1);
			STACKOUT(0, TINT);
			INCPC(1);
			break;

		case IREM:
			STACKIN(0, TINT);
			STACKIN(1, TINT);
			STKPOP(1);
			STACKOUT(0, TINT);
			INCPC(1);
			break;

		case LADD:
		case LSUB:
		case LMUL:
		case LDIV:
		case LREM:
		case LAND:
		case LOR:
		case LXOR:
			STACKIN(0, TLONG);
			STACKIN(1, TVOID);
			STACKIN(2, TLONG);
			STACKIN(3, TVOID);
			STKPOP(2);
			STACKOUT(0, TLONG);
			STACKOUT(1, TVOID);
			INCPC(1);
			break;

		case FADD:
		case FSUB:
		case FMUL:
		case FDIV:
		case FREM:
			STACKIN(1, TFLOAT);
			STACKIN(0, TFLOAT);
			STKPOP(1);
			STACKOUT(0, TFLOAT);
			INCPC(1);
			break;

		case DADD:
		case DSUB:
		case DMUL:
		case DDIV:
		case DREM:
			STACKIN(2, TDOUBLE);
			STACKIN(3, TVOID);
			STACKIN(0, TDOUBLE);
			STACKIN(1, TVOID);
			STKPOP(2);
			STACKOUT(0, TDOUBLE);
			STACKOUT(1, TVOID);
			INCPC(1);
			break;

		case INEG:
			STACKIN(0, TINT);
			STACKOUT(0, TINT);
			INCPC(1);
			break;

		case LNEG:
			STACKIN(0, TLONG);
			STACKIN(1, TVOID);
			STACKOUT(0, TLONG);
			STACKOUT(1, TVOID);
			INCPC(1);
			break;

		case FNEG:
			STACKIN(0, TFLOAT);
			STACKOUT(0, TFLOAT);
			INCPC(1);
			break;

		case DNEG:
			STACKIN(0, TDOUBLE);
			STACKIN(1, TVOID);
			STACKOUT(0, TDOUBLE);
			STACKOUT(1, TVOID);
			INCPC(1);
			break;

		case ISHL:
			STACKIN(0, TINT);
			STACKIN(1, TINT);
			STKPOP(1);
			STACKOUT(0, TINT);
			INCPC(1);
			break;

		case ISHR:
			STACKIN(0, TINT);
			STACKIN(1, TINT);
			STKPOP(1);
			STACKOUT(0, TINT);
			INCPC(1);
			break;

		case IUSHR:
			STACKIN(0, TINT);
			STACKIN(1, TINT);
			STKPOP(1);
			STACKOUT(0, TINT);
			INCPC(1);
			break;

		case LSHL:
		case LSHR:
		case LUSHR:
			STACKIN(0, TINT);
			STACKIN(1, TLONG);
			STACKIN(2, TVOID);
			STKPOP(1);
			STACKOUT(0, TLONG);
			STACKOUT(1, TVOID);
			INCPC(1);
			break;

		case IINC:
			if (wide) {
				LOCALINOUT(lclw, TINT);
				INCPC(2);
				wide = false;
			}
			else {
				LOCALINOUT(lcl, TINT);
			}
			INCPC(3);
			break;

		case I2L:
			STACKIN(0, TINT);
			STKPUSH(1);
			STACKOUT(0, TLONG);
			STACKOUT(1, TVOID);
			INCPC(1);
			break;

		case I2F:
			STACKIN(0, TINT);
			STACKOUT(0, TFLOAT);
			INCPC(1);
			break;

		case I2D:
			STACKIN(0, TINT);
			STKPUSH(1);
			STACKOUT(0, TDOUBLE);
			STACKOUT(1, TVOID);
			INCPC(1);
			break;

		case L2I:
			STACKIN(0, TLONG);
			STACKIN(1, TVOID);
			STKPOP(1);
			STACKOUT(0, TINT);
			INCPC(1);
			break;

		case L2F:
			STACKIN(0, TLONG);
			STACKIN(1, TVOID);
			STKPOP(1);
			STACKOUT(0, TFLOAT);
			INCPC(1);
			break;

		case L2D:
			STACKIN(0, TLONG);
			STACKIN(1, TVOID);
			STACKOUT(0, TDOUBLE);
			STACKOUT(1, TVOID);
			INCPC(1);
			break;

		case F2I:
			STACKIN(0, TFLOAT);
			STACKOUT(0, TINT);
			INCPC(1);
			break;

		case F2L:
			STACKIN(0, TFLOAT);
			STKPUSH(1);
			STACKOUT(0, TLONG);
			STACKOUT(1, TVOID);
			INCPC(1);
			break;

		case F2D:
			STACKIN(0, TFLOAT);
			STKPUSH(1);
			STACKOUT(0, TDOUBLE);
			STACKOUT(1, TVOID);
			INCPC(1);
			break;

		case D2I:
			STACKIN(0, TDOUBLE);
			STACKIN(1, TVOID);
			STKPOP(1);
			STACKOUT(0, TINT);
			INCPC(1);
			break;

		case D2L:
			STACKIN(0, TDOUBLE);
			STACKIN(1, TVOID);
			STACKOUT(0, TLONG);
			STACKOUT(1, TVOID);
			INCPC(1);
			break;

		case D2F:
			STACKIN(0, TDOUBLE);
			STACKIN(1, TVOID);
			STKPOP(1);
			STACKOUT(0, TFLOAT);
			INCPC(1);
			break;

		case INT2BYTE:
		case INT2CHAR:
		case INT2SHORT:
			STACKIN(0, TINT);
			STACKOUT(0, TINT);
			INCPC(1);
			break;

		case LCMP:
			STACKIN(2, TLONG);
			STACKIN(3, TVOID);
			STACKIN(0, TLONG);
			STACKIN(1, TVOID);
			STKPOP(3);
			STACKOUT(0, TINT);
			INCPC(1);
			break;

		case FCMPL:
		case FCMPG:
			STACKIN(1, TFLOAT);
			STACKIN(0, TFLOAT);
			STKPOP(1);
			STACKOUT(0, TINT);
			INCPC(1);
			break;

		case DCMPL:
		case DCMPG:
			STACKIN(2, TDOUBLE);
			STACKIN(3, TVOID);
			STACKIN(0, TDOUBLE);
			STACKIN(1, TVOID);
			STKPOP(3);
			STACKOUT(0, TINT);
			INCPC(1);
			break;

		case IFEQ:
		case IFNE:
		case IFLT:
		case IFGE:
		case IFGT:
		case IFLE:
			STACKIN(0, TINT);
			STKPOP(1);
			FRAMEMERGE(pc + lclw, sp);
			FRAMEMERGE(pc + 3, sp);
			INCPC(3);
			break;

		case IF_ICMPEQ:
		case IF_ICMPNE:
		case IF_ICMPLT:
		case IF_ICMPGT:
		case IF_ICMPGE:
		case IF_ICMPLE:
			STACKIN(0, TINT);
			STACKIN(1, TINT);
			STKPOP(2);
			FRAMEMERGE(pc + lclw, sp);
			FRAMEMERGE(pc + 3, sp);
			INCPC(3);
			break;

		case IF_ACMPEQ:
		case IF_ACMPNE:
			STACKIN(0, TOBJ);
			STACKIN(1, TOBJ);
			STKPOP(2);
			FRAMEMERGE(pc + lclw, sp);
			FRAMEMERGE(pc + 3, sp);
			INCPC(3);
			break;

		case GOTO:
			FRAMEMERGE(pc + lclw, sp);
			INCPC(3);
			break;

		case GOTO_W:
			FRAMEMERGE(pc + lclww, sp);
			INCPC(5);
			break;

		case JSR:
			STKPUSH(1);
			STACKOUT(0, TADDR);
			FRAMEMERGE(pc + lclw, sp);
			STKPOP(1);
			FRAMEMERGE(pc + 3, sp);
			INCPC(3);
			break;

		case JSR_W:
			STKPUSH(1);
			STACKOUT(0, TADDR);
			FRAMEMERGE(pc + lclww, sp);
			STKPOP(1);
			FRAMEMERGE(pc + 5, sp);
			INCPC(5);
			break;

		case RET:
			LOCALIN(lcl, TADDR);
			INCPC(2);
			break;

		case LOOKUPSWITCH:
			STACKIN(0, TINT);
			STKPOP(1);
			tabpc = (pc + 4) & -4;
			idx = DWORD(tabpc+4);
			for (; idx > 0; idx--) {
				FRAMEMERGE(pc+DWORD(tabpc+idx*8+4), sp);
			}
			FRAMEMERGE(pc + DWORD(tabpc), sp);
			pc = tabpc + (DWORD(tabpc+4)+1) * 8;
			break;

		case TABLESWITCH:
			STACKIN(0, TINT);
			STKPOP(1);
			tabpc = (pc + 4) & -4;
			idx = DWORD(tabpc+8)-DWORD(tabpc+4)+1;
			for (; idx > 0; idx--) {
				FRAMEMERGE(pc+DWORD(tabpc+idx*4+8), sp);
			}
			FRAMEMERGE(pc + DWORD(tabpc), sp);
			pc = tabpc + (DWORD(tabpc+8)-DWORD(tabpc+4)+1+3) * 4;
			break;

		case IRETURN:
			STACKIN(0, TINT);
			STKPOP(1);
			INCPC(1);
			break;

		case LRETURN:
			STACKIN(0, TLONG);
			STACKIN(1, TVOID);
			STKPOP(2);
			INCPC(1);
			break;

		case FRETURN:
			STACKIN(0, TFLOAT);
			STKPOP(1);
			INCPC(1);
			break;

		case DRETURN:
			STACKIN(0, TDOUBLE);
			STACKIN(1, TVOID);
			STKPOP(2);
			INCPC(1);
			break;

		case ARETURN:
			STACKIN(0, TOBJ);
			STKPOP(1);
			INCPC(1);
			break;

		case RETURN:
			INCPC(1);
			break;

		case GETSTATIC:
			getField(lclw, meth->class, true, &finfo);
			if (FIELD_ISREF(finfo.field)) {
				STKPUSH(1);
				STACKOUT(0, TOBJ);
			}
			else switch (CLASS_PRIM_SIG(FIELD_TYPE(finfo.field))){
			case 'I':
			case 'Z':
			case 'S':
			case 'B':
			case 'C':
				STKPUSH(1);
				STACKOUT(0, TINT);
				break;
			case 'F':
				STKPUSH(1);
				STACKOUT(0, TFLOAT);
				break;
			case 'J':
				STKPUSH(2);
				STACKOUT(0, TLONG);
				STACKOUT(1, TVOID);
				break;
			case 'D':
				STKPUSH(2);
				STACKOUT(0, TDOUBLE);
				STACKOUT(1, TVOID);
				break;
			default:
				ABORT();
				break;
			}
			INCPC(3);
			break;

		case PUTSTATIC:
			getField(lclw, meth->class, true, &finfo);
			if (FIELD_ISREF(finfo.field)) {
				STACKIN(0, TOBJ);
				STKPOP(1);
			}
			else switch (CLASS_PRIM_SIG(FIELD_TYPE(finfo.field))){
			case 'I':
			case 'Z':
			case 'S':
			case 'B':
			case 'C':
				STACKIN(0, TINT);
				STKPOP(1);
				break;
			case 'F':
				STACKIN(0, TFLOAT);
				STKPOP(1);
				break;
			case 'J':
				STACKIN(0, TLONG);
				STACKIN(1, TVOID);
				STKPOP(2);
				break;
			case 'D':
				STACKIN(0, TDOUBLE);
				STACKIN(1, TVOID);
				STKPOP(2);
				break;
			default:
				ABORT();
				break;
			}
			INCPC(3);
			break;

		case GETFIELD:
			getField(lclw, meth->class, false, &finfo);
			STACKIN(0, TOBJ);
			if (FIELD_ISREF(finfo.field)) {
				STACKOUT(0, TOBJ);
			}
			else switch (CLASS_PRIM_SIG(FIELD_TYPE(finfo.field))){
			case 'I':
			case 'Z':
			case 'S':
			case 'B':
			case 'C':
				STACKOUT(0, TINT);
				break;
			case 'F':
				STACKOUT(0, TFLOAT);
				break;
			case 'J':
				STKPUSH(1);
				STACKOUT(0, TLONG);
				STACKOUT(1, TVOID);
				break;
			case 'D':
				STKPUSH(1);
				STACKOUT(0, TDOUBLE);
				STACKOUT(1, TVOID);
				break;
			default:
				ABORT();
				break;
			}
			INCPC(3);
			break;

		case PUTFIELD:
			getField(lclw, meth->class, false, &finfo);
			if (FIELD_ISREF(finfo.field)) {
				STACKIN(0, TOBJ);
				STACKIN(1, TOBJ);
				STKPOP(2);
			}
			else switch (CLASS_PRIM_SIG(FIELD_TYPE(finfo.field))){
			case 'I':
			case 'Z':
			case 'S':
			case 'B':
			case 'C':
				STACKIN(0, TINT);
				STACKIN(1, TOBJ);
				STKPOP(2);
				break;
			case 'F':
				STACKIN(0, TFLOAT);
				STACKIN(1, TOBJ);
				STKPOP(2);
				break;
			case 'J':
				STACKIN(0, TLONG);
				STACKIN(1, TVOID);
				STACKIN(2, TOBJ);
				STKPOP(3);
				break;
			case 'D':
				STACKIN(0, TDOUBLE);
				STACKIN(1, TVOID);
				STACKIN(2, TOBJ);
				STKPOP(3);
				break;
			default:
				ABORT();
				break;
			}
			INCPC(3);
			break;

		case INVOKEVIRTUAL:
		case INVOKESPECIAL:
			getMethodSignatureClass(lclw, meth->class, false, false, &call);

			sig = call.signature->data;
			assert(sig[0] == '(');
			sig++;


			idx = call.in;
			STACKIN(idx, TOBJ);
			idx -= 1;

			while (sig[0] != ')') {
				switch (sig[0]) {
				case '[':
					STACKIN(idx, TOBJ);
					idx -= 1;
					while (sig[0] == '[') {
						sig++;
					}
					if (sig[0] == 'L') {
						while (sig[0] != ';') {
							sig++;
						}
					}
					sig++;
					break;
				case 'L':
					STACKIN(idx, TOBJ);
					idx -= 1;
					while (sig[0] != ';') {
						sig++;
					}
					sig++;
					break;
				case 'I':
				case 'Z':
				case 'S':
				case 'B':
				case 'C':
					STACKIN(idx, TINT);
					idx -= 1;
					sig++;
					break;
				case 'J':
					STACKIN(idx-1, TLONG);
					STACKIN(idx, TVOID);
					idx -= 2;
					sig++;
					break;
				case 'F':
					STACKIN(idx, TFLOAT);
					idx -= 1;
					sig++;
					break;
				case 'D':
					STACKIN(idx-1, TDOUBLE);
					STACKIN(idx, TVOID);
					idx -= 2;
					sig++;
					break;
				default:
					assert("Signature character unknown" == 0);
				}
			}
			STKPOP(call.in+1);

			STKPUSH(call.out);
			switch (call.rettype) {
			case '[':
			case 'L':
				STACKOUT(0, TOBJ);
				break;
			case 'I':
			case 'Z':
			case 'S':
			case 'B':
			case 'C':
				STACKOUT(0, TINT);
				break;
			case 'J':
				STACKOUT(0, TLONG);
				STACKOUT(1, TVOID);
				break;
			case 'F':
				STACKOUT(0, TFLOAT);
				break;
			case 'D':
				STACKOUT(0, TDOUBLE);
				STACKOUT(1, TVOID);
				break;
			case 'V':
			default:
				break;
			}
			INCPC(3);
			break;

		case INVOKEINTERFACE:
			getMethodSignatureClass(lclw, meth->class, false, false, &call);

			sig = call.signature->data;
			assert(sig[0] == '(');
			sig++;

			idx = call.in;
			STACKIN(idx, TOBJ);
			idx -= 1;

			while (sig[0] != ')') {
				switch (sig[0]) {
				case '[':
					STACKIN(idx, TOBJ);
					idx -= 1;
					while (sig[0] == '[') {
						sig++;
					}
					if (sig[0] == 'L') {
						while (sig[0] != ';') {
							sig++;
						}
					}
					sig++;
					break;
				case 'L':
					STACKIN(idx, TOBJ);
					idx -= 1;
					while (sig[0] != ';') {
						sig++;
					}
					sig++;
					break;
				case 'I':
				case 'Z':
				case 'S':
				case 'B':
				case 'C':
					STACKIN(idx, TINT);
					idx -= 1;
					sig++;
					break;
				case 'J':
					STACKIN(idx-1, TLONG);
					STACKIN(idx, TVOID);
					idx -= 2;
					sig++;
					break;
				case 'F':
					STACKIN(idx, TFLOAT);
					idx -= 1;
					sig++;
					break;
				case 'D':
					STACKIN(idx-1, TDOUBLE);
					STACKIN(idx, TVOID);
					idx -= 2;
					sig++;
					break;
				default:
					assert("Signature character unknown" == 0);
				}
			}
			STKPOP(call.in+1);

			STKPUSH(call.out);
			switch (call.rettype) {
			case '[':
			case 'L':
				STACKOUT(0, TOBJ);
				break;
			case 'I':
			case 'Z':
			case 'S':
			case 'B':
			case 'C':
				STACKOUT(0, TINT);
				break;
			case 'J':
				STACKOUT(0, TLONG);
				STACKOUT(1, TVOID);
				break;
			case 'F':
				STACKOUT(0, TFLOAT);
				break;
			case 'D':
				STACKOUT(0, TDOUBLE);
				STACKOUT(1, TVOID);
				break;
			case 'V':
			default:
				break;
			}
			INCPC(5);
			break;

		case INVOKESTATIC:
			getMethodSignatureClass(lclw, meth->class, false, false, &call);

			sig = call.signature->data;
			assert(sig[0] == '(');
			sig++;


			idx = call.in - 1;
			while (sig[0] != ')') {
				switch (sig[0]) {
				case '[':
					STACKIN(idx, TOBJ);
					idx -= 1;
					while (sig[0] == '[') {
						sig++;
					}
					if (sig[0] == 'L') {
						while (sig[0] != ';') {
							sig++;
						}
					}
					sig++;
					break;
				case 'L':
					STACKIN(idx, TOBJ);
					idx -= 1;
					while (sig[0] != ';') {
						sig++;
					}
					sig++;
					break;
				case 'I':
				case 'Z':
				case 'S':
				case 'B':
				case 'C':
					STACKIN(idx, TINT);
					idx -= 1;
					sig++;
					break;
				case 'J':
					STACKIN(idx-1, TLONG);
					STACKIN(idx, TVOID);
					idx -= 2;
					sig++;
					break;
				case 'F':
					STACKIN(idx, TFLOAT);
					idx -= 1;
					sig++;
					break;
				case 'D':
					STACKIN(idx-1, TDOUBLE);
					STACKIN(idx, TVOID);
					idx -= 2;
					sig++;
					break;
				default:
					assert("Signature character unknown" == 0);
				}
			}
			STKPOP(call.in);

			STKPUSH(call.out);
			switch (call.rettype) {
			case '[':
			case 'L':
				STACKOUT(0, TOBJ);
				break;
			case 'I':
			case 'Z':
			case 'S':
			case 'B':
			case 'C':
				STACKOUT(0, TINT);
				break;
			case 'J':
				STACKOUT(0, TLONG);
				STACKOUT(1, TVOID);
				break;
			case 'F':
				STACKOUT(0, TFLOAT);
				break;
			case 'D':
				STACKOUT(0, TDOUBLE);
				STACKOUT(1, TVOID);
				break;
			case 'V':
			default:
				break;
			}
			INCPC(3);
			break;

		case NEW:
			STKPUSH(1);
			STACKOUT(0, TOBJ);
			INCPC(3);
			break;

		case NEWARRAY:
			STACKIN(0, TINT);
			STACKOUT(0, TOBJ);
			INCPC(2);
			break;

		case ANEWARRAY:
			STACKIN(0, TINT);
			STACKOUT(0, TOBJ);
			INCPC(3);
			break;

		case MULTIANEWARRAY:
			for (idx = INSN(pc+3) - 1; idx >= 0; idx--) {
				STACKIN(idx, TINT);
			}
			STKPOP(INSN(pc+3) - 1);
			STACKOUT(0, TOBJ);
			INCPC(4);
			break;

		case ARRAYLENGTH:
			STACKIN(0, TOBJ);
			STACKOUT(0, TINT);
			INCPC(1);
			break;

		case ATHROW:
			STACKIN(0, TOBJ);
			STKPOP(1);
			INCPC(1);
			break;

		case CHECKCAST:
			class = getClass(lclw, meth->class);
			/* SET_INSN(pc, CHECKCAST_FAST); */
			STACKIN(0, TOBJ);
			STACKOUT(0, TOBJ);
			INCPC(3);
			break;

		case INSTANCEOF:
			class = getClass(lclw, meth->class);
			/* SET_INSN(pc, INSTANCEOF_FAST); */
			STACKIN(0, TOBJ);
			STACKOUT(0, TINT);
			INCPC(3);
			break;

		case MONITORENTER:
		case MONITOREXIT:
			STACKIN(0, TOBJ);
			STKPOP(1);
			INCPC(1);
			break;

		case IFNULL:
		case IFNONNULL:
			STACKIN(0, TOBJ);
			STKPOP(1);
			FRAMEMERGE(pc + lclw, sp);
			FRAMEMERGE(pc + 3, sp);
			INCPC(3);
			break;

		case WIDE:
			wide = true;
			INCPC(1);
			break;

		case BREAKPOINT:
			INCPC(1);
			break;
		}
	} while (pc < meth->c.bcode.codelen && !IS_STARTOFBASICBLOCK(pc) && !IS_STARTOFEXCEPTION(pc));

	/* If we flow into the next basic block, set the stack pointer
	 * and merge in the frame.
	 */
	if (pc < meth->c.bcode.codelen && IS_NORMALFLOW(pc)) {
		FRAMEMERGE(pc, sp);
	}

	/* Discard active frame */
	gc_free(activeFrame);

	return (failed);
}

/*
 * Merge the current frame into the beginning of a basic block.
 */
static
void
mergeFrame(int pc, int sp, frameElement* from, Method* meth)
{
	int m;
	frameElement* to;

	to = FRAME(pc);
	assert(to != 0);

	/* Merge locals */
	for (m = 0; m < meth->localsz; m++) {
		if (from[m].type != TUNASSIGNED && from[m].type != to[m].type) {
			SET_NEEDVERIFY(pc);
			if (to[m].type == TUNASSIGNED) {
				to[m].type = from[m].type;
			}
			else {
				to[m].type = TUNSTABLE;
			}
		}
	}

	/* Merge stacks */
	for (m = sp; m < meth->localsz + meth->stacksz; m++) {
		if (from[m].type != TUNASSIGNED && from[m].type != to[m].type) {
			SET_NEEDVERIFY(pc);
			if (to[m].type == TUNASSIGNED) {
				to[m].type = from[m].type;
			}
			else {
				to[m].type = TUNSTABLE;
			}
		}
	}
}

/*
 * Tidy up after verfication data has been finished with.
 */
void
tidyVerifyMethod(void)
{
	int pc;

	/* Free the old data */
	for (pc = 0; pc < codeInfo->codelen; pc++) {
		if (codeInfo->perPC[pc].frame != 0) {
			gc_free(codeInfo->perPC[pc].frame);
		}
	}
	gc_free(codeInfo);
	codeInfo = 0;
	/* now it's safe to unlock the verifier */
	unlockStaticMutex(&vlock);
}

