/*
 * gcj-except.h
 * ...
 *
 * Copyright (c) 1996, 1997, 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

#ifndef __gcj_except_h
#define __gcj_except_h

#define	GCJ_LANGUAGECODE	11
#define	GCJ_VERSIONCODE		1

/* From libgcc2.c */
void** __get_eh_info(void);

/* From eh-common.h */
typedef struct __eh_info {

	void*			match_function;
	short			language;
	short			version;

} __eh_info;

typedef struct _gcjException {

	__eh_info		eh_info;
	Hjava_lang_Class*	class;

} gcjException;

#endif
