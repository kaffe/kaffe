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
#include "lookup.h"
#include "exception.h"
#include "slots.h"
#include "machine.h"
#include "support.h"
#include "md.h"
#include "itypes.h"
#include "external.h"
#include "thread.h"
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
do_execute_java_method_v(void* obj, char* method_name, char* signature, Method* mb, int isStaticCall, va_list argptr)
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
do_execute_java_method(void* obj, char* method_name, char* signature, Method* mb, int isStaticCall, ...)
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
do_execute_java_class_method_v(char* cname, char* method_name, char* signature, va_list argptr)
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
do_execute_java_class_method(char* cname, char* method_name, char* signature, ...)
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
execute_java_constructor_v(char* cname, Hjava_lang_Class* cc, char* signature, va_list argptr)
{
	Hjava_lang_Object* obj;
	Method* mb;
	char buf[MAXEXCEPTIONLEN];
	jvalue retval;
	errorInfo info;

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

	mb = findMethodLocal(cc, makeUtf8Const(constructor_name->data, -1), 
				 makeUtf8Const(signature, -1));
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
execute_java_constructor(char* cname, Hjava_lang_Class* cc, char* signature, ...)
{
	va_list argptr;
	Hjava_lang_Object* obj;

	va_start(argptr, signature);
	obj = execute_java_constructor_v(cname, cc, signature, argptr);
	va_end(argptr);

	return (obj);
}

/*
 * Generic routine to call a native or Java method (array style).
 */
void
callMethodA(Method* meth, void* func, void* obj, jvalue* args, jvalue* ret)
{
	char* sig;
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
		call.callsize[i] = PTR_TYPE_SIZE / SIZEOF_INT;
		call.calltype[i] = 'L';
		in[i].l = (void*)&Kaffe_JNIEnv;
		s += call.callsize[i];
		i++;
		args--; /* because args[i] would be off by one */

		/* If method is static we must insert the class as an argument */
		if (meth->accflags & ACC_STATIC) {
			call.callsize[i] = PTR_TYPE_SIZE / SIZEOF_INT;
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
		call.callsize[i] = PTR_TYPE_SIZE / SIZEOF_INT;
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
		case 'I':
		case 'Z':
		case 'S':
		case 'B':
		case 'C':
		case 'F':
			call.callsize[i] = 1;
			in[i] = args[i];
			break;

		case 'D':
		case 'J':
			call.callsize[i] = 2;
			in[i] = args[i];
			in[i+1].i = (&args[i].i)[1];
			i++;
			call.callsize[i] = 0;
			break;

		case '[':
			call.calltype[i] = 'L';	/* Looks like an object */
			call.callsize[i] = PTR_TYPE_SIZE / SIZEOF_INT;
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
			call.callsize[i] = PTR_TYPE_SIZE / SIZEOF_INT;
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
		virtualMachine(meth, (slots*)call.args, (slots*)call.ret, (*Kaffe_ThreadInterface.currentJava)());
	}
	else {
		Hjava_lang_Object* sync;

		if (meth->accflags & ACC_SYNCHRONISED) {
			if (meth->accflags & ACC_STATIC) {
				sync = &meth->class->head;
			}
			else {
				sync = (Hjava_lang_Object*)call.args[0].l;
			}
			lockMutex(sync);
		}
		else {
			sync = 0;
		}

		/* Make the call - system dependent */
		sysdepCallMethod(&call);

		if (sync != 0) {
			unlockMutex(sync);
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
	char* sig;
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
		call.callsize[i] = PTR_TYPE_SIZE / SIZEOF_INT;
		call.calltype[i] = 'L';
		in[i].l = (void*)&Kaffe_JNIEnv;
		s += call.callsize[i];
		i++;

		/* If method is static we must insert the class as an 
		 * argument 
		 */
		if (meth->accflags & ACC_STATIC) {
			call.callsize[i] = PTR_TYPE_SIZE / SIZEOF_INT;
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
		call.callsize[i] = PTR_TYPE_SIZE / SIZEOF_INT;
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
			in[i].i = va_arg(args, jint);
			break;
		case 'F':
			call.callsize[i] = 1;
			in[i].f = va_arg(args, jfloat);
			break;
		case 'D':
			call.callsize[i] = 2;
			in[i].d = va_arg(args, jdouble);
			in[i+1].i = (&in[i].i)[1];
			i++;
			call.callsize[i] = 0;
			s += 2;
			break;
		case 'J':
			call.callsize[i] = 2;
			in[i].j = va_arg(args, jlong);
			in[i+1].i = (&in[i].i)[1];
			i++;
			call.callsize[i] = 0;
			s += 2;
			break;
		case '[':
			call.callsize[i] = PTR_TYPE_SIZE / SIZEOF_INT;
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
			call.callsize[i] = PTR_TYPE_SIZE / SIZEOF_INT;
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
		virtualMachine(meth, (slots*)call.args, (slots*)call.ret, (*Kaffe_ThreadInterface.currentJava)());
	}
	else {
		Hjava_lang_Object* sync;

		if (meth->accflags & ACC_SYNCHRONISED) {
			if (meth->accflags & ACC_STATIC) {
				sync = &meth->class->head;
			}
			else {
				sync = (Hjava_lang_Object*)call.args[0].l;
			}
			lockMutex(sync);
		}
		else {
			sync = 0;
		}

		/* Make the call - system dependent */
		sysdepCallMethod(&call);

		if (sync != 0) {
			unlockMutex(sync);
		}
	}
#endif
}

/*
 * Lookup a method given class, name and signature.
 */
Method*
lookupClassMethod(Hjava_lang_Class* cls, char* name, char* sig, errorInfo *einfo)
{
	return (findMethod(cls, makeUtf8Const(name,-1), makeUtf8Const(sig,-1), einfo));
}

/*
 * Lookup a method given object, name and signature.
 */
Method*
lookupObjectMethod(Hjava_lang_Object* obj, char* name, char* sig, errorInfo *einfo)
{
	return (lookupClassMethod(OBJECT_CLASS(obj), name, sig, einfo));
}

/*
 * Signal an error by creating the object and throwing the exception.
 */
void
SignalError(char* cname, char* str)
{
	Hjava_lang_Throwable* obj;

	obj = (Hjava_lang_Throwable*)execute_java_constructor(cname,
		0, ERROR_SIGNATURE, makeJavaString(str, strlen(str)));
	throwException(obj);
}

/*
 * Convert a class name to a path name.
 */
void
classname2pathname(char* from, char* to)
{
	int i;

	/* Convert any '.' in name to '/' */
	for (i = 0; from[i] != 0; i++) {
		if (from[i] == '.') {
			to[i] = '/';
		}
		else {
			to[i] = from[i];
		}
	}
	to[i] = 0;
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

	jkey = makeJavaString(key, strlen(key));
	jvalue = makeJavaString(value, strlen(value));

	do_execute_java_method(properties, "put",
		"(Ljava/lang/Object;Ljava/lang/Object;)Ljava/lang/Object;",
		0, false, jkey, jvalue);
}

/*
 * Allocate a new object of the given class name.
 */
Hjava_lang_Object*
AllocObject(char* classname)
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
AllocObjectArray(int sz, char* classname)
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
unimp(char* mess)
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

#if defined(NO_SHARED_LIBRARIES)
/*
 * Register an user function statically linked in the binary.
 */
void
addNativeMethod(char* name, void* func)
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

/* Disable interrupts and start timing some portion of JVM activity.
 * If we are called for the first time, install the exit function
 * to print timings.  If we are called for the first time on a counter,
 * initialize it and add it to the list of all counters.
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

/* End a timing run.  Adjust total time for this counter and enable
 * interrupts.
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
