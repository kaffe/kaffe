/*
 * baseClasses.c
 * Handle base classes.
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#define	DBG(s)

#include "config.h"
#include "config-std.h"
#include "config-mem.h"
#include "config-signal.h"
#include "gtypes.h"
#include "access.h"
#include "object.h"
#include "constants.h"
#include "classMethod.h"
#include "baseClasses.h"
#include "thread.h"
#include "lookup.h"
#include "slots.h"
#include "machine.h"
#include "exception.h"
#include "itypes.h"
#include "md.h"

Utf8Const* init_name;
Utf8Const* void_signature;
Utf8Const* final_name;
Utf8Const* constructor_name;
Utf8Const* Code_name;
Utf8Const* LineNumberTable_name;
Utf8Const* ConstantValue_name;

static Hjava_lang_Class dummyClassClass;
Hjava_lang_Class* ClassClass = &dummyClassClass;
Hjava_lang_Class* StringClass;
Hjava_lang_Class* ObjectClass;
Hjava_lang_Class* SystemClass;
Hjava_lang_Class* SerialClass;

Hjava_lang_Class* javaLangVoidClass;
Hjava_lang_Class* javaLangBooleanClass;
Hjava_lang_Class* javaLangByteClass;
Hjava_lang_Class* javaLangCharacterClass; 
Hjava_lang_Class* javaLangShortClass;
Hjava_lang_Class* javaLangIntegerClass;  
Hjava_lang_Class* javaLangLongClass;
Hjava_lang_Class* javaLangFloatClass;
Hjava_lang_Class* javaLangDoubleClass;

Hjava_lang_Class* javaLangArrayIndexOutOfBoundsException;

#define SYSTEMCLASS "java/lang/System"
#define	SERIALCLASS "java/io/Serializable"

#define	INIT			"<clinit>"
#define	VOIDSIG			"()V"
#define	FINAL			"finalize"
#define	CONSTRUCTOR_NAME	"<init>"

/* Initialisation prototypes */
void initClasspath(void);
void initExceptions(void);
void initNative(void);
void initThreads(void);
void initTypes(void);

/*
 * Initialise the machine.
 */
void
initialiseKaffe(void)
{
	Method* meth;

	/* Machine specific initialisation first */
#if defined(INIT_MD)
	INIT_MD();
#endif

	/* Setup CLASSPATH */
	initClasspath();

	/* Init native support */
	initNative();

	/* Create the initialise and finalize names and signatures. */
	init_name = makeUtf8ConstFixed(INIT, -1);
	void_signature = makeUtf8ConstFixed(VOIDSIG, -1);
	final_name = makeUtf8ConstFixed(FINAL, -1);
	constructor_name = makeUtf8ConstFixed(CONSTRUCTOR_NAME, -1);
	Code_name = makeUtf8ConstFixed("Code", -1);
	LineNumberTable_name = makeUtf8ConstFixed("LineNumberTable", -1);
	ConstantValue_name = makeUtf8ConstFixed("ConstantValue", -1);

	/* Read in base classes */
	initBaseClasses();

	/* Setup exceptions */
	initExceptions();

	/* Init thread support */
	initThreads();

	/* Initialise the System */
	do_execute_java_class_method("java.lang.System", "initializeSystemClass", "()V");
}

/*
 * We need to use certain classes in the internal machine so we better
 * get them in now in a known way so we can refer back to them.
 * Currently we need java/lang/Object, java/lang/Class, java/lang/String
 * and java/lang/System.
 */
void
initBaseClasses(void)
{
	/* Primitive types */
	initTypes();

	/* The base types */
	loadStaticClass(&ObjectClass, OBJECTCLASS);
	loadStaticClass(&SerialClass, SERIALCLASS);
	loadStaticClass(&ClassClass, CLASSCLASS);
	loadStaticClass(&StringClass, STRINGCLASS);
	loadStaticClass(&SystemClass, SYSTEMCLASS);

	/* We must to a little cross tidying */
	ObjectClass->head.dtable = ClassClass->dtable;
	SerialClass->head.dtable = ClassClass->dtable;
	ClassClass->head.dtable = ClassClass->dtable;

	/* Basic types classes */
	loadStaticClass(&javaLangVoidClass, "java/lang/Void");
	loadStaticClass(&javaLangBooleanClass, "java/lang/Boolean");
	loadStaticClass(&javaLangByteClass, "java/lang/Byte");
	loadStaticClass(&javaLangCharacterClass, "java/lang/Character");
	loadStaticClass(&javaLangShortClass, "java/lang/Short");
	loadStaticClass(&javaLangIntegerClass, "java/lang/Integer");
	loadStaticClass(&javaLangLongClass, "java/lang/Long");
	loadStaticClass(&javaLangFloatClass, "java/lang/Float");
	loadStaticClass(&javaLangDoubleClass, "java/lang/Double");

	/* Exception handling types */
	loadStaticClass(&javaLangArrayIndexOutOfBoundsException, "java/lang/ArrayIndexOutOfBoundsException");

	/* Fixup primitive types */
	finishTypes();
}

/*
 * Setup the internal exceptions.
 */
void
initExceptions(void)
{
DBG(	printf("initExceptions()\n");				)
#if !defined(DEBUG)
	/* Catch signal we need to convert to exceptions */
#if defined(SIGSEGV)
	catchSignal(SIGSEGV, nullException);
#endif
#if defined(SIGBUS)
	catchSignal(SIGBUS, nullException);
#endif
#if defined(SIGFPE)
	catchSignal(SIGFPE, arithmeticException);
#endif
#if defined(SIGPIPE)
	catchSignal(SIGPIPE, SIG_IGN);
#endif
#endif
}
