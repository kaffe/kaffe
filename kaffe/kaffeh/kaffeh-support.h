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

extern void initStub(void);
extern void startStub(void);

extern void initInclude(void);
extern void startInclude(void);
extern void endInclude(void);

extern void initJniInclude(void);
extern void startJniInclude(void);
extern void endJniInclude(void);

extern void kaffeh_findClass(const char *nm);

/* sig.c */
extern const char* translateSig(const char*, const char**, int*);
extern const char* translateSigType(const char*, char*);

#endif /* __kaffeh_kaffeh_support_h */
