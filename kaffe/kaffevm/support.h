/*
 * support.h
 * Various support routines.
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#ifndef __support_h
#define	__support_h

#include <stdarg.h>

/* For user defined properties */
typedef struct _userProperty {
        char*                   key;
        char*                   value;
        struct _userProperty*   next;
} userProperty;

typedef struct _nativeFunction {
	char*			name;
	void*			func;
} nativeFunction;

#define	NATIVE_FUNC_INCREMENT	(256)

extern nativeFunction* native_funcs;

extern userProperty* userProperties;

struct _methods;

#define	MAXMARGS		16

typedef struct {
	void*			function;
	jvalue*			args;
	jvalue*			ret;
	int			nrargs;
	int			argsize;
	char			retsize;
	char			rettype;
	char			callsize[MAXMARGS];
	char			calltype[MAXMARGS];
} callMethodInfo;

struct Hjava_lang_String;
struct Hjava_lang_Class;

extern void		setProperty(void*, char*, char*);
extern void		classname2pathname(char*, char*);
extern struct Hjava_lang_Object* makeJavaCharArray(char*, int);
extern struct Hjava_lang_String* makeJavaString(char*, int);
extern char*		javaString2CString(struct Hjava_lang_String*, char*, int);
extern char*		makeCString(struct Hjava_lang_String*);
extern struct Hjava_lang_String* makeReplaceJavaStringFromUtf8(unsigned char*, int, int, int);
extern jvalue		do_execute_java_method(void*, char*, char*, struct _methods*, int, ...);
extern jvalue		do_execute_java_class_method(char* cname, char* method_name, char* signature, ...);

extern Hjava_lang_Object* execute_java_constructor(char*, struct Hjava_lang_Class*, char*, ...);
extern jlong		currentTime(void);
extern void		addNativeMethod(char*, void*);

extern void	callMethodA(struct _methods*, void*, void*, jvalue*, jvalue*);
extern void	callMethodV(struct _methods*, void*, void*, va_list, jvalue*);
extern struct _methods*	lookupClassMethod(struct Hjava_lang_Class*, char*, char*);
extern struct _methods*	lookupObjectMethod(struct Hjava_lang_Object*, char*, char*);

struct _strconst;
extern void SignalError(char *, char *) __NORETURN__;
extern void unimp(char*) __NORETURN__;
extern struct Hjava_lang_String* Utf8Const2JavaString(struct _strconst*);
extern void kprintf(FILE*, const char*, ...);

#endif
