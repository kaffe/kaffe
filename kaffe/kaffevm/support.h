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


#include "config.h"

#if defined(HAVE_STRING_H)
#include <string.h>
#endif

#include "config-std.h"
#include "gtypes.h"
#include <stdarg.h>

#define KTHREAD(function)	jthread_ ## function
#define KSIGNAL(function)	jsignal_ ## function
#define KSEM(function)		ksem_ ## function
#define KMUTEX(function)	jmutex_ ## function
#define KCONDVAR(function)	jcondvar_ ## function

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

struct _jmethodID;
struct _errorInfo;

/* 64 should put us on the safe side */
#define	MAXMARGS		64

/*
 * The callMethodInfo structure describes the information necessary to 
 * invoke a native or just-in-time compiled method.
 *
 * It is the task of the sysdepCallMethod macro, usually defined in 
 * config/$arch/sysdepCallMethod.h, to generate a procedure call that 
 * conforms to the calling convention of a particular architecture or 
 * set of build tools.
 *
 * The sysdepCallMethod macro takes a single argument of type callMethodInfo *
 * that describes where the parameters are, where the return value should
 * go and what the signature of the method is.
 *
 * `jvalue' is a union defined in include/jni.h.  It corresponds
 * to an entry on the Java stack. 
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
 * To simplify 32 bit assembly code, we copy the last 32 bits of a 64
 * bit arg into the next slot.  This allows you to treat args as an
 * array of 32 bit values.  This simplification also makes a C version
 * of sysdepCallMethod more viable, and such a function is defined in
 * mi.c.
 *
 * Note that "callsize[i] == 2 iff callsize[i+1] == 0" -- this 
 * property can also be exploited by a sysdepCallMethod macros. 
 *
 * `function' is a pointer to the method to be invoked.
 *
 * `retsize' and `rettype' have the same value ranges as callsize[i] and 
 * calltype[i],  except they correspond to the return value.  The 
 * sysdepCallMethod must store the return value in the proper type at *ret.
 */
typedef struct {
	void*			function;  /* method information */
	jvalue*			args; /* treated as an array of method arguments */
	jvalue*			ret;
	int			nrargs; /* number of arguments in args */
	int			argsize;
	char			retsize;
	char			rettype;
	char			*callsize;
	char			*calltype;
} callMethodInfo;

struct Hjava_lang_String;
struct Hjava_lang_Class;
struct Hjava_lang_Object;
struct Hjava_lang_Throwable;
struct Hjava_lang_ClassLoader;

extern const char*		getEngine(void);
extern void initEngine(void);
extern void		do_execute_java_method(jvalue *retval, void*, const char*, const char*, struct _jmethodID*, int, ...);
extern void		do_execute_java_method_v(jvalue *retval, void*, const char*, const char*, struct _jmethodID*, int, va_list);
extern void		do_execute_java_class_method(jvalue *retval, const char*, struct Hjava_lang_ClassLoader*, const char*, const char*, ...);
extern void		do_execute_java_class_method_v(jvalue *retval, const char*, struct Hjava_lang_ClassLoader*, const char*, const char*, va_list);

extern struct Hjava_lang_Object* execute_java_constructor(const char*, struct Hjava_lang_ClassLoader*, struct Hjava_lang_Class*, const char*, ...);
extern struct Hjava_lang_Object* execute_java_constructor_v(const char*, struct Hjava_lang_ClassLoader*, struct Hjava_lang_Class*, const char*, va_list);
extern jlong		currentTime(void);

extern void	KaffeVM_callMethodA(struct _jmethodID*, void*, void*, jvalue*, jvalue*, int);
extern void	KaffeVM_callMethodV(struct _jmethodID*, void*, void*, va_list, jvalue*);
extern void     KaffeVM_safeCallMethodA(struct _jmethodID*, void*, void*, jvalue*, jvalue*, int);
extern void     KaffeVM_safeCallMethodV(struct _jmethodID*, void*, void*, va_list, jvalue*);
extern struct _jmethodID*	lookupClassMethod(struct Hjava_lang_Class*, const char*, const char*, struct _errorInfo*);
extern struct _jmethodID*	lookupObjectMethod(struct Hjava_lang_Object*, const char*, const char*, struct _errorInfo*);

struct _strconst;
extern void SignalError(const char *, const char *);
extern void SignalErrorf(const char *, const char *, ...) PRINTFFORMAT(2,3);
extern void unimp(const char*);
extern void kprintf(FILE*, const char*, ...) PRINTFFORMAT(2,3);
extern int addClasspath(const char*);
extern int prependClasspath(const char*);

void classname2pathname(const char* from, char* to);
void pathname2classname(const char* from, char* to);
int bitCount(int bits);

/*
 * Macros to manipulate bit arrays.  
 */
#define BITMAP_BPI	(sizeof(int) * 8)

/* compute the number of bytes needed to hold 'b' bits. */
#define BITMAP_BYTE_SIZE(b) (((b) + BITMAP_BPI - 1) / BITMAP_BPI)

/* create a new bitmap for b bits */
#define BITMAP_NEW(b, gctype)	\
    (int *)gc_calloc(BITMAP_BYTE_SIZE(b), sizeof(int), (gctype))

/* set nth bit, counting from MSB to the right */
#define BITMAP_SET(m, n) \
    (m[(n) / BITMAP_BPI] |= (1 << (BITMAP_BPI - 1 - (n) % BITMAP_BPI)))

/* clear the nth bit, counting from MSB to the right */
#define BITMAP_CLEAR(m, n) \
    (m[(n) / BITMAP_BPI] &= ~(1 << (BITMAP_BPI - 1 - (n) % BITMAP_BPI)))

/* test nth bit, counting from MSB to the right */
#define BITMAP_ISSET(m, n) \
    (m[(n) / BITMAP_BPI] & (1 << (BITMAP_BPI - 1 - (n) % BITMAP_BPI)))

/* copy nbits from bitmap src to bitmap dst */
#define BITMAP_COPY(dst, src, nbits) \
    memcpy(dst, src, ((nbits + BITMAP_BPI - 1)/BITMAP_BPI) * sizeof(int))

/* dump a bitmap for debugging */
#define BITMAP_DUMP(m, N) { int n; \
    for (n = 0; n < N; n++) \
	if (m[(n) / BITMAP_BPI] &  \
		(1 << (BITMAP_BPI - 1 - (n) % BITMAP_BPI))) \
	    dprintf("1"); else dprintf("0"); }

#endif
