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

extern void initStub(void);
extern void startStub(void);

extern void initInclude(void);
extern void startInclude(void);
extern void endInclude(void);

extern void initJniInclude(void);
extern void startJniInclude(void);
extern void endJniInclude(void);

extern void findClass(char *nm);

#ifndef dprintf
extern int kaffe_dprintf(const char *, ...);
#define dprintf kaffe_dprintf
#endif
#endif
