/*
 * constants.c
 * Constant management.
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#define	RDBG(s)

#include "config.h"
#include "config-std.h"
#include "config-mem.h"
#include "config-hacks.h"
#include "gtypes.h"
#include "slots.h"
#include "access.h"
#include "object.h"
#include "constants.h"
#include "object.h"
#include "errors.h"
#include "file.h"
#include "exception.h"
#include "classMethod.h"
#include "stringSupport.h"
#include "locks.h"

/*
 * Read in constant pool from opened file.
 */
bool
readConstantPool(Hjava_lang_Class* this, classFile* fp, errorInfo *einfo)
{
	constants* info = CLASS_CONSTANTS (this);
	ConstSlot* pool;
	u1* tags;
	int i;
	int j;
	u1 type;
	u2 len;
	u2 d2, d2b;
	u4 d4, d4b;
	u4 poolsize;

	readu2(&poolsize, fp);
RDBG(	dprintf("constant_pool_count=%d\n", poolsize);	)

	/* Allocate space for tags and data */
	pool = gc_malloc((sizeof(ConstSlot) + sizeof(u1)) * poolsize,
			 GC_ALLOC_CONSTANT);
	if (!pool) {
		postOutOfMemory(einfo);
		return false;
	}
	tags = (u1*)&pool[poolsize];
	info->data = pool;
	info->tags = tags;
	info->size = poolsize;

	pool[0] = 0;
	tags[0] = CONSTANT_Unknown;
	for (i = 1; i < info->size; i++) {

		readu1(&type, fp);
RDBG(		dprintf("Constant type %d\n", type);			)
		tags[i] = type;

		switch (type) {
		case CONSTANT_Utf8:
			readu2(&len, fp);
			if (!utf8ConstIsValidUtf8(fp->buf, len)) {
				postExceptionMessage(einfo,
					JAVA_LANG(ClassFormatError), 
					"Invalid UTF-8 constant");
				goto fail;
			}
			pool[i] = (ConstSlot) utf8ConstNew(fp->buf, len);
			if (!pool[i]) {
				postOutOfMemory(einfo);
				return 0;
			}
			fp->buf += len;
			break;
		case CONSTANT_Class:
		case CONSTANT_String:
			readu2(&d2, fp);
			pool[i] = d2;
			break;

		case CONSTANT_Fieldref:
		case CONSTANT_Methodref:
		case CONSTANT_InterfaceMethodref:
		case CONSTANT_NameAndType:
			readu2(&d2, fp);
			readu2(&d2b, fp);
			pool[i] = (d2b << 16) | d2;
			break;

		case CONSTANT_Integer:
		case CONSTANT_Float:
			readu4(&d4, fp);
			pool[i] = d4;
			break;

		case CONSTANT_Long:
			readu4(&d4, fp);
			readu4(&d4b, fp);
#if SIZEOF_VOIDP == 8
			pool[i] = WORDS_TO_LONG(d4, d4b);
			i++;
			pool[i] = 0;
#else
			pool[i] = d4;
			i++;
			pool[i] = d4b;
#endif /* SIZEOF_VOIDP == 8 */
			tags[i] = CONSTANT_Unknown;
			break;

		case CONSTANT_Double:
			readu4(&d4, fp);
			readu4(&d4b, fp);

#if SIZEOF_VOIDP == 8
#if defined(DOUBLE_ORDER_OPPOSITE)
			pool[i] = WORDS_TO_LONG(d4b, d4);
#else
			pool[i] = WORDS_TO_LONG(d4, d4b);
#endif /* DOUBLE_ORDER_OPPOSITE */
			i++;
			pool[i] = 0;
#else
#if defined(DOUBLE_ORDER_OPPOSITE)
			pool[i] = d4b;
			i++;
			pool[i] = d4;
#else
			pool[i] = d4;
			i++;
			pool[i] = d4b;
#endif /* DOUBLE_ORDER_OPPOSITE */
#endif /* SIZEOF_VOIDP == 8 */
			tags[i] = CONSTANT_Unknown;
			break;

		default:
			postExceptionMessage(einfo, 
				JAVA_LANG(ClassFormatError), 
				"Invalid constant type %d", type);
fail:
			while (--i >= 0) {
				if (tags[i] == CONSTANT_Utf8) {
					utf8ConstRelease((Utf8Const*)pool[i]);
				}
			}
			return (false);
		}
	}

	/* Perform some constant pool optimisations to allow for the
	 * use of pre-compiled classes.
	 */
	for (i = 1; i < info->size; i++) {
		switch (info->tags[i]) {
		case CONSTANT_Class:
		case CONSTANT_String:
			j = CLASS_NAME(i, info);
			if (info->tags[j] == CONSTANT_Utf8) {
				/* Rewrite so points directly at string */
				info->data[i] = info->data[j];
				utf8ConstAddRef(WORD2UTF(info->data[j]));
			}
			else {
				/* Set this tag so it will generate an error
				 * during verification.
				 */
				info->tags[i] = CONSTANT_Error;
			}
			break;

		default:
			/* Ignore the rest */
			break;
		}
	}
	return (true);
}
