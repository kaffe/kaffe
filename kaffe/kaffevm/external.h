/*
 * external.h
 * Handle method calls to other languages.
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#ifndef __external_h
#define __external_h

#define	MAXSTUBLEN	1024
#define	MAXLIBPATH	1024
#define	MAXLIBS		16

/* Default library info. */
#if !defined(LIBRARYPATH)
#define	LIBRARYPATH	"LD_LIBRARY_PATH"
#endif
#define	NATIVELIBRARY	"libnative"

struct _methods;

void	initNative(void);
int	loadNativeLibrary(char*);
void*	loadNativeLibrarySym(char*);
void	native(struct _methods*);
void	addNativeFunc(char*, void*);

#endif
