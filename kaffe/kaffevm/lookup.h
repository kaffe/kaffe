/*
 * lookup.h
 * Various lookup calls for resolving objects, methods, exceptions, etc.
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#ifndef __lookup_h
#define __lookup_h

struct _dispatchTable;

typedef struct _callInfo {
	Hjava_lang_Class*	class;
	Method*			method;
	short			in;
	short			out;
	Utf8Const*		name;
	Utf8Const*		signature;
	char			rettype;
} callInfo;

typedef struct _exceptionInfo {
	uintp			handler;
	Hjava_lang_Class*	class;
	Method*			method;
} exceptionInfo;

typedef struct _fieldInfo {
	Field*			field;
	Hjava_lang_Class*	class;
} fieldInfo;

void	getMethodSignatureClass(constIndex, Hjava_lang_Class*, bool, bool, callInfo*);
void	getField(constIndex, Hjava_lang_Class*, bool, fieldInfo*);
Hjava_lang_Class* getClass(constIndex, Hjava_lang_Class*);

Method* findMethod(Hjava_lang_Class*, Utf8Const*, Utf8Const*);
Method* findMethodLocal(Hjava_lang_Class*, Utf8Const*, Utf8Const*);

void	findExceptionInMethod(uintp, Hjava_lang_Class*, exceptionInfo*);
bool	findExceptionBlockInMethod(uintp, Hjava_lang_Class*, Method*, exceptionInfo*);

#endif
