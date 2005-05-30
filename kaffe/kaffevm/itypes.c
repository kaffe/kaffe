/*
 * itypes.c
 * Internal types.
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

#include "config.h"
#include "config-std.h"
#include "config-mem.h"
#include "debug.h"
#include "classMethod.h"
#include "jni_md.h"
#include "itypes.h"
#include "baseClasses.h"
#include "stringSupport.h"
#include "object.h"
#include "access.h"

Hjava_lang_Class* intClass;
Hjava_lang_Class* longClass;
Hjava_lang_Class* booleanClass;
Hjava_lang_Class* charClass;
Hjava_lang_Class* floatClass; 
Hjava_lang_Class* doubleClass;
Hjava_lang_Class* byteClass; 
Hjava_lang_Class* shortClass;     
Hjava_lang_Class* voidClass;

Hjava_lang_Class* types[MAXTYPES];

static
void
initPrimClass(Hjava_lang_Class** class, const char* name, char sig, int len)
{
	errorInfo info;
	classEntry* centry;
	char entryName[10];
	Hjava_lang_Class* clazz = newClass();
	Utf8Const *uname;

	if (clazz == 0) {
		goto bad;
	}
	(*class) = clazz;
	if (!gc_add_ref(clazz)) {
		goto bad;
	}

	clazz->vtable = _PRIMITIVE_DTABLE;
	clazz->name = utf8ConstNew(name, -1);
	clazz->accflags = ACC_PUBLIC | ACC_FINAL;
	CLASS_PRIM_SIG(clazz) = sig;
        CLASS_PRIM_NAME(clazz) = utf8ConstNew(&sig, 1);
	clazz->this_inner_index = -1;
	if (!clazz->name || !CLASS_PRIM_NAME(clazz)) {
		goto bad;
	}
	TYPE_PRIM_SIZE(clazz) = len;
	/* prevent any attempt to process those in processClass */
	clazz->state = CSTATE_COMPLETE;

	/* Add primitive types to the class pool as well 
	 * This allows us to find them by name---for instance from gcj code.
	 *
	 * Note that a user-defined "class int" is impossible cause "int"
	 * is a keyword.
	 */
	assert(strlen(name) <= 8);
	sprintf(entryName, ";%s", name);
	uname = utf8ConstNew(entryName, -1);
	centry = lookupClassEntry(uname, NULL, &info);
	utf8ConstRelease(uname);

	if (centry == 0) {
		goto bad;
	}
	clazz->centry = centry;
	centry->data.cl = clazz;

	return;
bad:
	dprintf("not enough memory to run kaffe\n");
	KAFFEVM_ABORT();
}

/*
 * Intialise the internal types.
 */
void
initTypes(void)
{
	DBG(INIT, dprintf("initTypes()\n"); );

	initPrimClass(&byteClass, "byte", 'B', 1);
	initPrimClass(&shortClass, "short", 'S', 2);
	initPrimClass(&intClass, "int", 'I', 4);
	initPrimClass(&longClass, "long", 'J', 8);
	initPrimClass(&booleanClass, "boolean", 'Z', 1);
	initPrimClass(&charClass, "char", 'C', 2);
	initPrimClass(&floatClass, "float", 'F', 4);
	initPrimClass(&doubleClass, "double", 'D', 8);
	initPrimClass(&voidClass, "void", 'V', 0);

	TYPE_CLASS(TYPE_Boolean) = booleanClass;
	TYPE_CLASS(TYPE_Char) = charClass;
	TYPE_CLASS(TYPE_Float) = floatClass;
	TYPE_CLASS(TYPE_Double) = doubleClass;
	TYPE_CLASS(TYPE_Byte) = byteClass;
	TYPE_CLASS(TYPE_Short) = shortClass;
	TYPE_CLASS(TYPE_Int) = intClass;
	TYPE_CLASS(TYPE_Long) = longClass;

#if defined(KAFFE_XDEBUGGING)
	byteClass->stab_id = STYPE_BYTE;
	shortClass->stab_id = STYPE_SHORT;
	intClass->stab_id = STYPE_INT;
	longClass->stab_id = STYPE_LONG;
	booleanClass->stab_id = STYPE_BOOLEAN;
	charClass->stab_id = STYPE_CHAR;
	floatClass->stab_id = STYPE_FLOAT;
	doubleClass->stab_id = STYPE_DOUBLE;
	voidClass->stab_id = STYPE_VOID;
#endif
	
	DBG(INIT, dprintf("initTypes() done\n"); );
}

/*
 * Finish the internal types.
 */
void
finishTypes(void)
{
	DBG(INIT, dprintf("finishTypes()\n"); );

	byteClass->head.vtable = getClassVtable();
	shortClass->head.vtable = getClassVtable();
	intClass->head.vtable = getClassVtable();
	longClass->head.vtable = getClassVtable();
	booleanClass->head.vtable = getClassVtable();
	charClass->head.vtable = getClassVtable();
	floatClass->head.vtable = getClassVtable();
	doubleClass->head.vtable = getClassVtable();
	voidClass->head.vtable = getClassVtable();

	DBG(INIT, dprintf("finishTypes() done\n"); );
}

static
Hjava_lang_Class*
classFromSig(const char** strp, Hjava_lang_ClassLoader* loader, errorInfo *einfo)
{
	Hjava_lang_Class* cl;
	Utf8Const* utf8;
	const char* start;
	const char* end;

	switch (*(*strp)++) {
	case 'V': return (voidClass);
	case 'I': return (intClass);
	case 'Z': return (booleanClass);
	case 'S': return (shortClass);
	case 'B': return (byteClass);
	case 'C': return (charClass);
	case 'F': return (floatClass);
	case 'D': return (doubleClass);
	case 'J': return (longClass);
	case '[': return (lookupArray(classFromSig(strp, loader, einfo),
				      einfo));
	case 'L':
		start = *strp;
		for (end = start; *end != 0 && *end != ';'; end++)
			;
		if (*end != ';') {
			postException(einfo, JAVA_LANG(VerifyError));
			return (NULL);
		}
		*strp = end + 1;
		utf8 = utf8ConstNew(start, end - start);
		if (!utf8) {
			postOutOfMemory(einfo);
			return (NULL);
		}
		cl = loadClass(utf8, loader, einfo);
		utf8ConstRelease(utf8);
		return(cl);

	default:
		/* malformed signature */
		postException(einfo, JAVA_LANG(VerifyError));
		return (NULL);
	}
}

/*
 * Get a class by name from loader 'loader'
 * Fails if there are extraneous characters as in "Ljava/lang/Object;V"
 *
 * To be used if "sig" contains exactly one class name.
 */
Hjava_lang_Class*
getClassFromSignature(const char* sig, Hjava_lang_ClassLoader* loader, errorInfo *einfo)
{
	const char *const sig0 = sig;
	Hjava_lang_Class *cls = classFromSig(&sig, loader, einfo);

	/* name must consume all characters in string */
	if (cls != NULL && *sig == '\0') {
		return (cls);
	}
	/* 
	 * or rather VerifyError?  It could be a malformed sig in a malformed
	 * .class file, or it could be a malformed user input from 
	 * Class.forName()
	 */
	postExceptionMessage(einfo, JAVA_LANG(NoClassDefFoundError), "%s", sig0);
	return (NULL);
}

/*
 * Like getClassFromSignature, except don't check that there are no
 * extraneous characters after a parsed name.
 *
 * Can be used if "sig" is a pointer in a signature, i.e., is a signature part.
 */
Hjava_lang_Class*
getClassFromSignaturePart(const char* sig, Hjava_lang_ClassLoader* loader, errorInfo *einfo)
{
	return (classFromSig(&sig, loader, einfo));
}
