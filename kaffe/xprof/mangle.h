/*
 * mangle.h
 * Routines for doing name mangling on Java types
 *
 * Copyright (c) 2000, 2004 University of Utah and the Flux Group.
 * All rights reserved.
 *
 * This file is licensed under the terms of the GNU Public License.
 * See the file "license.terms" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 * Contributed by the Flux Research Group, Department of Computer Science,
 * University of Utah, http://www.cs.utah.edu/flux/
 */

#ifndef __mangle_h
#define __mangle_h

#include <stdio.h>

#include "classMethod.h"

/* Bit numbers for the flags in the mangled_method structure */
enum {
	MMB_UNICODE_METHOD	/* Indicates the method name has utf chars */
};

/* Flags for the mangled_method structure */
enum {
	MMF_UNICODE_METHOD = (1L << MMB_UNICODE_METHOD)
};

/*
 * Root structure for storing a mangled method signature
 */
struct mangled_method {
	unsigned long mm_flags;	/* Flags for the structure */
	char *mm_method;	/* The mangled method name */
	char *mm_class;		/* The mangled class name */
	char **mm_args;		/* Array of mangled args */
	int mm_nargs;		/* Number of args */
};

/*
 * Allocate and initialize the root mangled_method structure
 */
struct mangled_method *createMangledMethod(void);
/*
 * Delete the mangled_method structure and any referenced memory
 */
void deleteMangledMethod(struct mangled_method *mm);
/*
 * Mangle the given method name and place it in the structure
 */
int mangleMethodName(struct mangled_method *mm, const char *name);
/*
 * Mangle the given class name and place it in the structure
 */
int mangleMethodClass(struct mangled_method *mm, void *cl, const char *name);
/*
 * Set the number of args for this mangled method
 */
int mangleMethodArgCount(struct mangled_method *mm, int count);
/*
 * Mangle the arguments in the given method
 */
int mangleMethodArgs(struct mangled_method *mm, Method *meth);
/*
 * Mangle a whole method and store it in the given mangled_method structure
 */
int mangleMethod(struct mangled_method *mm, Method *meth);
/*
 * Print the mangled method structure to the given file
 */
int printMangledMethod(struct mangled_method *mm, FILE *file);
/*
 * Mangle a primitive Java type, returns a statically allocated string that
 * has the proper mangled value.
 */
const char *manglePrimitiveType(char type);
/*
 * Mangle the given class name and return a KMALLOC'ed buffer with the string
 * and `prepend' extra bytes in front of the string.  (Note: the class name
 * needs to be in internal form, e.g. java/lang/Object.)
 */
char *mangleClassType(int prepend, void *cl, const char *name);
/*
 * Mangle a type description and return a KMALLOC'ed buffer with the mangled
 * string and `prepend' extra bytes at the front.
 */
char *mangleType(size_t prepend, const char *type);
/*
 * Mangle string `src' of length `len', and place the result into `dest'
 */
size_t mangleString(char *dest, const char *src, size_t slen, int uc);
/*
 * Determine the mangled length of the given string.  If the string doesn't
 * require any escapes then zero is returned.  If len is -1 then `term' is
 * taken as the terminating character.  If `out_len' is non-NULL then it is
 * always set to the unmangled length of `string'.
 */
size_t mangleLength(const char *string, int len, const char *term, size_t *out_len);

int vfmanglef(FILE *file, const char *format, va_list args);
int fmanglef(FILE *file, const char *format, ...);

#endif /* __mangle_h */
