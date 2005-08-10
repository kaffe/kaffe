/* machine.h
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * Copyright (c) 2004
 *	Kaffe.org contributors. See ChangeLog for details. All rights reserved.
 *   
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#ifndef __machine_h
#define __machine_h

#include "gtypes.h"

/* -------------------------------------------------------------------- */

#define KJIT(funcName) KaffeJIT3_ ## funcName

/* Instructions */
#define	define_insn(code)	break;					\
				case code :
#define	define_insn_alias(code)	case code :
#define	define_wide_insn(code)	break;					\
				case code :

/* Stack */
#define	push(_i)		stackno -= (_i)
#define	pop(_i)			lastuse_slot(stack(0), (_i));		\
				stackno += (_i)

/* PC */
#define	getopcode()		((int)base[pc])
#define	getpc(_p)		base[pc+1+(_p)]
#define	getcode(_p)		base[(_p)]
#define	adjustpc(_p)		npc = pc + (_p)

#define current_class()		(xmeth->class)

/* -------------------------------------------------------------------- */
/* Methods */

#define	get_method_info_noerror(IDX) \
	getMethodSignatureClass(IDX, xmeth->class, true, 0, &cinfo, einfo)

#define get_special_method_info_noerror(IDX) \
	getMethodSignatureClass(IDX, xmeth->class, true, 1, &cinfo, einfo)

#define	get_interface_method_info_noerror(IDX) \
	getMethodSignatureClass(IDX, xmeth->class, true, 2, &cinfo, einfo)

#define	get_method_info(IDX) \
	if (getMethodSignatureClass(IDX, xmeth->class, true, 0, &cinfo, einfo) == false) { success = false; goto done; }

#define get_special_method_info(IDX) \
	if (getMethodSignatureClass(IDX, xmeth->class, true, 1, &cinfo, einfo) == false) { success = false; goto done; }

#define	get_interface_method_info(IDX) \
	if (getMethodSignatureClass(IDX, xmeth->class, true, 2, &cinfo, einfo) == false) { success = false; goto done; }

#define	method_name()	(cinfo.name)
#define	method_sig()	(cinfo.signature)
#define	method_idx()	(cinfo.method->idx)
#define method_method()	(cinfo.method)
#define method_class()  (cinfo.class)
#define method_classname()  (cinfo.cname)
#define	get_dispatch_table(mtable)				\
			move_ref_const(mtable, cinfo.class->dtable)

#define	method_nargs()		(cinfo.in)
#define	method_dtable_offset	(OBJECT_DTABLE_OFFSET)
#define	method_returntype()	(cinfo.rettype)

/* -------------------------------------------------------------------- */
/* Fields */

#define	get_field_info(IDX) \
	if (getField((constIndex)(IDX), xmeth->class, false, &finfo, einfo) == false) { \
		success = false ; goto done; \
	}

#define get_static_field_info_noerror(IDX) \
        getField((constIndex)(IDX), xmeth->class, true, &finfo, einfo)

#define	get_static_field_info(IDX) \
	if (getField((constIndex)(IDX), xmeth->class, true, &finfo, einfo) == false) { \
		success = false ; goto done; \
	}

#define field_class()		(finfo.class)
#define field_field()		(finfo.field)
#define field_name()		(finfo.name)
#define field_classname()	(finfo.cname)
#define field_sig()		(finfo.signature)
#define field_static_data()	((finfo.class)->sdata)
#define field_statics()	(finfo.class->static_data)

/* -------------------------------------------------------------------- */
/* Classes */

#define	get_class_info_noerror(IDX) \
	crinfo = getClass((IDX), xmeth->class, einfo)

#define	get_class_info(IDX) \
	crinfo = getClass((IDX), xmeth->class, einfo); \
	if (crinfo == 0) { \
		success = false; goto done; \
	}

#define	class_object()		(crinfo)
#define class_name(IDX)		(WORD2UTF(xmeth->class->constants.data[IDX]))

/* -------------------------------------------------------------------- */
/* Objects */

#define	object_array_offset	(ARRAY_DATA_OFFSET)
#define	object_array_length	(ARRAY_SIZE_OFFSET)

/* -------------------------------------------------------------------- */
/* Switches */

#define	switchpair_size		8
#define	switchpair_addr		4
#define	switchtable_shift	2

/* Provide write barrier support for incremental GC */
#if defined(KGC_INCREMENTAL)
#define	SOFT_ADDREFERENCE(_f, _t)	 softcall_writeref(_f, _t)
#define	SOFT_ADDREFERENCE_STATIC(_f, _t) softcall_writeref_static(_f, _t)
#else
#define	SOFT_ADDREFERENCE(_f, _t)
#define	SOFT_ADDREFERENCE_STATIC(_f, _t)
#endif

typedef struct {
	bool ANY;
        bool BADARRAYINDEX;
        bool NULLPOINTER;
} jitflags;

#include "locks.h"
extern iStaticLock	translatorlock;

#define	enterTranslator()	lockStaticMutex(&translatorlock)
#define	leaveTranslator()	unlockStaticMutex(&translatorlock)

extern jitflags willcatch;
extern struct codeinfo* codeInfo;
extern int profFlag;
extern Method* globalMethod;

typedef struct _jitCodeHeader {
	unsigned long flags;
	Method *method;
	void *pool;
	nativecode *code_start;
	size_t code_len;
} jitCodeHeader;

/**
 * Encapsulate information about the native code of a method.
 *
 * @field mem address of the memory allocated for the code
 * @field memlen size of the memory
 * @field code address of the executable code of the method
 * @field codelen size of the executable code.
 */
typedef struct _nativeCodeInfo {
	void*   mem;
	int     memlen;
	void*   code;
	int     codelen;
} nativeCodeInfo;

#define	willCatch(FLAG)	willcatch. FLAG = true
#define	canCatch(FLAG)	willcatch. FLAG

void setupArgumentRegisters(void);

/* Desktop edition */
#define	compile_time_error(EINFO) { \
	*einfo = (EINFO); \
	success = false ; \
	goto done; \
}

/*
 * Various methods exported by machine.c
 */
struct SlotData;
struct _label_;
struct _sequence;
struct _errorInfo;
extern jboolean initInsnSequence(Method *meth, int codesize, int localsz, int stacksz, struct _errorInfo *einfo);
extern jboolean finishInsnSequence(void*, nativeCodeInfo*, struct _errorInfo*);
extern void     installMethodCode(void*, Method*, nativeCodeInfo*);
#define HAVE_FAKE_CALLS 1
void initFakeCalls(void);
extern struct _label_* newFakeCall(void*, uintp);
extern void 	doSpill(struct _sequence*);
extern void 	doReload(struct _sequence*);
extern struct SlotData** createSpillMask(void);
extern void     slotAlias(struct _sequence*);
extern void     startInsn(struct _sequence*);
extern jboolean translate(Method* xmeth, struct _errorInfo* einfo);

/*
 * Some global variables used by the translater which we must export.
 */
extern uint32 pc;
extern uint32 npc;
extern int maxPush;
extern int maxArgs;
extern int maxTemp;
extern int maxLocal;
extern int maxStack;
extern int isStatic;
extern nativecode* codeblock;
extern uintp CODEPC;

#endif
