/* code-analyse.h
 * Analyse a method's bytecodes.
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#ifndef __code_analyse_h
#define __code_analyse_h

#include "classMethod.h"

typedef	struct {
	Hjava_lang_Class*	type;
	uint8			used;
	uint8			modified;
} frameElement;

typedef struct perPCInfo {
	uint16			stackPointer;
	uint16			flags;
#if defined(TRANSLATOR)
	uintp			nativePC;
#endif
	struct perPCInfo*	nextBB;
	frameElement*		frame;
} perPCInfo;

typedef struct _localUse {
	int32			use;
	int32			first;
	int32			last;
	int32			write;
	Hjava_lang_Class*	type;
} localUse;

typedef struct codeinfo {
	uint16			codelen;
	uint16			stacksz;
	uint16			localsz;
	localUse*		localuse;
	perPCInfo		perPC[1];
} codeinfo;

#define	TUNASSIGNED		((Hjava_lang_Class*)0)
#define	TUNSTABLE		((Hjava_lang_Class*)1)
#define	TADDR			((Hjava_lang_Class*)2)
#define	TOBJ			((Hjava_lang_Class*)3)
#define	TVOID			(voidClass)
#define	TINT			(intClass)
#define	TLONG			(longClass)
#define	TFLOAT			(floatClass)
#define	TDOUBLE			(doubleClass)

#define	CONSTANTTYPE(VAL, LCL)					\
	switch (CLASS_CONST_TAG(meth->class, (LCL))) {		\
	case CONSTANT_Integer:					\
		VAL = TINT;					\
		break;						\
	case CONSTANT_Long:					\
		VAL = TLONG;					\
		break;						\
	case CONSTANT_Float:					\
		VAL = TFLOAT;					\
		break;						\
	case CONSTANT_Double:					\
		VAL = TDOUBLE;					\
		break;						\
	case CONSTANT_String:					\
	case CONSTANT_ResolvedString:				\
		VAL = TOBJ;					\
		break;						\
	default:						\
		VAL = TUNSTABLE;				\
		break;						\
	}

#define	FLAG_STARTOFBASICBLOCK		0x0001
#define	FLAG_STARTOFEXCEPTION		0x0002
#define	FLAG_STACKPOINTERSET		0x0004
#define	FLAG_NORMALFLOW			0x0008
#define	FLAG_JUMPFLOW			0x0010
#define	FLAG_JUMP			0x0020
#define	FLAG_NEEDVERIFY			0x0040
#define	FLAG_DONEVERIFY			0x0080
#define	FLAG_STARTOFINSTRUCTION		0x0100

#define	FLAGS(_pc)			codeInfo->perPC[_pc].flags
#define	STACKPOINTER(_pc)		codeInfo->perPC[_pc].stackPointer
#define	FRAME(_pc)			codeInfo->perPC[_pc].frame
#define	INSN(pc)			meth->c.bcode.code[(pc)]
#define	INSNLEN(pc)			insnLen[INSN(pc)]
#define	BYTE(pc)			(int8)(meth->c.bcode.code[(pc)+0])
#define	WORD(pc)			(int16)( \
					 (meth->c.bcode.code[(pc)+0] << 8) + \
					 (meth->c.bcode.code[(pc)+1]))
#define	DWORD(pc)			(int32)( \
					 (meth->c.bcode.code[(pc)+0] << 24) + \
					 (meth->c.bcode.code[(pc)+1] << 16) + \
					 (meth->c.bcode.code[(pc)+2] << 8) + \
					 (meth->c.bcode.code[(pc)+3]))
#define	INCPC(V)			pc += (V)

#define	SET_STARTOFBASICBLOCK(PC)	ATTACH_NEW_BASICBLOCK(PC); \
					SET_NEWFRAME(PC); \
					FLAGS(PC) |= FLAG_STARTOFBASICBLOCK

#define	SET_STACKPOINTER(PC, SP)	do { \
	if ((FLAGS(PC) & FLAG_STACKPOINTERSET) && STACKPOINTER(PC) != (SP)) { \
		failed = true; \
		postExceptionMessage(einfo, JAVA_LANG(VerifyError), "sp size change at pc %d: %d != %d\n", (PC), STACKPOINTER(PC), (SP)); \
	} \
	STACKPOINTER(PC) = (SP); \
	FLAGS(PC) |= FLAG_STACKPOINTERSET; \
} while(0)

#if defined(TRANSLATOR)
#define	SET_INSNPC(P, V)		codeInfo->perPC[(P)].nativePC = (V)
#define	INSNPC(P)			codeInfo->perPC[(P)].nativePC
#else
#define	SET_INSNPC(P, V)
#endif

#if defined(INTERPRETER)
#define	SET_INSN(PC, V)			INSN(PC) = (V)
#else
#define	SET_INSN(PC, V)
#endif
#define	SET_NORMALFLOW(pc)		FLAGS(pc) |= FLAG_NORMALFLOW
#define	SET_JUMPFLOW(from, to)		FLAGS(to) |= FLAG_JUMPFLOW; \
					FLAGS(from) |= FLAG_JUMP
#define	SET_STARTOFEXCEPTION(pc)	FLAGS(pc) |= FLAG_STARTOFEXCEPTION
#define	SET_NEEDVERIFY(pc)		FLAGS(pc) |= FLAG_NEEDVERIFY
#define	SET_DONEVERIFY(pc)		FLAGS(pc) &= ~FLAG_NEEDVERIFY; \
					FLAGS(pc) |= FLAG_DONEVERIFY
#define	SET_STARTOFINSTRUCTION(pc)	FLAGS(pc) |= FLAG_STARTOFINSTRUCTION

#define	IS_STARTOFBASICBLOCK(pc)	(FLAGS(pc) & FLAG_STARTOFBASICBLOCK)
#define	IS_STACKPOINTERSET(pc)		(FLAGS(pc) & FLAG_STACKPOINTERSET)
#define	IS_NORMALFLOW(pc)		(FLAGS(pc) & FLAG_NORMALFLOW)
#define	IS_STARTOFEXCEPTION(pc)		(FLAGS(pc) & FLAG_STARTOFEXCEPTION)
#define	IS_NEEDVERIFY(pc)		(FLAGS(pc) & FLAG_NEEDVERIFY)
#define	IS_DONEVERIFY(pc)		(FLAGS(pc) & FLAG_DONEVERIFY)
#define	IS_STARTOFINSTRUCTION(pc)	(FLAGS(pc) & FLAG_STARTOFINSTRUCTION)
#define	IS_UNREACHABLE(pc)		((IS_STARTOFBASICBLOCK(pc) || \
					  IS_STARTOFEXCEPTION(pc)) && \
					  !IS_DONEVERIFY(pc))

#define	ALLOCFRAME()			gc_malloc((codeInfo->stacksz+codeInfo->localsz+1) * sizeof(frameElement), KGC_ALLOC_CODEANALYSE)

#define	ATTACH_NEW_BASICBLOCK(DPC)				\
	if ((DPC) != 0 && !IS_STARTOFBASICBLOCK(DPC) &&		\
			  !IS_STARTOFEXCEPTION(DPC)) {		\
		btail->nextBB = &codeInfo->perPC[DPC];		\
		btail = btail->nextBB;				\
	}

#define	SET_NEWFRAME(pc)					\
	if (FRAME(pc) == 0) {					\
		FRAME(pc) = ALLOCFRAME();			\
		if (!FRAME(pc)) {				\
			meth->accflags &= ~ACC_VERIFIED;	\
			tidyAnalyzeMethod(&codeInfo);		\
			postOutOfMemory(einfo);			\
			return false;				\
		}						\
	}

#define	FRAMEMERGE(PC, SP)					\
	SET_STACKPOINTER(PC, SP);				\
	mergeFrame(codeInfo, PC, SP, activeFrame, meth);	\
        if (!IS_DONEVERIFY(PC)) {				\
                SET_NEEDVERIFY(PC);				\
        }

#define	FRAMEMERGE_LOCALS(PC)					\
	mergeFrame(codeInfo, PC, meth->localsz+meth->stacksz, activeFrame, meth); \
        if (!IS_DONEVERIFY(PC)) {				\
                SET_NEEDVERIFY(PC);				\
        }

#define	FRAMELOAD(_pc)						\
	{							\
		int m;						\
		for (m = codeInfo->stacksz+codeInfo->localsz; m >= 0; m--) { \
			activeFrame[m] = FRAME(_pc)[m];		\
		}						\
	}

#define	STKPUSH(L)			sp -= (L)
#define	STKPOP(L)			sp += (L)

#define	STACKINIT(S, T)			FRAME(pc)[sp+(S)].type = (T)

#ifdef notyet
#define	STACK_CHECKRANGE(S)		if ((S)+sp < meth->localsz || (S)+sp > meth->localsz+meth->stacksz) { \
						failed = true; \
						postExceptionMessage(einfo, JAVA_LANG(VerifyError), "sp out of range: %d <%d> %d\n", meth->localsz, (S)+sp, meth->localsz + meth->stacksz); \
					}

#define	STACK_CHECKTYPE(S, T)		if ((T) != SF(S).type) { \
						failed = true; \
						postExceptionMessage(einfo, JAVA_LANG(VerifyError), "pc %d: stk %d (is %d, want %d)\n", pc, sp+(S), SF(S).type, T); \
					}

#define	LOCAL_CHECKRANGE(L)		if ((L) < 0 || (L) >= meth->localsz) { \
						failed = true; \
						postExceptionMessage(einfo, JAVA_LANG(VerifyError), "lcl out of range: <%d> %d\n", L, meth->localsz); \
					}
#define	LOCAL_CHECKTYPE(L, T)		if ((T) != LCL(L).type) { \
						failed = true; \
						postExceptionMessage(einfo, JAVA_LANG(VerifyError), "pc %d: lcl %d (is %d, want %d)\n", pc, (L), LCL(L).type, T); \
					}
#else
#define	STACK_CHECKRANGE(S)
#define	STACK_CHECKTYPE(S, T)
#define	LOCAL_CHECKRANGE(L)
#define	LOCAL_CHECKTYPE(L, T)
#endif

#define	SF(S)				activeFrame[sp+(S)]
#define	LCL(L)				activeFrame[(L)]

#define	STACKIN(S, T)			STACK_CHECKRANGE(S); \
					STACK_CHECKTYPE(S, T)
#define	STACKOUT(S, T)			SF(S).type = (T)
#define	STACKOUT_CONST(S, T, V)		SF(S).type = (T)
#define	STACKOUT_LOCAL(S, T, L)		SF(S) = LCL(L); \
					LCL(L).used = 1

#define	LOCALOUT(L, T)			LCL(L).type = (T); \
					LCL(L).used = 1; \
					LCL(L).modified = 1
#define LOCALOUT_STACK(L, T, S)		LCL(L) = SF(S); \
					LCL(L).used = 1; \
					LCL(L).modified = 1
#define	LOCALINOUT(L, T)		LCL(L).used = 1; \
					LCL(L).modified = 1
#define	STACKCOPY(F, T)			SF(T) = SF(F)
#define	STACKOUT_STACK(T, X, F)		STACKCOPY(F, T)
#define	STACKSWAP(F, T)			{				\
						frameElement tmp;	\
						tmp = SF(T);		\
						SF(T) = SF(F);		\
						SF(F) = tmp;		\
					}

#define	LOCALINIT(L, T);		FRAME(pc)[(L)].type = (T); \
					FRAME(pc)[(L)].used = 1
#define	LOCALIN(L, T)			LOCAL_CHECKTYPE(L, T); \
					LCL(L).used = 1

struct _jmethodID;
bool analyzeMethod(struct _jmethodID*, codeinfo**, errorInfo*);
void tidyAnalyzeMethod(codeinfo**);

extern const uint8 insnLen[];

#endif
