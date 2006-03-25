/* slib.h
 * Macro'ise the shared library calls to be 'portable'.
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#ifndef __slib_h
#define __slib_h

/* ------------------------------------------------------------------------ */

/*
 * MACH style shared library interface.
 */
#if defined(HAVE_MACH_O_RLD_H)
#if defined(__APPLE__)

#if defined(HAVE_DLFCN_H)
  /* Mac OS X has rld.h, but we don't want to use it if the dlcompat
     package from the Fink project is installed. */
#include <dlfcn.h>
#else
#error Please install dlcompat library from Fink project
#endif

#else 

#include <mach-o/rld.h>

#define	LIBRARYHANDLE	long

static inline void
KaffeLib_Init(void)
{
}

#if !defined(NeXT)
static inline long
KaffeLib_Load(const char *libname)
{
  const char* filenames[2];
  struct mach_header* new_header;
  
  filenames[0]=libname;
  filenames[1]=NULL;
  return rld_load(NULL,&new_header,filenames,NULL);
}
#else
/*
 * The following is for the NeXTStep - I don't like putting this kind
 * of system dependency here and I'll find an auto way to detect this
 * later.
 */
static inline long
KaffeLib_Load(const char *libname)
{
  const char* filenames[2];
  long libhandle;

  NXStream* errorStream;
  filenames[0] = libname;
  filenames[1] = NULL;
  errorStream = NXOpenMemory(NULL, 0, NX_WRITEONLY);
  libhandle = !objc_loadModules(filenames,errorStream,NULL,NULL,NULL);
  if (!libhandle) {
    char* streamBuf;
    int len, maxLen;
    NXPutc(errorStream, (char)0);
    NXGetMemoryBuffer(errorStream, &streamBuf, &len, &maxLen);
    printf("LIBRARYLOAD(\"%s\") error: %s\n", libname,
	   streamBuf);
  }
  return libhandle;
}

static inline void
KaffeLib_Unload(long handle)
{
}

#endif

static inline void *
KaffeLib_GetSymbol(long handle, const char *symname)
{
  void *func;
  
  rld_lookup(handle, symname, &func);
  return func;
}

static inline const char *
KaffeLib_GetError(void)
{
  return "No specific error support";
}

#endif
#endif

/* ------------------------------------------------------------------------ */

#ifndef LIBRARYHANDLE

#define LT_NON_POSIX_NAMESPACE
#include "ltdl.h"

#define LIBRARYHANDLE lt_dlhandle

static inline void
KaffeLib_Init(void)
{
  lt_dlinit();
}

static inline lt_dlhandle
KaffeLib_Load(const char *libname)
{
  return lt_dlopenext(libname);
}

static inline void
KaffeLib_Unload(lt_dlhandle handle)
{
  lt_dlclose(handle);
}

static inline void *
KaffeLib_GetSymbol(lt_dlhandle handle,
		   const char *symname)
{
  return lt_dlsym(handle, symname);
}

static inline const char *
KaffeLib_GetError(void)
{
  return lt_dlerror();
}

#endif

/* ------------------------------------------------------------------------ */

#endif
