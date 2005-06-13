/*
 * baseClasses.c
 * Handle base classes.
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

#include "debug.h"

#include "config.h"
#include "config-std.h"
#include "config-mem.h"
#include "config-signal.h"
#include "config-hacks.h"
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
#include "xprofiler.h"
#if defined(KAFFE_FEEDBACK)
#include "feedback.h"
#endif
#include "debugFile.h"
#include "fileSections.h"
#include "verify-type.h"
#include "jar.h"

Utf8Const* init_name;
Utf8Const* final_name;
Utf8Const* void_signature;
Utf8Const* constructor_name;
Utf8Const* Code_name;
Utf8Const* LineNumberTable_name;
Utf8Const* LocalVariableTable_name;
Utf8Const* ConstantValue_name;
Utf8Const* Exceptions_name;
Utf8Const* SourceFile_name;
Utf8Const* InnerClasses_name;

static struct _dispatchTable *ClassClass_vtable;
static Hjava_lang_Class*	ClassClass;
Hjava_lang_Class* StringClass;
Hjava_lang_Class* ObjectClass;
Hjava_lang_Class* SystemClass;
Hjava_lang_Class* RuntimeClass;
Hjava_lang_Class* SerialClass;
Hjava_lang_Class* CloneClass;
Hjava_lang_Class* PtrClass;
Hjava_lang_Class* ClassLoaderClass;

Hjava_lang_Class* javaLangRefReference;
Hjava_lang_Class* javaLangRefSoftReference;
Hjava_lang_Class* javaLangRefWeakReference;
Hjava_lang_Class* javaLangRefPhantomReference;

Hjava_lang_Class* javaLangVoidClass;
Hjava_lang_Class* javaLangBooleanClass;
Hjava_lang_Class* javaLangByteClass;
Hjava_lang_Class* javaLangCharacterClass;
Hjava_lang_Class* javaLangShortClass;
Hjava_lang_Class* javaLangIntegerClass;
Hjava_lang_Class* javaLangLongClass;
Hjava_lang_Class* javaLangFloatClass;
Hjava_lang_Class* javaLangDoubleClass;

Hjava_lang_Class* javaLangThrowable;
Hjava_lang_Class* javaLangVMThrowable;
Hjava_lang_Class* javaLangStackTraceElement;
Hjava_lang_Class* javaLangException;
Hjava_lang_Class* javaLangArrayIndexOutOfBoundsException;
Hjava_lang_Class* javaLangNullPointerException;
Hjava_lang_Class* javaLangArithmeticException;
Hjava_lang_Class* javaLangClassNotFoundException;
Hjava_lang_Class* javaLangNoClassDefFoundError;
Hjava_lang_Class* javaLangStackOverflowError;
/* Let's not load this if we can't open Klasses.jar */
Hjava_lang_Class* javaIoIOException;

static Hjava_lang_Class** stateCompleteClass[] = {
  &StringClass,
  &javaLangThrowable,
  &javaLangVMThrowable,
  &javaLangStackTraceElement,
  &javaLangException,
  &javaLangArrayIndexOutOfBoundsException,
  &javaLangNullPointerException,
  &javaLangArithmeticException,
  &javaLangClassNotFoundException,
  &javaLangNoClassDefFoundError,
  &javaLangStackOverflowError,
  NULL
};

#define RUNTIMECLASS "java/lang/Runtime"
#define SYSTEMCLASS  "java/lang/System"
#define	SERIALCLASS  "java/io/Serializable"
#define	CLONECLASS   "java/lang/Cloneable"
#define	LOADERCLASS  "java/lang/ClassLoader"
#define PTRCLASS     "kaffe/util/Ptr"

/* Initialisation prototypes */
void initClasspath(void);
void initNative(void);

/**
 * get the java.lang.Class class.
 * Load it if it is not loaded.
 */
struct _dispatchTable*
getClassVtable(void)
{
  return ClassClass_vtable;
}

/**
 * get the java.lang.String class.
 * Load it if it is not loaded.
 */
Hjava_lang_Class* 
getStringClass(void)
{
  if (StringClass == NULL) {
    loadStaticClass(&StringClass, STRINGCLASS);
  }

  return StringClass;
}

/*
 * Initialise the machine.
 *
 * XXX: fix the naming conventions at some point (initXXX vs. xxxInit)
 */
void
initialiseKaffe(void)
{
        /* Set default thread stack size if not set */
	int threadStackSize;

	/* Machine specific initialisation first */
#if defined(INIT_MD)
	INIT_MD();
#endif

	/* Register allocation types with gc subsystem */
	main_collector = initCollector();
	KGC_init(main_collector);

#if defined(KAFFE_XPROFILER)
	/* Start up the profiler here so we can cover init stuff */
	if( xProfFlag )
	{
		if( !enableXProfiling() )
		{
			DBG(INIT,
				dprintf(
				"Unable to initialize cross "
				"language profiling\n");
			);
		}
	}
#endif
#if defined(KAFFE_XDEBUGGING)
	if( machine_debug_filename )
	{
		machine_debug_file = createDebugFile(machine_debug_filename);
	}
#endif
        threadStackSize = Kaffe_JavaVMArgs.nativeStackSize;

        if (threadStackSize == 0) {
                threadStackSize = THREADSTACKSIZE;
        }

	/* Initialise the (native) threading system */
	initNativeThreads(threadStackSize);

	initLocking();
	initEngine();
	KaffeVM_initClassPool();
	KaffeVM_initJarCache();

	/* Initialise the string and utf8 systems */
	stringInit();
	utf8ConstInit();

	/* Initialize the reference tracking subsystem */
	KaffeVM_referenceInit();

	/* Setup CLASSPATH */
	initClasspath();

	/* Init native support */
	initNative();

#if defined(KAFFE_FEEDBACK)
	/* Install any file sections used by feedback */
	installFileSections();
	if( feedback_filename )
	{
		if( feedbackFile(feedback_filename) )
		{
			syncFeedback();
			processFeedback();
		}
	}
#endif

	/* Create the initialise and finalize names and signatures. */
	DBG(INIT, dprintf("create names and signatures\n"); );

	init_name = utf8ConstNew("<clinit>", -1);
	final_name = utf8ConstNew("finalize", -1);
	void_signature = utf8ConstNew("()V", -1);
	constructor_name = utf8ConstNew("<init>", -1);
	Code_name = utf8ConstNew("Code", -1);
	LineNumberTable_name = utf8ConstNew("LineNumberTable", -1);
	LocalVariableTable_name = utf8ConstNew("LocalVariableTable", -1);
	ConstantValue_name = utf8ConstNew("ConstantValue", -1);
	Exceptions_name = utf8ConstNew("Exceptions", -1);
	SourceFile_name = utf8ConstNew("SourceFile", -1);
	InnerClasses_name = utf8ConstNew("InnerClasses", -1);

	if (!(init_name && final_name && void_signature &&
	      constructor_name && Code_name && LineNumberTable_name &&
	      LocalVariableTable_name && ConstantValue_name &&
	      Exceptions_name && SourceFile_name && InnerClasses_name)) {
		DBG(INIT, dprintf("not enough memory to run kaffe\n"); );
		KAFFEVM_ABORT();
	}

	DBG(INIT, dprintf("done\n"); );

#if defined(HAVE_GCJ_SUPPORT)
	/* Init GCJ support */
	gcjInit();
#endif

	/*
	 * Read in base classes.  No thread context at this point, so
	 * errors here are really hard to detect cleanly.
	 */
	initBaseClasses();

#if defined(HAVE_GCJ_SUPPORT)
	/* tell gcj where primitive classes are */
	gcjInitPrimitiveClasses();
#endif

	/* Setup exceptions */
	initExceptions();

	/* Init stuff for the java security model */
	initialiseSecurity();

	/* Init thread support */
	initThreads();

	/* Now enable collector */
	KGC_enable(main_collector);
}

static void
abortWithEarlyClassFailure(errorInfo* einfo)
{
	DBG(INIT,
	dprintf("\n"
		"Failure loading and/or initializing a critical class.\n"
		"This failure occured too early in the VM startup, and is\n"
		"indicative of bug in the initialization, or a insufficient\n"
		"stack space or heap space to complete initialization.\n");
	);

	/* XXX print einfo. */
	DBG(INIT,
	dprintf("*einfo: type=%d;\n\tclassname=`%s';\n\tmess=`%s'\n",
		einfo->type, einfo->classname, einfo->mess);
	);

	KAFFEVM_EXIT(-1);
}

/*
 * We need to use certain classes in the internal machine so we better
 * get them in now in a known way so we can refer back to them.
 */
void
initBaseClasses(void)
{
	errorInfo einfo;
	int i;

	DBG(INIT, dprintf("initBaseClasses()\n"); );

	/* Primitive types */
	initTypes();
	initVerifierPrimTypes();

	loadStaticClass(&ObjectClass, OBJECTCLASS);
	loadStaticClass(&SerialClass, SERIALCLASS);
	loadStaticClass(&CloneClass,  CLONECLASS);
	loadStaticClass(&ClassClass,  CLASSCLASS);

	ClassClass_vtable = ClassClass->vtable;
	ObjectClass->head.vtable = ClassClass_vtable;
	SerialClass->head.vtable = ClassClass_vtable;
	CloneClass->head.vtable  = ClassClass_vtable;
	ClassClass->head.vtable  = ClassClass_vtable;

	finishTypes();

	loadStaticClass(&StringClass, STRINGCLASS);
	loadStaticClass(&SystemClass, SYSTEMCLASS);
        loadStaticClass(&RuntimeClass, RUNTIMECLASS);

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
	loadStaticClass(&ClassLoaderClass, LOADERCLASS);

	/* Exception handling types */
	loadStaticClass(&javaLangThrowable, "java/lang/Throwable");
	loadStaticClass(&javaLangVMThrowable, "java/lang/VMThrowable");
	loadStaticClass(&javaLangStackTraceElement, "java/lang/StackTraceElement");
	loadStaticClass(&javaLangException, "java/lang/Exception");
	loadStaticClass(&javaLangArrayIndexOutOfBoundsException, "java/lang/ArrayIndexOutOfBoundsException");
	loadStaticClass(&javaLangNullPointerException, "java/lang/NullPointerException");
	loadStaticClass(&javaLangArithmeticException, "java/lang/ArithmeticException");
	loadStaticClass(&javaLangClassNotFoundException, "java/lang/ClassNotFoundException");
	loadStaticClass(&javaLangNoClassDefFoundError, "java/lang/NoClassDefFoundError");
	loadStaticClass(&javaLangStackOverflowError, "java/lang/StackOverflowError");
	loadStaticClass(&javaIoIOException, "java/io/IOException");

	/* Object references. */
	loadStaticClass(&javaLangRefReference, "java/lang/ref/Reference");
	loadStaticClass(&javaLangRefWeakReference, "java/lang/ref/WeakReference");
	loadStaticClass(&javaLangRefSoftReference, "java/lang/ref/SoftReference");
	loadStaticClass(&javaLangRefPhantomReference, "java/lang/ref/PhantomReference");

	DBG(INIT, dprintf("initBaseClasses() done\n"); );

	for (i = 0; stateCompleteClass[i] != NULL; i++) {
	    if (!processClass(*stateCompleteClass[i], CSTATE_COMPLETE, &einfo))
	      abortWithEarlyClassFailure(&einfo);
	}
}

