/*
 * itypes.c
 * Internal types.
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#include "config.h"
#include "config-std.h"
#include "config-mem.h"
#include "classMethod.h"
#include "jtypes.h"
#include "itypes.h"
#include "baseClasses.h"
#include "stringSupport.h"
#include "object.h"

Hjava_lang_Class* _Jv_intClass;
Hjava_lang_Class* _Jv_longClass;
Hjava_lang_Class* _Jv_booleanClass;
Hjava_lang_Class* _Jv_charClass;
Hjava_lang_Class* _Jv_floatClass; 
Hjava_lang_Class* _Jv_doubleClass;
Hjava_lang_Class* _Jv_byteClass; 
Hjava_lang_Class* _Jv_shortClass;     
Hjava_lang_Class* _Jv_voidClass;

extern gcFuncs gcClassObject;

Hjava_lang_Class* types[MAXTYPES];

static
void
initPrimClass(Hjava_lang_Class** class, char* name, char sig, int len)
{
	Hjava_lang_Class* clazz = newClass();
	(*class) = clazz;
	gc_add_ref(clazz);

	clazz->dtable = _PRIMITIVE_DTABLE;
	clazz->name = utf8ConstNew(name, -1);
	CLASS_PRIM_SIG(clazz) = sig;
        CLASS_PRIM_NAME(clazz) = utf8ConstNew(&sig, 1);
	TYPE_PRIM_SIZE(clazz) = len;
}

/*
 * Intialise the internal types.
 */
void
initTypes(void)
{
	initPrimClass(&_Jv_byteClass, "byte", 'B', 1);
	initPrimClass(&_Jv_shortClass, "short", 'S', 2);
	initPrimClass(&_Jv_intClass, "int", 'I', 4);
	initPrimClass(&_Jv_longClass, "long", 'J', 8);
	initPrimClass(&_Jv_booleanClass, "boolean", 'Z', 1);
	initPrimClass(&_Jv_charClass, "char", 'C', 2);
	initPrimClass(&_Jv_floatClass, "float", 'F', 4);
	initPrimClass(&_Jv_doubleClass, "double", 'D', 8);
	initPrimClass(&_Jv_voidClass, "void", 'V', 0);

	TYPE_CLASS(TYPE_Boolean) = _Jv_booleanClass;
	TYPE_CLASS(TYPE_Char) = _Jv_charClass;
	TYPE_CLASS(TYPE_Float) = _Jv_floatClass;
	TYPE_CLASS(TYPE_Double) = _Jv_doubleClass;
	TYPE_CLASS(TYPE_Byte) = _Jv_byteClass;
	TYPE_CLASS(TYPE_Short) = _Jv_shortClass;
	TYPE_CLASS(TYPE_Int) = _Jv_intClass;
	TYPE_CLASS(TYPE_Long) = _Jv_longClass;
}

/*
 * Finish the internal types.
 */
void
finishTypes(void)
{
	_Jv_byteClass->head.dtable = ClassClass->dtable;
	_Jv_shortClass->head.dtable = ClassClass->dtable;
	_Jv_intClass->head.dtable = ClassClass->dtable;
	_Jv_longClass->head.dtable = ClassClass->dtable;
	_Jv_booleanClass->head.dtable = ClassClass->dtable;
	_Jv_charClass->head.dtable = ClassClass->dtable;
	_Jv_floatClass->head.dtable = ClassClass->dtable;
	_Jv_doubleClass->head.dtable = ClassClass->dtable;
	_Jv_voidClass->head.dtable = ClassClass->dtable;
}

Hjava_lang_Class*
getClassFromSignature(const char* sig, Hjava_lang_ClassLoader* loader, errorInfo *einfo)
{
	return (classFromSig(&sig, loader, einfo));
}

Hjava_lang_Class*
classFromSig(const char** strp, Hjava_lang_ClassLoader* loader, errorInfo *einfo)
{
	Hjava_lang_Class* cl;
	Utf8Const* utf8;
	const char* start;
	const char* end;

	switch (*(*strp)++) {
	case 'V': return (_Jv_voidClass);
	case 'I': return (_Jv_intClass);
	case 'Z': return (_Jv_booleanClass);
	case 'S': return (_Jv_shortClass);
	case 'B': return (_Jv_byteClass);
	case 'C': return (_Jv_charClass);
	case 'F': return (_Jv_floatClass);
	case 'D': return (_Jv_doubleClass);
	case 'J': return (_Jv_longClass);
	case '[': return (lookupArray(classFromSig(strp, loader, einfo)));
	case 'L':
		start = *strp;
		for (end = start; *end != 0 && *end != ';'; end++)
			;
		*strp = end;
		if (*end != 0) {
			(*strp)++;
		}
		utf8 = utf8ConstNew(start, end - start);
		cl = loadClass(utf8, loader, einfo);
		utf8ConstRelease(utf8);
		return(cl);
	default:
		/* malformed signature */
		SET_LANG_EXCEPTION(einfo, VerifyError);
		return (NULL);
	}
}
