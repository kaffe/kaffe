/*
 * external.c
 * Handle method calls to other languages.
 *
 * Copyright (c) 1996, 1997, 1998, 1999
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#include "config.h"
#include "debug.h"
#include "config-std.h"
#include "config-mem.h"
#include "config-io.h"
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
#include "system.h"
#include "support.h"
#include "md.h"
#include "system.h"
#include "jthread.h"
#include "jsignal.h"
#include "ltdl.h"

#ifndef STUB_PREFIX
#define STUB_PREFIX ""
#define STUB_PREFIX_LEN 0
#endif

#ifndef STUB_POSTFIX
#define STUB_POSTFIX ""
#endif

#ifndef LIBRARYHANDLE
#define LIBRARYHANDLE lt_dlhandle
#endif

#ifndef LIBRARYINIT
static inline lt_ptr_t kdlmalloc(size_t len) { return KMALLOC(len); }
static inline void kdlfree(lt_ptr_t ptr) { KFREE(ptr); }
#define LIBRARYINIT() ((lt_dlmalloc=kdlmalloc),(lt_dlfree=kdlfree),lt_dlinit())
#endif

#ifndef LIBRARYSUFFIX
#define LIBRARYSUFFIX ""
#endif

#ifndef LIBRARYLOAD
#define LIBRARYLOAD(desc,filename) ((desc)=lt_dlopenext((filename)))
#endif

#ifndef LIBRARYERROR
#define LIBRARYERROR() (getLibraryError())
static inline const char *getLibraryError(void) {
/* Ignore file not found errors */
	const char *err = lt_dlerror();
	if (strcmp(err, "file not found") == 0)
		err = 0;
	return err;
}
#endif

static struct {
	LIBRARYHANDLE	desc;
	char*		name;
	int		ref;
} libHandle[MAXLIBS];

#ifndef LIBRARYFUNCTION
static inline lt_ptr_t findLibraryFunction(const char *name) {
  int i = 0;
  lt_ptr_t ptr = 0;

  while (!ptr && libHandle[i].ref && i < MAXLIBS) {
    ptr = lt_dlsym(libHandle[i].desc, name);
    ++i;
  }

  return ptr;
}

#define LIBRARYFUNCTION(ptr,name) ((ptr)=findLibraryFunction(name))
#endif

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
	char lib[MAXLIBPATH];
	char* lpath;
	char* nptr;
	char* ptr;
	int len;

	lpath = (char*)Kaffe_JavaVMArgs[0].libraryhome;
	if (lpath == 0) {
		lpath = getenv(LIBRARYPATH);
	}

	len = 0;
	if (lpath != 0) {
		len += strlen(lpath);
	}

	/*
	 * Build a library path from the given library path.
	 */
	libraryPath = KMALLOC(len+1);
	if (lpath != 0) {
		strcat(libraryPath, lpath);
	}

	LIBRARYINIT();

	/* Find the default library */
	for (ptr = libraryPath; ptr != 0; ptr = nptr) {
		/* FIXME: requires path_separator to have length 1 */
		nptr = strchr(ptr, path_separator[0]);
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
		strcat(lib, file_separator);
		strcat(lib, NATIVELIBRARY);
		strcat(lib, LIBRARYSUFFIX);

		if (loadNativeLibrary(lib) == 1) {
			return;
		}
	}
	fprintf(stderr, "Failed to locate native library \"%s\" in path:\n", NATIVELIBRARY);
	fprintf(stderr, "\t%s\n", libraryPath);
	fprintf(stderr, "Aborting.\n");
	fflush(stderr);
	EXIT(1);
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

#if 0
	/* If this file doesn't exist, ignore it */
	if (access(lib, R_OK) != 0) {
		return (0);
	}
#endif
/* if we tested for existence here, libltdl wouldn't be able to look
   for system-dependent library names */

	blockAsyncSignals();
        LIBRARYLOAD(libHandle[i].desc, lib);
	unblockAsyncSignals();

	if (libHandle[i].desc == 0) {
		const char *err = LIBRARYERROR();
		/* If the file doesn't exist, our lt_dlerror wrapper
                   will return NULL.  Since we don't want to print
                   file not found multiple times, we'll ignore this
                   particular error, and print only other kinds of
                   errors. */
		if (err) {
			fprintf(stderr, "Library load failed: %s\n", err);
		}
		return (0);
	}

	libHandle[i].ref = 1;
	libHandle[i].name = KMALLOC(strlen(lib) + 1);
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

	blockAsyncSignals();
	LIBRARYFUNCTION(func, name);
	unblockAsyncSignals();

	return (func);
}

bool
native(Method* m, errorInfo *einfo)
{
	char stub[MAXSTUBLEN];
	const char* s;
	int i;
	void* func;

	/* Construct the stub name */
	strcpy(stub, STUB_PREFIX);
	s = m->class->name->data;
	for (i = STUB_PREFIX_LEN; *s != 0; s++, i++) {
		if (*s == '/') {
			stub[i] = '_';
		}
		else {
			stub[i] = *s;
		}
	}
	stub[i] = '_';
	stub[i+1] = 0;
	strcat(stub, m->name->data);
	strcat(stub, STUB_POSTFIX);

DBG(LIBTOOL,	
	dprintf("Method = %s.%s%s\n", m->class->name->data, 
		m->name->data, m->signature->data);
	dprintf("Native stub = '%s'\n", stub);
    )

	/* Find the native method */
	func = loadNativeLibrarySym(stub);
	if (func != 0) {
		/* Fill it in */
		SET_METHOD_NATIVECODE(m, func);
		return (true);
	}

	/* Try to locate the nature function using the JNI interface */
        if (Kaffe_JNI_native(m)) {
                return (true);
        }

DBG(LIBTOOL,
	dprintf("Failed to locate native function:\n\t%s.%s%s\n",
		m->class->name->data, m->name->data, m->signature->data);
    )
	SET_METHOD_NATIVECODE(m, (void*)error_stub);

	postExceptionMessage(einfo, JAVA_LANG(UnsatisfiedLinkError),
		"Failed to locate native function:\t%s.%s%s",
		m->class->name->data, m->name->data, m->signature->data);
	return (false);
}

/*
 * Return the library path.
 */
char*
getLibraryPath(void)
{
	return (libraryPath);
}
