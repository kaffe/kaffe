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

#define	get_method_info(idx)  \
  if (getMethodSignatureClass(idx, meth->class, true, false, &cinfo, einfo) \
	== false) { success = false; goto done; }

#define	get_special_method_info(idx)  \
  if (getMethodSignatureClass(idx, meth->class, true, true, &cinfo, einfo) \
	== false) { success = false; goto done; } 

#define	method_name()	(cinfo.name)
#define	method_sig()	(cinfo.signature)
#define	method_idx()	(cinfo.method->idx)
#define method_method()	(cinfo.method)
#define method_class()  (cinfo.class)
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

#define	get_static_field_info(IDX) \
  if (getField((IDX), meth->class, true, &finfo, einfo) == false) { \
    success = false; goto done; \
  }

#define field_class()		(finfo.class)

/* -------------------------------------------------------------------- */
/* Classes */

#define	get_class_info(IDX)	\
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
bool translate(struct _methods*, errorInfo*);

typedef struct _nativeCodeInfo {
	void*	mem;
	int	memlen;
	void*	code;
	int	codelen;
} nativeCodeInfo;

void initInsnSequence(int, int, int);
void finishInsnSequence(nativeCodeInfo*);
void installMethodCode(Method*, nativeCodeInfo*);

typedef struct {
        bool BADARRAYINDEX;
        bool NULLPOINTER;
} jitflags;

extern jitflags willcatch;

#define	willCatch(FLAG)	willcatch. FLAG = true
#define	canCatch(FLAG)	willcatch. FLAG

#endif
