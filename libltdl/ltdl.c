/* ltdl.c -- system independent dlopen wrapper
   Copyright (C) 1998-1999 Thomas Tanner <tanner@gmx.de>
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

static const char *last_error = "unknown error";

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

#ifdef RTLD_LAZY	
#  define DLOPEN_MODE	RTLD_LAZY
#else
# ifdef DL_LAZY
#  define DLOPEN_MODE	DL_LAZY
# else
#  define DLOPEN_MODE	1
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
	handle->handle = dlopen(filename, DLOPEN_MODE);
	return !(handle->handle);
}

static int
dl_close (handle)
	lt_dlhandle handle;
{
	return dlclose(handle->handle);
}

static lt_ptr_t
dl_sym (handle, symbol)
	lt_dlhandle handle;
	const char *symbol;
{
	return dlsym(handle->handle, symbol);
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

/* the following lines are (c) Tim Janik */

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
	return !(handle->handle);
}

static int
shl_close (handle)
	lt_dlhandle handle;
{
	shl_unload((shl_t) (handle->handle));
	return 0;
}

static lt_ptr_t
shl_sym (handle, symbol)
	lt_dlhandle handle;
	const char *symbol;
{
	lt_ptr_t *sym;

	if (shl_findsym ((shl_t) (handle->handle), symbol,
			TYPE_UNDEFINED, &sym) || !(handle->handle) || !sym)
		return 0;
	return sym;
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
	if (dld_link(filename))
		return 1;
	handle->handle = strdup(filename);
	return 0;
}

static int
dld_close (handle)
	lt_dlhandle handle;
{
	dld_unlink_by_file((char*)(handle->handle), 1);
	free(handle->filename);
	return 0;
}

static lt_ptr_t
dld_sym (handle, symbol)
	lt_dlhandle handle;
	const char *symbol;
{
	return dld_get_func(symbol);
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
	return !(handle->handle);
}

static int
wll_close (handle)
	lt_dlhandle handle;
{
	FreeLibrary(handle->handle);
	return 0;
}

static lt_ptr_t
wll_sym (handle, symbol)
	lt_dlhandle handle;
	const char *symbol;
{
	return GetProcAddress(handle->handle, symbol);
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

struct lt_dlsymlist
{
  char *name;
  lt_ptr_t address;
};

static struct lt_dlsymlist *preloaded_symbols;

static int
dldpre_init ()
{
	return 0;
}

static int
dldpre_exit ()
{
	return 0;
}

static int
dldpre_open (handle, filename)
	lt_dlhandle handle;
	const char *filename;
{
	struct lt_dlsymlist *s = preloaded_symbols;

	if (!s)
		return 1;

	while (s->name) {
		if (!s->address && !strcmp(s->name, filename))
			break;
		s++;
	}
	if (!s->name)
		return 1;
	handle->handle = s;
	return 0;
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
	struct lt_dlsymlist *s = (struct lt_dlsymlist*)(handle->handle);

	if (!s)
		return 0;

#if NEED_USCORE
	/* lt_dlsym will have prepended a `_', but we don't need it */
	++symbol;
#endif
	s++;
	while (s->address) {
		if (strcmp(s->name, symbol) == 0)
			return s->address;
		s++;
	}
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
	if (typecount == 0)
		return 1;
	initialized = 1;
	return 0;
}

struct lt_dlsymlist *
lt_dlpreopen (preloaded)
	struct lt_dlsymlist *preloaded;
{
#if HAVE_DLPREOPEN
	struct lt_dlsymlist *prev = preloaded_symbols;
	preloaded_symbols = preloaded;
	return prev;
#else
	return 0;
#endif
}

int
lt_dlexit ()
{
	/* shut down libltdl */
	lt_dltype type = types;
	int	errors;
	
	if (!initialized)
		return 1;	/* already deinitialized */	
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
	
	while (type) {
		if (type->lib_open(*handle, filename) == 0)
			break;
		type = type->next;
	}
	(*handle)->type = type;
	(*handle)->filename = strdup(filename);
	return !(type);
}

#undef	MAX_FILENAME
#define MAX_FILENAME 1024

static int
find_module (handle, dir, libdir, dlname, old_name)
	lt_dlhandle *handle;
	const char *dir;
	const char *libdir;
	const char *dlname;
	const char *old_name;
{
	char	fullname[MAX_FILENAME]; /* FIXME: unchecked buffer */
	
	/* search a module */
	if (*dlname) {
		/* try to open the installed module */
		strcpy(fullname, libdir);
		strcat(fullname, "/");
		strcat(fullname, dlname);
		if (tryall_dlopen(handle, fullname) == 0)
			return 0;
		/* try to open the not-installed module */
		strcpy(fullname, dir);
		strcat(fullname, ".libs/");
		strcat(fullname, dlname);
		if (tryall_dlopen(handle, fullname) == 0)
			return 0;
		strcpy(fullname, dir);
		strcat(fullname, dlname);
		if (tryall_dlopen(handle, fullname) == 0)
			return 0;
	}
	if (*old_name && tryall_dlopen(handle, old_name) == 0)
		return 0;
	return 1;
}

#undef	READTEXT_MODE

#ifdef _WIN32
#define	READTEXT_MODE "rt"
#else
#define	READTEXT_MODE "r"
#endif

lt_dlhandle
lt_dlopen (filename)
	const char *filename;
{
	lt_dlhandle handle;
	FILE	*file;
	char	dir[MAX_FILENAME]; /* FIXME: unchecked buffer */
	char	tmp[MAX_FILENAME]; /* FIXME: unchecked buffer */
	const char *basename, *ext, *search_path;
	
	handle = (lt_dlhandle) malloc(sizeof(lt_dlhandle_t));
	if (!handle)
		return 0;
	basename = strrchr(filename, '/');
	if (basename)
		basename++;
	else
		basename = filename;
	strncpy(dir, filename, basename - filename);
	dir[basename - filename] = '\0';
	search_path = getenv("LTDL_LIBRARY_PATH"); /* get the search path */
	/* check whether we open a libtool module (.la extension) */
	ext = strrchr(basename, '.');
	if (ext && strcmp(ext, ".la") == 0) {
		/* FIXME: unchecked buffers */
		char	dlname[MAX_FILENAME], old_name[MAX_FILENAME];
		char	libdir[MAX_FILENAME], preload[MAX_FILENAME];
		int	i;

		dlname[0] = old_name[0] = libdir[0] = preload[0] = '\0';

		file = fopen(filename, READTEXT_MODE);
		if (!file && !*dir && search_path) {
			/* try other directories */
			const char *p, *next;
			
			p = search_path;
			while (!file && p) {
				next = strchr(p, ':');
				if (next) {
					strncpy(dir, p, next - p);
					dir[next - p] = '\0';
					p = next+1;
				} else {
					strcpy(dir, p);
					p = 0;
				}
				if (!*dir)
					continue;
				strcat(dir, "/");
				strcpy(tmp, dir);
				strcat(tmp, basename);
				file = fopen(tmp, READTEXT_MODE);
			}
		}
		if (!file) {
			free(handle);
			return 0;
		}
		while (!feof(file)) {
			if (!fgets(tmp, MAX_FILENAME, file))
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
			if (strncmp(tmp, "preload_libs=", 13) == 0)
				trim(preload, &tmp[13]);
		}
		fclose(file);
		/* TODO: preload required libraries */
		
		if (find_module(&handle, dir, libdir, dlname, old_name)) {
			free(handle);
			return 0;
		}
		/* extract the module name from the file name */
		strcpy(tmp, basename);
		tmp[ext - basename] = '\0';
		/* canonicalize the modul name */		
		for (i = 0; i < ext - basename; i++)
			if (!isalnum(tmp[i]))
				tmp[i] = '_';
		handle->name = strdup(tmp);
	} else {
		/* not a libtool module */
		if (tryall_dlopen(*handle, filename)) {
			int	error = 1;
			
			if (!*dir && search_path) {
				/* try other directories */
				const char *p, *next;
			
				p = search_path;
				while (error && p) {
					next = strchr(p, ':');
					if (next) {
						strncpy(dir, p, next - p);
						dir[next - p] = '\0';
						p = next+1;
					} else {
						strcpy(dir, p);
						p = 0;
					}
					if (!*dir)
						continue;
					strcat(dir, "/");
					strcpy(tmp, dir);
					strcat(tmp, basename);
					error = tryall_dlopen(*handle, tmp);
				}
			}
			if (error) {
				free(handle);
				return 0;
			}
		}
		handle->name = 0;
	}
	handle->usage = 1;
	handle->next = handles;
	handles = handle;
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
	if (!cur)	
		return 1; /* invalid handle */
	handle->usage--;
	if (!handle->usage) {
		int	error;
	
		if (handle != handles)
			last->next = handle->next;
		else
			handles = handle->next;
		error = handle->type->lib_close(handle);
		free(handle->filename);
		if (handle->name)
			free(handle->name);
		free(handle);
		return error;
	}
	return 0;
}

lt_ptr_t
lt_dlsym (handle, symbol)
	lt_dlhandle handle;
	const char *symbol;
{
	char	sym[128]; /* FIXME: unchecked buffer */
	lt_ptr_t address;

	if (handle->name) { /* this is a libtool module */
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
		if (address)
			return address;
	}
	/* otherwise try "symbol" */
#ifdef NEED_USCORE
	/* prefix symbol with leading underscore */
	strcpy(sym, "_");
	strcat(sym, symbol);
	return handle->type->find_sym(handle, sym);
#else
	return handle->type->find_sym(handle, symbol);
#endif
}

const char *
lt_dlerror ()
{
	return last_error;
}
