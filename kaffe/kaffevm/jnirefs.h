/*
 * jnirefs.h
 * Java Native Interface - object references.
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#ifndef __jnirefs_h
#define	__jnirefs_h

#define	JNIREFS				61

typedef struct _jnirefs {
	int				next;
	int				used;
	struct _jnirefs*		prev;
	jref				objects[JNIREFS];
} jnirefs;

#endif
