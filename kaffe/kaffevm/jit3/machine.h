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

/* -------------------------------------------------------------------- */

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

#define current_class()		(meth->class)

/* -------------------------------------------------------------------- */
/* Methods */

#define get_method_info(idx)  \
  if (getMethodSignatureClass(idx, meth->class, true, false, &cinfo, einfo) \
        == false) { success = false; goto done; }

#define get_special_method_info(idx)  \
  if (getMethodSignatureClass(idx, meth->class, true, true, &cinfo, einfo) \
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

#define get_field_info(IDX) \
  if (getField((IDX), meth->class, false, &finfo, einfo) == false) { \
    success = false; goto done; \
  }

#define get_static_field_info(IDX) \
  if (getField((IDX), meth->class, true, &finfo, einfo) == false) { \
    success = false; goto done; \
  }

#define field_class()		(finfo.class)
#define field_field()		(finfo.field)
#define field_name()		(finfo.name)
#define field_classname()	(finfo.cname)
#define field_sig()		(finfo.signature)

/* -------------------------------------------------------------------- */
/* Classes */

#define get_class_info(IDX)     \
  crinfo = getClass((IDX), meth->class, einfo); \
  if (crinfo == 0) { \
    success = false; goto done; \
  }

#define	class_object()		(crinfo)

/* -------------------------------------------------------------------- */
/* Objects */

#define	object_array_offset	(ARRAY_DATA_OFFSET)
#define	object_array_length	(ARRAY_SIZE_OFFSET)

/* -------------------------------------------------------------------- */
/* Errors */

#define compile_time_error(EINFO) {             \
        *einfo = (EINFO);                       \
        success = false;                        \
        goto done;                              \
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

struct _methods;

typedef struct _nativeCodeInfo {
	void*	mem;
	int	memlen;
	void*	code;
	int	codelen;
} nativeCodeInfo;

typedef struct {
	bool ANY;
        bool BADARRAYINDEX;
} jitflags;

extern jitflags willcatch;

#define	willCatch(FLAG)	willcatch.##FLAG = true
#define	canCatch(FLAG)	willcatch.##FLAG

#include "locks.h"
extern iLock translatorlock;

extern int CODEPC;
extern nativecode* codeblock;

static
inline
void
enterTranslator(void)
{
        if (!staticLockIsInitialized(&translatorlock)) {
                initStaticLock(&translatorlock);
        }
        lockStaticMutex(&translatorlock);
}

static
inline
void
leaveTranslator(void)
{
        unlockStaticMutex(&translatorlock);
}

void setupGlobalRegisters(void);

#if defined(KAFFE_PROFILER)
extern int profFlag;
extern Method *globalMethod;
#endif

#endif
