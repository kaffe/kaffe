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
#include "stats.h"
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
static inline lt_ptr_t kdlmalloc(size_t len) { 
	void *ptr = KMALLOC(len);
	addToCounter(&ltmem, "vmmem-libltdl", 1, GCSIZEOF(ptr));
	return (ptr);
}
static inline void kdlfree(lt_ptr_t ptr) { 
	addToCounter(&ltmem, "vmmem-libltdl", 1, -((jlong)GCSIZEOF(ptr)));
	KFREE(ptr); 
}
#define LIBRARYINIT() ((lt_dlmalloc=kdlmalloc),(lt_dlfree=kdlfree),lt_dlinit())
#endif

#ifndef LIBRARYSUFFIX
#define LIBRARYSUFFIX ""
#endif

#ifndef LIBRARYLOAD
#define LIBRARYLOAD(desc,filename)	((desc)=lt_dlopenext((filename)))
#endif

#ifndef LIBRARYUNLOAD
#define LIBRARYUNLOAD(desc)		(lt_dlclose(desc))
#endif

#ifndef LIBRARYERROR
#define LIBRARYERROR() lt_dlerror()
#endif

static struct _libHandle {
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

static char *libraryPath = "";

extern jint Kaffe_JNI_native(Method*);

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
	addToCounter(&ltmem, "vmmem-libltdl", 1, GCSIZEOF(libraryPath));
	if (lpath != 0) {
		strcat(libraryPath, lpath);
	}

	LIBRARYINIT();

	/* Find the default library */
	for (ptr = libraryPath; ptr != 0; ptr = nptr) {
		nptr = strstr(ptr, path_separator);
		if (nptr == 0) {
			strcpy(lib, ptr);
		}
		else if (nptr == ptr) {
			nptr += strlen(path_separator);
			continue;
		}
		else {
			strncpy(lib, ptr, nptr - ptr);
			lib[nptr-ptr] = '\0';
			nptr += strlen(path_separator);
		}
		strcat(lib, "/"); /* should be file_separator, libltdl
				     does not handle backslashes yet */
		strcat(lib, NATIVELIBRARY);
		strcat(lib, LIBRARYSUFFIX);

		if (loadNativeLibrary(lib, NULL, 0) >= 0) {
			return;
		}
	}
	fprintf(stderr, "Failed to locate native library \"%s\" in path:\n", NATIVELIBRARY);
	fprintf(stderr, "\t%s\n", libraryPath);
	fprintf(stderr, "Aborting.\n");
	fflush(stderr);
	EXIT(1);
}

/*
 * Link in a native library. If successful, returns an index >= 0 that
 * can be passed to unloadNativeLibrary(). Otherwise, returns -1 and
 * fills errbuf (if not NULL) with the error message. Assumes synchronization.
 */
int
loadNativeLibrary(char* path, char *errbuf, size_t errsiz)
{
	struct _libHandle *lib;
	int index;

	/* Find a library handle.  If we find the library has already
	 * been loaded, don't bother to get it again, just increase the
	 * reference count.
	 */
	for (index = 0; index < MAXLIBS; index++) {
		lib = &libHandle[index];
		if (lib->desc == 0) {
			goto open;
		}
		if (strcmp(lib->name, path) == 0) {
			lib->ref++;
DBG(NATIVELIB,
			dprintf("Native lib %s\n"
			    "\tLOAD desc=%p index=%d ++ref=%d\n",
			    lib->name, lib->desc, index, lib->ref);
    )
			return index;
		}
	}
	if (errbuf != 0) {
		strncpy(errbuf, "Too many open libraries", errsiz);
		errbuf[errsiz - 1] = '\0';
	}
	return -1;

	/* Open the library */
	open:

#if 0
	/* If this file doesn't exist, ignore it */
	if (access(path, R_OK) != 0) {
		if (errbuf != 0) {
			strncpy(errbuf, SYS_ERROR(errno), errsiz);
			errbuf[errsiz - 1] = '\0';
		}
		return -1;
	}
#endif
/* if we tested for existence here, libltdl wouldn't be able to look
   for system-dependent library names */

	blockAsyncSignals();
	LIBRARYLOAD(lib->desc, path);
	unblockAsyncSignals();

	if (lib->desc == 0) {
		const char *err = LIBRARYERROR();

		if (err == 0) {
			err = "Unknown error";
		}
		if (errbuf != 0) {
			strncpy(errbuf, err, errsiz);
			errbuf[errsiz - 1] = '\0';
		}
		return -1;
	}

	lib->ref = 1;
	lib->name = KMALLOC(strlen(path) + 1);
	addToCounter(&ltmem, "vmmem-libltdl", 1, GCSIZEOF(lib->name));
	strcpy(lib->name, path);

DBG(NATIVELIB,
	dprintf("Native lib %s\n"
	    "\tLOAD desc=%p index=%d ++ref=%d\n",
	    lib->name, lib->desc, index, lib->ref);
    )

	return index;
}

/*
 * Unlink a native library. Assumes synchronization.
 * Note that libnative is always at index zero and should
 * never be unloaded. So index should never equal zero here.
 */
void
unloadNativeLibrary(int index)
{
	struct _libHandle *lib;

	assert(index > 0 && index < MAXLIBS);
	lib = &libHandle[index];

DBG(NATIVELIB,
	dprintf("Native lib %s\n"
	    "\tUNLOAD desc=%p index=%d --ref=%d\n",
	    lib->name, lib->desc, index, lib->ref - 1);
    )

	assert(lib->desc != 0);
	assert(lib->ref > 0);
	if (--lib->ref == 0) {
		blockAsyncSignals();
		LIBRARYUNLOAD(lib->desc);
		unblockAsyncSignals();
		KFREE(lib->name);
		lib->desc = 0;
	}
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

DBG(NATIVELIB,	
	dprintf("Method = %s.%s%s\n", m->class->name->data, 
		m->name->data, METHOD_SIGD(m));
	dprintf("Native stub = '%s'\n", stub);
    )

	/* Find the native method */
	func = loadNativeLibrarySym(stub);
	if (func != 0) {
		/* Fill it in */
		extern void Kaffe_KNI_wrapper(Method* xmeth, void* func);
		Kaffe_KNI_wrapper(m, func);
		return (true);
	}

	/* Try to locate the nature function using the JNI interface */
        if (Kaffe_JNI_native(m)) {
                return (true);
        }

DBG(NATIVELIB,
	dprintf("Failed to locate native function:\n\t%s.%s%s\n",
		m->class->name->data, m->name->data, METHOD_SIGD(m));
    )
	SET_METHOD_NATIVECODE(m, (void*)error_stub);

	postExceptionMessage(einfo, JAVA_LANG(UnsatisfiedLinkError),
		"Failed to locate native function:\t%s.%s%s",
		m->class->name->data, m->name->data, METHOD_SIGD(m));
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
