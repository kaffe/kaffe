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
 * UNIX BSD & SysV style shared library interface.
 */
#if defined(HAVE_DLOPEN)
#if defined(HAVE_DLFCN_H) 
#include <dlfcn.h>
#else
void* dlopen(char*, int);
void* dlsym(void*, char*);
char* dlerror(void);
#endif
#define	LIBRARYINIT()
#define	LIBRARYLOAD(HAND, LIB)						\
	HAND = dlopen(LIB, 1)
#define	LIBRARYFUNCTION(FUNC, SYM)					\
	{								\
		int i;							\
		FUNC = 0;						\
		for (i = 0; i < MAXLIBS && libHandle[i].ref > 0; i++) {	\
			FUNC = dlsym(libHandle[i].desc, SYM);		\
			if (FUNC != 0) {				\
				break;					\
			}						\
		}							\
	}
#define	LIBRARYHANDLE	void*
#if defined(HAVE_DLERROR)
#define	LIBRARYERROR()	dlerror()
#else
#define	LIBRARYERROR()	"No specific error support"
#endif
#endif

/* ------------------------------------------------------------------------ */

/*
 * MACH style shared library interface.
 */
#if defined(HAVE_MACH_O_RLD_H)
#include <mach-o/rld.h>
#define	LIBRARYINIT()
#define	LIBRARYLOAD(HAND, LIB)						\
	{								\
		const char* filenames[2];				\
		struct mach_header* new_header;				\
		filenames[0]=(LIB);					\
		filenames[1]=NULL;					\
		HAND = rld_load(NULL,&new_header,filenames,NULL);\
	}
#define	LIBRARYFUNCTION(FUNC, SYM)					\
	rld_lookup(NULL,SYM,&FUNC)
#define	LIBRARYHANDLE	long
#define	LIBRARYERROR()	"No specific error support"
#endif

/* ------------------------------------------------------------------------ */

/*
 * HP-UX style shared library interface.
 */
#if defined(HAVE_SHL_LOAD)
#if defined(HAVE_DL_H)
#include <dl.h>
#endif
#define	LIBRARYINIT()
#define	LIBRARYLOAD(HAND, LIB)						\
	HAND = shl_load(LIB, BIND_DEFERRED, 0L)
#define	LIBRARYFUNCTION(FUNC, SYM)					\
	{								\
		shl_t all_libs = 0;					\
		if (shl_findsym(&all_libs,SYM,TYPE_PROCEDURE,&FUNC))	\
			FUNC = 0;					\
	}
#define	LIBRARYHANDLE	shl_t
#define	LIBRARYERROR()	"No specific error support"
#endif

/* ------------------------------------------------------------------------ */

/*
 * MS-Windows style shared library interface.
 */
#if defined(__WIN32__)
#define	LIBRARYINIT()
#define LIBRARYLOAD(HAND, LIB)						\
	HAND = LoadLibrary(LIB)
#define LIBRARYFUNCTION(FUNC, SYM)					\
	{								\
		int i;							\
		FUNC = 0;						\
		for (i = 0; i < MAXLIBS && libHandle[i].ref > 0; i++) {	\
			FUNC = GetProcAddress(libHandle[i].desc, SYM);	\
			if (FUNC != 0) {				\
				break;					\
			}						\
		}							\
	}
#define	LIBRARYHANDLE	void*
#define	LIBRARYERROR()	"No specific error support"
#endif 

/* ------------------------------------------------------------------------ */

/*
 * GNU style shared library interface.
 */
#if defined(HAVE_LIBDLD) && !defined(HAVE_DLFCN_H)
#define	LIBRARYINIT()							\
        {								\
                static int init_dld = 0;				\
                if (init_dld == 0) {					\
                        init_dld = 1;					\
                        if (dld_init("/usr/local/bin/kaffe") != 0) {	\
                                return (-1);				\
                        }						\
                }							\
        }
#define LIBRARYLOAD(HAND, LIB)						\
	HAND = !dld_link(LIB)
#define LIBRARYFUNCTION(FUNC, SYM)					\
	FUNC = dld_get_func(SYM)
#define	LIBRARYHANDLE	int
#define	LIBRARYERROR()	"No specific error support"
#endif

/* ------------------------------------------------------------------------ */

/*
 * If we don't have any shared library support, we use the buildin native
 * function tables.
 */
#if defined(NO_SHARED_LIBRARIES)
#undef	LIBRARYLOAD
#undef	LIBRARYFUNCTION
#undef	LIBRARYHANDLE
#undef	LIBRARYERROR

#define	LIBRARYLOAD(HAND, LIB)						\
	HAND = 1
#define	LIBRARYFUNCTION(FUNC, STUB)					\
	do {								\
		int z;							\
		FUNC = 0;						\
		for (z = 0; native_funcs[z].name != 0; z++) {		\
			if (strcmp(native_funcs[z].name, STUB) == 0) {	\
				FUNC = native_funcs[z].func;		\
				break;					\
			}						\
		}							\
	} while (0)
#define	LIBRARYHANDLE			int
#define	LIBRARYERROR()			"No shared library support"

#endif

/* ------------------------------------------------------------------------ */

#endif
