/*
 * external.c
 * Handle method calls to other languages.
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#define	DBG(s)
#define	LDBG(s)

#include "config.h"
#include "config-std.h"
#include "config-mem.h"
#include "gtypes.h"
#include "access.h"
#include "object.h"
#include "constants.h"
#include "classMethod.h"
#include "slots.h"
#include "external.h"
#include "errors.h"
#include "exception.h"
#include "slib.h"
#include "paths.h"
#include "support.h"
#include "md.h"
#if defined(NO_SHARED_LIBRARIES)
#include "../../libraries/clib/external_native.h"
#endif
#include "system.h"

#if defined(NO_SHARED_LIBRARIES)

#define	STUB_PREFIX		""
#define	STUB_PREFIX_LEN		0
#define	STUB_POSTFIX		""

#else
/*
 * Some version of dlsym need an underscore, some don't.
 */
#if defined(HAVE_DYN_UNDERSCORE)
#define	STUB_PREFIX		"_"
#define	STUB_PREFIX_LEN		1
#else
#define	STUB_PREFIX		""
#define	STUB_PREFIX_LEN		0
#endif
#define	STUB_POSTFIX		""

#endif

static struct {
	LIBRARYHANDLE	desc;
	char*		name;
	int		ref;
} libHandle[MAXLIBS];

char* libraryPath = "";

void* loadNativeLibrarySym(char*);
jint Kaffe_JNI_native(Method*);

/*
 * Error stub function.  Point unresolved link errors here to avoid
 * problems.
 */
static
void*
error_stub(void)
{
	return (0);
}

void
initNative(void)
{
#if !defined(NO_SHARED_LIBRARIES)
	char lib[MAXLIBPATH];
	char* lpath;
	char* home;
	char* nptr;
	char* ptr;
	int len;

	lpath = (char*)Kaffe_JavaVMArgs[0].librarypath;
	home = (char*)Kaffe_JavaVMArgs[0].classhome;

	len = 0;
	if (lpath != 0) {
		len += strlen(lpath);
	}
	if (home != 0) {
		len += strlen(home);
	}

	/*
	 * Build a library path from the given library path and one
	 * constructed from the kaffe home + system type.
	 */
	libraryPath = gc_malloc_fixed(len + strlen(ARCHOS) + 7);
	if (lpath != 0) {
		strcat(libraryPath, lpath);
		if (home != 0) {
			strcat(libraryPath, ":");
		}
	}
	if (home != 0) {
		strcat(libraryPath, home);
		strcat(libraryPath, "/lib/");
		strcat(libraryPath, ARCHOS);
	}

	LIBRARYINIT();

	/* Find the default library */
	for (ptr = libraryPath; ptr != 0; ptr = nptr) {
		nptr = strchr(ptr, PATHSEP);
		if (nptr == 0) {
			strcpy(lib, ptr);
		}
		else if (nptr == ptr) {
			nptr++;
			continue;
		}
		else {
			strncpy(lib, ptr, nptr - ptr);
			lib[nptr-ptr] = 0;
			nptr++;
		}
		strcat(lib, DIRSEP);
		strcat(lib, NATIVELIBRARY);
		strcat(lib, LIBRARYSUFFIX);

		if (loadNativeLibrary(lib) == 1) {
			return;
		}
	}
	fprintf(stderr, "Failed to locate native library in path:\n");
	fprintf(stderr, "\t%s\n", libraryPath);
	fprintf(stderr, "Aborting.\n");
	fflush(stderr);
	EXIT(1);
#else
	int i;

	/* Initialise the native function table */
	for (i = 0; default_natives[i].name != 0; i++) {
		addNativeMethod(default_natives[i].name, default_natives[i].func);
	}
#endif
}

int
loadNativeLibrary(char* lib)
{
	int i;

	/* Find a library handle.  If we find the library has already
	 * been loaded, don't bother to get it again, just increase the
	 * reference count.
	 */
	for (i = 0; i < MAXLIBS; i++) {
		if (libHandle[i].desc == 0) {
			goto open;
		}
		if (strcmp(libHandle[i].name, lib) == 0) {
			libHandle[i].ref++;
			return (1);
		}
	}
	return (0);

	/* Open the library */
	open:
        LIBRARYLOAD(libHandle[i].desc, lib);

	if (libHandle[i].desc == 0) {
LDBG(		printf("Library load failed: %s\n", LIBRARYERROR());	)
		return (0);
	}

	libHandle[i].ref = 1;
	libHandle[i].name = gc_malloc_fixed(strlen(lib) + 1);
	strcpy(libHandle[i].name, lib);

	return (1);
}

/*
 * Get pointer to symbol from symbol name.
 */
void*
loadNativeLibrarySym(char* name)
{
	void* func;

	LIBRARYFUNCTION(func, name);

	return (func);
}

void
native(Method* m)
{
	char stub[MAXSTUBLEN];
	char* ptr;
	int i;
	void* func;

	/* Construct the stub name */
	strcpy(stub, STUB_PREFIX);
	ptr = m->class->name->data;
	for (i = STUB_PREFIX_LEN; *ptr != 0; ptr++, i++) {
		if (*ptr == '/') {
			stub[i] = '_';
		}
		else {
			stub[i] = *ptr;
		}
	}
	stub[i] = '_';
	stub[i+1] = 0;
	strcat(stub, m->name->data);
	strcat(stub, STUB_POSTFIX);

DBG(	printf("Method = %s.%s%s\n", m->class->name->data, m->name->data, m->signature->data);)
DBG(	printf("Native stub = '%s'\n", stub);fflush(stdout);		)

	/* Find the native method */
	func = loadNativeLibrarySym(stub);
	if (func != 0) {
		/* Fill it in */
		SET_METHOD_NATIVECODE(m, func);
		return;
	}

	/* Try to locate the nature function using the JNI interface */
        if (Kaffe_JNI_native(m)) {
                return;
        }

	fprintf(stderr, "Failed to locate native function:\n\t%s.%s%s\n", m->class->name->data, m->name->data, m->signature->data);
	fflush(stderr);
	SET_METHOD_NATIVECODE(m, (void*)error_stub);

        throwException(UnsatisfiedLinkError);
}

/*
 * Return the library path.
 */
char*
getLibraryPath(void)
{
	return (libraryPath);
}
