/* machine.h
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#ifndef __machine_h
#define __machine_h

#include "errors.h"
#include "gtypes.h"

/* -------------------------------------------------------------------- */

#define KJIT(funcName) KaffeJIT_ ## funcName

/* Instructions */
#define	define_insn(code)	break;					\
				case code :
#define	define_insn_alias(code)	case code :
#define	define_wide_insn(code)	break;					\
				case code :

/* Stack */
#define	push(_i)		stackno -= (_i)
#define	pop(_i)			stackno += (_i)

/* PC */
#define	getopcode()		((int)base[pc])
#define	getpc(_p)		base[pc+1+(_p)]
#define	getcode(_p)		base[(_p)]
#define	adjustpc(_p)		npc = pc + (_p)

#define current_class()		(meth->class)

/* -------------------------------------------------------------------- */
/* Methods */

#define	get_method_info_noerror(idx) \
	getMethodSignatureClass(idx, meth->class, true, 0, &cinfo, einfo)

#define get_special_method_info_noerror(idx) \
	getMethodSignatureClass(idx, meth->class, true, 1, &cinfo, einfo)

#define	get_interface_method_info_noerror(idx) \
	getMethodSignatureClass(idx, meth->class, true, 2, &cinfo, einfo)

#define	get_method_info(idx)  \
  if (getMethodSignatureClass(idx, meth->class, true, 0, &cinfo, einfo) \
	== false) { success = false; goto done; }

#define	get_special_method_info(idx)  \
  if (getMethodSignatureClass(idx, meth->class, true, 1, &cinfo, einfo) \
	== false) { success = false; goto done; } 

#define	get_interface_method_info(idx)  \
  if (getMethodSignatureClass(idx, meth->class, true, 2, &cinfo, einfo) \
	== false) { success = false; goto done; } 

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
  if (getField((IDX), meth->class, false, &finfo, einfo) == false) { \
    success = false; goto done; \
  }

#define	get_static_field_info_noerror(IDX) \
  getField((constIndex)(IDX), meth->class, true, &finfo, einfo)

#define	get_static_field_info(IDX) \
  if (getField((IDX), meth->class, true, &finfo, einfo) == false) { \
    success = false; goto done; \
  }

#define field_class()		(finfo.class)
#define field_field()		(finfo.field)
#define field_name()		(finfo.name)
#define field_classname()	(finfo.cname)
#define field_sig()		(finfo.signature)
#define field_statics()		(finfo.class->static_data)

/* -------------------------------------------------------------------- */
/* Classes */

#define	get_class_info_noerror(IDX) \
	crinfo = getClass((IDX), meth->class, einfo)

#define	get_class_info(IDX)	\
  crinfo = getClass((IDX), meth->class, einfo); \
  if (crinfo == 0) { \
    success = false; goto done; \
  }

#define	class_object()		(crinfo)
#define class_name(IDX)		(WORD2UTF(meth->class->constants.data[IDX]))

/* -------------------------------------------------------------------- */
/* Objects */

#define	object_array_offset	(ARRAY_DATA_OFFSET)
#define	object_array_length	(ARRAY_SIZE_OFFSET)

/* -------------------------------------------------------------------- */
/* Errors */

#define compile_time_error(EINFO) {		\
	*einfo = (EINFO);			\
	success = false;			\
	goto done;				\
}

/* -------------------------------------------------------------------- */
/* Switches */

#define	switchpair_size		8
#define	switchpair_addr		4
#define	switchtable_shift	2

/* Provide write barrier support for incremental GC */
#if defined(GC_INCREMENTAL)
#define	SOFT_ADDREFERENCE(_f, _t)	 softcall_addreference(_f, _t)
#define	SOFT_ADDREFERENCE_STATIC(_f, _t) softcall_addreference_static(_f, _t)
#else
#define	SOFT_ADDREFERENCE(_f, _t)
#define	SOFT_ADDREFERENCE_STATIC(_f, _t)
#endif

struct _jmethodID;
bool translate(struct _jmethodID*, errorInfo*);

typedef struct _jitCodeHeader {
	unsigned long flags;
	Method *method;
	void *pool;
	nativecode *code_start;
	size_t code_len;
} jitCodeHeader;

typedef struct _nativeCodeInfo {
	void*	mem;
	int	memlen;
	void*	code;
	int	codelen;
} nativeCodeInfo;

struct codeinfo;
bool initInsnSequence(Method *meth, int codesize, int localsz, int stacksz,
		      struct _errorInfo *einfo);
bool finishInsnSequence(struct codeinfo*, nativeCodeInfo*, errorInfo*);
void installMethodCode(struct codeinfo*, Method*, nativeCodeInfo*);

typedef struct {
        bool BADARRAYINDEX;
        bool NULLPOINTER;
} jitflags;

extern jitflags willcatch;

#define	willCatch(FLAG)	willcatch. FLAG = true
#define	canCatch(FLAG)	willcatch. FLAG

#include "locks.h"
extern iStaticLock	translatorlock;

#define	enterTranslator()	lockStaticMutex(&translatorlock)
#define	leaveTranslator()	unlockStaticMutex(&translatorlock)

extern int maxLocal;
extern int maxStack;
extern int maxArgs;
extern int maxTemp;
extern int maxPush;
extern int isStatic;
extern int CODEPC;
extern nativecode* codeblock;
extern uint32 pc;
extern uint32 npc;
extern Method *globalMethod;

#if defined(KAFFE_PROFILER)
extern int profFlag;
#endif

#endif
