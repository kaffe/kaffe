/*
 * java.lang.Runtime.c
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "lib-license.terms" for information on usage and redistribution 
 * of this file. 
 */

#define	DBG(s)

#include "config.h"
#include "config-std.h"
#include "config-mem.h"
#include "config-io.h"
#include "../../../kaffe/kaffevm/gtypes.h"
#include <native.h>
#include "files.h"
#include "defs.h"
#include "Runtime.h"
#include "../../../kaffe/kaffevm/external.h"
#include "../../../kaffe/kaffevm/gc.h"
#include "../../../kaffe/kaffevm/support.h"

#define	LIBRARY_PREFIX	"/lib"

extern char* libraryPath;
extern size_t gc_heap_limit;
extern size_t gc_heap_total;
extern jbool runFinalizerOnExit;


/*
 * Initialise the linker and return the search path for shared libraries.
 */
struct Hjava_lang_String*
java_lang_Runtime_initializeLinkerInternal(struct Hjava_lang_Runtime* this)
{
#if defined(NO_SHARED_LIBRARIES)
	return (0);
#else
	return (makeJavaString(libraryPath, strlen(libraryPath)));
#endif
}

/*
 * Construct a library name.
 */
struct Hjava_lang_String*
java_lang_Runtime_buildLibName(struct Hjava_lang_Runtime* this, struct Hjava_lang_String* s1, struct Hjava_lang_String* s2)
{
	char lib[MAXLIBPATH];
	char str[MAXPATHLEN];

	/*
	 * Note. Although the code below will build a library string, if
	 * it doesn't fit into the buffer, it will truncate the path
	 * silently.
	 */
	javaString2CString(s1, str, sizeof(str));
	strncpy(lib, str, MAXLIBPATH-1);
	strncat(lib, LIBRARY_PREFIX, MAXLIBPATH-1);
	javaString2CString(s2, str, sizeof(str));
	strncat(lib, str, MAXLIBPATH-1);
	strncat(lib, LIBRARYSUFFIX, MAXLIBPATH-1);
	lib[MAXLIBPATH-1] = 0;

	return (makeJavaString(lib, strlen(lib)));
}

/*
 * Load in a library file.
 */
jint
java_lang_Runtime_loadFileInternal(struct Hjava_lang_Runtime* this, struct Hjava_lang_String* s1)
{
	char lib[MAXPATHLEN];
	int r;

	javaString2CString(s1, lib, sizeof(lib));
	r = loadNativeLibrary(lib);

	return (r);
}

/*
 * Exit this VM
 */
void
java_lang_Runtime_exitInternal(struct Hjava_lang_Runtime* r, jint v)
{
	EXIT (v);
}

/*
 * Exec another program.
 */
struct Hjava_lang_Process*
java_lang_Runtime_execInternal(struct Hjava_lang_Runtime* this, HArrayOfObject* argv, HArrayOfObject* arge)
{
	struct Hjava_lang_Process* child;

	child = (struct Hjava_lang_Process*)execute_java_constructor("kaffe.lang.UNIXProcess", 0, "([Ljava/lang/String;[Ljava/lang/String;)V", argv, arge);

	return (child);
}

/*
 * Free memory.
 */
jlong
java_lang_Runtime_freeMemory(struct Hjava_lang_Runtime* this)
{
	/* This is a particularly inaccurate guess - it's basically how
	 * much more memory we can claim from the heap, and ignores any
	 * free memory already within the GC system.
	 * Well it'll do for now.
	 */
	return (gc_heap_limit - gc_heap_total);
}

/*
 * Total memory.
 */
jlong
java_lang_Runtime_totalMemory(struct Hjava_lang_Runtime* this)
{
	return (gc_heap_limit);
}

/*
 * Run the garbage collector.
 */
void
java_lang_Runtime_gc(struct Hjava_lang_Runtime* this)
{
	invokeGC();
}

/*
 * Run any pending finialized methods.
 *  Finalising is part of the garbage collection system - so just run that.
 */
void
java_lang_Runtime_runFinalization(struct Hjava_lang_Runtime* this)
{
	invokeGC();
}

/*
 * Enable/disable tracing of instructions.
 */
void
java_lang_Runtime_traceInstructions(struct Hjava_lang_Runtime* this, jbool on)
{
	if (on == true) {
		SignalError("java.lang.RuntimeException", "Cannot trace instructions");   
	}
}

/*
 * Enable/disable tracing of method calls.
 */
void
java_lang_Runtime_traceMethodCalls(struct Hjava_lang_Runtime* this, jbool on)
{
	if (on == true) {
		SignalError("java.lang.RuntimeException", "Cannot trace method calls");   
	}
}

/*
 * Inform the runtime that it must run the finalizer when it exits.
 */
void
java_lang_Runtime_runFinalizersOnExit0(jbool on)
{
	runFinalizerOnExit = on;
}
