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

#include "debug.h"

#include "config.h"
#include "config-std.h"
#include "config-mem.h"
#include "config-signal.h"
#include "gtypes.h"
#include "access.h"
#include "object.h"
#include "constants.h"
#include "errors.h"
#include "baseClasses.h"
#include "classMethod.h"
#include "stringSupport.h"
#include "thread.h"
#include "lookup.h"
#include "slots.h"
#include "machine.h"
#include "exception.h"
#include "itypes.h"
#include "gc.h"
#include "md.h"
#include "java_lang_Cloneable.h"
#include "gcj/gcj.h"

Utf8Const* init_name;
Utf8Const* final_name;
Utf8Const* void_signature;
Utf8Const* constructor_name;
Utf8Const* Code_name;
Utf8Const* LineNumberTable_name;
Utf8Const* ConstantValue_name;
Utf8Const* Exceptions_name;
Utf8Const* SourceFile_name;

static Hjava_lang_Class dummyClassClass;
Hjava_lang_Class* ClassClass = &dummyClassClass;
Hjava_lang_Class* StringClass;
Hjava_lang_Class* ObjectClass;
Hjava_lang_Class* SystemClass;
Hjava_lang_Class* SerialClass;
Hjava_lang_Class* CloneClass;
Hjava_lang_Class* PtrClass;

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
Hjava_lang_Class* javaLangNullPointerException;
Hjava_lang_Class* javaLangArithmeticException;

/* Let's not load this if we can't open Klasses.jar */
Hjava_lang_Class* javaIoIOException;

#define SYSTEMCLASS "java/lang/System"
#define	SERIALCLASS "java/io/Serializable"
#define	CLONECLASS  "java/lang/Cloneable"
#define PTRCLASS    "kaffe/util/Ptr"

#define	INIT			"<clinit>"
#define	FINAL			"finalize"
#define	VOIDSIG			"()V"
#define	CONSTRUCTOR_NAME	"<init>"

/* Initialisation prototypes */
void initClasspath(void);
void initNative(void);
void initThreads(void);

/*
 * Initialise the machine.
 *
 * XXX: fix the naming conventions at some point (initXXX vs. xxxInit)
 */
void
initialiseKaffe(void)
{
        /* Set default thread stack size if not set */
        extern JavaVMInitArgs Kaffe_JavaVMArgs[];
	int threadStackSize;

	/* Machine specific initialisation first */
#if defined(INIT_MD)
	INIT_MD();
#endif

	/* Register allocation types with gc subsystem */
	main_collector = initCollector();
	GC_init(main_collector);

        threadStackSize = Kaffe_JavaVMArgs[0].nativeStackSize;
 
        if (threadStackSize == 0) {
                threadStackSize = THREADSTACKSIZE;
        }
  
	/* Initialise the (native) threading system */
	initNativeThreads(threadStackSize);

	/* Initialise the string and utf8 systems */
	stringInit();
	utf8ConstInit();

	/* Setup CLASSPATH */
	initClasspath();

	/* Init native support */
	initNative();

	/* Create the initialise and finalize names and signatures. */
	init_name = utf8ConstNew(INIT, -1);
	final_name = utf8ConstNew(FINAL, -1);
	void_signature = utf8ConstNew(VOIDSIG, -1);
	constructor_name = utf8ConstNew(CONSTRUCTOR_NAME, -1);
	Code_name = utf8ConstNew("Code", -1);
	LineNumberTable_name = utf8ConstNew("LineNumberTable", -1);
	ConstantValue_name = utf8ConstNew("ConstantValue", -1);
	Exceptions_name = utf8ConstNew("Exceptions", -1);
	SourceFile_name = utf8ConstNew("SourceFile", -1);

	if (!(init_name && final_name && void_signature &&
	      constructor_name && Code_name && LineNumberTable_name &&
	      ConstantValue_name && Exceptions_name &&
	      SourceFile_name)) {
		fprintf(stderr, "not enough memory to run kaffe\n");
		ABORT();
	}
#if defined(HAVE_GCJ_SUPPORT)
	/* Init GCJ support */
	gcjInit();
#endif

	/* Read in base classes */
	initBaseClasses();

	/* Setup exceptions */
	initExceptions();

	/* Seup locking */
	initLocking();

	/* Init thread support */
	initThreads();

	/* Now enable collector */
	GC_enable(main_collector);
}

/*
 * We need to use certain classes in the internal machine so we better
 * get them in now in a known way so we can refer back to them.
 */
void
initBaseClasses(void)
{
	Field *f;
	errorInfo einfo;
	Utf8Const *utf8;
	extern char* realClassPath;

	/* Primitive types */
	initTypes();

	/* The base types */
	loadStaticClass(&ObjectClass, OBJECTCLASS);
	loadStaticClass(&SerialClass, SERIALCLASS);
	loadStaticClass(&CloneClass, CLONECLASS);
	loadStaticClass(&ClassClass, CLASSCLASS);
	loadStaticClass(&StringClass, STRINGCLASS);
	loadStaticClass(&SystemClass, SYSTEMCLASS);

	/* We must to a little cross tidying */
	ObjectClass->head.dtable = ClassClass->dtable;
	SerialClass->head.dtable = ClassClass->dtable;
	CloneClass->head.dtable = ClassClass->dtable;
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
	loadStaticClass(&PtrClass, PTRCLASS);

	/* Exception handling types */
	loadStaticClass(&javaLangArrayIndexOutOfBoundsException, "java/lang/ArrayIndexOutOfBoundsException");
	loadStaticClass(&javaLangNullPointerException, "java/lang/NullPointerException");
	loadStaticClass(&javaLangArithmeticException, "java/lang/ArithmeticException");
	loadStaticClass(&javaIoIOException, "java/io/IOException");

	/* Fixup primitive types */
	finishTypes();
	processClass(StringClass, CSTATE_COMPLETE, &einfo);

	/* 
	 * To make sure we have some ground to stand on, we doublecheck
	 * that we really got Kaffe's java.lang.Cloneable class here.
	 * Kaffe's class has a final public static field called 
	 * "KAFFE_VERSION".
	 */
	utf8 = utf8ConstNew("KAFFE_VERSION", -1);
	if (!utf8) {
		fprintf(stderr, "not enough memory to run kaffe\n");
		ABORT();
	}
	f = lookupClassField(CloneClass, utf8, true, &einfo);
	utf8ConstRelease(utf8);
	if (f == 0) {
		fprintf(stderr,
		    "\nCould not initialize Kaffe.\n"
		    "It's likely that your CLASSPATH settings are wrong.  "
		    "Please make sure\nyour CLASSPATH does not include any "
		    "java.lang.* classes from other JVM\nvendors, such as "
		    "Sun's classes.zip, BEFORE Kaffe's Klasses.jar.\n"
		    "It is okay to have classes.zip AFTER Klasses.jar\n\n"
		    "The current effective classpath is `%s'\n\n", 
		    realClassPath
		);
		EXIT(-1);
	}

	if (*(jint*)FIELD_ADDRESS(f) != java_lang_Cloneable_KAFFE_VERSION) {
		fprintf(stderr,
		    "\nCould not initialize Kaffe.\n"
		    "Your Klasses.jar version is %3.2f, but this VM "
		    "was compiled with version %3.2f\n\n"
		    "The current effective classpath is `%s'\n\n", 
		    *(jint*)FIELD_ADDRESS(f)/100.0, 
		    java_lang_Cloneable_KAFFE_VERSION/100.0,
		    realClassPath
		);
		EXIT(-1);
	}
}

