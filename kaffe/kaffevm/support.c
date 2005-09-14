/*
 * support.c
 * Native language support (excluding string routines).
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * Copyright (c) 2002-2005 
 *      Kaffe.org contributors. See ChangeLog for details. All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#include "debug.h"
#include "config.h"

#include "config-std.h"
#include "config-mem.h"
#include "jni.h"
#include <stdarg.h>
#include "errors.h"
#include "jni_md.h"
#include "kaffe/jmalloc.h"
#include "gtypes.h"
#include "access.h"
#include "object.h"
#include "constants.h"
#include "baseClasses.h"
#include "stringSupport.h"
#include "lookup.h"
#include "exception.h"
#include "slots.h"
#include "support.h"
#include "classMethod.h"
#include "machine.h"
#include "md.h"
#include "itypes.h"
#include "external.h"
#include "thread.h"
#include "jthread.h"
#include "locks.h"
#include "methodcalls.h"
#include "native.h"
#include "jni_i.h"

#if !defined(HAVE_GETTIMEOFDAY)
#include <sys/timeb.h>
#endif

#define	MAXEXCEPTIONLEN		200
#define	ERROR_SIGNATURE0	"()V"
#define	ERROR_SIGNATURE		"(Ljava/lang/String;)V"

/* Anchor point for user defined properties */
userProperty* userProperties = NULL;

/**
 * Call a Java method from native code.
 *
 * @param obj the obj or 0 if the method is static
 * @param method_name the name of the method to be called
 * @param signature the signature of the method to be called
 * @param mb pointer to the struct _jmethodID of the method to be called
 * @param isStaticCall true if the call is static, otherwise false
 * @param argptr va_list of the arguments to be passed to the method
 *
 * @throws NoSuchMethodError method was not found
 * @throws NullPointerException if obj is 0 but method is not static
 *
 * @return the return value of the method to be called
 */
void
do_execute_java_method_v(jvalue *retval, void* obj, const char* method_name, const char* signature, Method* mb, int isStaticCall, va_list argptr)
{
	errorInfo info;

	if (obj == 0 && (! isStaticCall || ! mb)) {
		throwException(NullPointerException);
	}

	if (mb == 0) {
		assert(method_name != NULL);
		assert(signature != NULL);
		if (isStaticCall) {
			mb = lookupClassMethod((Hjava_lang_Class*)obj, method_name, signature, &info);
		}
		else {
			mb = lookupObjectMethod((Hjava_lang_Object*)obj, method_name, signature, &info);
		}
	}

	/* No method or wrong type - throw exception */
	if (mb == 0) {
		throwError(&info);
	}
	else if (isStaticCall && (mb->accflags & ACC_STATIC) == 0) {
		throwException(NoSuchMethodError(method_name));
	}
	else if (!isStaticCall && (mb->accflags & ACC_STATIC) != 0) {
		throwException(NoSuchMethodError(method_name));
	}

	KaffeVM_callMethodV(mb, METHOD_NATIVECODE(mb), obj, argptr, retval);
}

/**
 * Call a Java method from native code.
 *
 * @param obj the obj or 0 if the method is static
 * @param method_name the name of the method to be called
 * @param signature the signature of the method to be called
 * @param mb pointer to the struct _jmethodID of the method to be called
 * @param isStaticCall true if the call is static, otherwise false
 * @param ... the arguments to be passed to the method
 *
 * @return the return value of the method to be called
 */
void
do_execute_java_method(jvalue *retval, void* obj, const char* method_name, const char* signature, Method* mb, int isStaticCall, ...)
{
	va_list argptr;

	assert(method_name != 0 || mb != 0);

	va_start(argptr, isStaticCall);
	do_execute_java_method_v(retval, obj, method_name, signature, mb, isStaticCall, argptr);
	va_end(argptr);
}

/**
 * Call a Java static method on a class from native code.
 *
 * @param cname name of the class whose method is to be called
 * @param loader the class loader that's to be used to lookup the class
 * @param method_name the name of the method to be called
 * @param signature the signature of the method to be called
 * @param argptr the arguments to be passed to the method
 *
 * @throws NuSuchMethodError if method cannot be found
 * 
 * @return the return value of the method
 */
void
do_execute_java_class_method_v(jvalue *retval, const char* cname,
	Hjava_lang_ClassLoader* loader, const char* method_name,
	const char* signature, va_list argptr)
{
	Hjava_lang_Class* clazz;
	errorInfo info;
	Method* mb = NULL;
	char *buf;

	/* Convert "." to "/" and lookup class */
	buf = checkPtr(KMALLOC(strlen(cname) + 1));
	classname2pathname(cname, buf);
	clazz = lookupClass(buf, loader, &info);
	KFREE(buf);

	/* Get method */
	if (clazz != NULL) {
		mb = lookupClassMethod(clazz, method_name, signature, &info);
	}
	if (mb == NULL) {
		throwError(&info);
	}

	/* Method must be static to invoke it here */
	if ((mb->accflags & ACC_STATIC) == 0) {
		throwException(NoSuchMethodError(method_name));
	}

	/* Make the call */
	KaffeVM_callMethodV(mb, METHOD_NATIVECODE(mb), NULL, argptr, retval);
}

/**
 * Call a Java static method on a class from native code.
 *
 * @param cname name of the class whose method is to be called
 * @param loader the class loader that's to be used to lookup the class
 * @param method_name the name of the method to be called
 * @param signature the signature of the method to be called
 * @param ... the arguments to be passed to the method
 *
 * @return the return value of the method
 */
void
do_execute_java_class_method(jvalue *retval, const char* cname, Hjava_lang_ClassLoader*loader,
	const char* method_name, const char* signature, ...)
{
	va_list argptr;

	va_start(argptr, signature);
	do_execute_java_class_method_v(retval, cname, loader, method_name, signature, argptr);
	va_end(argptr);
}

/**
 * Allocate an object and execute the constructor.
 *
 * @param cname the name of the class to be instantiated (may be 0 if cc is != 0)
 * @param loader the class loader that's to be used to lookup the class
 * @param cc the class to be instantiated (may be 0 if cname is != 0)
 * @param signature the signature of the constructor to be executed
 * @param argptr arguments to be passed to the constructor
 *
 * @throws InstantiationException if class is an interface or abstract
 * @throws NoSuchMethodError if the specified constructor cannot be found
 *
 * @return the newly allocated object
 */
Hjava_lang_Object*
execute_java_constructor_v(const char* cname, Hjava_lang_ClassLoader* loader,
	Hjava_lang_Class* cc, const char* signature, va_list argptr)
{
	Hjava_lang_Object* obj;
	Method* mb;
	jvalue retval;
	errorInfo info;
	Utf8Const* sig;

	if (cc == 0) {
		char *buf;

		/* Convert "." to "/" and lookup class */
		buf = checkPtr(KMALLOC(strlen(cname) + 1));
		classname2pathname(cname, buf);
		cc = lookupClass(buf, loader, &info);
		KFREE(buf);
		if (!cc) {
			throwError(&info);
		}
	}

	/* We cannot construct interfaces or abstract classes */
	if (CLASS_IS_INTERFACE(cc) || CLASS_IS_ABSTRACT(cc)) {
		throwException(InstantiationException(cc->name->data));
	}

	if (cc->state < CSTATE_USABLE) {
		if (processClass(cc, CSTATE_COMPLETE, &info) == false) {
			throwError(&info);
		}
	}

	sig = checkPtr(utf8ConstNew(signature, -1));
	mb = findMethodLocal(cc, constructor_name, sig);
	utf8ConstRelease(sig);
	if (mb == 0) {
		throwException(NoSuchMethodError(constructor_name->data));
	}

	obj = newObject(cc);
	assert(obj != 0);

	/* Make the call */
	KaffeVM_callMethodV(mb, METHOD_NATIVECODE(mb), obj, argptr, &retval);

	return (obj);
}

/**
 * Allocate an object and execute the constructor.
 *
 * @param cname the name of the class to be instantiated
 * @param loader the class loader that's to be used to lookup the class
 * @param cc the class to be instantiated
 * @param signature the signature of the constructor to be executed
 * @param ... arguments to be passed to the constructor
 *
 * @return the new allocated object
 */
Hjava_lang_Object*
execute_java_constructor(const char* cname, Hjava_lang_ClassLoader* loader,
	Hjava_lang_Class* cc, const char* signature, ...)
{
	va_list argptr;
	Hjava_lang_Object* obj;

	va_start(argptr, signature);
	obj = execute_java_constructor_v(cname, loader, cc, signature, argptr);
	va_end(argptr);

	return (obj);
}

/**
 * Generic routine to call a native or Java method (array style). This is the exception-safe version of
 * KaffeVM_callMethodA. Exceptions are not thrown but caught the JNI way. This is useful for internal VM
 * functions which are not exception-safe.
 *
 * @param meth the struct _jmethodID of the method to be executed
 * @param func the code that's to be executed
 * @param obj  the object whose method is to be called (my be 0 iff method is static)
 * @param args the arguments to be passed to the method
 * @param ret  buffer for the return value of the method (may be 0 iff return type is void)
 * @param promoted true iff 64 bit values occupy two entries in args, otherwise false
 */
void
KaffeVM_safeCallMethodA(Method* meth, void* func, void* obj, jvalue* args, jvalue* ret,
			int promoted)
{
  BEGIN_EXCEPTION_HANDLING_VOID();

  KaffeVM_callMethodA(meth, func, obj, args, ret, promoted);
    
  END_EXCEPTION_HANDLING();
}

/**
 * Generic routine to call a native or Java method (varargs style). This is the exception-safe version of
 * KaffeVM_callMethodV. Exceptions are not thrown but caught the JNI way. This is useful for internal VM
 * functions which are not exception-safe.
 *
 * @param meth the struct _jmethodID of the method to be executed
 * @param func the code that's to be executed
 * @param obj  the object whose method is to be called (my be 0 iff method is static)
 * @param args the arguments to be passed to the method
 * @param ret  buffer for the return value of the method (may be 0 iff return type is void)
 * @param promoted true iff 64 bit values occupy two entries in args, otherwise false
 */
void
KaffeVM_safeCallMethodV(Method* meth, void* func, void* obj, va_list args, jvalue* ret)
{
  BEGIN_EXCEPTION_HANDLING_VOID();

  KaffeVM_callMethodV(meth, func, obj, args, ret);
    
  END_EXCEPTION_HANDLING();
}

/**
 * Lookup a method given class, name and signature.
 *
 * @param cls the class where to start search
 * @param name name of the method being searched
 * @param sig signature of the method being searched
 * @param einfo struct errorInfo
 *
 * @throws OutOfMemoryError 
 *
 * @return struct _jmethodID of the method being searched or 0 in case of an error
 */
Method*
lookupClassMethod(Hjava_lang_Class* cls, const char* name, const char* sig, errorInfo *einfo)
{
	Method *meth;
	Utf8Const *name_utf8, *sig_utf8;

	assert(cls != NULL);
	assert(name != NULL);
	assert(sig != NULL);

	name_utf8 = utf8ConstNew(name, -1);
	if (!name_utf8) {
		postOutOfMemory(einfo);
		return NULL;
	}
	sig_utf8 = utf8ConstNew(sig, -1);
	if (!sig_utf8) {
		utf8ConstRelease(name_utf8);
		postOutOfMemory(einfo);
		return NULL;
	}
	meth = findMethod(cls, name_utf8, sig_utf8, einfo);
	utf8ConstRelease(name_utf8);
	utf8ConstRelease(sig_utf8);
	return(meth);
}

/**
 * Lookup a method given object, name and signature.
 *
 * @param obj the object where to start search
 * @param name name of the method being searched
 * @param sig signature of the method being searched
 * @param einfo struct errorInfo
 *
 * @return struct _jmethodID of the method being searched or 0 in case of an error
 */
Method*
lookupObjectMethod(Hjava_lang_Object* obj, const char* name, const char* sig, errorInfo *einfo)
{
	assert(obj != NULL && name != NULL && sig != NULL);
	return (lookupClassMethod(OBJECT_CLASS(obj), name, sig, einfo));
}

/**
 * Signal an error by creating the object and throwing the exception.
 * See also SignalErrorf
 *
 * @param cname the name of the class of the exception object to be created
 * @param str the message to be passed to the constructor of the exception object
 */
void
SignalError(const char* cname, const char* str)
{
	Hjava_lang_Throwable* obj;

	if (str == NULL || *str == '\0') {
		obj = (Hjava_lang_Throwable*)execute_java_constructor(cname,
			NULL, NULL, ERROR_SIGNATURE0);
	} else {
		obj = (Hjava_lang_Throwable*)execute_java_constructor(cname,
			NULL, NULL, ERROR_SIGNATURE, checkPtr(stringC2Java(str)));
	}
	throwException(obj);
}

/**
 * Signal an error by creating the object and throwing the exception.
 * allows for printf-like msg format with additional parameters.
 *
 * @param cname the name of the class of the exception object to be created
 * @param fmt the printf like format of the message
 * @param ... the parameters necessary for the message format denoted by fmt
 */
void
SignalErrorf(const char* cname, const char* fmt, ...)
{
	errorInfo info;
	va_list args;

	va_start(args, fmt);
	vpostExceptionMessage(&info, cname, fmt, args);
	va_end(args);
	throwError(&info);
}

static void
replacechar(const char* from, char* to, char old, char new)
{
	int i;

	/* Convert any 'old' in name to 'new' */
	for (i = 0; from[i] != 0; i++) {
		if (from[i] == old) {
			to[i] = new;
		}
		else {
			to[i] = from[i];
		}
	}
	to[i] = 0;
}

/**
 * Convert a class name to a path name.
 *
 * @param from points to the class name
 * @param to points to path name
 */
void
classname2pathname(const char* from, char* to)
{
	replacechar(from, to, '.', '/');
}

/**
 * Convert a path name to a class name.
 *
 * @param from points to the path name
 * @param to points to the class name
 */
void
pathname2classname(const char* from, char* to)
{
	replacechar(from, to, '/', '.');
}

/**
 * Return current time in milliseconds.
 */
jlong
currentTime(void)
{
	jlong tme;

#if defined(HAVE_GETTIMEOFDAY)
	struct timeval tm;
	gettimeofday(&tm, NULL);
	tme = (((jlong)tm.tv_sec * (jlong)1000) + ((jlong)tm.tv_usec / (jlong)1000));
#elif defined(HAVE_FTIME)
	struct timeb tm;
	ftime(&tm);
	tme = (((jlong)tm.time * (jlong)1000) + (jlong)tm.millitm);
#elif defined(HAVE__FTIME)
	struct timeb tm;
	_ftime(&tm);
	tme = (((jlong)tm.time * (jlong)1000) + (jlong)tm.millitm);
#elif defined(HAVE_TIME)
	tme = (jlong)1000 * (jlong)time(0);
#else
	tme = 0;
#endif
	return (tme);
}

/**
 * Allocate a new object of the given class name.
 *
 * @param classname the name of the class to be instantiated
 * @param loader the loader to be used to lookup the class
 *
 * @return the newly allocated object
 */
Hjava_lang_Object*
AllocObject(const char* classname, Hjava_lang_ClassLoader* loader)
{
	Hjava_lang_Class* clazz;
	errorInfo info;

	clazz = lookupClass(classname, loader, &info);
	if (clazz == 0) {
		throwError(&info);
	}
	return (newObject(clazz));
}

/**
 * Allocate a new array of a given size and type.
 *
 * @param len the size of the array
 * @param type the type of the elements of the array.
 *
 * @return the new allocated array
 */
Hjava_lang_Object*
AllocArray(jsize len, int type)
{
	return (newArray(TYPE_CLASS(type), len));
}

/**
 * Allocate a new array of the given size and class name.
 *
 * @param sz the size of the array
 * @param classname name of the class of the elements
 * @param loader the class loader to be used to lookup the class
 *
 * @return the newly allocated array
 *
 * @throws NegativeArraySizeException iff the size<0
 */
Hjava_lang_Object*
AllocObjectArray(int sz, const char* classname, Hjava_lang_ClassLoader* loader)
{
	Hjava_lang_Class *elclass;
	errorInfo info;

	if (sz < 0) {
		throwException(NegativeArraySizeException);
	}
	elclass = getClassFromSignature(classname, loader, &info);
	if (elclass == 0) {
		throwError(&info);
	}
	return (newArray(elclass, sz));

}

/**
 * Used to generate exception for unimplemented features.
 *
 * @param mess the message to be displayed
 *
 * @throws an InternalError
 */
void
unimp(const char* mess)
{
	SignalError("java.lang.InternalError", mess);
}

/**
 * Print messages.
 *
 * @param out the FILE* to write the message to
 * @param mess the printf like format of the message
 * @param ... the parameters needed for the format
 */
void
kprintf(FILE* out, const char* mess, ...)
{
	va_list argptr;

	va_start(argptr, mess);
	Kaffe_JavaVMArgs.jni_vfprintf(out, mess, argptr);
	va_end(argptr);
}

/**
 * Enter/leave critical region.  This interface is exported to 
 * native libraries to protect calls to non-reentrant functions.
 * It works as a global masterlock for C libraries that are not
 * thread-safe.
 */
void 
enterUnsafeRegion(void)
{
	KTHREAD(spinon)(NULL);
}

void 
leaveUnsafeRegion(void)
{
	KTHREAD(spinoff)(NULL);
}

/* XXX Ick */
int bitCount(int bits)
{
	int lpc, retval = 0;

	for( lpc = 0; lpc < (sizeof(int) * 8); lpc++ )
	{
		if( (1L << lpc) & bits )
			retval++;
	}
	return( retval );
}
