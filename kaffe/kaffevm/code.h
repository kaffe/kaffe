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

struct _methods;
struct _classFile;

void	addCode(struct _methods*, uint32, struct _classFile*);
void	addLineNumbers(struct _methods*, uint32, struct _classFile*);
void	addCheckedExceptions(struct _methods*, uint32, struct _classFile*);

#endif
