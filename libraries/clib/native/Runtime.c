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
#include "java_lang_Runtime.h"
#include "external.h"
#include "gc.h"
#include "support.h"
#include "stringSupport.h"

extern jboolean runFinalizerOnExit;

/*
 * Exit this VM
 */
void
NONRETURNING
java_lang_Runtime_exit0(struct Hjava_lang_Runtime* r UNUSED, jint v)
{
	EXIT (v);
}

/*
 * Exec another program.
 */
struct Hjava_lang_Process*
java_lang_Runtime_execInternal(struct Hjava_lang_Runtime* this UNUSED,
		HArrayOfObject* argv, HArrayOfObject* arge, Hjava_io_File* dir)
{
	return (struct Hjava_lang_Process*)execute_java_constructor(
	    "kaffe.lang.UNIXProcess", 0, 0,
	    "([Ljava/lang/String;[Ljava/lang/String;Ljava/io/File;)V",
	    argv, arge, dir);
}

/*
 * Free memory.
 */
jlong
java_lang_Runtime_freeMemory(struct Hjava_lang_Runtime* this UNUSED)
{
	return KGC_getHeapFree(main_collector);
}

/*
 * Maximally available memory.
 */
jlong
java_lang_Runtime_maxMemory(struct Hjava_lang_Runtime* this UNUSED)
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
java_lang_Runtime_totalMemory(struct Hjava_lang_Runtime* this UNUSED)
{
	return KGC_getHeapTotal(main_collector);
}

/*
 * Run the garbage collector.
 */
void
java_lang_Runtime_gc(struct Hjava_lang_Runtime* this UNUSED)
{
	invokeGC();
}

/*
 * Run any pending finialized methods.
 *  Finalising is part of the garbage collection system - so just run that.
 */
void
java_lang_Runtime_runFinalization(struct Hjava_lang_Runtime* this UNUSED)
{
	invokeGC();
}

/*
 * Enable/disable tracing of instructions.
 */
void
java_lang_Runtime_traceInstructions(struct Hjava_lang_Runtime* this UNUSED, jboolean on)
{
	if (on == true) {
		SignalError("java.lang.RuntimeException", "Cannot trace instructions");   
	}
}

/*
 * Enable/disable tracing of method calls.
 */
void
java_lang_Runtime_traceMethodCalls(struct Hjava_lang_Runtime* this UNUSED, jboolean on)
{
	if (on == true) {
		SignalError("java.lang.RuntimeException", "Cannot trace method calls");   
	}
}

/*
 * Inform the runtime that it must run the finalizer when it exits.
 */
void
java_lang_Runtime_runFinalizersOnExit(jboolean on)
{
	runFinalizerOnExit = on;
}
