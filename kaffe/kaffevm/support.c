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
#include "kaffe/jni_md.h"
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
#if !defined(HAVE_LIBFFI)
# define NEED_sysdepCallMethod 1
#endif /* HAVE_LIBFFI */
#include "classMethod.h"
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
#define	ERROR_SIGNATURE0	"()V"
#define	ERROR_SIGNATURE		"(Ljava/lang/String;)V"

/* Anchor point for user defined properties */
userProperty* userProperties = NULL;

#if defined(HAVE_LIBFFI)
#include <ffi.h>
static inline ffi_type *j2ffi(char type)
{
	ffi_type *ftype;

	switch (type) {
	case 'J':
		ftype = &ffi_type_sint64;
		break;
	case 'D':
		ftype = &ffi_type_double;
		break;
	case 'F':
		ftype = &ffi_type_float;
		break;
	default:
		ftype = &ffi_type_uint;
	}
	return ftype;
}

#define sysdepCallMethod(CALL)						\
do {									\
	int i;								\
	ffi_cif cif;							\
	ffi_type *rtype;						\
	ffi_type *argtypes[(CALL)->nrargs + 1];				\
	void *argvals[(CALL)->nrargs + 1];				\
	int fargs = 0;							\
	for (i = 0; i < (CALL)->nrargs; i++) {				\
		switch ((CALL)->callsize[i]) {				\
		case 1:							\
			argtypes[fargs] = j2ffi((CALL)->calltype[i]);	\
			argvals[fargs] = &((CALL)->args[i].i);		\
			fargs++;					\
			break;						\
		case 2:							\
			argtypes[fargs] = j2ffi((CALL)->calltype[i]);	\
			argvals[fargs] = &((CALL)->args[i].j);		\
			fargs++;					\
		default:						\
			break;						\
		}							\
	}								\
	rtype = j2ffi ((CALL)->rettype);				\
	if (ffi_prep_cif(&cif, FFI_DEFAULT_ABI, fargs,			\
	    rtype, argtypes) == FFI_OK) {				\
		ffi_call(&cif, (CALL)->function, (CALL)->ret, argvals);	\
	}								\
	else								\
		ABORT();						\
} while (0);
#endif /* HAVE_LIBFFI */

/**
 * Call a Java method from native code.
 *
 * @param obj the obj or 0 if the method is static
 * @param method_name the name of the method to be called
 * @param signature the signature of the method to be called
 * @param mb pointer to the struct _methods of the method to be called
 * @param isStaticCall true if the call is static, otherwise false
 * @param argptr va_list of the arguments to be passed to the method
 *
 * @throws NoSuchMethodError method was not found
 * @throws NullPointerException if obj is 0 but method is not static
 *
 * @return the return value of the method to be called
 */
jvalue
do_execute_java_method_v(void* obj, const char* method_name, const char* signature, Method* mb, int isStaticCall, va_list argptr)
{
	jvalue retval;
	errorInfo info;

	if (obj == 0 && (! isStaticCall || ! mb)) {
		throwException(NullPointerException);
	}

	if (mb == 0) {
		assert(method_name != 0 && signature != 0);
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

/**
 * Call a Java method from native code.
 *
 * @param obj the obj or 0 if the method is static
 * @param method_name the name of the method to be called
 * @param signature the signature of the method to be called
 * @param mb pointer to the struct _methods of the method to be called
 * @param isStaticCall true if the call is static, otherwise false
 * @param ... the arguments to be passed to the method
 *
 * @return the return value of the method to be called
 */
jvalue
do_execute_java_method(void* obj, const char* method_name, const char* signature, Method* mb, int isStaticCall, ...)
{
	va_list argptr;
	jvalue retval;

	assert(method_name != 0 || mb != 0);

	va_start(argptr, isStaticCall);
	retval = do_execute_java_method_v(obj, method_name, signature, mb, isStaticCall, argptr);
	va_end(argptr);

	return (retval);
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
jvalue
do_execute_java_class_method_v(const char* cname,
	Hjava_lang_ClassLoader* loader, const char* method_name,
	const char* signature, va_list argptr)
{
	Hjava_lang_Class* clazz;
	errorInfo info;
	Method* mb = 0;
	jvalue retval;
	char *buf;

	/* Convert "." to "/" and lookup class */
	buf = checkPtr(KMALLOC(strlen(cname) + 1));
	classname2pathname(cname, buf);
	clazz = lookupClass(buf, loader, &info);
	KFREE(buf);

	/* Get method */
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
jvalue
do_execute_java_class_method(const char* cname, Hjava_lang_ClassLoader*loader,
	const char* method_name, const char* signature, ...)
{
	va_list argptr;
	jvalue retval;

	va_start(argptr, signature);
	retval = do_execute_java_class_method_v(cname, loader, method_name, signature, argptr);
	va_end(argptr);

	return (retval);
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
	callMethodV(mb, METHOD_INDIRECTMETHOD(mb), obj, argptr, &retval);

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

/* This is defined in the alpha port.  It causes all integer arguments
   to be promoted to jlong, and all jfloats to be promoted to jdouble,
   unless explicitly overridden.  It implies NO_HOLES, unless
   explicitly defined otherwise.  */
#if defined(PROMOTE_TO_64bits)
# if !defined(PROMOTE_jint2jlong)
#  define PROMOTE_jint2jlong 1
# endif
# if !defined(PROMOTE_jfloat2jdouble)
#  define PROMOTE_jfloat2jdouble 1
# endif
/* NO_HOLES causes all types to occupy only one slot in callargs, but
   not affecting their callsizes, that can still be used to
   distinguish among types.  */
# if !defined(NO_HOLES)
#  define NO_HOLES 1
# endif
#endif

/* If PROMOTE_jint2jlong is enabled, all integer values are to be
   passed as jlongs.  It is only set by PROMOTE_TO_64bits.  */
#if defined(PROMOTE_jint2jlong)
# define PROM_i j
#else
# define PROM_i i
#endif

/* If PROMOTE_jfloat2jdouble is enabled, jfloats are to be passed as
   jdoubles.  Note that, when a jfloat is promoted, its calltype will
   be marked as 'D'.  No known port uses this.  In fact, alpha must
   explicitly set it to 0, to prevent PROMOTE_TO_64bits from enabling
   it.  */
#if defined(PROMOTE_jfloat2jdouble)
# define PROM_f d
#else
# define PROM_f f
#endif

/* This is currently defined in the MIPS O32 port.  It causes jlongs
   and jdoubles to be forced into even arguments, by introducing a
   padding integer argument. The argument DO is used to adjust the
   input argument list.  */
#if defined(ALIGN_AT_64bits)
# if defined(NO_HOLES)
#  error "ALIGN_AT_64bits is incompatible with NO_HOLES"
# endif
# define ENSURE_ALIGN64(DO) do { \
		if (call.callsize[i] == 2 && (i & 1)) { \
			char tmptype = call.calltype[i]; \
			call.callsize[i] = 0; \
			call.calltype[i] = 'I'; \
			DO; \
			++i; ++s; \
			call.calltype[i] = tmptype; \
			call.callsize[i] = 2; \
		} \
	} while (0)
#else
# define ENSURE_ALIGN64(DO) do {} while (0)
#endif

/* Finally, make sure that undefined symbols are defined as 0 */
#if ! defined(PROMOTE_jfloat2jdouble)
#define PROMOTE_jfloat2jdouble	0
#endif /* ! defined(PROMOTE_jfloat2jdouble) */ 

#if ! defined(NO_HOLES)
#define NO_HOLES	0
#endif /*  ! defined(NO_HOLES) */

/**
 * Generic routine to call a native or Java method (array style).
 *
 * @param meth the struct _methods of the method to be executed
 * @param func the code that's to be executed
 * @param obj  the object whose method is to be called (my be 0 iff method is static)
 * @param args the arguments to be passed to the method
 * @param ret  buffer for the return value of the method (may be 0 iff return type is void)
 * @param promoted true iff 64 bit values occupy two entries in args, otherwise false
 */
void
callMethodA(Method* meth, void* func, void* obj, jvalue* args, jvalue* ret,
	    int promoted)
{
	int i;
	int j;
	int s;
	/* XXX call.callsize and call.calltype arrays are statically sized 
	   and are not checked for running out of bounds */
	callMethodInfo call;	
	jvalue in[MAXMARGS];
	jvalue tmp;

	if (ret == 0) {
		ret = &tmp;
	}
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
		if (!METHOD_TRANSLATED(meth)) {
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
		in[i].l = THREAD_JNIENV(); 
		s += call.callsize[i];
		i++;

		/* If method is static we must insert the class as an argument */
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
	 * an argument.
 	 */
	if ((meth->accflags & ACC_STATIC) == 0) {
		call.callsize[i] = PTR_TYPE_SIZE / SIZEOF_INT;
		s += call.callsize[i];
		call.calltype[i] = 'L';
		in[i].l = obj;
		i++;
	}

	for (j = 0; j < METHOD_NARGS(meth); i++, j++) {
		call.calltype[i] = *METHOD_ARG_TYPE(meth, j);
		switch (call.calltype[i]) {
		case 'Z':
			if (promoted) goto use_int;
			call.callsize[i] = 1;
			in[i].PROM_i = args[j].z;
			break;

		case 'S':
			if (promoted) goto use_int;
			call.callsize[i] = 1;
			in[i].PROM_i = args[j].s;
			break;

		case 'B':
			if (promoted) goto use_int;
			call.callsize[i] = 1;
			in[i].PROM_i = args[j].b;
			break;

		case 'C':
			if (promoted) goto use_int;
			call.callsize[i] = 1;
			in[i].PROM_i = args[j].c;
			break;

		case 'F':
			call.callsize[i] = 1;
			in[i].PROM_f = args[j].f;
			if (PROMOTE_jfloat2jdouble) {
				call.calltype[i] = 'D';
			}
			break;
		case 'I':
		use_int:
			call.callsize[i] = 1;
			in[i].PROM_i = args[j].i;
			break;
		case 'D':
		case 'J':
			call.callsize[i] = 2;
			ENSURE_ALIGN64({});
			in[i] = args[j];
			if (promoted) { /* compensate for the second array element by incrementing args */
			  args++;
			}
			if (! NO_HOLES) {
				s += call.callsize[i];
				in[i+1].i = (&in[i].i)[1];
				i++; 
				call.calltype[i] = 0;
				call.callsize[i] = 0;
			}
			break;
		case '[':
			call.calltype[i] = 'L';
			/* fall through */
		case 'L':
			call.callsize[i] = PTR_TYPE_SIZE / SIZEOF_INT;
			in[i] = args[j];
			break;
		default:
			ABORT();
		}
		s += call.callsize[i];
	}

#if defined(STACK_LIMIT)
	call.calltype[i] = 'L';
	call.callsize[i] = PTR_TYPE_SIZE / SIZEOF_INT;
	in[i].l = jthread_stacklimit();
	s += PTR_TYPE_SIZE / SIZEOF_INT;
	i++;
#endif

	/* Return info */
	call.rettype = *METHOD_RET_TYPE(meth);
	switch (call.rettype) {
	case '[':
		call.rettype = 'L';
		/* fall through */
	case 'L':
		call.retsize = PTR_TYPE_SIZE / SIZEOF_INT;
		break;
	case 'V':
		call.retsize = 0;
		break;
	case 'D':
	case 'J':
		call.retsize = 2;
		break;
	default:
		call.retsize = 1;
		break;
	}

	/* Call info and arguments */
	call.nrargs = i;
	call.argsize = s;
	call.args = in;
	call.ret = ret;


#if defined(TRANSLATOR)
	call.function = func;

	/* GCDIAG wipes free memory with 0xf4... */
	assert(call.function);
	assert(*(uint32*)(call.function) != 0xf4f4f4f4);

	/* Make the call - system dependent */
	sysdepCallMethod(&call);
#endif
#if defined(INTERPRETER)
	if ((meth->accflags & ACC_NATIVE) == 0) {
		virtualMachine(meth, (slots*)call.args, (slots*)call.ret, THREAD_DATA()); 
	}
	else {
		Hjava_lang_Object* syncobj = 0;
		VmExceptHandler mjbuf;
		threadData* thread_data = THREAD_DATA(); 
		struct Hjava_lang_Throwable *save_except = NULL;

		if (meth->accflags & ACC_SYNCHRONISED) {
			if (meth->accflags & ACC_STATIC) {
				syncobj = &meth->class->head;
			}
			else {
				syncobj = (Hjava_lang_Object*)call.args[0].l;
			}
			lockObject(syncobj);
		}

		setupExceptionHandling(&mjbuf, meth, syncobj, thread_data);

		/* This exception has yet been handled by the VM creator.
		 * We are putting it in stand by until it is cleared. For
		 * that JNI call we're cleaning up the pointer and we will
		 * put it again to the value afterward.
		 */
		if ((meth->accflags & ACC_JNI) != 0) {
			if (thread_data->exceptObj != NULL)
				save_except = thread_data->exceptObj;
			else
				save_except = NULL;
			thread_data->exceptObj = NULL;
		}
			
		/* Make the call - system dependent */
		sysdepCallMethod(&call);

		if (syncobj != 0) {
			unlockObject(syncobj);
		}

		/* If we have a pending exception and this is JNI, throw it */
		if ((meth->accflags & ACC_JNI) != 0) {
			struct Hjava_lang_Throwable *eobj;
			
			eobj = thread_data->exceptObj;
			if (eobj != 0) {
				thread_data->exceptObj = 0;
				throwExternalException(eobj);
			}
			thread_data->exceptObj = save_except;
		}

		cleanupExceptionHandling(&mjbuf, thread_data);

	}
#endif
	if (!promoted && call.retsize == 1) {
		switch (call.rettype) {
		case 'Z':
			ret->z = ret->i;
			break;
		case 'S':
			ret->s = ret->i;
			break;
		case 'B':
			ret->b = ret->i;
			break;
		case 'C':
			ret->c = ret->i;
			break;
		}
	}
}

/**
 * Generic routine to call a native or Java method (varargs style).
 *
 * @param meth the struct _methods of the method to be executed
 * @param func the code that's to be executed
 * @param obj  the object whose method is to be called (my be 0 iff method is static)
 * @param args the arguments to be passed to the method
 * @param ret  buffer for the return value of the method (may be 0 iff return type is void)
 */
void
callMethodV(Method* meth, void* func, void* obj, va_list args, jvalue* ret)
{
  /* const char* sig; FIXME */
	int i;
	int s;
	int j;
	/* XXX call.callsize and call.calltype arrays are statically sized 
	   and are not checked for running out of bounds */
	callMethodInfo call;
	jvalue in[MAXMARGS];
	jvalue tmp;

	if (ret == 0) {
		ret = &tmp;
	}
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
		in[i].l = THREAD_JNIENV(); 
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

	for (j = 0; j < METHOD_NARGS(meth); i++, j++) {
		call.calltype[i] = *METHOD_ARG_TYPE(meth, j);
		switch (call.calltype[i]) {
		case 'I':
		case 'Z':
		case 'S':
		case 'B':
		case 'C':
			call.callsize[i] = 1;
			in[i].PROM_i = va_arg(args, jint);
			break;
		case 'F':
			call.callsize[i] = 1;
			in[i].PROM_f = (jfloat)va_arg(args, jdouble);
			if (PROMOTE_jfloat2jdouble) {
				call.calltype[i] = 'D';
			}
			break;
		case 'D':
			call.callsize[i] = 2;
			ENSURE_ALIGN64({});
			in[i].d = va_arg(args, jdouble);
			goto second_word;
		case 'J':
			call.callsize[i] = 2;
			ENSURE_ALIGN64({});
			in[i].j = va_arg(args, jlong);
		second_word:
			if(! NO_HOLES) {
				s += call.callsize[i];
				in[i+1].i = (&in[i].i)[1];
				i++;
				call.callsize[i] = 0;
			}
			break;
		case '[':
			call.calltype[i] = 'L';
			/* fall through */
		case 'L':
			call.callsize[i] = PTR_TYPE_SIZE / SIZEOF_INT;
			in[i].l = va_arg(args, jref);
			break;
		default:
			ABORT();
		}
		s += call.callsize[i];
	}

#if defined(STACK_LIMIT)
	call.calltype[i] = 'L';
	call.callsize[i] = PTR_TYPE_SIZE / SIZEOF_INT;
	in[i].l = jthread_stacklimit();
	s += PTR_TYPE_SIZE / SIZEOF_INT;
	i++;
#endif

	/* Return info */
	call.rettype = *METHOD_RET_TYPE(meth);
	switch (call.rettype) {
	case '[':
		call.rettype = 'L';
		/* fall through */
	case 'L':
		call.retsize = PTR_TYPE_SIZE / SIZEOF_INT;
		break;
	case 'V':
		call.retsize = 0;
		break;
	case 'D':
	case 'J':
		call.retsize = 2;
		break;
	default:
		call.retsize = 1;
		break;
	}

	/* Call info and arguments */
	call.nrargs = i;
	call.argsize = s;
	call.args = in;
	call.ret = ret;

#if defined(TRANSLATOR)
	call.function = func;

	/* GCDIAG wipes free memory with 0xf4... */
	assert(call.function);
	assert(*(uint32*)(call.function) != 0xf4f4f4f4);

	/* Make the call - system dependent */
	sysdepCallMethod(&call);
#endif
#if defined(INTERPRETER)
	if ((meth->accflags & ACC_NATIVE) == 0) {
		virtualMachine(meth, (slots*)call.args, (slots*)call.ret, THREAD_DATA()); 
	}
	else {
		Hjava_lang_Object* syncobj = 0;
		VmExceptHandler mjbuf;
		threadData* thread_data = THREAD_DATA(); 
		struct Hjava_lang_Throwable *save_except = NULL;

		if (meth->accflags & ACC_SYNCHRONISED) {
			if (meth->accflags & ACC_STATIC) {
				syncobj = &meth->class->head;
			}
			else {
				syncobj = (Hjava_lang_Object*)call.args[0].l;
			}
			lockObject(syncobj);
		}

		setupExceptionHandling(&mjbuf, meth, syncobj, thread_data);

		/* This exception has yet been handled by the VM creator.
		 * We are putting it in stand by until it is cleared. For
		 * that JNI call we're cleaning up the pointer and we will
		 * put it again to the value afterward.
		 */
		if ((meth->accflags & ACC_JNI) != 0) {
			if (thread_data->exceptObj != NULL)
				save_except = thread_data->exceptObj;
			else
				save_except = NULL;
			thread_data->exceptObj = NULL;
		}

		/* Make the call - system dependent */
		sysdepCallMethod(&call);

		if (syncobj != 0) {
			unlockObject(syncobj);
		}

		/* If we have a pending exception and this is JNI, throw it */
		if ((meth->accflags & ACC_JNI) != 0) {
			struct Hjava_lang_Throwable *eobj;

			eobj = thread_data->exceptObj;
			if (eobj != 0) {
				thread_data->exceptObj = 0;
				throwExternalException(eobj);
			}
			thread_data->exceptObj = save_except;
		}

		cleanupExceptionHandling(&mjbuf, thread_data);
	}
#endif
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
 * @return struct _methods of the method being searched or 0 in case of an error
 */
Method*
lookupClassMethod(Hjava_lang_Class* cls, const char* name, const char* sig, errorInfo *einfo)
{
	Method *meth;
	Utf8Const *name_utf8, *sig_utf8;

	assert(cls != 0 && name != 0 && sig != 0);

	name_utf8 = utf8ConstNew(name, -1);
	if (!name_utf8) {
		postOutOfMemory(einfo);
		return 0;
	}
	sig_utf8 = utf8ConstNew(sig, -1);
	if (!sig_utf8) {
		utf8ConstRelease(name_utf8);
		postOutOfMemory(einfo);
		return 0;
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
 * @return struct _methods of the method being searched or 0 in case of an error
 */
Method*
lookupObjectMethod(Hjava_lang_Object* obj, const char* name, const char* sig, errorInfo *einfo)
{
	assert(obj != 0 && name != 0 && sig != 0);
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
			0, 0, ERROR_SIGNATURE0);
	} else {
		obj = (Hjava_lang_Throwable*)execute_java_constructor(cname,
			0, 0, ERROR_SIGNATURE, checkPtr(stringC2Java(str)));
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
	gettimeofday(&tm, 0);
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
 * Set a property to a value.
 *
 * @param properties pointer to the properties object whose contents are to be modified
 * @param key the key of the property to be set
 * @param value the value of the property to be set
 */
void
setProperty(void* properties, const char* key, const char* value)
{
	Hjava_lang_String* jkey;
	Hjava_lang_String* jvalue;

	jkey = checkPtr(stringC2Java(key));
	jvalue = checkPtr(stringC2Java(value));

	do_execute_java_method(properties, "put",
		"(Ljava/lang/Object;Ljava/lang/Object;)Ljava/lang/Object;",
		0, false, jkey, jvalue);
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
AllocArray(int len, int type)
{
	if (len < 0) {
		throwException(NegativeArraySizeException);
	}
	return (newArray(TYPE_CLASS(type), (size_t)len));
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
	vfprintf(out, mess, argptr);
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
	jthread_spinon(0);
}

void 
leaveUnsafeRegion(void)
{
	jthread_spinoff(0);
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
