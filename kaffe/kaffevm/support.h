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

/* 64 should put us on the safe side */
#define	MAXMARGS		64

/*
 * The callMethodInfo structure describes the information necessary to 
 * invoke a native or just-in-time compiled method.
 *
 * It is the task of the sysdepCallMethod macro, usually defined in 
 * config/$arch/common.h, to generate a procedure call that conforms to 
 * the calling convention of a particular architecture or set of build tools.
 *
 * The sysdepCallMethod macro takes a single argument of type callMethodInfo *
 * that describes where the parameters are, where the return value should
 * go and what the signature of the method is.
 *
 * `jvalue' is a union defined in include/jtypes.h.  It corresponds to an
 * entry on the Java stack. 
 * The suffixes i,j,b,c,s,l,f,d access the corresponding element of
 * (Java) type int, long, boolean, char, short, ref, float, and double.
 *
 * `args' is an array containing the arguments passed to the function.
 * It corresponds to the Java stack and has `nrargs' valid entries with 
 * the following property:
 *
 * If two adjacent slots on the Java stack are combined to a 64bit value,
 * it will also use two array entries, and not one.  However, the first 
 * entry will contain the 64bit value (in j or d, depending on the type), 
 * and the second entry will be undefined.  This allows for easier access, 
 * while preserving indexing.  Thus, some array entries will have 0, some
 * 32 and some 64 bits of valid data in them.  The callsize array says
 * which one it is.  
 *
 * callsize[i] may be 0, 1, or 2, depending on the number of valid bits
 * in args[i].  Similarly, calltype[i] contains the signature type of the
 * argument in args[i] ('J', 'F', etc.)
 *
 * Porters note that "callsize[i] == 2 iff callsize[i+1] == 0" -- this 
 * property is exploited by some sysdepCallMethod macros (sparc).
 *
 * `function' is a pointer to the method to be invoked.
 *
 * `retsize' and `rettype' have the same value ranges as callsize[i] and 
 * calltype[i],  except they correspond to the return value.  The 
 * sysdepCallMethod must store the return value in the proper type at *ret.
 */
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
extern jvalue		do_execute_java_method_v(void*, char*, char*, struct _methods*, int, va_list);
extern jvalue		do_execute_java_class_method(char*, char*, char*, ...);
extern jvalue		do_execute_java_class_method_v(char*, char*, char*, va_list);

extern Hjava_lang_Object* execute_java_constructor(char*, struct Hjava_lang_Class*, char*, ...);
extern Hjava_lang_Object* execute_java_constructor_v(char*, struct Hjava_lang_Class*, char*, va_list);
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
extern int addClasspath(char*);

#endif
