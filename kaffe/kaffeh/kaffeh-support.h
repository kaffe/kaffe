/*
 * kaffeh-support.h
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#ifndef __kaffeh_support_h
#define __kaffeh_support_h

void initStub(void);
void startStub(void);

void initInclude(void);
void startInclude(void);
void endInclude(void);

void initJniInclude(void);
void startJniInclude(void);
void endJniInclude(void);

void findClass(char *nm);
void* gc_malloc_fixed(size_t sz);
 
#endif
