/*
 * kaffeh-support.h
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#ifndef __kaffeh_kaffeh_support_h
#define __kaffeh_kaffeh_support_h

#include "gtypes.h"
#include "file.h"

extern void initStub(void);
extern void startStub(void);

extern void initInclude(void);
extern void startInclude(void);
extern void endInclude(void);

extern void initJniInclude(void);
extern void startJniInclude(void);
extern void endJniInclude(void);

extern void kaffeh_findClass(const char *nm);

/* support.c */
extern bool addCode(Method* m, size_t len, classFile* fp, errorInfo *einfo);	
extern bool addLineNumbers(Method* m, size_t len, classFile* fp, errorInfo *info);
extern bool addLocalVariables(Method* m, size_t len, classFile* fp, errorInfo *info);
extern bool addCheckedExceptions(Method* m, size_t len, classFile* fp, errorInfo *info);

/* sig.c */
extern const char* translateSig(const char*, const char**, int*);
extern const char* translateSigType(const char*, char*);

#endif /* __kaffeh_kaffeh_support_h */
