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
#include "jtypes.h"
#include "file.h"
#include "itypes.h"
#include "constants.h"
#include "stringSupport.h"
#include "kaffeh-support.h"

constants* constant_pool;

/*
 * Read in constant pool from opened file.
 */
bool
readConstantPool(struct Hjava_lang_Class* this, classFile* fp, struct _errorInfo* einfo)
{
	constants* info;
	jword* pool;
	u1* tags;
	int i;
	u1 type;
	u2 len;
	u2 d2, d2b;
	u4 d4;

	info = (constants*)malloc(sizeof(constants));
	if (info == 0) {
		return (false);
	}

	readu2(&info->size, fp);
RDBG(	printf("constant_pool_count=%d\n", info->size);	)

	/* Allocate space for tags and data */
	pool = (jword*)malloc((sizeof(jword) + sizeof(u1)) * info->size);
	if (pool == 0) {
		return (false);
	}
	tags = (u1*)&pool[info->size];
	info->data = pool;
	info->tags = tags;

	pool[0] = 0;
	tags[0] = CONSTANT_Unknown;
	for (i = 1; i < info->size; i++) {

		readu1(&type, fp);
RDBG(		if (type != CONSTANT_Utf8) {
			printf("%2d: Cnst %2d\n", i, type);
		}							)
		tags[i] = type;

		switch (type) {
		case CONSTANT_Utf8:
			readu2(&len, fp);
			{
				Utf8Const *utf8;
				char *buf;

				buf = KMALLOC(len);
				readm(buf, len, sizeof(u1), fp);
				utf8 = utf8ConstNew(buf, len);
				KFREE(buf);
				pool[i] = (jword) &(utf8->data);
			}
			break;

		case CONSTANT_Class:
			readu2(&d2, fp);
			pool[i] = d2;
			break;

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
		case CONSTANT_Double:
			tags[i+1] = CONSTANT_Unknown;
#if defined(WORDS_BIGENDIAN)
			readu4(&d4, fp);
			pool[i] = d4;
			readu4(&d4, fp);
			pool[i+1] = d4;
#else
			readu4(&d4, fp);
			pool[i+1] = d4;
			readu4(&d4, fp);
			pool[i] = d4;
#endif
			i++;
			break;

		default:
			fprintf(stderr, "Bad constant %d\n", type);
			return (false);
		}
	}

	constant_pool = info;
	return (true);
}
