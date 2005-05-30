/*
 * baseClasses.h
 * Handle base classes.
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#ifndef __baseclasses_h
#define __baseclasses_h

#include "object.h"
#include "native.h"

#define	STRINGCLASS	"java/lang/String"
#define	OBJECTCLASS	"java/lang/Object"
#define	CLASSCLASS	"java/lang/Class"

#define	OBJECTCLASSSIG	"Ljava/lang/Object;"

#define	PTRCLASS	"kaffe/util/Ptr"
#define	PTRCLASSSIG	"Lkaffe/util/Ptr;"

void initBaseClasses(void);
void initialiseKaffe(void);

extern struct Hjava_lang_Class*	ObjectClass;
extern struct Hjava_lang_Class*	StringClass;
extern struct Hjava_lang_Class*	RuntimeClass;
extern struct Hjava_lang_Class*	SystemClass;
extern struct Hjava_lang_Class*	SerialClass;
extern struct Hjava_lang_Class*	CloneClass;
extern struct Hjava_lang_Class* PtrClass;
extern struct Hjava_lang_Class* ClassLoaderClass;

extern struct Hjava_lang_Class* javaLangRefReference;
extern struct Hjava_lang_Class* javaLangRefSoftReference;
extern struct Hjava_lang_Class* javaLangRefWeakReference;
extern struct Hjava_lang_Class* javaLangRefPhantomReference;

extern struct Hjava_lang_Class*	javaLangVoidClass;
extern struct Hjava_lang_Class*	javaLangBooleanClass;
extern struct Hjava_lang_Class*	javaLangByteClass;
extern struct Hjava_lang_Class*	javaLangCharacterClass;
extern struct Hjava_lang_Class*	javaLangShortClass;
extern struct Hjava_lang_Class*	javaLangIntegerClass;
extern struct Hjava_lang_Class*	javaLangLongClass;
extern struct Hjava_lang_Class*	javaLangFloatClass;
extern struct Hjava_lang_Class*	javaLangDoubleClass;

extern struct Hjava_lang_Class* javaLangThrowable;
extern struct Hjava_lang_Class* javaLangVMThrowable;
extern struct Hjava_lang_Class* javaLangStackTraceElement;
extern struct Hjava_lang_Class* javaLangException;
extern struct Hjava_lang_Class*	javaLangNullPointerException;
extern struct Hjava_lang_Class*	javaLangArithmeticException;
extern struct Hjava_lang_Class* javaLangArrayIndexOutOfBoundsException;
extern struct Hjava_lang_Class* javaLangClassNotFoundException;
extern struct Hjava_lang_Class* javaLangNoClassDefFoundError;
extern struct Hjava_lang_Class* javaLangStackOverflowError;

extern struct _dispatchTable* getClassVtable(void);
extern Hjava_lang_Class* getStringClass(void);

#endif
