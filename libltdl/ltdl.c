/* ltdl.c -- system independent dlopen wrapper
   Copyright (C) 1998-1999 Free Software Foundation, Inc.
   Originally by Thomas Tanner <tanner@gmx.de>
   This file is part of GNU Libtool.

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Library General Public
License as published by the Free Software Foundation; either
version 2 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Library General Public License for more details.

You should have received a copy of the GNU Library General Public
License along with this library; if not, write to the Free
Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

#define _LTDL_COMPILE_

#if HAVE_STRING_H
#include <string.h>
#endif

#if HAVE_STRINGS_H
#include <strings.h>
#endif

#if HAVE_CTYPE_H
#include <ctype.h>
#endif

#if HAVE_MALLOC_H
#include <malloc.h>
#endif

#if HAVE_MEMORY_H
#include <memory.h>
#endif

#if HAVE_STDLIB_H
#include <stdlib.h>
#endif

#if HAVE_UNISTD_H
#include <unistd.h>
#endif

#if HAVE_STDIO_H
#include <stdio.h>
#endif

#include "ltdl.h"

static const char *unknown_error = "unknown error";
static const char *dlopen_not_supported_error = "dlopen support not available";
static const char *dlpreopen_not_supported_error = "dlpreopen support not available";
static const char *file_not_found_error = "file not found";
static const char *no_symbols_error = "no symbols defined";
static const char *symbol_error = "symbol not found";
static const char *memory_error = "not enough memory";
static const char *invalid_handle_error = "invalid handle";
static const char *buffer_overflow_error = "internal buffer overflow";
static const char *shutdown_error = "libraries already shutdown";

static const char *last_error;

typedef struct lt_dltype_t {
	struct lt_dltype_t *next;
	int (*mod_init) __P((void));
	int (*mod_exit) __P((void));
	int (*lib_open) __P((lt_dlhandle handle, const char *filename));
	int (*lib_close) __P((lt_dlhandle handle));
	lt_ptr_t (*find_sym) __P((lt_dlhandle handle, const char *symbol));
} lt_dltype_t, *lt_dltype;

#define LT_DLTYPE_TOP 0

typedef	struct lt_dlhandle_t {
	struct lt_dlhandle_t *next;
	lt_dltype_t *type;	/* dlopening interface */
	char	*filename;	/* file name */
	char	*name;		/* module name */
	int	usage;		/* usage */
	int	depcount;	/* number of dependencies */
	lt_dlhandle *deps;	/* dependencies */
	lt_ptr_t handle;	/* system handle */
	lt_ptr_t system;	/* system specific data */
} lt_dlhandle_t;

#if ! HAVE_STRDUP

#undef strdup
#define strdup xstrdup

static char *
strdup(str)
	const char *str;
{
	char *tmp;

	if (!str)
		return str;
	tmp = malloc(strlen(str)+1);
	if (tmp)
		strcpy(tmp, str);
	return tmp;
}

#endif

#if ! HAVE_STRCHR

# if HAVE_INDEX

#  define strchr index

# else

#  define strchr xstrchr

static const char*
strchr(str, ch)
	const char *str;
	int ch;
{
	const char *p;

	for (p = str; *p != (char)ch && p != '\0'; p++)
		/*NOWORK*/;

	return (*p == (char)ch) ? p : 0;
}

# endif

#endif

#if ! HAVE_STRRCHR

# if HAVE_RINDEX

#  define strrchr rindex

# else

#  define strrchr xstrrchr

static const char*
strrchr(str, ch)
	const char *str;
	int ch;
{
	const char *p;

	for (p = str; p != '\0'; p++)
		/*NOWORK*/;

	while (*p != (char)ch && p >= str)
		p--;

	return (*p == (char)ch) ? p : 0;
}

# endif

#endif

#if HAVE_LIBDL

/* dynamic linking with dlopen/dlsym */

#if HAVE_DLFCN_H
# include <dlfcn.h>
#endif

#if ! HAVE_DLERROR	/* not all platforms have dlerror() */
#define	dlerror()	unknown_error
#endif

#if RTLD_GLOBAL
# define LTDL_GLOBAL	RTLD_GLOBAL
#else
# if DL_GLOBAL
#  define LTDL_GLOBAL	DL_GLOBAL
# else
#  define LTDL_GLOBAL	0
# endif
#endif

#if RTLD_NOW
# define LTDL_NOW	RTLD_NOW
#else
# if DL_NOW
#  define LTDL_NOW	DL_NOW
# else
#  define LTDL_NOW	0
# endif
#endif

static int
dl_init ()
{
	return 0;
}

static int
dl_exit ()
{
	return 0;
}

static int
dl_open (handle, filename)
	lt_dlhandle handle;
	const char *filename;
{
	handle->handle = dlopen(filename, LTDL_GLOBAL | LTDL_NOW);
	if (!handle->handle) {
		last_error = dlerror();
		return 1;
	}
	return 0;
}

static int
dl_close (handle)
	lt_dlhandle handle;
{
	if (dlclose(handle->handle) != 0) {
		last_error = dlerror();
		return 1;
	}
	return 0;
}

static lt_ptr_t
dl_sym (handle, symbol)
	lt_dlhandle handle;
	const char *symbol;
{
	lt_ptr_t address = dlsym(handle->handle, symbol);
	
	if (!address) 
		last_error = dlerror();
	return address;
}

static
lt_dltype_t
dl = { LT_DLTYPE_TOP, dl_init, dl_exit,
       dl_open, dl_close, dl_sym };

#undef LT_DLTYPE_TOP
#define LT_DLTYPE_TOP &dl

#endif

#if HAVE_SHL_LOAD

/* dynamic linking with shl_load (HP-UX) */

#ifdef HAVE_DL_H
#include <dl.h>
#endif

/* some flags are missing on some systems, so we provide
 * harmless defaults.
 *
 * Mandatory:
 * BIND_IMMEDIATE  - Resolve symbol references when the library is loaded.
 * BIND_DEFERRED   - Delay code symbol resolution until actual reference.
 *
 * Optionally:
 * BIND_FIRST	   - Place the library at the head of the symbol search order.
 * BIND_NONFATAL   - The default BIND_IMMEDIATE behavior is to treat all unsatisfied
 *		     symbols as fatal.	This flag allows binding of unsatisfied code
 *		     symbols to be deferred until use.
 *		     [Perl: For certain libraries, like DCE, deferred binding often
 *		     causes run time problems.	Adding BIND_NONFATAL to BIND_IMMEDIATE
 *		     still allows unresolved references in situations like this.]
 * BIND_NOSTART	   - Do not call the initializer for the shared library when the
 *		     library is loaded, nor on a future call to shl_unload().
 * BIND_VERBOSE	   - Print verbose messages concerning possible unsatisfied symbols.
 *
 * hp9000s700/hp9000s800:
 * BIND_RESTRICTED - Restrict symbols visible by the library to those present at
 *		     library load time.
 * DYNAMIC_PATH	   - Allow the loader to dynamically search for the library specified
 *		     by the path argument.
 */

#ifndef	DYNAMIC_PATH
#define	DYNAMIC_PATH	0
#endif	/* DYNAMIC_PATH */
#ifndef	BIND_RESTRICTED
#define	BIND_RESTRICTED	0
#endif	/* BIND_RESTRICTED */

#define	OPT_BIND_FLAGS	(BIND_IMMEDIATE | BIND_NONFATAL | BIND_VERBOSE | DYNAMIC_PATH)

static int
shl_init ()
{
	return 0;
}

static int
shl_exit ()
{
	return 0;
}

static int
shl_open (handle, filename)
	lt_dlhandle handle;
	const char *filename;
{
	handle->handle = shl_load (filename, OPT_BIND_FLAGS, 0L);
	/* the hp-docs say we should better abort() if errno==ENOSYM ;( */
	if (!handle->handle) {
		last_error = unknown_error;
		return 1;
	}
	return 0;
}

static int
shl_close (handle)
	lt_dlhandle handle;
{
	if (shl_unload((shl_t) (handle->handle)) != 0) {
		last_error = unknown_error;
		return 1;
	}
	return 0;
}

static lt_ptr_t
shl_sym (handle, symbol)
	lt_dlhandle handle;
	const char *symbol;
{
	lt_ptr_t address;

	if (shl_findsym ((shl_t) (handle->handle), symbol, TYPE_UNDEFINED, 
	    &address) != 0 || !(handle->handle) || !address) {
		last_error = unknown_error;
		return 0;
	}
	return address;
}

static
lt_dltype_t
shl = { LT_DLTYPE_TOP, shl_init, shl_exit,
	shl_open, shl_close, shl_sym };

#undef LT_DLTYPE_TOP
#define LT_DLTYPE_TOP &shl

#endif

#if HAVE_DLD

/* dynamic linking with dld */

static int
dld_init ()
{
	return 0;
}

static int
dld_exit ()
{
	return 0;
}

static int
dld_open (handle, filename)
	lt_dlhandle handle;
	const char *filename;
{
	handle->handle = strdup(filename);
	if (!handle->handle) {
		last_error = no_memory_error;
		return 1;
	}
	if (dld_link(filename) != 0) {
		last_error = unknown_error;
		free(handle->handle);
		return 1;
	}
	return 0;
}

static int
dld_close (handle)
	lt_dlhandle handle;
{
	if (dld_unlink_by_file((char*)(handle->handle), 1) != 0) {
		last_error = unknown_error;
		return 1;
	}
	free(handle->filename);
	return 0;
}

static lt_ptr_t
dld_sym (handle, symbol)
	lt_dlhandle handle;
	const char *symbol;
{
	lt_ptr_t address = dld_get_func(symbol);
	
	if (!address) 
		last_error = unknown_error;
	return address;
}

static
lt_dltype_t
dld = { LT_DLTYPE_TOP, dld_init, dld_exit,
	dld_open, dld_close, dld_sym };

#undef LT_DLTYPE_TOP
#define LT_DLTYPE_TOP &dld

#endif

#ifdef _WIN32

/* dynamic linking for Win32 */

#include <windows.h>

static int
wll_init ()
{
	return 0;
}

static int
wll_exit ()
{
	return 0;
}

static int
wll_open (handle, filename)
	lt_dlhandle handle;
	const char *filename;
{
	handle->handle = LoadLibrary(filename);
	if (!handle->handle) {
		last_error = unknown_error;
		return 1;
	}
	return 0;
}

static int
wll_close (handle)
	lt_dlhandle handle;
{
	if (FreeLibrary(handle->handle) != 0) {
		last_error = unknown_error;
		return 1;
	}
	return 0;
}

static lt_ptr_t
wll_sym (handle, symbol)
	lt_dlhandle handle;
	const char *symbol;
{
	lt_ptr_t address = GetProcAddress(handle->handle, symbol);
	
	if (!address) 
		last_error = unknown_error;
	return address;
}

static
lt_dltype_t
wll = { LT_DLTYPE_TOP, wll_init, wll_exit,
	wll_open, wll_close, wll_sym };

#undef LT_DLTYPE_TOP
#define LT_DLTYPE_TOP &wll

#endif

#if HAVE_DLPREOPEN

/* emulate dynamic linking using dld_preloaded_symbols */

typedef struct lt_dlsymlists_t {
	struct lt_dlsymlists_t *next;
	lt_dlsymlist	*syms;
} lt_dlsymlists_t;

static lt_dlsymlists_t *preloaded_symbols;

static int
dldpre_init ()
{
	preloaded_symbols = 0;
	return 0;
}

static void
dldpre_free_symlists ()
{
	lt_dlsymlists_t	*lists = preloaded_symbols;
	
	while (lists) {
		lt_dlsymlists_t	*tmp = lists;
		
		lists = lists->next;
		free(tmp);
	}
	preloaded_symbols = 0;
}

static int
dldpre_exit ()
{
	dldpre_free_symlists();
	return 0;
}

static int
dldpre_add_symlist (preloaded)
	lt_dlsymlist *preloaded;
{
	lt_dlsymlists_t *tmp;
	lt_dlsymlists_t *lists = preloaded_symbols;
	
	while (lists) {
		if (lists->syms == preloaded)
			return 0;
		lists = lists->next;
	}

	tmp = (lt_dlsymlists_t*) malloc(sizeof(lt_dlsymlists_t));
	if (!tmp) {
		last_error = memory_error;
		return 1;
	}
	tmp->syms = preloaded;
	tmp->next = 0;
	if (!preloaded_symbols)
		preloaded_symbols = tmp;
	else {
		/* append to the end */
		lists = preloaded_symbols;
		while (lists->next)
			lists = lists->next;
		lists->next = tmp;
	}
	return 0;
}

static int
dldpre_open (handle, filename)
	lt_dlhandle handle;
	const char *filename;
{
	lt_dlsymlists_t *lists = preloaded_symbols;

	if (!filename) {
		last_error = file_not_found_error;
		return 1;
	}
	if (!lists) {
		last_error = no_symbols_error;
		return 1;
	}
	while (lists) {
		lt_dlsymlist *syms = lists->syms;
	
		while (syms->name) {
			if (!syms->address &&
			    strcmp(syms->name, filename) == 0) {
				handle->handle = syms;
				return 0;
			}
			syms++;
		}
		lists = lists->next;
	}
	last_error = file_not_found_error;
	return 1;
}

static int
dldpre_close (handle)
	lt_dlhandle handle;
{
	return 0;
}

static lt_ptr_t
dldpre_sym (handle, symbol)
	lt_dlhandle handle;
	const char *symbol;
{
	lt_dlsymlist *syms = (lt_dlsymlist*)(handle->handle);

#if NEED_USCORE
	/* lt_dlsym will have prepended a `_', but we don't need it */
	symbol++;
#endif
	syms++;
	while (syms->address) {
		if (syms->address && strcmp(syms->name, symbol) == 0)
			return syms->address;
		syms++;
	}
	last_error = symbol_error;
	return 0;
}

static
lt_dltype_t
dldpre = { LT_DLTYPE_TOP, dldpre_init, dldpre_exit,
	   dldpre_open, dldpre_close, dldpre_sym };

#undef LT_DLTYPE_TOP
#define LT_DLTYPE_TOP &dldpre

#endif

static lt_dlhandle handles;
static int initialized = 0;

static lt_dltype types = LT_DLTYPE_TOP;
#undef LT_DLTYPE_TOP

int
lt_dlinit ()
{
	/* initialize libltdl */
	lt_dltype *type = &types;
	int typecount = 0;

	if (initialized) {	/* Initialize only at first call. */
		initialized++;
		return 0;
	}
	handles = 0;

	while (*type) {
		if ((*type)->mod_init())
			*type = (*type)->next; /* Remove it from the list */
		else {
			type = &(*type)->next; /* Keep it */
			typecount++;
		}
	}
	if (typecount == 0) {
		last_error = dlopen_not_supported_error;
		return 1;
	}
	last_error = 0;
	initialized = 1;
	return 0;
}

int
lt_dlpreopen (preloaded)
	lt_dlsymlist *preloaded;
{
#if HAVE_DLPREOPEN
	if (preloaded)
		return dldpre_add_symlist(preloaded);
	else {
		dldpre_free_symlists();
		return 0;
	}
#else
	last_error = dlpreopen_not_supported_error;
	return 1;
#endif
}

int
lt_dlexit ()
{
	/* shut down libltdl */
	lt_dltype type = types;
	int	errors;
	
	if (!initialized) {
		last_error = shutdown_error;
		return 1;
	}
	if (initialized != 1) { /* shut down only at last call. */
		initialized--;
		return 0;
	}
	/* close all modules */
	errors = 0;
	while (handles) {
		/* FIXME: what if a module depends on another one? */
		if (lt_dlclose(handles))
			errors++;
	}
	initialized = 0;
	while (type) {
		if (type->mod_exit())
			errors++;
		type = type->next;
	}
	return errors;
}

static void
trim (dest, s)
	char *dest;
	const char *s;
{
	char *i = strrchr(s, '\'');
	int len = strlen(s);

	if (len > 3 && s[0] == '\'') {
		strncpy(dest, &s[1], (i - s) - 1);
		dest[len-3] = '\0';
	} else
		*dest = '\0';
}

static int
tryall_dlopen (handle, filename)
	lt_dlhandle *handle;
	const char *filename;
{
	lt_dlhandle cur;
	lt_dltype type = types;
	const char *saved_error = last_error;
	
	/* check whether the module was already opened */
	cur = handles;
	while (cur && strcmp(cur->filename, filename))
		cur = cur->next;
	if (cur) {
		cur->usage++;
		free(*handle);
		*handle = cur;
		return 0;
	}
	
	(*handle)->filename = strdup(filename);
	if (!(*handle)->filename)
		return 1;
	while (type) {
		if (type->lib_open(*handle, filename) == 0)
			break;
		type = type->next;
	}
	if (!type) {
		free((*handle)->filename);
		return 1;
	}
	(*handle)->type = type;
	last_error = saved_error;
	return 0;
}

#undef	FILENAME_MAX
/* max. filename length */
#ifndef FILENAME_MAX
#define FILENAME_MAX 1024
#endif

static int
find_module (handle, dir, libdir, dlname, old_name)
	lt_dlhandle *handle;
	const char *dir;
	const char *libdir;
	const char *dlname;
	const char *old_name;
{
	char	fullname[FILENAME_MAX];
	
	/* search a module */
	if (*dlname) {
		/* try to open the installed module */
		if (strlen(libdir)+strlen(dlname)+1 < FILENAME_MAX) {
			strcpy(fullname, libdir);
			strcat(fullname, "/");
			strcat(fullname, dlname);
			if (tryall_dlopen(handle, fullname) == 0)
				return 0;
		}
		/* try to open the not-installed module */
		if (strlen(dir)+strlen(dlname)+6 < FILENAME_MAX) {
			strcpy(fullname, dir);
			strcat(fullname, ".libs/");
			strcat(fullname, dlname);
			if (tryall_dlopen(handle, fullname) == 0)
				return 0;
		}
		if (strlen(dir)+strlen(dlname) < FILENAME_MAX) {
			strcpy(fullname, dir);
			strcat(fullname, dlname);
			if (tryall_dlopen(handle, fullname) == 0)
				return 0;
		}
	}
	if (*old_name && tryall_dlopen(handle, old_name) == 0)
		return 0;
	last_error = file_not_found_error;
	return 1;
}

static int
find_library (handle, filename, have_dir, basename, search_path)
	lt_dlhandle *handle;
	const char *filename;
	int have_dir;
	const char *basename;
	const char *search_path;
{
	char	dir[FILENAME_MAX], fullname[FILENAME_MAX];
	const char *p, *next;
	
	if (tryall_dlopen(handle, filename) == 0)
		return 0; 
			
	if (have_dir && !search_path) {
		last_error = file_not_found_error;
		return 1;
	}
	/* try other directories */
	
	/* search_path is a colon-separated
	   list of search directories */
	p = search_path; 
	while (p) {
		next = strchr(p, ':');
		if (next) {
			if (next - p + 1 >= FILENAME_MAX) {
				last_error = buffer_overflow_error;
				return 1;
			}
			strncpy(dir, p, next - p);
			dir[next - p] = '\0';
			p = next+1;
		} else {
			if (strlen(p)+1 >= FILENAME_MAX) {
				last_error = buffer_overflow_error;
				return 1;
			}
			strcpy(dir, p);
			p = 0;
		}
		if (!*dir)
			continue;
		strcat(dir, "/");
		if (strlen(dir)+strlen(basename) < FILENAME_MAX) {
			strcpy(fullname, dir);
			strcat(fullname, basename);
			if (tryall_dlopen(handle, fullname) == 0)
				return 0; 
		}
	}
	last_error = file_not_found_error;
	return 1;
}

#undef	READTEXT_MODE
/* fopen() mode flags for reading a text file */
#ifdef _WIN32
#define	READTEXT_MODE "rt"
#else
#define	READTEXT_MODE "r"
#endif

static FILE *
find_file (filename, basename, have_dir, search_path)
	const char *filename;
	int have_dir;
	const char *basename;
	const char *search_path;
{
	char	dir[FILENAME_MAX], fullname[FILENAME_MAX];
	const char *p, *next;
	FILE	*file;
	
	file = fopen(filename, READTEXT_MODE);
	if (file)
		return file;
			
	if (have_dir && !search_path) {
		last_error = file_not_found_error;
		return 0;
	}
	/* try other directories */
	
	/* search_path is a colon-separated
	   list of search directories */
	p = search_path; 
	while (p) {
		next = strchr(p, ':');
		if (next) {
			if (next - p + 1 >= FILENAME_MAX) {
				last_error = buffer_overflow_error;
				return 0;
			}
			strncpy(dir, p, next - p);
			dir[next - p] = '\0';
			p = next+1;
		} else {
			if (strlen(p)+1 >= FILENAME_MAX) {
				last_error = buffer_overflow_error;
				return 0;
			}
			strcpy(dir, p);
			p = 0;
		}
		if (!*dir)
			continue;
		strcat(dir, "/");
		if (strlen(dir)+strlen(basename) < FILENAME_MAX) {
			strcpy(fullname, dir);
			strcat(fullname, basename);
			file = fopen(fullname, READTEXT_MODE);
			if (file)
				return file;
		}
	}
	last_error = file_not_found_error;
	return 0;
}

static int
load_deplibs(handle, deplibs)
	lt_dlhandle *handle;
	const char *deplibs;
{
	/* FIXME: load deplibs */
	return 0;
}

static int
unload_deplibs(handle)
	lt_dlhandle *handle;
{
	/* FIXME: unload deplibs */
	return 0;
}

lt_dlhandle
lt_dlopen (filename)
	const char *filename;
{
	lt_dlhandle handle;
	char	dir[FILENAME_MAX];
	const char *basename, *ext, *search_path;
	const char *saved_error = last_error;
	
	basename = strrchr(filename, '/');
	if (basename)
		basename++;
	else
		basename = filename;
	if (basename - filename >= FILENAME_MAX) {
		last_error = buffer_overflow_error;
		return 0;
	}
	strncpy(dir, filename, basename - filename);
	dir[basename - filename] = '\0';
	search_path = getenv("LTDL_LIBRARY_PATH"); /* get the search path */
	/* check whether we open a libtool module (.la extension) */
	ext = strrchr(basename, '.');
	if (ext && strcmp(ext, ".la") == 0) {
		char	dlname[FILENAME_MAX], old_name[FILENAME_MAX];
		char	libdir[FILENAME_MAX], deplibs[FILENAME_MAX];
		char	tmp[FILENAME_MAX];
		char	*name;
		FILE	*file;
		int	i;

		dlname[0] = old_name[0] = libdir[0] = deplibs[0] = '\0';

		/* extract the module name from the file name */
		if (strlen(basename) >= FILENAME_MAX) {
			last_error = buffer_overflow_error;
			return 0;
		}
		strcpy(tmp, basename);
		tmp[ext - basename] = '\0';
		/* canonicalize the module name */
		for (i = 0; i < ext - basename; i++)
			if (!isalnum(tmp[i]))
				tmp[i] = '_';
		name = strdup(tmp);
		if (!name) {
			last_error = memory_error;
			return 0;
		}
		file = find_file(filename, *dir, basename, search_path);
		if (!file) {
			free(name);
			return 0;
		}
		while (!feof(file)) {
			if (!fgets(tmp, FILENAME_MAX, file))
				break;
			if (strncmp(tmp, "dlname=", 7) == 0)
				trim(dlname, &tmp[7]);
			else
			if (strncmp(tmp, "old_library=", 12) == 0)
				trim(old_name, &tmp[12]);
			else
			if (strncmp(tmp, "libdir=", 7) == 0)
				trim(libdir, &tmp[7]);
			else
			if (strncmp(tmp, "dl_dependency_libs=", 20) == 0)
				trim(deplibs, &tmp[20]);
		}
		fclose(file);
		
		handle = (lt_dlhandle) malloc(sizeof(lt_dlhandle_t));
		if (!handle) {
			last_error = memory_error;
			free(name);
			return 0;
		}
		if (load_deplibs(handle, deplibs)) {
			free(handle);
			free(name);
			return 0;
		}
		if (find_module(&handle, dir, libdir, dlname, old_name)) {
			unload_deplibs(handle);
			free(handle);
			free(name);
			return 0;
		}
		handle->name = name;
	} else {
		/* not a libtool module */
		handle = (lt_dlhandle) malloc(sizeof(lt_dlhandle_t));
		if (!handle) {
			last_error = memory_error;
			return 0;
		}
		if (find_library(&handle, filename, *dir,
				basename, search_path)) {
			free(handle);
			return 0;
		}
		handle->name = 0;
	}
	handle->usage = 1;
	handle->next = handles;
	handles = handle;
	last_error = saved_error;
	return handle;
}

int
lt_dlclose (handle)
	lt_dlhandle handle;
{
	lt_dlhandle cur, last;
	
	/* check whether the handle is valid */
	last = cur = handles;
	while (cur && handle != cur) {
		last = cur;
		cur = cur->next;
	}
	if (!cur) {
		last_error = invalid_handle_error;
		return 1;
	}
	handle->usage--;
	if (!handle->usage) {
		int	error;
	
		if (handle != handles)
			last->next = handle->next;
		else
			handles = handle->next;
		error = handle->type->lib_close(handle);
		error += unload_deplibs(handle);
		free(handle->filename);
		if (handle->name)
			free(handle->name);
		free(handle);
		return error;
	}
	return 0;
}

#undef LT_SYMBOL_LENGTH
/* This is the maximum symbol size that won't require malloc/free */
#define LT_SYMBOL_LENGTH	256

#undef LT_SYMBOL_OVERHEAD
/* This accounts for the initial underscore, the _LTX_ separator */
/* and the string terminator */
#define LT_SYMBOL_OVERHEAD	7

lt_ptr_t
lt_dlsym (handle, symbol)
	lt_dlhandle handle;
	const char *symbol;
{
	int	lensym, lenhand;
	char	lsym[LT_SYMBOL_LENGTH];
	char	*sym;
	lt_ptr_t address;

	if (!handle) {
		last_error = invalid_handle_error;
		return 0;
	}
	lensym = strlen(symbol);
	if (handle->name)
		lenhand = strlen(handle->name);
	else
		lenhand = 0;
	if (lensym + lenhand + LT_SYMBOL_OVERHEAD < LT_SYMBOL_LENGTH)
		sym = lsym;
	else
		sym = malloc(lensym + lenhand + LT_SYMBOL_OVERHEAD);
	if (!sym) {
		last_error = buffer_overflow_error;
		return 0;
	}
	if (handle->name) {
		/* this is a libtool module */
#ifdef NEED_USCORE
		/* prefix symbol with leading underscore */
		strcpy(sym, "_");
		strcat(sym, handle->name);
#else
		strcpy(sym, handle->name);
#endif
		strcat(sym, "_LTX_");
		strcat(sym, symbol);
		/* try "modulename_LTX_symbol" */
		address = handle->type->find_sym(handle, sym);
		if (address) {
			if (sym != lsym)
				free(sym);
			return address;
		}
	}
	/* otherwise try "symbol" */
#ifdef NEED_USCORE
	/* prefix symbol with leading underscore */
	strcpy(sym, "_");
	strcat(sym, symbol);
	address = handle->type->find_sym(handle, sym);
#else
	address = handle->type->find_sym(handle, symbol);
#endif
	if (sym != lsym)
		free(sym);
	return address;
}

const char *
lt_dlerror ()
{
	const char *error = last_error;
	
	last_error = 0;
	return error;
}
