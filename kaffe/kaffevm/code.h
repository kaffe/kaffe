/* code.h
 * Define a code module.
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#ifndef __code_h
#define __code_h

#include "gtypes.h"

union _attribute_info;
struct _jexception;

typedef struct _Code {
	u2			max_stack;
	u2			max_locals;
	u4			code_length;
	u1*			code;
	u2			attribute_count;
	struct _jexception*	exception_table;
	union _attribute_info*	attributes;
} Code;

typedef struct _lineNumberEntry {
	uint16			line_nr;
	uintp			start_pc;
} lineNumberEntry;

typedef struct _lineNumbers {
	uint32			length;
	lineNumberEntry		entry[1];
} lineNumbers;

typedef struct _localVariableEntry {
	uintp start_pc;
	u2 length;
	u2 name_index;
	u2 descriptor_index;
	u2 index;
} localVariableEntry;

typedef struct _localVariables {
	uint32 length;
	localVariableEntry entry[1];
} localVariables;

struct _jmethodID;
struct classFile;

bool	addCode(struct _jmethodID*, size_t, struct classFile*,
		errorInfo *info);
bool	addLineNumbers(struct _jmethodID*, size_t, struct classFile*,
		       errorInfo *info);
bool	addLocalVariables(struct _jmethodID*, size_t, struct classFile *,
			  errorInfo *info);
bool	addCheckedExceptions(struct _jmethodID*, size_t,
			     struct classFile*, errorInfo *info);

#endif
