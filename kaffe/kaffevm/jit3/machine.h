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

#define current_class()		(xmeth->class)

/* -------------------------------------------------------------------- */
/* Methods */

#define	get_method_info(IDX) \
	if (getMethodSignatureClass(idx, xmeth->class, true, 0, &cinfo, einfo) == false) { success = false; goto done; }

#define get_special_method_info(IDX) \
	if (getMethodSignatureClass(idx, xmeth->class, true, 1, &cinfo, einfo) == false) { success = false; goto done; }

#define	get_interface_method_info(IDX) \
	if (getMethodSignatureClass(idx, xmeth->class, true, 2, &cinfo, einfo) == false) { success = false; goto done; }

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
	getField((constIndex)(IDX), xmeth->class, false, &finfo)

#define	get_static_field_info(IDX) \
	getField((constIndex)(IDX), xmeth->class, true, &finfo)

/* Desktop edition */
#undef	get_field_info
#undef	get_static_field_info

#define	get_field_info(IDX) \
	if (getField((constIndex)(IDX), xmeth->class, false, &finfo, einfo) == false) { \
		success = false ; goto done; \
	}

#define	get_static_field_info(IDX) \
	if (getField((constIndex)(IDX), xmeth->class, true, &finfo, einfo) == false) { \
		success = false ; goto done; \
	}

#define field_class()		(finfo.class)
#define field_field()		(finfo.field)
#define field_name()		(finfo.name)
#define field_classname()	(finfo.cname)
#define field_sig()		(finfo.signature)

/* -------------------------------------------------------------------- */
/* Classes */

#define	get_class_info(IDX)	crinfo = getClass((constIndex)(IDX), xmeth->class)

/* Desktop edition */
#undef	get_class_info
#define	get_class_info(IDX) \
	crinfo = getClass((IDX), xmeth->class, einfo); \
	if (crinfo == 0) { \
		success = false; goto done; \
	}

#define	class_object()		(crinfo)

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
#if defined(GC_INCREMENTAL)
#define	SOFT_ADDREFERENCE(_f, _t)	 softcall_addreference(_f, _t)
#define	SOFT_ADDREFERENCE_STATIC(_f, _t) softcall_addreference_static(_f, _t)
#else
#define	SOFT_ADDREFERENCE(_f, _t)
#define	SOFT_ADDREFERENCE_STATIC(_f, _t)
#endif

typedef struct {
	bool ANY;
        bool BADARRAYINDEX;
} jitflags;

#include "locks.h"
extern iLock* translatorlock;

#define	enterTranslator()	lockStaticMutex(&translatorlock)
#define	leaveTranslator()	unlockStaticMutex(&translatorlock)

extern jitflags willcatch;
extern int profFlag;
extern struct codeinfo* codeInfo;
extern Method* globalMethod;

typedef struct _nativeCodeInfo {
	void*   mem;
	int     memlen;
	void*   code;
	int     codelen;
} nativeCodeInfo;

#define	willCatch(FLAG)	willcatch.##FLAG = true
#define	canCatch(FLAG)	willcatch.##FLAG

void setupGlobalRegisters(void);
void setupArgumentRegisters(void);

/* Desktop edition */
#define	compile_time_error(EINFO) { \
	*einfo = (EINFO); \
	success = false ; \
	goto done; \
}

#endif
