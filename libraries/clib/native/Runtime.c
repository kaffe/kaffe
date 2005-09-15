/*
 * java.lang.Runtime.c
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
#include "config-io.h"
#include "gtypes.h"
#include <native.h>
#include "files.h"
#include "defs.h"
#include "java_io_File.h"
#include "java_lang_String.h"
#include "java_lang_VMRuntime.h"
#include "external.h"
#include "gc.h"
#include "support.h"
#include "stringSupport.h"
#include "external.h"

#ifndef LIBRARYPREFIX
#define	LIBRARYPREFIX	"lib"
#endif

#ifndef LIBRARYSUFFIX
#define LIBRARYSUFFIX	LTDL_SHLIB_EXT
#endif

extern jboolean runFinalizerOnExit;

/*
 * Exit this VM
 */
void
java_lang_VMRuntime_exit(jint v)
{
	KAFFEVM_EXIT (v);
}

/*
 * Free memory.
 */
jlong
java_lang_VMRuntime_freeMemory(void)
{
	return KGC_getHeapFree(main_collector);
}

/*
 * Maximally available memory.
 */
jlong
java_lang_VMRuntime_maxMemory(void)
{
	jlong max = KGC_getHeapLimit(main_collector);

	if (max <= 0) {
		return 0x7fffffffffffffffLL;
	}
	else {
		return max;
	}
}

/*
 * Total memory.
 */
jlong
java_lang_VMRuntime_totalMemory(void)
{
	return KGC_getHeapTotal(main_collector);
}

/*
 * Run the garbage collector.
 */
void
java_lang_VMRuntime_gc(void)
{
	invokeGC();
}

/*
 * Run any pending finialized methods.
 *  Finalising is part of the garbage collection system - so just run that.
 */
void
java_lang_VMRuntime_runFinalization(void)
{
	invokeGC();
}

/*
 * Invoke the finalizer for all finalizable objects.
 */
void
java_lang_VMRuntime_runFinalizationForExit(void)
{
  invokeFinalizer();
}

/*
 * Enable/disable tracing of instructions.
 */
void
java_lang_VMRuntime_traceInstructions(jboolean on)
{
	if (on == true) {
		SignalError("java.lang.RuntimeException", "Cannot trace instructions");   
	}
}

/*
 * Enable/disable tracing of method calls.
 */
void
java_lang_VMRuntime_traceMethodCalls(jboolean on)
{
	if (on == true) {
		SignalError("java.lang.RuntimeException", "Cannot trace method calls");   
	}
}

/*
 * Inform the runtime that it must run the finalizer when it exits.
 */
void
java_lang_VMRuntime_runFinalizersOnExit(jboolean on)
{
	runFinalizerOnExit = on;
}


/*
 * Attempt to link in a shared library. Return false
 * if the attempt fails, true otherwise.
 */

jint
java_lang_VMRuntime_nativeLoad(struct Hjava_lang_String *jpath, struct Hjava_lang_ClassLoader* loader)
{
	char path[MAXPATHLEN];
	char errbuf[128];

	stringJava2CBuf(jpath, path, sizeof(path));
	if (loadNativeLibrary(path, loader, errbuf, sizeof(errbuf)) < 0) {
		return false;
	}
	return true;
}

struct Hjava_lang_String*
java_lang_VMRuntime_getLibPrefix(void)
{
	return checkPtr(stringC2Java(LIBRARYPREFIX));
}

struct Hjava_lang_String*
java_lang_VMRuntime_getLibSuffix(void)
{
	return checkPtr(stringC2Java(LIBRARYSUFFIX));
}

