/*
 * code.c
 * Process a new code attribute.
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#include "debug.h"
#include "config.h"
#include "config-std.h"
#include "config-mem.h"
#include "kaffe/jtypes.h"
#include "file.h"
#include "errors.h"
#include "bytecode.h"
#include "code.h"
#include "access.h"
#include "object.h"
#include "classMethod.h"
#include "readClass.h"
#include "slots.h"
#include "exception.h"

bool
addCode(Method* m, uint32 len, classFile* fp, errorInfo *einfo)
{
	Code c;
	int i;
	u2 i2;
	u2 elen;

	/* no checkBufSize calls necessary, done in caller (readAttributes) */

	readu2(&c.max_stack, fp);
	readu2(&c.max_locals, fp);
	readu4(&c.code_length, fp);
	DBG(CODEATTR,	
	    dprintf("addCode for method %s.%s\n", CLASS_CNAME(m->class), m->name->data);	
	dprintf("Max stack = %d\n", c.max_stack);
	dprintf("Max locals = %d\n", c.max_locals);
	dprintf("Code length = %d\n", c.code_length);
		);
	
	if (c.code_length > 0) {
		c.code = gc_malloc(c.code_length, GC_ALLOC_BYTECODE);
		if (c.code == 0) {
			postOutOfMemory(einfo);
			return (false);
		}
		DBG(CODEATTR,	
		dprintf("allocating bytecode @%p\n", c.code);
			);

		readm(c.code, c.code_length, sizeof(bytecode), fp);
	}
	else {
		c.code = 0;
	}

	readu2(&elen, fp);
	DBG(CODEATTR,
	    dprintf("Exception table length = %d\n", elen);
		);

	if (elen > 0) {
		c.exception_table = gc_malloc(sizeof(jexception) + ((elen - 1) * sizeof(jexceptionEntry)), GC_ALLOC_EXCEPTIONTABLE);
		if (c.exception_table == 0) {
			if (c.code) {
				gc_free(c.code);
			}
			return false;
		}
		c.exception_table->length = elen;

		for (i = 0; i < elen; i++) {
			readu2(&i2, fp);
			c.exception_table->entry[i].start_pc = i2;
			readu2(&i2, fp);
			c.exception_table->entry[i].end_pc = i2;
			readu2(&i2, fp);
			c.exception_table->entry[i].handler_pc = i2;
			readu2(&i2, fp);
			c.exception_table->entry[i].catch_idx = i2;
			c.exception_table->entry[i].catch_type = NULL;
		}
	}
	else {
		c.exception_table = 0;
	}
	GC_WRITE(m, c.code);
	GC_WRITE(m, c.exception_table);
	addMethodCode(m, &c);

	return (readAttributes(fp, m->class, READATTR_METHOD, m, einfo));
}

/*
 * Read in line numbers assocated with code.
 */
bool
addLineNumbers(Method* m, uint32 len, classFile* fp, errorInfo *info)
{
	lineNumbers* lines;
	int i;
	u2 nr;
	u2 data;

	/* no checkBufSize, done in caller (readAttributes) */

	readu2(&nr, fp);

	lines = KMALLOC(sizeof(lineNumbers)+sizeof(lineNumberEntry) * nr);
	if (!lines) {
		postOutOfMemory(info);
		return false;
	}
	
	lines->length = nr;
	for (i = 0; i < nr; i++) {
		readu2(&data, fp);
		lines->entry[i].start_pc = data;
		readu2(&data, fp);
		lines->entry[i].line_nr = data;
	}

	/* Attach lines to method */
	m->lines = lines;
	return true;
}

/*
 * Read in (checked) exceptions declared for a method
 */
bool
addCheckedExceptions(Method* m, uint32 len, classFile* fp,
		     errorInfo *info)
{
	int i;
	u2 nr;
	constIndex *idx;

	/* no checkBufSize, done in caller (readAttributes) */

	readu2(&nr, fp);
	if (nr == 0) {
		return true;
	}

	m->ndeclared_exceptions = nr;
	idx = KMALLOC(sizeof(constIndex) * nr);
	if (!idx) {
		postOutOfMemory(info);
		return false;
	}
	
	m->declared_exceptions = idx;

	for (i = 0; i < nr; i++) {
		readu2(idx + i, fp);
	}
	return true;
}
