/* java_lang_Object.h
 * Java's base class - the Object.
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#ifndef __java_lang_object_h
#define __java_lang_object_h

struct _dispatchTable;
struct _iLock;

typedef struct Hjava_lang_Object {
	struct _dispatchTable*  dtable;
	struct _iLock*		lock;
	/* Data follows on immediately */
} Hjava_lang_Object;

#endif
