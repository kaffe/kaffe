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

#define current_class()		(meth->class)

#define getopcode()             ((int)code[pc])
#define getpc(n)		(code[pc+1+(n)])
#define getcode(n)		(code[(n)])
#define	putpc(n, v)		code[pc+(n)] = (v)

#define	object_array_offset	(ARRAY_DATA_OFFSET)
#define	object_array_length	(ARRAY_SIZE_OFFSET)

#define	get_method_info(idx)  getMethodSignatureClass(idx, meth->class, true, false, &cinfo)
#define	get_special_method_info(idx)  getMethodSignatureClass(idx, meth->class, true, true, &cinfo)

#define	get_dispatch_table(mtable) \
	move_ref(mtable, ((slots*)&cinfo.class->dtable))

#define	method_name()		(cinfo.name)
#define	method_sig()		(cinfo.signature)
#define method_idx()		(cinfo.method->idx)
#define method_method()		(cinfo.method)
#define method_class()		(cinfo.class)
#define	method_nargs()		(cinfo.in)
#define	method_dtable_offset	OBJECT_DTABLE_OFFSET
#define	method_returntype()	(cinfo.rettype)

#define	get_field_info(IDX) \
	getField(IDX, meth->class, false, &finfo)

#define	get_static_field_info(IDX) \
	getField(IDX, meth->class, true, &finfo)

#define field_class()		(finfo.class)

#define	get_class_info(_idx) \
	crinfo = getClass(_idx, meth->class)

#define	class_object()		(crinfo)

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
struct Hjava_lang_Thread;
void virtualMachine(struct _methods*, slots*, slots*, struct Hjava_lang_Thread*);

#endif
