/*
 * support.c
 * Native language support (excluding string routines).
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
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
#include "classMethod.h"
#include "jtypes.h"
#include "access.h"
#include "object.h"
#include "constants.h"
#include "baseClasses.h"
#include "stringSupport.h"
#include "lookup.h"
#include "exception.h"
#include "slots.h"
#include "support.h"
#define NEED_sysdepCallMethod 1
#include "machine.h"
#include "md.h"
#include "itypes.h"
#include "external.h"
#include "thread.h"
#include "jthread.h"
#include "locks.h"

#if !defined(HAVE_GETTIMEOFDAY)
#include <sys/timeb.h>
#endif

#define	MAXEXCEPTIONLEN		200
#define	ERROR_SIGNATURE		"(Ljava/lang/String;)V"

/* Anchor point for user defined properties */
userProperty* userProperties;

#if defined(NO_SHARED_LIBRARIES)
/* Internal native functions */
static nativeFunction null_funcs[1];
nativeFunction* native_funcs = null_funcs;
#endif

extern struct JNIEnv_ Kaffe_JNIEnv;


/*
 * Call a Java method from native code.
 */
jvalue
do_execute_java_method_v(void* obj, const char* method_name, const char* signature, Method* mb, int isStaticCall, va_list argptr)
{
	jvalue retval;
	errorInfo info;

	if (mb == 0) {
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

	callMethodV(mb, METHOD_INDIRECTMETHOD(mb), obj, argptr, &retval);

	return (retval);
}

jvalue
do_execute_java_method(void* obj, const char* method_name, const char* signature, Method* mb, int isStaticCall, ...)
{
	va_list argptr;
	jvalue retval;

	va_start(argptr, isStaticCall);
	retval = do_execute_java_method_v(obj, method_name, signature, mb, isStaticCall, argptr);
	va_end(argptr);

	return (retval);
}

/*
 * Call a Java static method on a class from native code.
 */
jvalue
do_execute_java_class_method_v(const char* cname, const char* method_name, const char* signature, va_list argptr)
{
	Method* mb = 0;
	jvalue retval;
	char cnname[CLASSMAXSIG];	/* Unchecked buffer - FIXME! */
	Hjava_lang_Class* clazz;
	errorInfo info;

	/* Convert "." to "/" */
	classname2pathname(cname, cnname);

	clazz = lookupClass(cnname, &info);
	if (clazz != 0) {
		mb = lookupClassMethod(clazz, method_name, signature, &info);
	}

	if (mb == 0) {
		throwError(&info);
	}

	/* Method must be static to invoke it here */
	if ((mb->accflags & ACC_STATIC) == 0) {
		throwException(NoSuchMethodError(method_name));
	}

	/* Make the call */
	callMethodV(mb, METHOD_INDIRECTMETHOD(mb), 0, argptr, &retval);

	return (retval);
}

jvalue
do_execute_java_class_method(const char* cname, const char* method_name, const char* signature, ...)
{
	va_list argptr;
	jvalue retval;

	va_start(argptr, signature);
	retval = do_execute_java_class_method_v(cname, method_name, signature, argptr);
	va_end(argptr);

	return (retval);
}

/*
 * Allocate an object and execute the constructor.
 */
Hjava_lang_Object*
execute_java_constructor_v(const char* cname, Hjava_lang_Class* cc, const char* signature, va_list argptr)
{
	Hjava_lang_Object* obj;
	Method* mb;
	char buf[MAXEXCEPTIONLEN];
	jvalue retval;
	errorInfo info;
	Utf8Const* sig;

	if (cc == 0) {
		/* Convert "." to "/" */
		classname2pathname(cname, buf);

		cc = lookupClass(buf, &info);
		assert(cc != 0);
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

	sig = utf8ConstNew(signature, -1);
	mb = findMethodLocal(cc, constructor_name, sig);
	utf8ConstRelease(sig);
	if (mb == 0) {
		throwException(NoSuchMethodError(constructor_name->data));
	}

	obj = newObject(cc);
	assert(obj != 0);

	/* Make the call */
	callMethodV(mb, METHOD_INDIRECTMETHOD(mb), obj, argptr, &retval);

	return (obj);
}

Hjava_lang_Object*
execute_java_constructor(const char* cname, Hjava_lang_Class* cc, const char* signature, ...)
{
	va_list argptr;
	Hjava_lang_Object* obj;

	va_start(argptr, signature);
	obj = execute_java_constructor_v(cname, cc, signature, argptr);
	va_end(argptr);

	return (obj);
}

/* This is defined in the alpha and mips N32 ports.  */
#if PROMOTE_TO_64bits
# define PROMOTE_jint2jlong 1
# define PROMOTE_jfloat2jdouble 1
#endif

/* This is currently only defined by PROMOTE_TO_64bits.  It causes
   32-bit integer types to be promoted to 64-bits.  */
#if PROMOTE_jint2jlong
# if ! PROMOTE_TO_64bits
#  error "PROMOTE_jint2jlong is only supported with PROMOTE_TO_64bits"
# endif
# define PROM_i j
#else
# define PROM_i i
#endif

/* This is currently defined by PROMOTE_TO_64bits and in the mips O32
   port.  It causes 32-bit float types to be promoted to 64-bit
   doubles.  */
#if PROMOTE_jfloat2jdouble
# define PROM_f d
#else
# define PROM_f f
#endif

/* If we promote everything to 64bits, all sizes can be divided by 2.  */
#if PROMOTE_jint2jlong && PROMOTE_jfloat2jdouble
# define PROM_DIV 2
# if PTR_TYPE_SIZE != 2 * SIZEOF_INT
#  error "PROMOTE_TO_64bits requires 64bits pointers"
# endif
#else
# define PROM_DIV 1
#endif

/* This is currently defined in the MIPS O32 port.  It causes 64bits
   arguments to be forced into even arguments, by introducing a padding
   integer argument. */
#if ALIGN_AT_64bits
# define ENSURE_ALIGN64() do { if (call.callsize[i] == 2 && (i & 1)) { call.callsize[i] = 1; call.calltype[i] = 'I'; ++i; ++s; call.callsize[i] = 2; } } while (0)
#else
# define ENSURE_ALIGN64() do {} while (0)
#endif

/*
 * Generic routine to call a native or Java method (array style).
 */
void
callMethodA(Method* meth, void* func, void* obj, jvalue* args, jvalue* ret)
{
	const char* sig;
	int i;
	int s;
	/* XXX call.callsize and call.calltype arrays are statically sized 
	   and are not checked for running out of bounds */
	callMethodInfo call;	
	jvalue in[MAXMARGS];
	jvalue tmp;

	if (ret == 0) {
		ret = &tmp;
	}
	sig = meth->signature->data;
	i = 0;
	s = 0;

#if defined(INTERPRETER)
	/*
	 * If the method is native, we must find it so that we know whether
	 * it is a JNI method or not.  If it is one, ACC_JNI will be set
	 * upon return from native and we will add additional parameters 
	 * according to the JNI calling convention.
	 */
	meth = (Method*)func;
	if (meth->accflags & ACC_NATIVE) {
		if (METHOD_NATIVECODE(meth) == 0) {
			errorInfo info;
			if (native(meth, &info) == false) {
				throwError(&info);
			}
		}
		call.function = METHOD_NATIVECODE(meth);
	}

	/* Insert the JNI environment */
	if (meth->accflags & ACC_JNI) {
		call.callsize[i] = PTR_TYPE_SIZE / SIZEOF_INT / PROM_DIV;
		ENSURE_ALIGN64();
		call.calltype[i] = 'L';
		in[i].l = (void*)&Kaffe_JNIEnv;
		s += call.callsize[i];
		i++;
		args--; /* because args[i] would be off by one */

		/* If method is static we must insert the class as an argument */
		if (meth->accflags & ACC_STATIC) {
			call.callsize[i] = PTR_TYPE_SIZE / SIZEOF_INT / PROM_DIV;
			ENSURE_ALIGN64();
			s += call.callsize[i];
			call.calltype[i] = 'L';
			in[i].l = meth->class;
			i++;
			args--; /* because args[i] would be off by one */
		}
	} 
#endif

	/* If this method isn't static, we must insert the object as
	 * an argument.
 	 */
	if ((meth->accflags & ACC_STATIC) == 0) {
		call.callsize[i] = PTR_TYPE_SIZE / SIZEOF_INT / PROM_DIV;
		ENSURE_ALIGN64();
		s += call.callsize[i];
		call.calltype[i] = 'L';
		in[i].l = obj;
		i++;
		args--; /* because args[i] would be off by one */
	}

	sig++;	/* Skip leading '(' */
	for (; *sig != ')'; i++, sig++) {
		call.calltype[i] = *sig;
		switch (*sig) {
		case 'Z':
			call.callsize[i] = 1;
			in[i].PROM_i = args[i].z;
			break;

		case 'S':
			call.callsize[i] = 1;
			in[i].PROM_i = args[i].s;
			break;

		case 'B':
			call.callsize[i] = 1;
			in[i].PROM_i = args[i].b;
			break;

		case 'C':
			call.callsize[i] = 1;
			in[i].PROM_i = args[i].c;
			break;

		case 'F':
#if ! PROMOTE_jfloat2jdouble
			call.callsize[i] = 1;
			in[i].f = args[i].f;
			break;
#else
			call.callsize[i] = 2 / PROM_DIV;
			ENSURE_ALIGN64();
			call.calltype[i] = 'D';
			in[i].d = args[i].f;
			goto second_word;
#endif
		case 'I':
			call.callsize[i] = 1;
			in[i].PROM_i = args[i].i;
			break;
		case 'D':
		case 'J':
			call.callsize[i] = 2 / PROM_DIV;
			ENSURE_ALIGN64();
			in[i] = args[i];
		second_word:
			if (call.callsize[i] > 1) {
			  s += call.callsize[i];
			  in[i+1].i = (&args[i].i)[1];
			  i++;
			  call.calltype[i] = 0;
			  call.callsize[i] = 0;
			}
			break;

		case '[':
			call.callsize[i] = PTR_TYPE_SIZE / SIZEOF_INT / PROM_DIV;
			ENSURE_ALIGN64();
			call.calltype[i] = 'L';	/* Looks like an object */
			in[i] = args[i];
			while (*sig == '[') {
				sig++;
			}
			if (*sig == 'L') {
				while (*sig != ';') {
					sig++;
				}
			}
			break;
		case 'L':
			call.callsize[i] = PTR_TYPE_SIZE / SIZEOF_INT / PROM_DIV;
			ENSURE_ALIGN64();
			in[i] = args[i];
			while (*sig != ';') {
				sig++;
			}
			break;
		default:
			ABORT();
		}
		s += call.callsize[i];
	}
	sig++;	/* Skip trailing ')' */

	/* Return info */
	call.rettype = *sig;
	if (*sig == 'L' || *sig == '[') {
		call.retsize = PTR_TYPE_SIZE / SIZEOF_INT;
	}
	else if (*sig == 'V') {
		call.retsize = 0;
	}
	else if (*sig == 'D' || *sig == 'J') {
		call.retsize = 2;
	}
	else {
		call.retsize = 1;
	}

	/* Call info and arguments */
	call.nrargs = i;
	call.argsize = s;
	call.args = in;
	call.ret = ret;

#if defined(TRANSLATOR)
	call.function = func;
	/* Make the call - system dependent */
	sysdepCallMethod(&call);
#endif
#if defined(INTERPRETER)
	if ((meth->accflags & ACC_NATIVE) == 0) {
		virtualMachine(meth, (slots*)call.args, (slots*)call.ret, getCurrentThread());
	}
	else {
		iLock* sync;

		if (meth->accflags & ACC_SYNCHRONISED) {
			Hjava_lang_Object* syncobj;
			if (meth->accflags & ACC_STATIC) {
				syncobj = &meth->class->head;
			}
			else {
				syncobj = (Hjava_lang_Object*)call.args[0].l;
			}
			sync = lockMutex(syncobj);
		}
		else {
			sync = 0;
		}

		/* Make the call - system dependent */
		sysdepCallMethod(&call);

		if (sync != 0) {
			unlockKnownMutex(sync);
		}
	}
#endif
}

/*
 * Generic routine to call a native or Java method (varargs style).
 */
void
callMethodV(Method* meth, void* func, void* obj, va_list args, jvalue* ret)
{
	const char* sig;
	int i;
	int s;
	/* XXX call.callsize and call.calltype arrays are statically sized 
	   and are not checked for running out of bounds */
	callMethodInfo call;
	jvalue in[MAXMARGS];
	jvalue tmp;

	if (ret == 0) {
		ret = &tmp;
	}
	sig = meth->signature->data;
	i = 0;
	s = 0;

#if defined(INTERPRETER)
	meth = (Method*)func;
	if (meth->accflags & ACC_NATIVE) {
                if (METHOD_NATIVECODE(meth) == 0) {
			errorInfo info;
			if (native(meth, &info) == false) {
				throwError(&info);
			}
                }
		call.function = METHOD_NATIVECODE(meth);
	}

	/* Insert the JNI environment */
	if (meth->accflags & ACC_JNI) {
		call.callsize[i] = PTR_TYPE_SIZE / SIZEOF_INT / PROM_DIV;
		ENSURE_ALIGN64();
		call.calltype[i] = 'L';
		in[i].l = (void*)&Kaffe_JNIEnv;
		s += call.callsize[i];
		i++;

		/* If method is static we must insert the class as an 
		 * argument 
		 */
		if (meth->accflags & ACC_STATIC) {
			call.callsize[i] = PTR_TYPE_SIZE / SIZEOF_INT / PROM_DIV;
			ENSURE_ALIGN64();
			s += call.callsize[i];
			call.calltype[i] = 'L';
			in[i].l = meth->class;
			i++;
		}
	}
#endif

	/* If this method isn't static, we must insert the object as
	 * the first argument and get the function code.
 	 */
	if ((meth->accflags & ACC_STATIC) == 0) {
		call.callsize[i] = PTR_TYPE_SIZE / SIZEOF_INT / PROM_DIV;
		ENSURE_ALIGN64();
		s += call.callsize[i];
		call.calltype[i] = 'L';
		in[i].l = obj;
		i++;
	}

	sig++;	/* Skip leading '(' */
	for (; *sig != ')'; i++, sig++) {
		call.calltype[i] = *sig;
		switch (*sig) {
		case 'I':
		case 'Z':
		case 'S':
		case 'B':
		case 'C':
			call.callsize[i] = 1;
			in[i].PROM_i = va_arg(args, jint);
			break;
		case 'F':
#if ! PROMOTE_jfloat2jdouble
			call.callsize[i] = 1;
			in[i].PROM_f = (jfloat)va_arg(args, jdouble);
			break;
#else
			call.callsize[i] = 2 / PROM_DIV;
			ENSURE_ALIGN64();
			call.calltype[i] = 'D';
			in[i].d = va_arg(args, jdouble);
			goto second_word;
#endif
		case 'D':
			call.callsize[i] = 2 / PROM_DIV;
			ENSURE_ALIGN64();
			in[i].d = va_arg(args, jdouble);
			goto second_word;
		case 'J':
			call.callsize[i] = 2 / PROM_DIV;
			ENSURE_ALIGN64();
			in[i].j = va_arg(args, jlong);
		second_word:
			if (call.callsize[i] > 1) {
			  s += call.callsize[i];
			  in[i+1].i = (&in[i].i)[1];
			  i++;
			  call.callsize[i] = 0;
			}
			break;
		case '[':
			call.callsize[i] = PTR_TYPE_SIZE / SIZEOF_INT / PROM_DIV;
			call.calltype[i] = 'L';	/* Looks like an object */
			in[i].l = va_arg(args, jref);
			while (*sig == '[') {
				sig++;
			}
			if (*sig == 'L') {
				while (*sig != ';') {
					sig++;
				}
			}
			break;
		case 'L':
			call.callsize[i] = PTR_TYPE_SIZE / SIZEOF_INT / PROM_DIV;
			ENSURE_ALIGN64();
			in[i].l = va_arg(args, jref);
			while (*sig != ';') {
				sig++;
			}
			break;
		default:
			ABORT();
		}
		s += call.callsize[i];
	}
	sig++;	/* Skip trailing ')' */

	/* Return info */
	call.rettype = *sig;
	if (*sig == 'L' || *sig == '[') {
		call.retsize = PTR_TYPE_SIZE / SIZEOF_INT;
	}
	else if (*sig == 'V') {
		call.retsize = 0;
	}
	else if (*sig == 'D' || *sig == 'J') {
		call.retsize = 2;
	}
	else {
		call.retsize = 1;
	}

	/* Call info and arguments */
	call.nrargs = i;
	call.argsize = s;
	call.args = in;
	call.ret = ret;

#if defined(TRANSLATOR)
	call.function = func;
	/* Make the call - system dependent */
	sysdepCallMethod(&call);
#endif
#if defined(INTERPRETER)
	if ((meth->accflags & ACC_NATIVE) == 0) {
		virtualMachine(meth, (slots*)call.args, (slots*)call.ret, getCurrentThread());
	}
	else {
		iLock* sync;

		if (meth->accflags & ACC_SYNCHRONISED) {
			Hjava_lang_Object* syncobj;
			if (meth->accflags & ACC_STATIC) {
				syncobj = &meth->class->head;
			}
			else {
				syncobj = (Hjava_lang_Object*)call.args[0].l;
			}
			sync = lockMutex(syncobj);
		}
		else {
			sync = 0;
		}

		/* Make the call - system dependent */
		sysdepCallMethod(&call);

		if (sync != 0) {
			unlockKnownMutex(sync);
		}
	}
#endif
}

/*
 * Lookup a method given class, name and signature.
 */
Method*
lookupClassMethod(Hjava_lang_Class* cls, const char* name, const char* sig, errorInfo *einfo)
{
	Method *meth;
	Utf8Const *name_utf8, *sig_utf8;

	name_utf8 = utf8ConstNew(name, -1);
	sig_utf8 = utf8ConstNew(sig, -1);
	meth = findMethod(cls, name_utf8, sig_utf8, einfo);
	utf8ConstRelease(name_utf8);
	utf8ConstRelease(sig_utf8);
	return(meth);
}

/*
 * Lookup a method given object, name and signature.
 */
Method*
lookupObjectMethod(Hjava_lang_Object* obj, const char* name, const char* sig, errorInfo *einfo)
{
	return (lookupClassMethod(OBJECT_CLASS(obj), name, sig, einfo));
}

/*
 * Signal an error by creating the object and throwing the exception.
 */
void
SignalError(const char* cname, const char* str)
{
	Hjava_lang_Throwable* obj;

	obj = (Hjava_lang_Throwable*)execute_java_constructor(cname,
		0, ERROR_SIGNATURE, stringC2Java(str));
	throwException(obj);
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

/*
 * Convert a class name to a path name.
 */
void
classname2pathname(const char* from, char* to)
{
	replacechar(from, to, '.', '/');
}

/*
 * Convert a path name to a class name.
 */
void
pathname2classname(const char* from, char* to)
{
	replacechar(from, to, '/', '.');
}

/*
 * Return current time in milliseconds.
 */
jlong
currentTime(void)
{
	jlong tme;

#if defined(HAVE_GETTIMEOFDAY)
	struct timeval tm;
	gettimeofday(&tm, 0);
	tme = (((jlong)tm.tv_sec * (jlong)1000) + ((jlong)tm.tv_usec / (jlong)1000));
#elif defined(HAVE_FTIME)
	struct timeb tm;
	ftime(&tm);
	tme = (((jlong)tm.time * (jlong)1000) + (jlong)tm.millitm);
#elif defined(HAVE_TIME)
	tme = (jlong)1000 * (jlong)time(0);
#else
	tme = 0;
#endif
	return (tme);
}

/*
 * Set a property to a value.
 */
void
setProperty(void* properties, char* key, char* value)
{
	Hjava_lang_String* jkey;
	Hjava_lang_String* jvalue;

	jkey = stringC2Java(key);
	jvalue = stringC2Java(value);

	do_execute_java_method(properties, "put",
		"(Ljava/lang/Object;Ljava/lang/Object;)Ljava/lang/Object;",
		0, false, jkey, jvalue);
}

/*
 * Allocate a new object of the given class name.
 */
Hjava_lang_Object*
AllocObject(const char* classname)
{
	errorInfo info;
	Hjava_lang_Class* clazz = lookupClass(classname, &info);

	if (clazz == 0) {
		throwError(&info);
	}
	return (newObject(clazz));
}

/*
 * Allocate a new array of a given size and type.
 */
Hjava_lang_Object*
AllocArray(int len, int type)
{
	return (newArray(TYPE_CLASS(type), len));
}

/*
 * Allocate a new array of the given size and class name.
 */
Hjava_lang_Object*
AllocObjectArray(int sz, const char* classname)
{
	Hjava_lang_Class *elclass;
	errorInfo info;

	if (sz < 0) {
		throwException(NegativeArraySizeException);
	}
        elclass = getClassFromSignature(classname, NULL, &info);
	if (elclass == 0) {
		throwError(&info);
	}
        return (newArray(elclass, sz));

}

/*
 * Used to generate exception for unimplemented features.
 */
void
unimp(const char* mess)
{
	SignalError("java.lang.InternalError", mess);
}

/*
 * Print messages.
 */
void
kprintf(FILE* out, const char* mess, ...)
{
	va_list argptr;

	va_start(argptr, mess);
	vfprintf(out, mess, argptr);
	va_end(argptr);
}

/*
 * Enter/leave critical region.  This interface is exported to 
 * native libraries to protect calls to non-reentrant functions.
 * It works as a global masterlock for C libraries that are not
 * thread-safe.
 */
void 
enterUnsafeRegion(void)
{
	jthread_spinon(0);
}

void 
leaveUnsafeRegion(void)
{
	jthread_spinoff(0);
}

#if defined(NO_SHARED_LIBRARIES)
/*
 * Register an user function statically linked in the binary.
 */
void
addNativeMethod(const char* name, void* func)
{
	static int funcs_nr = 0;
	static int funcs_max = 0;

	/* If we run out of space, reallocate */
	if (funcs_nr + 1 >= funcs_max) {
		funcs_max += NATIVE_FUNC_INCREMENT;
		if (native_funcs != null_funcs) {
			native_funcs = KREALLOC(native_funcs, funcs_max * sizeof(nativeFunction));
		}
		else {
			native_funcs = KMALLOC(NATIVE_FUNC_INCREMENT * sizeof(nativeFunction));
		}
	}
	native_funcs[funcs_nr].name = KMALLOC(strlen(name) + 1);
	strcpy(native_funcs[funcs_nr].name, name);
	native_funcs[funcs_nr].func = func;
	funcs_nr++;
	native_funcs[funcs_nr].name = 0;
	native_funcs[funcs_nr].func = 0;
}
#endif

#if defined(TIMING)

static timespent *counters;	/* all timers that have been run */

/* Print total user time, user time spent by each defined counter, along
 * with the number of passes each counter took.
 */
static void
print_time_spent(void)
{
	timespent *p;
	struct rusage tot_usage;

	getrusage(RUSAGE_SELF, &tot_usage);
	fprintf(stderr, "FACILITY\tUSER TIME\tSYSTEM TIME\tCALLS\n"
		"--------------- --------------- "
		"--------------- ---------------\n");
	for (p = counters; p; p = p->next) {
		fprintf(stderr, "%-15s %8d.%06d %8d.%06d %15d\n",
			p->name, p->total.tv_sec, p->total.tv_usec,
			p->stotal.tv_sec, p->stotal.tv_usec,
			p->calls);
	}
	fprintf(stderr, "%-15s %8d.%06d %8d.%06d\n",
		"TOTAL",
		tot_usage.ru_utime.tv_sec, tot_usage.ru_utime.tv_usec,
		tot_usage.ru_stime.tv_sec, tot_usage.ru_stime.tv_usec);
}

/* Start timing some portion of JVM activity.
 * If we are called for the first time, install the exit function
 * to print timings.  If we are called for the first time on a counter,
 * initialize it and add it to the list of all counters.
 * Caller is responsible for synchronization.
 */
void
startTiming(timespent *counter, char *name)
{
	struct rusage ru;
	
	if (!counters) atexit(print_time_spent);
	if (!counter->name) {
		counter->name = name;
		counter->next = counters;
		counters = counter;
	}
	counter->calls++;
	getrusage(RUSAGE_SELF, &ru);
	counter->current = ru.ru_utime;
	counter->scurrent = ru.ru_stime;
}

/* timeval arithmetic macros */
#define DECF(D, S)				\
{						\
	(D).tv_usec -= (S).tv_usec;		\
	if ((D).tv_usec < 0) {			\
		(D).tv_usec += 1000000;		\
		(D).tv_sec -= 1;		\
	}					\
	(D).tv_sec -= (S).tv_sec;		\
}

#define INCF(D,S)				\
{						\
	(D).tv_usec += (S).tv_usec;		\
	if ((D).tv_usec > 1000000) {		\
		(D).tv_usec -= 1000000;		\
		(D).tv_sec += 1;		\
	}					\
	(D).tv_sec += (S).tv_sec;		\
}

/*
 * End a timing run.  Adjust total time for this counter.
 */
void
stopTiming(timespent *counter)
{
	struct rusage ru;

	getrusage(RUSAGE_SELF, &ru);
	DECF(ru.ru_utime, counter->current);
	INCF(counter->total, ru.ru_utime);
	DECF(ru.ru_stime, counter->scurrent);
	INCF(counter->stotal, ru.ru_stime);
}
#endif /* TIMING */
