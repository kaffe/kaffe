/*
 * readClassConfig.h
 * Various bits of information in a Java class file.
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#ifndef __readclassconfig_h
#define __readclassconfig_h

#include <stdio.h>

typedef struct Hjava_lang_Class Hjava_lang_Class;
typedef struct _methods methods;

/* Following functions are defined in kaffeh/support.c */
void addClass(u2 this, u2 super, u2 access, constants* cpool);
void readFieldEnd(void);
void readField(classFile* fp, Hjava_lang_Class* this, constants* cpool);
void readMethod(classFile* fp, Hjava_lang_Class* this, constants* cpool);

#define	ADDCLASS(t, s, a, c)	addClass(t, s, a, c)
#define	READFIELD(f, t)		readField(f, t, constant_pool)
#define	READFIELD_ATTRIBUTE(f,t)
#define	READFIELD_END(c)	readFieldEnd()
#define	READMETHOD(f, t)	readMethod(f, t, constant_pool)

extern constants* constant_pool;

#undef	EXIT
#undef	ABORT
#define	EXIT(X)	exit(X)
#define	ABORT()	abort()

#endif
