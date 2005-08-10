/*
 * machine.h
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#ifndef __machine_h
#define	__machine_h

#include "threadData.h"

#define current_class()		(meth->class)

#define getopcode()             ((int)code[pc])
#define getpc(n)		(code[pc+1+(n)])
#define getcode(n)		(code[(n)])
#define	putpc(n, v)		code[pc+(n)] = (v)

#define	object_array_offset	(ARRAY_DATA_OFFSET)
#define	object_array_length	(ARRAY_SIZE_OFFSET)

#define	get_method_info_noerror(idx) \
	get_method_info(idx)

#define get_special_method_info_noerror(idx) \
	get_special_method_info(idx)

#define	get_interface_method_info_noerror(idx) \
	get_interface_method_info(idx)

#define get_method_info(idx)  \
  if (getMethodSignatureClass(idx, meth->class, true, 0, &cinfo, &einfo) \
	== false) { throwError(&einfo); }

#define get_special_method_info(idx)  \
  if (getMethodSignatureClass(idx, meth->class, true, 1, &cinfo, &einfo) \
        == false) { throwError(&einfo); }

#define get_interface_method_info(idx)  \
  if (getMethodSignatureClass(idx, meth->class, true, 2, &cinfo, &einfo) \
        == false) { throwError(&einfo); }

#define	method_name()		(cinfo.name)
#define	method_sig()		(cinfo.signature)
#define method_idx()		(cinfo.method->idx)
#define method_method()		(cinfo.method)
#define method_class()		(cinfo.class)
#define method_classname()	(cinfo.cname)
#define	get_dispatch_table(mtable) \
	move_ref(mtable, ((slots*)&cinfo.class->dtable))

#define	method_nargs()		(cinfo.in)
#define	method_dtable_offset	OBJECT_DTABLE_OFFSET
#define	method_returntype()	(cinfo.rettype)

/* -------------------------------------------------------------------- */
/* Fields */

#define	get_field_info(IDX) \
  if (getField(IDX, meth->class, false, &finfo, &einfo) == false) { \
    throwError(&einfo); \
  }

#define	get_static_field_info_noerror(IDX) \
  getField((constIndex)(IDX), meth->class, true, &finfo, &einfo)

#define	get_static_field_info(IDX) \
  if (getField(IDX, meth->class, true, &finfo, &einfo) == false) { \
    throwError(&einfo); \
  }

#define field_class()		(finfo.class)
#define field_field()		(finfo.field)
#define field_name()		(finfo.name)
#define field_classname()	(finfo.cname)
#define field_sig()		(finfo.signature)
#define field_statics()		(finfo.class->static_data)

/* -------------------------------------------------------------------- */
/* Classes */

#define	get_class_info_noerror(_idx) \
	get_class_info(_idx)

#define	get_class_info(_idx) \
  crinfo = getClass(_idx, meth->class, &einfo); \
  if (crinfo == 0) { throwError(&einfo); }

#define	class_object()		(crinfo)
#define class_name(IDX)		(WORD2UTF(meth->class->constants.data[IDX]))

#define compile_time_error(EINFO) throwError(&EINFO)

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
struct _slots;
void virtualMachine(struct _jmethodID*, struct _slots*, struct _slots*, threadData*); 
void setupExceptionHandling(VmExceptHandler* eh, struct _jmethodID* meth, struct Hjava_lang_Object* syncobj, threadData*);
void cleanupExceptionHandling(VmExceptHandler* eh, threadData*); 

#endif
