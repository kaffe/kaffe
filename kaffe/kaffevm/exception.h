/*
 * exception.h
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#ifndef __exception_h
#define __exception_h

#include "config-setjmp.h"

struct _exceptionFrame;
struct Hjava_lang_Class;
struct Hjava_lang_Object;
struct _methods;

typedef struct _jexceptionEntry {
	uintp				start_pc;
	uintp				end_pc;
	uintp				handler_pc;
	constIndex			catch_idx;
	struct Hjava_lang_Class*	catch_type;
} jexceptionEntry;

typedef struct _jexception {
	uint32				length;
	jexceptionEntry			entry[1];
} jexception;

typedef struct _vmException {
	struct _vmException*		prev;
	jmp_buf				jbuf;
	struct _methods*		meth;
	u4				pc;
	struct Hjava_lang_Object*	mobj;
} vmException;

struct _exceptionFrame;

void throwException(struct Hjava_lang_Object*) __NORETURN__;
void throwExternalException(struct Hjava_lang_Object*) __NORETURN__;

struct Hjava_lang_Object* buildStackTrace(struct _exceptionFrame*);

extern void initExceptions(void);
extern void catchSignal(int, void*);

#if defined(__WIN32__)
#define SIG_T   void(*)()
#else
#define SIG_T   void*
#endif

#endif
