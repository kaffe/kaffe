/*
 * jit_stub.c
 *
 * Copyright (c) 2003, 2004 University of Utah and the Flux Group.
 * All rights reserved.
 *
 * This file is licensed under the terms of the GNU Public License.
 * See the file "license.terms" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 * Contributed by the Flux Research Group, Department of Computer Science,
 * University of Utah, http://www.cs.utah.edu/flux/
 */

#include "config.h"

#if defined(TRANSLATOR)

#include <stdio.h>

#include <jni.h>
#include <kaffe/version.h>

#include "../../libltdl/ltdl.h"
#include "debug.h"
#include "itypes.h"
#include "classMethod.h"
#include "thread.h"
#include "jthread.h"
#include "stringParsing.h"
#include "stringSupport.h"
#include "md.h"

#include <xprof/debugFile.h>

KaffeVM_Arguments vmargs;
JNIEnv* env;
int noExit = 0;

extern int internal_test(parsedString *ps);

/* If not otherwise specified, assume at least 1MB for main thread */
#ifndef MAINSTACKSIZE
#define MAINSTACKSIZE (1024*1024)
#endif

#define	CLASSPATH1	"KAFFECLASSPATH"
#define	CLASSPATH2	"CLASSPATH"
#define BOOTCLASSPATH 	"BOOTCLASSPATH"

#define TEST_CLASSES	"TEST_CLASSES"

extern Hjava_lang_Class* ObjectClass;
extern Hjava_lang_Class* StringClass;
extern Hjava_lang_Class* SystemClass;
extern Hjava_lang_Class* javaLangFloatClass;
extern Hjava_lang_Class* javaLangDoubleClass;
extern Hjava_lang_Class* javaLangThrowable;
extern Hjava_lang_Class* javaLangNullPointerException;
extern Hjava_lang_Class* javaLangArrayIndexOutOfBoundsException;

/* Initialisation prototypes */
void initClasspath(void);
void initNative(void);

int main(int argc, char *argv[])
{
	int threadStackSize;
	int retval = 1;
	char *cp;

	dbgSetMaskStr("MOREJIT,JIT");
	vmargs.version = JNI_VERSION_1_1;
	JNI_GetDefaultJavaVMInitArgs(&vmargs);
	
#if defined(KAFFE_XDEBUGGING)
	machine_debug_filename = "xdb.as";
#endif

	/* set up libtool/libltdl dlopen emulation */
	LTDL_SET_PRELOADED_SYMBOLS();
	
	cp = getenv(BOOTCLASSPATH);
	vmargs.bootClasspath = cp;
	
	cp = getenv(CLASSPATH1);
	if (cp == 0) {
		cp = getenv(CLASSPATH2);
#if defined(DEFAULT_CLASSPATH)
		if (cp == 0) {
			cp = DEFAULT_CLASSPATH;
		}
#endif
	}
	vmargs.classpath = cp;

	/* Machine specific initialisation first */
#if defined(INIT_MD)
	INIT_MD();
#endif

	Kaffe_JavaVMArgs = vmargs;
	threadStackSize = Kaffe_JavaVMArgs.nativeStackSize;
	
	/* Register allocation types with gc subsystem */
	main_collector = initCollector();
	KGC_init(main_collector);

	/* Initialise the (native) threading system */
	initNativeThreads(threadStackSize);

	/* Initialise the string and utf8 systems */
	stringInit();
	utf8ConstInit();

#if defined(KAFFE_XDEBUGGING)
	machine_debug_file = createDebugFile(machine_debug_filename);
#endif
	
	/* Setup CLASSPATH */
	initClasspath();

	/* Init native support */
	initNative();
	
	/* Create the initialise and finalize names and signatures. */
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
		dprintf("not enough memory to run kaffe\n");
		KAFFEVM_ABORT();
	}

	{
		Hjava_lang_Thread mainThread;
		parsedString testName;
		char *tests;
		int stackSize;
		
		initTypes();
		loadStaticClass(&ObjectClass, "java/lang/Object");
		loadStaticClass(&StringClass, "java/lang/String");
		loadStaticClass(&SystemClass, "java/lang/System");
		loadStaticClass(&javaLangFloatClass, "java/lang/Float");
		loadStaticClass(&javaLangDoubleClass, "java/lang/Double");
		loadStaticClass(&javaLangThrowable, "java/lang/Throwable");
		loadStaticClass(&javaLangNullPointerException, "java/lang/NullPointerException");
		loadStaticClass(&javaLangArrayIndexOutOfBoundsException, "java/lang/ArrayIndexOutOfBoundsException");
		memset(&mainThread, 0, sizeof(mainThread));

		if( (tests = getenv(TEST_CLASSES)) )
		{
			retval = parseString(
				tests,
				SPO_NotEmpty,
				  SPO_Do,
				    SPO_NonEmptyString, &testName,
				    SPO_Expect, ".class",
				    SPO_WhileSpace,
				    SPO_Handle, internal_test, &testName,
				    SPO_End,
				  SPO_End,
				SPO_End);
			retval = !retval;
		}
		else
		{
			int lpc;

			for( lpc = 1; lpc < argc; lpc++ )
			{
				testName.data = argv[lpc];
				testName.len = strlen(argv[lpc]);
				internal_test(&testName);
			}
		}
	}
	return( retval );
}

#else

int main(int argc, char *argv[])
{
	return( 0 );
}

#endif
