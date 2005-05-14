/*
 * code.c
 * Process a new code attribute.
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * Copyright (c) 2004
 *	Kaffe.org contributors. See ChangeLog for details. All rights reserved.
 *   
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#include "debug.h"
#include "config.h"
#include "config-std.h"
#include "config-mem.h"
#include "jni_md.h"
#include "file.h"
#include "errors.h"
#include "bytecode.h"
#include "code.h"
#include "access.h"
#include "object.h"
#include "classMethod.h"
#include "readClass.h"
#include "exception.h"

bool
addCode(Method* m, size_t len UNUSED, classFile* fp, errorInfo *einfo)
{
	bool retval = false;
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
	
	if ((c.code_length > 0) && (c.code_length < 65536)) {
		c.code = gc_malloc(c.code_length, KGC_ALLOC_BYTECODE);
		if (c.code == 0) {
			postOutOfMemory(einfo);
			return (false);
		}
		DBG(CODEATTR,	
		dprintf("allocating bytecode @%p\n", c.code);
			);

		readm(c.code, c.code_length, sizeof(bytecode), fp);
		readu2(&elen, fp);
		DBG(CODEATTR,
		    dprintf("Exception table length = %d\n", elen);
		    );
		
		if (elen > 0) {
			c.exception_table = gc_malloc(sizeof(jexception) + ((elen - 1) * sizeof(jexceptionEntry)), KGC_ALLOC_EXCEPTIONTABLE);
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
			c.exception_table = NULL;
		}
		KGC_WRITE(m, c.code);
		KGC_WRITE(m, c.exception_table);
		addMethodCode(m, &c);
		
		retval = readAttributes(fp,
					m->class,
					READATTR_METHOD,
					m,
					einfo);
	}
	else if (c.code_length == 0)
	{
		postExceptionMessage(einfo,
				     JAVA_LANG(ClassFormatError),
				     "(class: %s, method: %s signature: %s) "
				     "Code of a method has length 0",
				     m->class->name->data,
				     m->name->data,
				     m->class->
				     methods[m - CLASS_METHODS(m->class)].
				     parsed_sig->signature->data);
	}
	else
	{
		postExceptionMessage(einfo,
				     JAVA_LANG(ClassFormatError),
				     "(class: %s, method: %s signature: %s) "
				     "Code of a method longer than 65535 bytes",
				     m->class->name->data,
				     m->name->data,
				     m->class->
				     methods[m - CLASS_METHODS(m->class)].
				     parsed_sig->signature->data);
	}
	return (retval);
}

/*
 * Read in line numbers assocated with code.
 */
bool
addLineNumbers(Method* m, size_t len UNUSED, classFile* fp, errorInfo *info)
{
	lineNumbers* lines;
	int i;
	u2 nr;
	u2 data;

	/* no checkBufSize, done in caller (readAttributes) */

	readu2(&nr, fp);

	lines = gc_malloc(sizeof(lineNumbers)+sizeof(lineNumberEntry) * nr, KGC_ALLOC_LINENRTABLE);
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
		if (lines->entry[i].start_pc >= m->c.bcode.codelen) {
			postExceptionMessage(info,
					     JAVA_LANG(ClassFormatError),
					     "%s "
					     "(Method \"%s\" has invalid pc, "
					     "%ld, for line number %d)",
					     CLASS_CNAME(m->class),
					     m->name->data,
					     (long)lines->entry[i].start_pc,
					     lines->entry[i].line_nr);
			return false;
		}
	}

	/* Attach lines to method */
	m->lines = lines;
	return true;
}

bool
addLocalVariables(Method *m, size_t len UNUSED, classFile *fp, errorInfo *info)
{
	constants *pool = CLASS_CONSTANTS(m->class);
	localVariables *lv;
	int i;
	u2 nr;
	u2 data;

	readu2(&nr, fp);

	lv = gc_malloc(sizeof(localVariables) +
		       (sizeof(localVariableEntry) * nr),
		       KGC_ALLOC_LOCALVARTABLE);
	if( lv == NULL )
	{
		postOutOfMemory(info);
		return false;
	}

	lv->length = nr;
	for( i = 0; i < nr; i++ )
	{
		readu2(&data, fp);
		lv->entry[i].start_pc = data;
		readu2(&data, fp);
		lv->entry[i].length = data;
		readu2(&data, fp);
		lv->entry[i].name_index = data;
		readu2(&data, fp);
		lv->entry[i].descriptor_index = data;
		readu2(&data, fp);
		lv->entry[i].index = data;

		if (pool->tags[lv->entry[i].name_index] != CONSTANT_Utf8) {
			postExceptionMessage(info,
					     JAVA_LANG(ClassFormatError),
					     "invalid local variable "
					     "name_index: %d",
					     lv->entry[i].name_index);
			return false;
		}
		if (pool->tags[lv->entry[i].descriptor_index] !=
		    CONSTANT_Utf8) {
			postExceptionMessage(info,
					     JAVA_LANG(ClassFormatError),
					     "invalid local variable "
					     "descriptor_index: %d",
					     lv->entry[i].name_index);
			return false;
		}
		if (lv->entry[i].index > m->localsz) {
			postExceptionMessage(info,
					     JAVA_LANG(ClassFormatError),
					     "invalid local variable "
					     "index: %d",
					     lv->entry[i].index);
			return false;
		}
	}

	m->lvars = lv;
	
	return true;
}

/*
 * Read in (checked) exceptions declared for a method
 */
bool
addCheckedExceptions(Method* m, size_t len UNUSED, classFile* fp,
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
	idx = gc_malloc(sizeof(constIndex) * nr, KGC_ALLOC_DECLAREDEXC);
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
