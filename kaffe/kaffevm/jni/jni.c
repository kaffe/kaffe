/*
 * jni.c
 * Java Native Interface.
 *
 * Copyright (c) 1996, 1997, 2004
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#include "config.h"
#include "config-std.h"
#include "config-mem.h"
#include "jni.h"
#include "jnirefs.h"
#include "classMethod.h"
#include "soft.h"
#include "support.h"
#include "itypes.h"
#include "object.h"
#include "errors.h"
#include "native.h"
#include "file.h"
#include "baseClasses.h"
#include "stringSupport.h"
#include "readClass.h"
#include "access.h"
#include "lookup.h"
#include "thread.h"
#include "external.h"
#include "gc.h"
#include "locks.h"
#include "md.h"
#include "exception.h"
#if defined(TRANSLATOR)
#include "constpool.h"
#include "seq.h"
#include "slots.h"
#include "registers.h"
#include "labels.h"
#include "codeproto.h"
#include "basecode.h"
#include "icode.h"
#include "machine.h"
#if defined(KAFFE_FEEDBACK)
#include "feedback.h"
#endif
#endif
#include "jvmpi_kaffe.h"
#include "jni_i.h"
#include "jni_funcs.h"

/*
 * Get and set fields.
 */
#define	GET_FIELD(T,O,F)	*(T*)((char*)(O) + FIELD_BOFFSET((Field*)(F)))
#define	SET_FIELD(T,O,F,V)	*(T*)((char*)(O) + FIELD_BOFFSET((Field*)(F))) = (V)
#define	GET_STATIC_FIELD(T,F)	*(T*)FIELD_ADDRESS((Field*)F)
#define	SET_STATIC_FIELD(T,F,V)	*(T*)FIELD_ADDRESS((Field*)F) = (V)

extern int Kaffe_Java_Major_Version;
extern int Kaffe_Java_Minor_Version;
extern struct JNINativeInterface Kaffe_JNINativeInterface;
extern JavaVMInitArgs Kaffe_JavaVMInitArgs;
extern JavaVM Kaffe_JavaVM;

static void Kaffe_JNI_wrapper(Method*, void*);
#if defined(TRANSLATOR)
static void *startJNIcall(void);
static void finishJNIcall(void);
static void Kaffe_wrapper(Method* xmeth, void* func, bool use_JNI);
#endif

static jint Kaffe_GetVersion(JNIEnv*);
static jclass Kaffe_FindClass(JNIEnv*, const char*);
static jint Kaffe_ThrowNew(JNIEnv*, jclass, const char*);
static jint Kaffe_Throw(JNIEnv* env, jobject obj);

/*
 * Everything from this point to Kaffe_GetVersion is not
 * exception-aware.  Asynchronous exceptions should not be delivered
 * to them.
 *
 * Everything from Kaffe_GetVersion to Kaffe_GetJavaVM
 * should be bracketed with BEGIN and END _EXCEPTION_HANDLING.
 */
void NONRETURNING
Kaffe_FatalError(JNIEnv* env UNUSED, const char* mess)
{
	kprintf(stderr, "FATAL ERROR: %s\n", mess);
	exit(1);
}

static void
Kaffe_DeleteGlobalRef(JNIEnv* env UNUSED, jref obj)
{
#if defined(ENABLE_JVMPI)
	if( JVMPI_EVENT_ISENABLED(JVMPI_EVENT_JNI_GLOBALREF_FREE) )
	{
		JVMPI_Event ev;

		ev.event_type = JVMPI_EVENT_JNI_GLOBALREF_FREE;
		ev.u.jni_globalref_free.ref_id = obj;
		jvmpiPostEvent(&ev);
	}
#endif
	gc_rm_ref(obj);
}

static void
/* ARGSUSED */
Kaffe_DeleteLocalRef(JNIEnv* env UNUSED, jref obj)
{
	REMOVE_REF(obj);
}

static jboolean
Kaffe_IsSameObject(JNIEnv* env UNUSED, jobject obj1, jobject obj2)
{
	if (obj1 == obj2) {
		return (JNI_TRUE);
	}
	else {
		return (JNI_FALSE);
	}
}

static void
Kaffe_ReleaseStringChars(JNIEnv* env UNUSED, jstring data UNUSED, const jchar* chars UNUSED)
{
	/* Does nothing */
}

static jint
Kaffe_GetVersion(JNIEnv* env UNUSED)
{
	return ((Kaffe_Java_Major_Version << 16) | Kaffe_Java_Minor_Version);
}

/*
 * take a VM error and throw it as JNI error 
 */
static void
postError(JNIEnv* env, errorInfo* info)
{
	Kaffe_Throw(env, error2Throwable(info));
}

static jref
Kaffe_NewGlobalRef(JNIEnv* env, jref obj)
{
	BEGIN_EXCEPTION_HANDLING(0);
	if (!gc_add_ref(obj)) {
		errorInfo info;
		postOutOfMemory(&info);
		postError(env, &info);
	}
#if defined(ENABLE_JVMPI)
	if( JVMPI_EVENT_ISENABLED(JVMPI_EVENT_JNI_GLOBALREF_ALLOC) )
	{
		JVMPI_Event ev;

		ev.event_type = JVMPI_EVENT_JNI_GLOBALREF_ALLOC;
		ev.u.jni_globalref_alloc.obj_id = obj;
		ev.u.jni_globalref_alloc.ref_id = obj;
		jvmpiPostEvent(&ev);
	}
#endif
	END_EXCEPTION_HANDLING();
	return obj;
}

static jclass
Kaffe_DefineClass(JNIEnv* env, jobject loader, const jbyte* buf, jsize len)
{
	Hjava_lang_Class* cls;
	classFile hand;
	errorInfo info;

	BEGIN_EXCEPTION_HANDLING(0);

	classFileInit(&hand, buf, len, CP_BYTEARRAY);

	cls = newClass();
	if (cls == 0) {
		postOutOfMemory(&info);
	} else {
		cls = readClass(cls, &hand, loader, &info);
	}
	if (cls == 0) {
		postError(env, &info);
	}

	END_EXCEPTION_HANDLING();
	return (cls);
}

/*
 * For this routine, we defer most of the work to Class.forName(),
 * which handles the task of figuring out the right ClassLoader to
 * use based on the calling method, which requires examining the
 * stack backtrace.
 */

static jclass
tryClassForName(jstring nameString)
{
	jvalue retval;

	BEGIN_EXCEPTION_HANDLING(0);

	/* Call Class.forName() */
	retval = do_execute_java_class_method("java.lang.Class", NULL,
	    "forName", "(Ljava/lang/String;)Ljava/lang/Class;", nameString);

	END_EXCEPTION_HANDLING();

	return retval.l;
}

static jclass
Kaffe_FindClass(JNIEnv* env, const char* name)
{
	jstring nameString;
	Utf8Const* utf8;
	jobject retval;
	jobject exc;

	BEGIN_EXCEPTION_HANDLING(0);

	/* We accepts slashes, but Class.forName() does not */
	utf8 = checkPtr(utf8ConstNew(name, -1));
	nameString = utf8Const2JavaReplace(utf8, '/', '.');
	utf8ConstRelease(utf8);
	checkPtr(nameString);

	retval = tryClassForName(nameString);

	exc = thread_data->exceptObj;
	if (exc != NULL)
	{
		if (soft_instanceof(javaLangClassNotFoundException, exc))
		{
			int iLockRoot;
			static iStaticLock appLock = KAFFE_STATIC_LOCK_INITIALIZER;

			thread_data->exceptObj = NULL;
			if (appClassLoader == NULL)
			{
				lockStaticMutex(&appLock);
				if (appClassLoader == NULL)
					appClassLoader = do_execute_java_method(kaffeLangAppClassLoaderClass, "getSingleton", "()Ljava/lang/ClassLoader;", NULL, true).l;
				unlockStaticMutex(&appLock);

				if (thread_data->exceptObj != NULL)
				{
					fprintf(stderr,
						"ERROR: The default user class loader "
					       	APPCLASSLOADERCLASS " can not be loaded.\n"
						"Aborting...\n");
					ABORT();
				}
			}

			
			retval = do_execute_java_class_method("java.lang.Class", NULL,
			    "forName", "(Ljava/lang/String;ZLjava/lang/ClassLoader;)Ljava/lang/Class;", nameString, true, appClassLoader).l;
			ADD_REF(retval);
		}
	} else {
		ADD_REF(retval);
	}

	END_EXCEPTION_HANDLING();
	return (retval);
}

static jclass
Kaffe_GetSuperClass(JNIEnv* env UNUSED, jclass cls)
{
	jclass clz;

	BEGIN_EXCEPTION_HANDLING(0);

	clz = ((Hjava_lang_Class*)cls)->superclass;

	END_EXCEPTION_HANDLING();
	return (clz);
}

static jbool
Kaffe_IsAssignableFrom(JNIEnv* env UNUSED, jclass cls1, jclass cls2)
{
	jbool r;

	BEGIN_EXCEPTION_HANDLING(0);

	if (instanceof(cls2, cls1) != 0) {
		r = JNI_TRUE;
	}
	else {
		r = JNI_FALSE;
	}
	END_EXCEPTION_HANDLING();
	return (r);
}

static jint
Kaffe_Throw(JNIEnv* env UNUSED, jobject obj)
{
	BEGIN_EXCEPTION_HANDLING(0);

	if( obj )
	{
		assert(((Hjava_lang_Object *)obj)->vtable);
		
		thread_data->exceptObj = (struct Hjava_lang_Throwable*)obj;
	}

	END_EXCEPTION_HANDLING();
	return (0);
}

static jint
Kaffe_ThrowNew(JNIEnv* env UNUSED, jclass cls, const char* mess)
{
	Hjava_lang_Object* eobj;

	BEGIN_EXCEPTION_HANDLING(0);

	eobj = execute_java_constructor(NULL, NULL, cls,
					"(Ljava/lang/String;)V",
					checkPtr(stringC2Java((char*)mess)));

	thread_data->exceptObj = (struct Hjava_lang_Throwable*)eobj;

	END_EXCEPTION_HANDLING();
	return (0);
}

static jobject
Kaffe_ExceptionOccurred(JNIEnv* env UNUSED)
{
	jobject obj;

	BEGIN_EXCEPTION_HANDLING(0);

	obj = thread_data->exceptObj;

	ADD_REF(obj);
	END_EXCEPTION_HANDLING();
	return (obj);
}

static jboolean
Kaffe_ExceptionCheck(JNIEnv* env UNUSED)
{
	jboolean result;
	jobject obj;

	BEGIN_EXCEPTION_HANDLING(0);

	obj = thread_data->exceptObj;
	result = (obj == NULL) ? JNI_FALSE : JNI_TRUE;

	END_EXCEPTION_HANDLING();
	return (result);
}

static void
Kaffe_ExceptionDescribe(JNIEnv* env UNUSED)
{
	BEGIN_EXCEPTION_HANDLING_VOID();

	if (thread_data->exceptObj != 0) {
		do_execute_java_method(thread_data->exceptObj, "printStackTrace", "()V",
				       0, 0, thread_data->exceptObj); 
	}
	END_EXCEPTION_HANDLING();
}

static void
Kaffe_ExceptionClear(JNIEnv* env UNUSED)
{
	BEGIN_EXCEPTION_HANDLING_VOID();

	thread_data->exceptObj = 0;

	END_EXCEPTION_HANDLING();
}

static jobject
Kaffe_AllocObject(JNIEnv* env UNUSED, jclass cls)
{
	jobject obj;
	Hjava_lang_Class* clazz;

	BEGIN_EXCEPTION_HANDLING(0);

	clazz = (Hjava_lang_Class*)cls;

	if (CLASS_IS_INTERFACE(clazz) || CLASS_IS_ABSTRACT(clazz)) {
		throwException(InstantiationException(clazz->name->data));
	}
	obj = newObject(clazz);

	ADD_REF(obj);
	END_EXCEPTION_HANDLING();
	return (obj);
}

static jobject
Kaffe_NewObjectV(JNIEnv* env UNUSED, jclass cls, jmethodID meth, va_list args)
{
	Hjava_lang_Object* obj;
	Hjava_lang_Class* clazz;
	jvalue retval;
	Method* m = (Method*)meth;

	BEGIN_EXCEPTION_HANDLING(0);

	clazz = (Hjava_lang_Class*)cls;

	if (CLASS_IS_INTERFACE(clazz) || CLASS_IS_ABSTRACT(clazz) || !METHOD_IS_CONSTRUCTOR(m)) {
		throwException(InstantiationException(clazz->name->data));
	}
	obj = newObject(clazz);

	callMethodV(m, METHOD_INDIRECTMETHOD(m), obj, args, &retval);

	ADD_REF(obj);
	END_EXCEPTION_HANDLING();
	return (obj);
}

static jobject
Kaffe_NewObject(JNIEnv* env UNUSED, jclass cls, jmethodID meth, ...)
{
	jobject obj;
	va_list args;

	BEGIN_EXCEPTION_HANDLING(0);

	va_start(args, meth);
	obj = Kaffe_NewObjectV(env, cls, meth, args);
	va_end(args);

	END_EXCEPTION_HANDLING();
	return (obj);
}

static jobject
Kaffe_NewObjectA(JNIEnv* env UNUSED, jclass cls, jmethodID meth, jvalue* args)
{
	Hjava_lang_Object* obj;
	Hjava_lang_Class* clazz;
	jvalue retval;
	Method* m = (Method*)meth;

	BEGIN_EXCEPTION_HANDLING(0);

	clazz = (Hjava_lang_Class*)cls;

	if (CLASS_IS_INTERFACE(clazz) || CLASS_IS_ABSTRACT(clazz) || !METHOD_IS_CONSTRUCTOR(m)) {
		throwException(InstantiationException(clazz->name->data));
	}
	obj = newObject(clazz);

	callMethodA(m, METHOD_INDIRECTMETHOD(m), obj, args, &retval, 0);

	ADD_REF(obj);
	END_EXCEPTION_HANDLING();
	return (obj);
}

static jclass
Kaffe_GetObjectClass(JNIEnv* env UNUSED, jobject obj)
{
	jclass cls;

	BEGIN_EXCEPTION_HANDLING(0);

	cls = ((Hjava_lang_Object*)obj)->vtable->class;

	END_EXCEPTION_HANDLING();
	return (cls);
}

static jbool
Kaffe_IsInstanceOf(JNIEnv* env UNUSED, jobject obj, jclass cls)
{
	jbool r;

	BEGIN_EXCEPTION_HANDLING(0);

	if (soft_instanceof((Hjava_lang_Class*)cls, (Hjava_lang_Object*)obj) != 0) {
		r = JNI_TRUE;
	}
	else {
		r = JNI_FALSE;
	}

	END_EXCEPTION_HANDLING();
	return (r);
}

static jmethodID
Kaffe_GetMethodID(JNIEnv* env, jclass cls, const char* name, const char* sig)
{
	Method* meth;
	errorInfo info;

	BEGIN_EXCEPTION_HANDLING(0);
	meth = lookupClassMethod((Hjava_lang_Class*)cls, (char*)name, (char*)sig, &info);
	if (meth == 0) {
		postError(env, &info);
	} 
	else if (METHOD_IS_STATIC(meth)) {
		postExceptionMessage(&info, JAVA_LANG(NoSuchMethodError), "%s", name);
		postError(env, &info);
		meth = 0;
	}
	END_EXCEPTION_HANDLING();
	
	return (meth);
}


static jfieldID
Kaffe_GetFieldID(JNIEnv* env, jclass cls, const char* name, const char* sig UNUSED)
{
	Field* fld;
	errorInfo info;
	Utf8Const* utf8;

	BEGIN_EXCEPTION_HANDLING(0);
	utf8 = checkPtr(utf8ConstNew(name, -1));
	fld = lookupClassField((Hjava_lang_Class*)cls, utf8, false, &info);
	utf8ConstRelease(utf8);
	if (fld == NULL) {
		postError(env, &info);
	}
	END_EXCEPTION_HANDLING();
	return (fld);
}

static jobject
Kaffe_GetObjectField(JNIEnv* env UNUSED, jobject obj, jfieldID fld)
{
	jobject nobj;

	BEGIN_EXCEPTION_HANDLING(0);

	nobj = GET_FIELD(jobject, obj, fld);

	ADD_REF(nobj);
	END_EXCEPTION_HANDLING();
	return (nobj);
}

static jboolean
Kaffe_GetBooleanField(JNIEnv* env UNUSED, jobject obj, jfieldID fld)
{
	jboolean r;

	BEGIN_EXCEPTION_HANDLING(0);

	r = GET_FIELD(jboolean, obj, fld);

	END_EXCEPTION_HANDLING();
	return (r);
}

static jbyte
Kaffe_GetByteField(JNIEnv* env UNUSED, jobject obj, jfieldID fld)
{
	jbyte r;
	BEGIN_EXCEPTION_HANDLING(0);

	r = GET_FIELD(jbyte, obj, fld);

	END_EXCEPTION_HANDLING();
	return (r);
}

static jchar
Kaffe_GetCharField(JNIEnv* env UNUSED, jobject obj, jfieldID fld)
{
	jchar r;
	BEGIN_EXCEPTION_HANDLING(0);

	r = GET_FIELD(jchar, obj, fld);

	END_EXCEPTION_HANDLING();
	return (r);
}

static jshort
Kaffe_GetShortField(JNIEnv* env UNUSED, jobject obj, jfieldID fld)
{
	jshort r;
	BEGIN_EXCEPTION_HANDLING(0);

	r = GET_FIELD(jshort, obj, fld);

	END_EXCEPTION_HANDLING();
	return (r);
}

static jint
Kaffe_GetIntField(JNIEnv* env UNUSED, jobject obj, jfieldID fld)
{
	jint r;
	BEGIN_EXCEPTION_HANDLING(0);

	r = GET_FIELD(jint, obj, fld);

	END_EXCEPTION_HANDLING();
	return (r);
}

static jlong
Kaffe_GetLongField(JNIEnv* env UNUSED, jobject obj, jfieldID fld)
{
	jlong r;
	BEGIN_EXCEPTION_HANDLING(0);

	r = GET_FIELD(jlong, obj, fld);

	END_EXCEPTION_HANDLING();
	return (r);
}

static jfloat
Kaffe_GetFloatField(JNIEnv* env UNUSED, jobject obj, jfieldID fld)
{
	jfloat r;
	BEGIN_EXCEPTION_HANDLING(0);

	r = GET_FIELD(jfloat, obj, fld);

	END_EXCEPTION_HANDLING();
	return (r);
}

static jdouble
Kaffe_GetDoubleField(JNIEnv* env UNUSED, jobject obj, jfieldID fld)
{
	jdouble r;
	BEGIN_EXCEPTION_HANDLING(0);

	r = GET_FIELD(jdouble, obj, fld);

	END_EXCEPTION_HANDLING();
	return (r);
}

static void
Kaffe_SetObjectField(JNIEnv* env UNUSED, jobject obj, jfieldID fld, jobject val)
{
	BEGIN_EXCEPTION_HANDLING_VOID();

	SET_FIELD(jobject, obj, fld, val);

	END_EXCEPTION_HANDLING();
}

static void
Kaffe_SetBooleanField(JNIEnv* env UNUSED, jobject obj, jfieldID fld, jbool val)
{
	BEGIN_EXCEPTION_HANDLING_VOID();

	SET_FIELD(jboolean, obj, fld, val);

	END_EXCEPTION_HANDLING();
}

static void
Kaffe_SetByteField(JNIEnv* env UNUSED, jobject obj, jfieldID fld, jbyte val)
{
	BEGIN_EXCEPTION_HANDLING_VOID();

	SET_FIELD(jbyte, obj, fld, val);

	END_EXCEPTION_HANDLING();
}

static void
Kaffe_SetCharField(JNIEnv* env UNUSED, jobject obj, jfieldID fld, jchar val)
{
	BEGIN_EXCEPTION_HANDLING_VOID();

	SET_FIELD(jchar, obj, fld, val);

	END_EXCEPTION_HANDLING();
}

static void
Kaffe_SetShortField(JNIEnv* env UNUSED, jobject obj, jfieldID fld, jshort val)
{
	BEGIN_EXCEPTION_HANDLING_VOID();

	SET_FIELD(jshort, obj, fld, val);

	END_EXCEPTION_HANDLING();
}

static void
Kaffe_SetIntField(JNIEnv* env UNUSED, jobject obj, jfieldID fld, jint val)
{
	BEGIN_EXCEPTION_HANDLING_VOID();

	SET_FIELD(jint, obj, fld, val);

	END_EXCEPTION_HANDLING();
}

static void
Kaffe_SetLongField(JNIEnv* env UNUSED, jobject obj, jfieldID fld, jlong val)
{
	BEGIN_EXCEPTION_HANDLING_VOID();

	SET_FIELD(jlong, obj, fld, val);

	END_EXCEPTION_HANDLING();
}

static void
Kaffe_SetFloatField(JNIEnv* env UNUSED, jobject obj, jfieldID fld, jfloat val)
{
	BEGIN_EXCEPTION_HANDLING_VOID();

	SET_FIELD(jfloat, obj, fld, val);

	END_EXCEPTION_HANDLING();
}

static void
Kaffe_SetDoubleField(JNIEnv* env UNUSED, jobject obj, jfieldID fld, jdouble val)
{
	BEGIN_EXCEPTION_HANDLING_VOID();

	SET_FIELD(jdouble, obj, fld, val);

	END_EXCEPTION_HANDLING();
}

static jmethodID
Kaffe_GetStaticMethodID(JNIEnv* env, jclass cls, const char* name, const char* sig)
{
	Method* meth;
	errorInfo info;

	BEGIN_EXCEPTION_HANDLING(0);
	meth = lookupClassMethod((Hjava_lang_Class*)cls, (char*)name, (char*)sig, &info);
	if (meth == 0) {
		postError(env, &info);
	} else if (!METHOD_IS_STATIC(meth)) {
		postExceptionMessage(&info, JAVA_LANG(NoSuchMethodError), "%s", name);
		postError(env, &info);
		meth = 0;
	}
	END_EXCEPTION_HANDLING();

	return (meth);
}




static jfieldID
Kaffe_GetStaticFieldID(JNIEnv* env, jclass cls, const char* name, const char* sig UNUSED)
{
	Field* fld;
	errorInfo info;
	Utf8Const* utf8;

	BEGIN_EXCEPTION_HANDLING(0);
	utf8 = checkPtr(utf8ConstNew(name, -1));
	fld = lookupClassField((Hjava_lang_Class*)cls, utf8, true, &info);
	utf8ConstRelease(utf8);
	if (fld == NULL) {
		postError(env, &info);
	}
	END_EXCEPTION_HANDLING();

	return (fld);
}

static jobject
Kaffe_GetStaticObjectField(JNIEnv* env UNUSED, jclass cls UNUSED, jfieldID fld)
{
	jobject obj;

	BEGIN_EXCEPTION_HANDLING(0);

	obj = GET_STATIC_FIELD(jobject, fld);

	ADD_REF(obj);
	END_EXCEPTION_HANDLING();
	return (obj);
}

static jboolean
Kaffe_GetStaticBooleanField(JNIEnv* env UNUSED, jclass cls UNUSED, jfieldID fld)
{
	jboolean r;
	BEGIN_EXCEPTION_HANDLING(0);

	r = GET_STATIC_FIELD(jboolean, fld);

	END_EXCEPTION_HANDLING();
	return (r);
}

static jbyte
Kaffe_GetStaticByteField(JNIEnv* env UNUSED, jclass cls UNUSED, jfieldID fld)
{
	jbyte r;
	BEGIN_EXCEPTION_HANDLING(0);

	r = GET_STATIC_FIELD(jbyte, fld);

	END_EXCEPTION_HANDLING();
	return (r);
}

static jchar
Kaffe_GetStaticCharField(JNIEnv* env UNUSED, jclass cls UNUSED, jfieldID fld)
{
	jchar r;
	BEGIN_EXCEPTION_HANDLING(0);

	r = GET_STATIC_FIELD(jchar, fld);

	END_EXCEPTION_HANDLING();
	return (r);
}

static jshort
Kaffe_GetStaticShortField(JNIEnv* env UNUSED, jclass cls UNUSED, jfieldID fld)
{
	jint r;
	BEGIN_EXCEPTION_HANDLING(0);

	r = GET_STATIC_FIELD(jshort, fld);

	END_EXCEPTION_HANDLING();
	return (r);
}

static jint
Kaffe_GetStaticIntField(JNIEnv* env UNUSED, jclass cls UNUSED, jfieldID fld)
{
	jint r;
	BEGIN_EXCEPTION_HANDLING(0);

	r = GET_STATIC_FIELD(jint, fld);

	END_EXCEPTION_HANDLING();
	return (r);
}

static jlong
Kaffe_GetStaticLongField(JNIEnv* env UNUSED, jclass cls UNUSED, jfieldID fld)
{
	jlong r;
	BEGIN_EXCEPTION_HANDLING(0);

	r = GET_STATIC_FIELD(jlong, fld);

	END_EXCEPTION_HANDLING();
	return (r);
}

static jfloat
Kaffe_GetStaticFloatField(JNIEnv* env UNUSED, jclass cls UNUSED, jfieldID fld)
{
	jfloat r;
	BEGIN_EXCEPTION_HANDLING(0);

	r = GET_STATIC_FIELD(jfloat, fld);

	END_EXCEPTION_HANDLING();
	return (r);
}

static jdouble
Kaffe_GetStaticDoubleField(JNIEnv* env UNUSED, jclass cls UNUSED, jfieldID fld)
{
	jdouble r;
	BEGIN_EXCEPTION_HANDLING(0);

	r = GET_STATIC_FIELD(jdouble, fld);

	END_EXCEPTION_HANDLING();
	return (r);
}

static void
Kaffe_SetStaticObjectField(JNIEnv* env UNUSED, jclass cls UNUSED, jfieldID fld, jobject val)
{
	BEGIN_EXCEPTION_HANDLING_VOID();

	SET_STATIC_FIELD(jobject, fld, val);

	END_EXCEPTION_HANDLING();
}

static void
Kaffe_SetStaticBooleanField(JNIEnv* env UNUSED, jclass cls UNUSED, jfieldID fld, jbool val)
{
	BEGIN_EXCEPTION_HANDLING_VOID();

	SET_STATIC_FIELD(jboolean, fld, val);

	END_EXCEPTION_HANDLING();
}

static void
Kaffe_SetStaticByteField(JNIEnv* env UNUSED, jclass cls UNUSED, jfieldID fld, jbyte val)
{
	BEGIN_EXCEPTION_HANDLING_VOID();

	SET_STATIC_FIELD(jbyte, fld, val);

	END_EXCEPTION_HANDLING();
}

static void
Kaffe_SetStaticCharField(JNIEnv* env UNUSED, jclass cls UNUSED, jfieldID fld, jchar val)
{
	BEGIN_EXCEPTION_HANDLING_VOID();

	SET_STATIC_FIELD(jchar, fld, val);

	END_EXCEPTION_HANDLING();
}

static void
Kaffe_SetStaticShortField(JNIEnv* env UNUSED, jclass cls UNUSED, jfieldID fld, jshort val)
{
	BEGIN_EXCEPTION_HANDLING_VOID();

	SET_STATIC_FIELD(jshort, fld, val);

	END_EXCEPTION_HANDLING();
}

static void
Kaffe_SetStaticIntField(JNIEnv* env UNUSED, jclass cls UNUSED, jfieldID fld, jint val)
{
	BEGIN_EXCEPTION_HANDLING_VOID();

	SET_STATIC_FIELD(jint, fld, val);

	END_EXCEPTION_HANDLING();
}

static void
Kaffe_SetStaticLongField(JNIEnv* env UNUSED, jclass cls UNUSED, jfieldID fld, jlong val)
{
	BEGIN_EXCEPTION_HANDLING_VOID();

	SET_STATIC_FIELD(jlong, fld, val);

	END_EXCEPTION_HANDLING();
}

static void
Kaffe_SetStaticFloatField(JNIEnv* env UNUSED, jclass cls UNUSED, jfieldID fld, jfloat val)
{
	BEGIN_EXCEPTION_HANDLING_VOID();

	SET_STATIC_FIELD(jfloat, fld, val);

	END_EXCEPTION_HANDLING();
}

static void
Kaffe_SetStaticDoubleField(JNIEnv* env UNUSED, jclass cls UNUSED, jfieldID fld, jdouble val)
{
	BEGIN_EXCEPTION_HANDLING_VOID();

	SET_STATIC_FIELD(jdouble, fld, val);

	END_EXCEPTION_HANDLING();
}

static jstring
Kaffe_NewString(JNIEnv* env UNUSED, const jchar* data, jsize len)
{
	Hjava_lang_String* str;

	BEGIN_EXCEPTION_HANDLING(0);

	str = (Hjava_lang_String*)newObject(StringClass);
	unhand(str)->offset = 0;
	unhand(str)->count = len;
	unhand(str)->value = (HArrayOfChar*)newArray(TYPE_CLASS(TYPE_Char), len);                   
	unhand(str)->interned = 0;
	memcpy(STRING_DATA(str), data, len * sizeof(jchar));

	END_EXCEPTION_HANDLING();
	return (str);
}

static jsize
Kaffe_GetStringLength(JNIEnv* env UNUSED, jstring data)
{
	jsize len;
	BEGIN_EXCEPTION_HANDLING(0);

	len = STRING_SIZE((Hjava_lang_String*)data);
	END_EXCEPTION_HANDLING();
	return (len);
}

static const jchar*
Kaffe_GetStringChars(JNIEnv* env UNUSED, jstring data, jboolean* copy)
{
	jchar* c;
	BEGIN_EXCEPTION_HANDLING(0);

	if (copy != NULL) {
		*copy = JNI_FALSE;
	}
	c = STRING_DATA(((Hjava_lang_String*)data));

	END_EXCEPTION_HANDLING();
	return (c);
}

static jstring
Kaffe_NewStringUTF(JNIEnv* env UNUSED, const char* data)
{
	Hjava_lang_String* str;
	Utf8Const* utf8;
	unsigned int len;

	BEGIN_EXCEPTION_HANDLING(0);

	len = strlen(data);
	if (!utf8ConstIsValidUtf8(data, len)) {
		str = NULL;
	} else {
		utf8 = checkPtr(utf8ConstNew(data, len));
		str = utf8Const2Java(utf8);
		utf8ConstRelease(utf8);
		if (!str) {
			errorInfo info;
			postOutOfMemory(&info);
			throwError(&info);
		}
	}

	END_EXCEPTION_HANDLING();
	return (str);
}

static jsize
Kaffe_GetStringUTFLength(JNIEnv* env UNUSED, jstring data)
{
	Hjava_lang_String* const str = (Hjava_lang_String*)data;
	jchar* ptr;
	jsize len;
	jsize count;
	jsize i;

	BEGIN_EXCEPTION_HANDLING(0);

	ptr = STRING_DATA(str);
	len = STRING_SIZE(str);

	count = 0;
	for (i = 0; i < len; i++) {
		if (ptr[i] >= 0x0001 && ptr[i] <= 0x007F) {
			count += 1;
		}
		else if (ptr[i] >= 0x0080 && ptr[i] <= 0x07FF) {
			count += 2;
		}
		else {
			count += 3;
		}
	}

	END_EXCEPTION_HANDLING();
	return (count);
}

static const jbyte*
Kaffe_GetStringUTFChars(JNIEnv* env, jstring data, jbool* copy)
{
	Hjava_lang_String* const str = (Hjava_lang_String*)data;
	jchar* ptr;
	jbyte* buf;
	jsize len;
	jsize i;
	jsize j;

	BEGIN_EXCEPTION_HANDLING(0);

	/* We always copy data */
	if (copy != NULL) {
		*copy = JNI_TRUE;
	}

	buf = checkPtr(KMALLOC(Kaffe_GetStringUTFLength(env, data) + 1));

	ptr = STRING_DATA(str);
	len = STRING_SIZE(str);

	for (j = 0, i = 0; i < len; i++) {
		if (ptr[i] >= 0x0001 && ptr[i] <= 0x007F) {
			buf[j++] = ptr[i] & 0x7F;
		}
		else if (ptr[i] >= 0x0080 && ptr[i] <= 0x07FF) {
			buf[j++] = 0xC0 | ((ptr[i] >> 6) & 0x1F);
			buf[j++] = 0x80 | (ptr[i] & 0x3F);
		}
		else {
			buf[j++] = 0xE0 | ((ptr[i] >> 12) & 0x0F);
			buf[j++] = 0x80 | ((ptr[i] >> 6) & 0x3F);
			buf[j++] = 0x80 | (ptr[i] & 0x3F);
		}
	}

	END_EXCEPTION_HANDLING();
	return (buf);
}

static void
Kaffe_ReleaseStringUTFChars(JNIEnv* env UNUSED, jstring data UNUSED, const jbyte* chars)
{
	BEGIN_EXCEPTION_HANDLING_VOID();

	KFREE(chars);
	
	END_EXCEPTION_HANDLING();
}

static void
Kaffe_GetStringRegion(JNIEnv *env, jstring data, jsize start, jsize len, jchar *buf)
{
	Hjava_lang_String* const str = (Hjava_lang_String*)data;
        jchar *str_ptr;
	jsize str_len;
	
        BEGIN_EXCEPTION_HANDLING_VOID();

	str_ptr = STRING_DATA(str);
	str_len = STRING_SIZE(str);

	if (start >= len || start+len >= str_len) {
	      errorInfo einfo;
	      
	      postException(&einfo, "java.lang.StringIndexOutOfBoundsException");
	      throwError(&einfo);
	}
	
	memcpy(buf, &str_ptr[start], len*sizeof(jchar));

        END_EXCEPTION_HANDLING();
}

static void
Kaffe_GetStringUTFRegion(JNIEnv *env, jstring data, jsize start, jsize len, char *buf)
{
        Hjava_lang_String* const str = (Hjava_lang_String*)data;
	jchar *str_ptr;
	jsize str_len;

	BEGIN_EXCEPTION_HANDLING_VOID();
	
	str_ptr = STRING_DATA(str);
	str_len = STRING_SIZE(str); 
	if (start >= len || start+len >= str_len) {
	      errorInfo einfo;
	      
	      postException(&einfo, "java.lang.StringIndexOutOfBoundsException");
	      throwError(&einfo);
	}

	utf8ConstEncodeTo(&str_ptr[start], len, buf);

	END_EXCEPTION_HANDLING();
}

static jsize
Kaffe_GetArrayLength(JNIEnv* env UNUSED, jarray arr)
{
	jsize len;
	BEGIN_EXCEPTION_HANDLING(0);

	len = obj_length((HArrayOfObject*)arr);

	END_EXCEPTION_HANDLING();
	return (len);
}

static jobjectArray
Kaffe_NewObjectArray(JNIEnv* env UNUSED, jsize len, jclass cls, jobject init)
{
	HArrayOfObject* obj;
	int i;

	BEGIN_EXCEPTION_HANDLING(0);

	obj = (HArrayOfObject*)newArray((Hjava_lang_Class*)cls, len);

	/* I assume this is what init is for */
	for (i = 0; i < len; i++) {
		unhand_array(obj)->body[i] = (Hjava_lang_Object*)init;
	}

	ADD_REF(obj);
	END_EXCEPTION_HANDLING();
	return (obj);
}

static jobject
Kaffe_GetObjectArrayElement(JNIEnv* env UNUSED, jobjectArray arr, jsize elem)
{
	jobject obj;

	BEGIN_EXCEPTION_HANDLING(0);

	if (elem >= obj_length((HArrayOfObject*)arr)) {
		throwException(ArrayIndexOutOfBoundsException);
	}
	obj = unhand_array((HArrayOfObject*)arr)->body[elem];

	ADD_REF(obj);
	END_EXCEPTION_HANDLING();
	return (obj);
}

static void
Kaffe_SetObjectArrayElement(JNIEnv* env UNUSED, jobjectArray arr, jsize elem, jobject val)
{
	BEGIN_EXCEPTION_HANDLING_VOID();

	if (elem >= obj_length((HArrayOfObject*)arr)) {
		throwException(ArrayIndexOutOfBoundsException);
	}
	unhand_array((HArrayOfObject*)arr)->body[elem] = (Hjava_lang_Object*)val;

	END_EXCEPTION_HANDLING();
}

static jbooleanArray
Kaffe_NewBooleanArray(JNIEnv* env UNUSED, jsize len)
{
	jbooleanArray arr;

	BEGIN_EXCEPTION_HANDLING(0);

	arr = newArray(booleanClass, len);

	ADD_REF(arr);
	END_EXCEPTION_HANDLING();
	return (arr);
}

static jbyteArray
Kaffe_NewByteArray(JNIEnv* env UNUSED, jsize len)
{
	jbyteArray arr;

	BEGIN_EXCEPTION_HANDLING(0);

	arr = newArray(byteClass, len);

	ADD_REF(arr);
	END_EXCEPTION_HANDLING();
	return (arr);
}

static jcharArray
Kaffe_NewCharArray(JNIEnv* env UNUSED, jsize len)
{
	jcharArray arr;

	BEGIN_EXCEPTION_HANDLING(0);

	arr = newArray(charClass, len);

	ADD_REF(arr);
	END_EXCEPTION_HANDLING();
	return (arr);
}

static jshortArray
Kaffe_NewShortArray(JNIEnv* env UNUSED, jsize len)
{
	jshortArray arr;

	BEGIN_EXCEPTION_HANDLING(0);

	arr = newArray(shortClass, len);

	ADD_REF(arr);
	END_EXCEPTION_HANDLING();
	return (arr);
}

static jintArray
Kaffe_NewIntArray(JNIEnv* env UNUSED, jsize len)
{
	jintArray arr;

	BEGIN_EXCEPTION_HANDLING(0);

	arr = newArray(intClass, len);

	ADD_REF(arr);
	END_EXCEPTION_HANDLING();
	return (arr);
}

static jlongArray
Kaffe_NewLongArray(JNIEnv* env UNUSED, jsize len)
{
	jlongArray arr;

	BEGIN_EXCEPTION_HANDLING(0);

	arr = newArray(longClass, len);

	ADD_REF(arr);
	END_EXCEPTION_HANDLING();
	return (arr);
}

static jfloatArray
Kaffe_NewFloatArray(JNIEnv* env UNUSED, jsize len)
{
	jfloatArray arr;

	BEGIN_EXCEPTION_HANDLING(0);

	arr = newArray(floatClass, len);

	ADD_REF(arr);
	END_EXCEPTION_HANDLING();
	return (arr);
}

static jdoubleArray
Kaffe_NewDoubleArray(JNIEnv* env UNUSED, jsize len)
{
	jdoubleArray arr;

	BEGIN_EXCEPTION_HANDLING(0);

	arr = newArray(doubleClass, len);

	ADD_REF(arr);
	END_EXCEPTION_HANDLING();
	return (arr);
}

static jboolean*
Kaffe_GetBooleanArrayElements(JNIEnv* env UNUSED, jbooleanArray arr, jbool* iscopy)
{
	jboolean* array;
	BEGIN_EXCEPTION_HANDLING(0);

	if (iscopy != NULL) {
		*iscopy = JNI_FALSE;
	}
	array = unhand_array((HArrayOfBoolean*)arr)->body;

	END_EXCEPTION_HANDLING();
	return (array);
}

static jbyte*
Kaffe_GetByteArrayElements(JNIEnv* env UNUSED, jbyteArray arr, jbool* iscopy)
{
	jbyte* array;
	BEGIN_EXCEPTION_HANDLING(0);

	if (iscopy != NULL) {
		*iscopy = JNI_FALSE;
	}
	array = unhand_array((HArrayOfByte*)arr)->body;

	END_EXCEPTION_HANDLING();
	return (array);
}

static void*
Kaffe_GetPrimitiveArrayCritical(JNIEnv* env, jarray arr, jbool* iscopy)
{
  return (Kaffe_GetByteArrayElements(env, (jbyteArray)arr, iscopy));
}

static jchar*
Kaffe_GetCharArrayElements(JNIEnv* env UNUSED, jcharArray arr, jbool* iscopy)
{
	jchar* array;
	BEGIN_EXCEPTION_HANDLING(0);

	if (iscopy != NULL) {
		*iscopy = JNI_FALSE;
	}
	array = unhand_array((HArrayOfChar*)arr)->body;

	END_EXCEPTION_HANDLING();
	return (array);
}

static jshort*
Kaffe_GetShortArrayElements(JNIEnv* env UNUSED, jshortArray arr, jbool* iscopy)
{
	jshort* array;
	BEGIN_EXCEPTION_HANDLING(0);

	if (iscopy != NULL) {
		*iscopy = JNI_FALSE;
	}
	array = unhand_array((HArrayOfShort*)arr)->body;

	END_EXCEPTION_HANDLING();
	return (array);
}

static jint*
Kaffe_GetIntArrayElements(JNIEnv* env UNUSED, jintArray arr, jbool* iscopy)
{
	jint* array;
	BEGIN_EXCEPTION_HANDLING(0);

	if (iscopy != NULL) {
		*iscopy = JNI_FALSE;
	}
	array = unhand_array((HArrayOfInt*)arr)->body;

	END_EXCEPTION_HANDLING();
	return (array);
}

static jlong*
Kaffe_GetLongArrayElements(JNIEnv* env UNUSED, jlongArray arr, jbool* iscopy)
{
	jlong* array;
	BEGIN_EXCEPTION_HANDLING(0);

	if (iscopy != NULL) {
		*iscopy = JNI_FALSE;
	}
	array = unhand_array((HArrayOfLong*)arr)->body;

	END_EXCEPTION_HANDLING();
	return (array);
}

static jfloat*
Kaffe_GetFloatArrayElements(JNIEnv* env UNUSED, jfloatArray arr, jbool* iscopy)
{
	jfloat* array;
	BEGIN_EXCEPTION_HANDLING(0);

	if (iscopy != NULL) {
		*iscopy = JNI_FALSE;
	}
	array = unhand_array((HArrayOfFloat*)arr)->body;

	END_EXCEPTION_HANDLING();
	return (array);
}

static jdouble*
Kaffe_GetDoubleArrayElements(JNIEnv* env UNUSED, jdoubleArray arr, jbool* iscopy)
{
	jdouble* array;
	BEGIN_EXCEPTION_HANDLING(0);

	if (iscopy != NULL) {
		*iscopy = JNI_FALSE;
	}
	array = unhand_array((HArrayOfDouble*)arr)->body;

	END_EXCEPTION_HANDLING();
	return (array);
}

static void
Kaffe_ReleaseBooleanArrayElements(JNIEnv* env UNUSED, jbooleanArray arr, jbool* elems, jint mode)
{
	BEGIN_EXCEPTION_HANDLING_VOID();

	if (elems != unhand_array((HArrayOfBoolean*)arr)->body) {
		switch (mode) {
		case JNI_COMMIT:
			memcpy(unhand_array((HArrayOfBoolean*)arr)->body, elems, obj_length((HArrayOfBoolean*)arr) * sizeof(jboolean));
			break;
		case 0:
			memcpy(unhand_array((HArrayOfBoolean*)arr)->body, elems, obj_length((HArrayOfBoolean*)arr) * sizeof(jboolean));
			KFREE(elems);
			break;
		case JNI_ABORT:
			KFREE(elems);
			break;
		}
	}
	END_EXCEPTION_HANDLING();
}

static void
Kaffe_ReleaseByteArrayElements(JNIEnv* env UNUSED, jbyteArray arr, jbyte* elems, jint mode)
{
	BEGIN_EXCEPTION_HANDLING_VOID();

	if (elems != unhand_array((HArrayOfByte*)arr)->body) {
		switch (mode) {
		case JNI_COMMIT:
			memcpy(unhand_array((HArrayOfByte*)arr)->body, elems, obj_length((HArrayOfByte*)arr) * sizeof(jbyte));
			break;
		case 0:
			memcpy(unhand_array((HArrayOfByte*)arr)->body, elems, obj_length((HArrayOfByte*)arr) * sizeof(jbyte));
			KFREE(elems);
			break;
		case JNI_ABORT:
			KFREE(elems);
			break;
		}
	}
	END_EXCEPTION_HANDLING();
}

static void
Kaffe_ReleasePrimitiveArrayCritical(JNIEnv* env, jbyteArray arr, void* elems, jint mode)
{
  Kaffe_ReleaseByteArrayElements(env, (jbyteArray)arr, (jbyte*)elems, mode);
}

static void
Kaffe_ReleaseCharArrayElements(JNIEnv* env UNUSED, jcharArray arr, jchar* elems, jint mode)
{
	BEGIN_EXCEPTION_HANDLING_VOID();

	if (elems != unhand_array((HArrayOfChar*)arr)->body) {
		switch (mode) {
		case JNI_COMMIT:
			memcpy(unhand_array((HArrayOfChar*)arr)->body, elems, obj_length((HArrayOfChar*)arr) * sizeof(jchar));
			break;
		case 0:
			memcpy(unhand_array((HArrayOfChar*)arr)->body, elems, obj_length((HArrayOfChar*)arr) * sizeof(jchar));
			KFREE(elems);
			break;
		case JNI_ABORT:
			KFREE(elems);
			break;
		}
	}
	END_EXCEPTION_HANDLING();
}

static void
Kaffe_ReleaseShortArrayElements(JNIEnv* env UNUSED, jshortArray arr, jshort* elems, jint mode)
{
	BEGIN_EXCEPTION_HANDLING_VOID();

	if (elems != unhand_array((HArrayOfShort*)arr)->body) {
		switch (mode) {
		case JNI_COMMIT:
			memcpy(unhand_array((HArrayOfShort*)arr)->body, elems, obj_length((HArrayOfShort*)arr) * sizeof(jshort));
			break;
		case 0:
			memcpy(unhand_array((HArrayOfShort*)arr)->body, elems, obj_length((HArrayOfShort*)arr) * sizeof(jshort));
			KFREE(elems);
			break;
		case JNI_ABORT:
			KFREE(elems);
			break;
		}
	}
	END_EXCEPTION_HANDLING();
}

static void
Kaffe_ReleaseIntArrayElements(JNIEnv* env UNUSED, jintArray arr, jint* elems, jint mode)
{
	BEGIN_EXCEPTION_HANDLING_VOID();

	if (elems != unhand_array((HArrayOfInt*)arr)->body) {
		switch (mode) {
		case JNI_COMMIT:
			memcpy(unhand_array((HArrayOfInt*)arr)->body, elems, obj_length((HArrayOfInt*)arr) * sizeof(jint));
			break;
		case 0:
			memcpy(unhand_array((HArrayOfInt*)arr)->body, elems, obj_length((HArrayOfInt*)arr) * sizeof(jint));
			KFREE(elems);
			break;
		case JNI_ABORT:
			KFREE(elems);
			break;
		}
	}
	END_EXCEPTION_HANDLING();
}

static void
Kaffe_ReleaseLongArrayElements(JNIEnv* env UNUSED, jlongArray arr, jlong* elems, jint mode)
{
	BEGIN_EXCEPTION_HANDLING_VOID();

	if (elems != unhand_array((HArrayOfLong*)arr)->body) {
		switch (mode) {
		case JNI_COMMIT:
			memcpy(unhand_array((HArrayOfLong*)arr)->body, elems, obj_length((HArrayOfLong*)arr) * sizeof(jlong));
			break;
		case 0:
			memcpy(unhand_array((HArrayOfLong*)arr)->body, elems, obj_length((HArrayOfLong*)arr) * sizeof(jlong));
			KFREE(elems);
			break;
		case JNI_ABORT:
			KFREE(elems);
			break;
		}
	}
	END_EXCEPTION_HANDLING();
}

static void
Kaffe_ReleaseFloatArrayElements(JNIEnv* env UNUSED, jfloatArray arr, jfloat* elems, jint mode)
{
	BEGIN_EXCEPTION_HANDLING_VOID();

	if (elems != unhand_array((HArrayOfFloat*)arr)->body) {
		switch (mode) {
		case JNI_COMMIT:
			memcpy(unhand_array((HArrayOfFloat*)arr)->body, elems, obj_length((HArrayOfFloat*)arr) * sizeof(jfloat));
			break;
		case 0:
			memcpy(unhand_array((HArrayOfFloat*)arr)->body, elems, obj_length((HArrayOfFloat*)arr) * sizeof(jfloat));
			KFREE(elems);
			break;
		case JNI_ABORT:
			KFREE(elems);
			break;
		}
	}
	END_EXCEPTION_HANDLING();
}

static void
Kaffe_ReleaseDoubleArrayElements(JNIEnv* env UNUSED, jdoubleArray arr, jdouble* elems, jint mode)
{
	BEGIN_EXCEPTION_HANDLING_VOID();

	if (elems != unhand_array((HArrayOfDouble*)arr)->body) {
		switch (mode) {
		case JNI_COMMIT:
			memcpy(unhand_array((HArrayOfDouble*)arr)->body, elems, obj_length((HArrayOfDouble*)arr) * sizeof(jdouble));
			break;
		case 0:
			memcpy(unhand_array((HArrayOfDouble*)arr)->body, elems, obj_length((HArrayOfDouble*)arr) * sizeof(jdouble));
			KFREE(elems);
			break;
		case JNI_ABORT:
			KFREE(elems);
			break;
		}
	}
	END_EXCEPTION_HANDLING();
}

static void
Kaffe_GetBooleanArrayRegion(JNIEnv* env UNUSED, jbooleanArray arr, jsize start, jsize len, jbool* data)
{
	BEGIN_EXCEPTION_HANDLING_VOID();

	if (start >= obj_length((HArrayOfBoolean*)arr) || start + len > obj_length((HArrayOfBoolean*)arr)) {
		throwException(ArrayIndexOutOfBoundsException);
	}
	memcpy(data, &unhand_array((HArrayOfBoolean*)arr)->body[start], len * sizeof(jboolean));

	END_EXCEPTION_HANDLING();
}

static void
Kaffe_GetByteArrayRegion(JNIEnv* env UNUSED, jbyteArray arr, jsize start, jsize len, jbyte* data)
{
	BEGIN_EXCEPTION_HANDLING_VOID();

	if (start >= obj_length((HArrayOfByte*)arr) || start + len > obj_length((HArrayOfByte*)arr)) {
		throwException(ArrayIndexOutOfBoundsException);
	}
	memcpy(data, &unhand_array((HArrayOfByte*)arr)->body[start], len * sizeof(jbyte));

	END_EXCEPTION_HANDLING();
}

static void
Kaffe_GetCharArrayRegion(JNIEnv* env UNUSED, jcharArray arr, jsize start, jsize len, jchar* data)
{
	BEGIN_EXCEPTION_HANDLING_VOID();

	if (start >= obj_length((HArrayOfChar*)arr) || start + len > obj_length((HArrayOfChar*)arr)) {
		throwException(ArrayIndexOutOfBoundsException);
	}
	memcpy(data, &unhand_array((HArrayOfChar*)arr)->body[start], len * sizeof(jchar));

	END_EXCEPTION_HANDLING();
}

static void
Kaffe_GetShortArrayRegion(JNIEnv* env UNUSED, jshortArray arr, jsize start, jsize len, jshort* data)
{
	BEGIN_EXCEPTION_HANDLING_VOID();

	if (start >= obj_length((HArrayOfShort*)arr) || start + len > obj_length((HArrayOfShort*)arr)) {
		throwException(ArrayIndexOutOfBoundsException);
	}
	memcpy(data, &unhand_array((HArrayOfShort*)arr)->body[start], len * sizeof(jshort));

	END_EXCEPTION_HANDLING();
}

static void
Kaffe_GetIntArrayRegion(JNIEnv* env UNUSED, jintArray arr, jsize start, jsize len, jint* data)
{
	BEGIN_EXCEPTION_HANDLING_VOID();

	if (start >= obj_length((HArrayOfInt*)arr) || start + len > obj_length((HArrayOfInt*)arr)) {
		throwException(ArrayIndexOutOfBoundsException);
	}
	memcpy(data, &unhand_array((HArrayOfInt*)arr)->body[start], len * sizeof(jint));

	END_EXCEPTION_HANDLING();
}

static void
Kaffe_GetLongArrayRegion(JNIEnv* env UNUSED, jlongArray arr, jsize start, jsize len, jlong* data)
{
	BEGIN_EXCEPTION_HANDLING_VOID();

	if (start >= obj_length((HArrayOfLong*)arr) || start + len > obj_length((HArrayOfLong*)arr)) {
		throwException(ArrayIndexOutOfBoundsException);
	}
	memcpy(data, &unhand_array((HArrayOfLong*)arr)->body[start], len * sizeof(jlong));

	END_EXCEPTION_HANDLING();
}

static void
Kaffe_GetFloatArrayRegion(JNIEnv* env UNUSED, jfloatArray arr, jsize start, jsize len, jfloat* data)
{
	BEGIN_EXCEPTION_HANDLING_VOID();

	if (start >= obj_length((HArrayOfFloat*)arr) || start + len > obj_length((HArrayOfFloat*)arr)) {
		throwException(ArrayIndexOutOfBoundsException);
	}
	memcpy(data, &unhand_array((HArrayOfFloat*)arr)->body[start], len * sizeof(jfloat));

	END_EXCEPTION_HANDLING();
}

static void
Kaffe_GetDoubleArrayRegion(JNIEnv* env UNUSED, jdoubleArray arr, jsize start, jsize len, jdouble* data)
{
	BEGIN_EXCEPTION_HANDLING_VOID();

	if (start >= obj_length((HArrayOfDouble*)arr) || start + len > obj_length((HArrayOfDouble*)arr)) {
		throwException(ArrayIndexOutOfBoundsException);
	}
	memcpy(data, &unhand_array((HArrayOfDouble*)arr)->body[start], len * sizeof(jdouble));

	END_EXCEPTION_HANDLING();
}

static void
Kaffe_SetBooleanArrayRegion(JNIEnv* env UNUSED, jbooleanArray arr, jsize start, jsize len, jbool* data)
{
	BEGIN_EXCEPTION_HANDLING_VOID();

	if (start >= obj_length((HArrayOfBoolean*)arr) || start+len > obj_length((HArrayOfBoolean*)arr)) {
		throwException(ArrayIndexOutOfBoundsException);
	}
	memcpy(&unhand_array((HArrayOfBoolean*)arr)->body[start], data, len * sizeof(jboolean));

	END_EXCEPTION_HANDLING();
}

static void
Kaffe_SetByteArrayRegion(JNIEnv* env UNUSED, jbyteArray arr, jsize start, jsize len, jbyte* data)
{
	BEGIN_EXCEPTION_HANDLING_VOID();

	if (start >= obj_length((HArrayOfByte*)arr) || start+len > obj_length((HArrayOfByte*)arr)) {
		throwException(ArrayIndexOutOfBoundsException);
	}
	memcpy(&unhand_array((HArrayOfByte*)arr)->body[start], data, len * sizeof(jbyte));

	END_EXCEPTION_HANDLING();
}

static void
Kaffe_SetCharArrayRegion(JNIEnv* env UNUSED, jcharArray arr, jsize start, jsize len, jchar* data)
{
	BEGIN_EXCEPTION_HANDLING_VOID();

	if (start >= obj_length((HArrayOfChar*)arr) || start+len > obj_length((HArrayOfChar*)arr)) {
		throwException(ArrayIndexOutOfBoundsException);
	}
	memcpy(&unhand_array((HArrayOfChar*)arr)->body[start], data, len * sizeof(jchar));

	END_EXCEPTION_HANDLING();
}

static void
Kaffe_SetShortArrayRegion(JNIEnv* env UNUSED, jshortArray arr, jsize start, jsize len, jshort* data)
{
	BEGIN_EXCEPTION_HANDLING_VOID();

	if (start >= obj_length((HArrayOfShort*)arr) || start+len > obj_length((HArrayOfShort*)arr)) {
		throwException(ArrayIndexOutOfBoundsException);
	}
	memcpy(&unhand_array((HArrayOfShort*)arr)->body[start], data, len * sizeof(jshort));

	END_EXCEPTION_HANDLING();
}

static void
Kaffe_SetIntArrayRegion(JNIEnv* env UNUSED, jintArray arr, jsize start, jsize len, jint* data)
{
	BEGIN_EXCEPTION_HANDLING_VOID();

	if (start >= obj_length((HArrayOfInt*)arr) || start+len > obj_length((HArrayOfInt*)arr)) {
		throwException(ArrayIndexOutOfBoundsException);
	}
	memcpy(&unhand_array((HArrayOfInt*)arr)->body[start], data, len * sizeof(jint));

	END_EXCEPTION_HANDLING();
}

static void
Kaffe_SetLongArrayRegion(JNIEnv* env UNUSED, jlongArray arr, jsize start, jsize len, jlong* data)
{
	BEGIN_EXCEPTION_HANDLING_VOID();

	if (start >= obj_length((HArrayOfLong*)arr) || start+len > obj_length((HArrayOfLong*)arr)) {
		throwException(ArrayIndexOutOfBoundsException);
	}
	memcpy(&unhand_array((HArrayOfLong*)arr)->body[start], data, len * sizeof(jlong));

	END_EXCEPTION_HANDLING();
}

static void
Kaffe_SetFloatArrayRegion(JNIEnv* env UNUSED, jfloatArray arr, jsize start, jsize len, jfloat* data)
{
	BEGIN_EXCEPTION_HANDLING_VOID();

	if (start >= obj_length((HArrayOfFloat*)arr) || start+len > obj_length((HArrayOfFloat*)arr)) {
		throwException(ArrayIndexOutOfBoundsException);
	}
	memcpy(&unhand_array((HArrayOfFloat*)arr)->body[start], data, len * sizeof(jfloat));

	END_EXCEPTION_HANDLING();
}

static void
Kaffe_SetDoubleArrayRegion(JNIEnv* env UNUSED, jdoubleArray arr, jsize start, jsize len, jdouble* data)
{
	BEGIN_EXCEPTION_HANDLING_VOID();

	if (start >= obj_length((HArrayOfDouble*)arr) || start+len > obj_length((HArrayOfDouble*)arr)) {
		throwException(ArrayIndexOutOfBoundsException);
	}
	memcpy(&unhand_array((HArrayOfDouble*)arr)->body[start], data, len * sizeof(jdouble));

	END_EXCEPTION_HANDLING();
}

static jint
Kaffe_RegisterNatives(JNIEnv* env UNUSED, jclass cls, const JNINativeMethod* methodArray, jint nmethods)
{
	Method* meth;
	int nmeth;
	int i;
	int j;

	BEGIN_EXCEPTION_HANDLING(0);

	meth = CLASS_METHODS((Hjava_lang_Class*)cls);
	nmeth = CLASS_NMETHODS((Hjava_lang_Class*)cls);

	for (j = 0; j < nmethods; j++) {
		for (i = 0; i < nmeth; i++) {
			if (strcmp(meth[i].name->data, methodArray[j].name) == 0 &&
			    strcmp(METHOD_SIGD(&meth[i]), methodArray[j].signature) == 0 &&
			    (meth[i].accflags & ACC_NATIVE) != 0) {
				Kaffe_JNI_wrapper(&meth[i], methodArray[j].fnPtr); 
				goto found;
			}
		}
		/* Failed to find method */
		throwException(NoSuchMethodError(methodArray[j].name));

		found:;
	}

	END_EXCEPTION_HANDLING();

	return (0);
}

static jint
Kaffe_UnregisterNatives(JNIEnv* env UNUSED, jclass cls UNUSED)
{
	/* We do not support unloading */
	return (-1);
}

static jint
Kaffe_MonitorEnter(JNIEnv* env UNUSED, jobject obj)
{
	BEGIN_EXCEPTION_HANDLING(0);

	/* We should never throw out of a JNI call */
	lockObject(obj);

	END_EXCEPTION_HANDLING();
	return (0);
}

static jint
Kaffe_MonitorExit(JNIEnv* env UNUSED, jobject obj)
{
	BEGIN_EXCEPTION_HANDLING(0);

	unlockObject(obj);

	END_EXCEPTION_HANDLING();
	return (0);
}

/*
 * Functions past this point don't bother with jni exceptions.
 */
static jint
Kaffe_GetJavaVM(JNIEnv* env UNUSED, JavaVM** vm)
{
	(*vm) = &Kaffe_JavaVM;
	return (0);
}

static jint
NONRETURNING
Kaffe_DestroyJavaVM(JavaVM* vm UNUSED)
{
	/* Right now, calling this from main2 is what prevents us from 
	   exiting there */
	exitThread();
}

static jint
Kaffe_AttachCurrentThread(JavaVM* vm UNUSED, void** penv, ThreadAttachArgs* args UNUSED)
{
	if (jthread_attach_current_thread (false)) {
		ksemInit(&THREAD_DATA()->sem);
		attachFakedThreadInstance ("test attach");
		*penv = THREAD_JNIENV();
		return 0;
	}
	return -1;
}

static jint
Kaffe_DetachCurrentThread(JavaVM* vm UNUSED)
{
	if (jthread_detach_current_thread ()) {
		return 0;
	} else {
		return -1;
	}
}

static jint
Kaffe_GetEnv(JavaVM* vm, void** penv, jint interface_id)
{
	JavaVM* currentVM;
	JNIEnv *je;

	/* In the event of any error condition, we side effect the argument
	   pointer as well as return an error code */
	(*penv) = NULL;

	je = THREAD_JNIENV();
	/* Insure that the current thread is associated with the
	   given VM. This gets the JavaVM to which the current thread
	   is attached.  I *think* this is a good way to do this. */
	Kaffe_GetJavaVM(je, &currentVM);
	if (!Kaffe_IsSameObject(je, currentVM, vm))
		return (JNI_EDETACHED);

	/* Is the requested version of the interface known? */
	switch (interface_id) {
	case JNI_VERSION_1_1:
		(*penv) = je;
		return (JNI_OK);
	case JNI_VERSION_1_2:
		(*penv) = je;
		return (JNI_OK);
#if 0
	case JVMDI_VERSION_1:
		(*penv) = (JVMDI_Interface_1*)&Kaffe_JVMDIEnv;
		return (JNI_OK);
#endif

#if defined(ENABLE_JVMPI)
	case JVMPI_VERSION_1:
	case JVMPI_VERSION_1_1:
		(*penv) = jvmpiCreateInterface(interface_id);
		return (JNI_OK);
#endif
		
	default:
		return (JNI_EVERSION);
	}
}

static void
strcatJNI(char* to, const char* from)
{
	char* ptr;

	ptr = &to[strlen(to)];
	for (; *from != 0; from++) {
		switch (*from) {
		case '(':
			/* Ignore */
			break;
		case ')':
			/* Terminate here */
			goto end;
		case '_':
			*ptr++ = '_';
			*ptr++ = '1';
			break;
		case ';':
			*ptr++ = '_';
			*ptr++ = '2';
			break;
		case '[':
			*ptr++ = '_';
			*ptr++ = '3';
			break;
		case '/':
			*ptr++ = '_';
			break;
		default:
			*ptr++ = *from;
			break;
		}
	}

	end:;
	*ptr = 0;
}

#if defined(TRANSLATOR)
/*
 * Wrap up a native function in a calling wrapper, with JNI or KNI.
 */
static void
Kaffe_wrapper(Method* xmeth, void* func, bool use_JNI)
{
	errorInfo info;
	int count;
	nativeCodeInfo ncode;
	SlotInfo* tmp = 0;
	bool success = true;
	int j;
	int an;
	int iLockRoot;

	isStatic = METHOD_IS_STATIC(xmeth) ? 1 : 0;
	count = sizeofSigMethod(xmeth, false);
	count += 1 - isStatic;

#if defined(KAFFE_FEEDBACK)
	if( kaffe_feedback_file )
		lockMutex(kaffe_feedback_file);
#endif
	/* Construct a wrapper to call the JNI method with the correct
	 * arguments.
	 */
	enterTranslator();

#if defined(KAFFE_PROFILER)
	if (profFlag) {
		profiler_get_clicks(xmeth->jitClicks);
		xmeth->callsCount = 0;
		xmeth->totalClicks = 0;
		xmeth->totalChildrenClicks = 0;
	}
#endif
	globalMethod = xmeth;

	maxArgs = maxLocal = count; /* make sure args are spilled if needed */
	maxStack = 0;
	success = initInsnSequence(xmeth, 0, maxLocal, maxStack, &info);
	if (!success) {
		goto done;
	}

	start_basic_block();
	prologue(xmeth);

	if (use_JNI) {
		/* Start a JNI call */
		slot_alloctmp(tmp);
		begin_func_sync();
		call_soft(startJNIcall);
		end_func_sync();
		return_ref(tmp);

#if defined(NEED_JNIREFS)
		{
			int j;
			int jcount;

			/* Make the necesary JNI ref calls first */
			if (!METHOD_IS_STATIC(xmeth)) {
				pusharg_ref(local(0), 0);
				end_sub_block();
				call_soft(addJNIref);
				popargs();
			}
			j = METHOD_NARGS(xmeth);
			jcount = count;
			while (j > 0) {
				j--;
				jcount -= sizeofSigChar(*METHOD_ARG_TYPE(xmeth, j));
				switch (*METHOD_ARG_TYPE(xmeth, j)) {
				case 'L':
				case '[':
					pusharg_ref(local(jcount), 0);
					end_sub_block();
					call_soft(addJNIref);
					popargs();
				}
			}
			start_sub_block();
		}
#endif
	}

	/* Add synchronisation if necessary */
	if (xmeth->accflags & ACC_SYNCHRONISED) {
		mon_enter(xmeth, local(0));
	}

#if defined(PUSHARG_FORWARDS)

	if (use_JNI) {
		/* Push the JNI info */
		pusharg_ref(tmp, 0);

		/* If static, push the class, else push the object */
		if (METHOD_IS_STATIC(xmeth)) {
			pusharg_ref_const(xmeth->class, 1);
			an = 0;
		}
		else {
			pusharg_ref(local(0), 1);
			an = 1;
		}
		count = 2;
	}
	else {
		/* If static, nothing, else push the object */
		if (!METHOD_IS_STATIC(xmeth)) {
			pusharg_ref(local(0), 0);
			an = 1;
		}
		else {
			an = 0;
		}
		count = an;
	}

	/* Push the specified arguments */
	for (j = 0; j < METHOD_NARGS(xmeth); j++) {
		switch (*METHOD_ARG_TYPE(xmeth, j)) {
		case 'L':
		case '[':
			pusharg_ref(local(an), count);
			break;
		case 'I':
		case 'Z':
		case 'S':
		case 'B':
		case 'C':
			pusharg_int(local(an), count);
			break;
		case 'F':
			pusharg_float(local(an), count);
			break;
		case 'J':
			pusharg_long(local(an), count);
			count += pusharg_long_idx_inc - 1;
			an++;
			break;
		case 'D':
			pusharg_double(local(an), count);
			count += pusharg_long_idx_inc - 1;
			an++;
			break;
		}
		count++;
		an++;
	}

#else
	/* TODO: Deal with 64bits where J and D use only one slot.  */
	/* Push the specified arguments */
	count = maxArgs;
	if (use_JNI) {
		count++;
		if (isStatic) {
			count++;
		}
	}
	an = maxArgs;

	for (j = METHOD_NARGS(xmeth); --j >= 0; ) {
		count--;
		an--;
		switch (*METHOD_ARG_TYPE(xmeth, j)) {
		case 'L':
		case '[':
			pusharg_ref(local(an), count);
			break;
		case 'I':
		case 'Z':
		case 'S':
		case 'B':
		case 'C':
			pusharg_int(local(an), count);
			break;
		case 'F':
			pusharg_float(local(an), count);
			break;
		case 'J':
			count--;
			an--;
			pusharg_long(local(an), count);
			break;
		case 'D':
			count--;
			an--;
			pusharg_double(local(an), count);
			break;
		}
	}

	if (use_JNI) {
		/* If static, push the class, else push the object */
		if (METHOD_IS_STATIC(xmeth)) {
			pusharg_ref_const(xmeth->class, 1);
		}
		else {
			pusharg_ref(local(0), 1);
		}

		/* Push the JNI info */
		pusharg_ref(tmp, 0);
	}
	else {
		/* If static, nothing, else push the object */
		if (!METHOD_IS_STATIC(xmeth)) {
			pusharg_ref(local(0), 0);
		}
	}

#endif

	/* Make the call */
	end_sub_block();
	call_soft(func);
	popargs();

	if (use_JNI) {
		slot_freetmp(tmp);
	}
	
	start_sub_block();

	/* Determine return type */
	switch (*METHOD_RET_TYPE(xmeth)) {
	case 'L':
	case '[':
		slot_alloctmp(tmp);
		return_ref(tmp);
		/* Remove synchronisation if necessary */
		if (xmeth->accflags & ACC_SYNCHRONISED) {
			mon_exit(xmeth, local(0));
		}
		if (use_JNI) {
			end_sub_block();
			call_soft(finishJNIcall);
			start_sub_block();
		}
		returnarg_ref(tmp);
		break;
	case 'I':
	case 'Z':
	case 'S':
	case 'B':
	case 'C':
		slot_alloctmp(tmp);
		return_int(tmp);
		/* Remove synchronisation if necessary */
		if (xmeth->accflags & ACC_SYNCHRONISED) {
			mon_exit(xmeth, local(0));
		}
		if (use_JNI) {
			end_sub_block();
			call_soft(finishJNIcall);
			start_sub_block();
		}
		returnarg_int(tmp);
		break;
	case 'F':
		slot_alloctmp(tmp);
		return_float(tmp);
		/* Remove synchronisation if necessary */
		if (xmeth->accflags & ACC_SYNCHRONISED) {
			mon_exit(xmeth, local(0));
		}
		if (use_JNI) {
			end_sub_block();
			call_soft(finishJNIcall);
			start_sub_block();
		}
		returnarg_float(tmp);
		break;
	case 'J':
		slot_alloc2tmp(tmp);
		return_long(tmp);
		/* Remove synchronisation if necessary */
		if (xmeth->accflags & ACC_SYNCHRONISED) {
			mon_exit(xmeth, local(0));
		}
		if (use_JNI) {
			end_sub_block();
			call_soft(finishJNIcall);
			start_sub_block();
		}
		returnarg_long(tmp);
		break;
	case 'D':
		slot_alloc2tmp(tmp);
		return_double(tmp);
		/* Remove synchronisation if necessary */
		if (xmeth->accflags & ACC_SYNCHRONISED) {
			mon_exit(xmeth, local(0));
		}
		if (use_JNI) {
			end_sub_block();
			call_soft(finishJNIcall);
			start_sub_block();
		}
		returnarg_double(tmp);
		break;
	case 'V':
		/* Remove synchronisation if necessary */
		if (xmeth->accflags & ACC_SYNCHRONISED) {
			mon_exit(xmeth, local(0));
		}
		if (use_JNI)
			call_soft(finishJNIcall);
#if defined(ENABLE_JVMPI)
		softcall_exit_method(globalMethod);
#endif
		ret();
		break;
	}

	end_function();

	/* Generate the code */
	if (tmpslot > maxTemp) {
		maxTemp = tmpslot;
	}
	/* The codeinfo argument is only used in linkLabel, and it is 
	 * only needed if we have labels referring to bytecode.  This is
	 * not the case here.
	 */
	success = finishInsnSequence(0, &ncode, &info);
	if (!success) {
		goto done;
	}

	assert(xmeth->exception_table == 0);
	installMethodCode(0, xmeth, &ncode);

	if (use_JNI)
		xmeth->accflags |= ACC_JNI;

done:
#if defined(TRANSLATOR) && defined(JIT3)
	resetConstants();
#endif
	resetLabels();

#if defined(KAFFE_PROFILER)
	if (profFlag) {
		profiler_click_t end;

		profiler_get_clicks(end);
		xmeth->jitClicks = end - xmeth->jitClicks;
	}
#endif
	globalMethod = 0;

	leaveTranslator();
#if defined(KAFFE_FEEDBACK)
	if( kaffe_feedback_file )
		unlockMutex(kaffe_feedback_file);
#endif
	if (!success) {
		throwError(&info);
	}
}

static void
Kaffe_JNI_wrapper(Method* xmeth, void* func)
{
	Kaffe_wrapper(xmeth, func, true);
}

void
Kaffe_KNI_wrapper(Method* xmeth, void* func)
{
    	/* to build stackStace, we must be able to lookup this native
	   method.  So always create a wrapper.  */
	Kaffe_wrapper(xmeth, func, false);
}
#endif
#if defined(INTERPRETER)
/*
 * Wrap up a native function in a calling wrapper.  The interpreter
 * lets the callMethod[AV] macros functions handle the JNI specifics.
 */
static void
Kaffe_JNI_wrapper(Method* xmeth, void* func)
{
	SET_METHOD_NATIVECODE(xmeth, func);
	xmeth->accflags |= ACC_JNI;
}

void
Kaffe_KNI_wrapper(Method* xmeth, void* func)
{
	SET_METHOD_NATIVECODE(xmeth, func);
}
#endif /* INTERPRETER */

#if defined(TRANSLATOR)
static void*
startJNIcall(void)
{
	threadData 	*thread_data = THREAD_DATA();
#if defined(NEED_JNIREFS)
	jnirefs* table;

	table = gc_malloc(sizeof(jnirefs), &gcNormal);
	table->prev = thread_data->jnireferences;
	thread_data->jnireferences = table;
#endif
	/* No pending exception when we enter JNI routine */
	thread_data->exceptObj = 0;
	return( &thread_data->jniEnv ); 
}

static void
finishJNIcall(void)
{
	jref eobj;
	threadData	*thread_data = THREAD_DATA();

#if defined(NEED_JNIREFS)
	{
		jnirefs* table;

		table = thread_data->jnireferences;
		thread_data->jnireferences = table->prev;
	}
#endif
	/* If we have a pending exception, throw it */
	eobj = thread_data->exceptObj;
	if (eobj != 0) {
		thread_data->exceptObj = 0;
		throwExternalException(eobj);
	}
}
#endif /* TRANSLATOR */

#if defined(NEED_JNIREFS)
static void
addJNIref(jref obj)
{
	jnirefs* table;
	int idx;

	table = THREAD_DATA()->jnireferences;

	if (table->used == JNIREFS) {
		abort();	/* FIX ME */
	}

	idx = table->next;
	for (;;) {
		if (table->objects[idx] == 0) {
			table->objects[idx] = obj;
			table->used++;
			table->next = (idx + 1) % JNIREFS;
			return;
		}
		idx = (idx + 1) % JNIREFS;
	}
}

static void
removeJNIref(jref obj)
{
	int idx;
	jnirefs* table;

	table = THREAD_DATA()->jnireferences;

	for (idx = 0; idx < JNIREFS; idx++) {
		if (table->objects[idx] == obj) {
			table->objects[idx] = 0;
			table->used--;
			return;
		}
	}
}
#endif /* NEED_JNIREFS */

/*
 * Look up a native function using the JNI interface system.
 */
jint
Kaffe_JNI_native(Method* meth)
{
	char name[1024];
	void* func;

	/* Build the simple JNI name for the method */
#if defined(NO_SHARED_LIBRARIES)
        strcpy(name, "Java_");
#elif defined(HAVE_DYN_UNDERSCORE)
	strcpy(name, "_Java_");
#else
	strcpy(name, "Java_");
#endif
	strcatJNI(name, meth->class->name->data);
	strcat(name, "_");
	strcatJNI(name, meth->name->data);

	func = loadNativeLibrarySym(name);
	if (func == NULL) {
		/* Try the long signatures */
		strcat(name, "__");
		strcatJNI(name, METHOD_SIGD(meth));
		func = loadNativeLibrarySym(name);
		if (func == 0) {
			return (JNI_FALSE);
		}
	}

	/* Wrap the function in a calling wrapper */
	Kaffe_JNI_wrapper(meth, func);

	return (JNI_TRUE);
}

/*
 * Setup the Kaffe JNI interfaces.
 */
struct JNINativeInterface Kaffe_JNINativeInterface = {

	NULL,
	NULL,
	NULL,
	NULL,
	Kaffe_GetVersion,
	Kaffe_DefineClass,
	Kaffe_FindClass,
	NULL,
	NULL,
	NULL,
	Kaffe_GetSuperClass,
	Kaffe_IsAssignableFrom,
	NULL,
	Kaffe_Throw,
	Kaffe_ThrowNew,
	Kaffe_ExceptionOccurred,
	Kaffe_ExceptionDescribe,
	Kaffe_ExceptionClear,
	Kaffe_FatalError,
	NULL,
	NULL,
	Kaffe_NewGlobalRef,
	Kaffe_DeleteGlobalRef,
	Kaffe_DeleteLocalRef,
	Kaffe_IsSameObject,
	NULL,
	NULL,
	Kaffe_AllocObject,
	Kaffe_NewObject,
	Kaffe_NewObjectV,
	Kaffe_NewObjectA,
	Kaffe_GetObjectClass,
	Kaffe_IsInstanceOf,
	Kaffe_GetMethodID,
	KaffeJNI_CallObjectMethod,
	KaffeJNI_CallObjectMethodV,
	KaffeJNI_CallObjectMethodA,
	KaffeJNI_CallBooleanMethod,
	KaffeJNI_CallBooleanMethodV,
	KaffeJNI_CallBooleanMethodA,
	KaffeJNI_CallByteMethod,
	KaffeJNI_CallByteMethodV,
	KaffeJNI_CallByteMethodA,
	KaffeJNI_CallCharMethod,
	KaffeJNI_CallCharMethodV,
	KaffeJNI_CallCharMethodA,
	KaffeJNI_CallShortMethod,
	KaffeJNI_CallShortMethodV,
	KaffeJNI_CallShortMethodA,
	KaffeJNI_CallIntMethod,
	KaffeJNI_CallIntMethodV,
	KaffeJNI_CallIntMethodA,
	KaffeJNI_CallLongMethod,
	KaffeJNI_CallLongMethodV,
	KaffeJNI_CallLongMethodA,
	KaffeJNI_CallFloatMethod,
	KaffeJNI_CallFloatMethodV,
	KaffeJNI_CallFloatMethodA,
	KaffeJNI_CallDoubleMethod,
	KaffeJNI_CallDoubleMethodV,
	KaffeJNI_CallDoubleMethodA,
	KaffeJNI_CallVoidMethod,
	KaffeJNI_CallVoidMethodV,
	KaffeJNI_CallVoidMethodA,
	KaffeJNI_CallNonvirtualObjectMethod,
	KaffeJNI_CallNonvirtualObjectMethodV,
	KaffeJNI_CallNonvirtualObjectMethodA,
	KaffeJNI_CallNonvirtualBooleanMethod,
	KaffeJNI_CallNonvirtualBooleanMethodV,
	KaffeJNI_CallNonvirtualBooleanMethodA,
	KaffeJNI_CallNonvirtualByteMethod,
	KaffeJNI_CallNonvirtualByteMethodV,
	KaffeJNI_CallNonvirtualByteMethodA,
	KaffeJNI_CallNonvirtualCharMethod,
	KaffeJNI_CallNonvirtualCharMethodV,
	KaffeJNI_CallNonvirtualCharMethodA,
	KaffeJNI_CallNonvirtualShortMethod,
	KaffeJNI_CallNonvirtualShortMethodV,
	KaffeJNI_CallNonvirtualShortMethodA,
	KaffeJNI_CallNonvirtualIntMethod,
	KaffeJNI_CallNonvirtualIntMethodV,
	KaffeJNI_CallNonvirtualIntMethodA,
	KaffeJNI_CallNonvirtualLongMethod,
	KaffeJNI_CallNonvirtualLongMethodV,
	KaffeJNI_CallNonvirtualLongMethodA,
	KaffeJNI_CallNonvirtualFloatMethod,
	KaffeJNI_CallNonvirtualFloatMethodV,
	KaffeJNI_CallNonvirtualFloatMethodA,
	KaffeJNI_CallNonvirtualDoubleMethod,
	KaffeJNI_CallNonvirtualDoubleMethodV,
	KaffeJNI_CallNonvirtualDoubleMethodA,
	KaffeJNI_CallNonvirtualVoidMethod,
	KaffeJNI_CallNonvirtualVoidMethodV,
	KaffeJNI_CallNonvirtualVoidMethodA,
	Kaffe_GetFieldID,
	Kaffe_GetObjectField,
	Kaffe_GetBooleanField,
	Kaffe_GetByteField,
	Kaffe_GetCharField,
	Kaffe_GetShortField,
	Kaffe_GetIntField,
	Kaffe_GetLongField,
	Kaffe_GetFloatField,
	Kaffe_GetDoubleField,
	Kaffe_SetObjectField,
	Kaffe_SetBooleanField,
	Kaffe_SetByteField,
	Kaffe_SetCharField,
	Kaffe_SetShortField,
	Kaffe_SetIntField,
	Kaffe_SetLongField,
	Kaffe_SetFloatField,
	Kaffe_SetDoubleField,
	Kaffe_GetStaticMethodID,
	KaffeJNI_CallStaticObjectMethod,
	KaffeJNI_CallStaticObjectMethodV,
	KaffeJNI_CallStaticObjectMethodA,
	KaffeJNI_CallStaticBooleanMethod,
	KaffeJNI_CallStaticBooleanMethodV,
	KaffeJNI_CallStaticBooleanMethodA,
	KaffeJNI_CallStaticByteMethod,
	KaffeJNI_CallStaticByteMethodV,
	KaffeJNI_CallStaticByteMethodA,
	KaffeJNI_CallStaticCharMethod,
	KaffeJNI_CallStaticCharMethodV,
	KaffeJNI_CallStaticCharMethodA,
	KaffeJNI_CallStaticShortMethod,
	KaffeJNI_CallStaticShortMethodV,
	KaffeJNI_CallStaticShortMethodA,
	KaffeJNI_CallStaticIntMethod,
	KaffeJNI_CallStaticIntMethodV,
	KaffeJNI_CallStaticIntMethodA,
	KaffeJNI_CallStaticLongMethod,
	KaffeJNI_CallStaticLongMethodV,
	KaffeJNI_CallStaticLongMethodA,
	KaffeJNI_CallStaticFloatMethod,
	KaffeJNI_CallStaticFloatMethodV,
	KaffeJNI_CallStaticFloatMethodA,
	KaffeJNI_CallStaticDoubleMethod,
	KaffeJNI_CallStaticDoubleMethodV,
	KaffeJNI_CallStaticDoubleMethodA,
	KaffeJNI_CallStaticVoidMethod,
	KaffeJNI_CallStaticVoidMethodV,
	KaffeJNI_CallStaticVoidMethodA,
	Kaffe_GetStaticFieldID,
	Kaffe_GetStaticObjectField,
	Kaffe_GetStaticBooleanField,
	Kaffe_GetStaticByteField,
	Kaffe_GetStaticCharField,
	Kaffe_GetStaticShortField,
	Kaffe_GetStaticIntField,
	Kaffe_GetStaticLongField,
	Kaffe_GetStaticFloatField,
	Kaffe_GetStaticDoubleField,
	Kaffe_SetStaticObjectField,
	Kaffe_SetStaticBooleanField,
	Kaffe_SetStaticByteField,
	Kaffe_SetStaticCharField,
	Kaffe_SetStaticShortField,
	Kaffe_SetStaticIntField,
	Kaffe_SetStaticLongField,
	Kaffe_SetStaticFloatField,
	Kaffe_SetStaticDoubleField,
	Kaffe_NewString,
	Kaffe_GetStringLength,
	Kaffe_GetStringChars,
	Kaffe_ReleaseStringChars,
	Kaffe_NewStringUTF,
	Kaffe_GetStringUTFLength,
	Kaffe_GetStringUTFChars,
	Kaffe_ReleaseStringUTFChars,
	Kaffe_GetArrayLength,
	Kaffe_NewObjectArray,
	Kaffe_GetObjectArrayElement,
	Kaffe_SetObjectArrayElement,
	Kaffe_NewBooleanArray,
	Kaffe_NewByteArray,
	Kaffe_NewCharArray,
	Kaffe_NewShortArray,
	Kaffe_NewIntArray,
	Kaffe_NewLongArray,
	Kaffe_NewFloatArray,
	Kaffe_NewDoubleArray,
	Kaffe_GetBooleanArrayElements,
	Kaffe_GetByteArrayElements,
	Kaffe_GetCharArrayElements,
	Kaffe_GetShortArrayElements,
	Kaffe_GetIntArrayElements,
	Kaffe_GetLongArrayElements,
	Kaffe_GetFloatArrayElements,
	Kaffe_GetDoubleArrayElements,
	Kaffe_ReleaseBooleanArrayElements,
	Kaffe_ReleaseByteArrayElements,
	Kaffe_ReleaseCharArrayElements,
	Kaffe_ReleaseShortArrayElements,
	Kaffe_ReleaseIntArrayElements,
	Kaffe_ReleaseLongArrayElements,
	Kaffe_ReleaseFloatArrayElements,
	Kaffe_ReleaseDoubleArrayElements,
	Kaffe_GetBooleanArrayRegion,
	Kaffe_GetByteArrayRegion,
	Kaffe_GetCharArrayRegion,
	Kaffe_GetShortArrayRegion,
	Kaffe_GetIntArrayRegion,
	Kaffe_GetLongArrayRegion,
	Kaffe_GetFloatArrayRegion,
	Kaffe_GetDoubleArrayRegion,
	Kaffe_SetBooleanArrayRegion,
	Kaffe_SetByteArrayRegion,
	Kaffe_SetCharArrayRegion,
	Kaffe_SetShortArrayRegion,
	Kaffe_SetIntArrayRegion,
	Kaffe_SetLongArrayRegion,
	Kaffe_SetFloatArrayRegion,
	Kaffe_SetDoubleArrayRegion,
	Kaffe_RegisterNatives,
	Kaffe_UnregisterNatives,
	Kaffe_MonitorEnter,
	Kaffe_MonitorExit,
	Kaffe_GetJavaVM,
	Kaffe_GetStringRegion,
	Kaffe_GetStringUTFRegion,
	Kaffe_GetPrimitiveArrayCritical,
	Kaffe_ReleasePrimitiveArrayCritical,
	NULL,
	NULL,
	NULL,
	NULL,
	Kaffe_ExceptionCheck,
	NULL,
	NULL,
	NULL,

};

/*
 * Setup the Kaffe invoke interface.
 */
struct JNIInvokeInterface Kaffe_JNIInvokeInterface = {
	NULL,
	NULL,
	NULL,
	Kaffe_DestroyJavaVM,
	Kaffe_AttachCurrentThread,
	Kaffe_DetachCurrentThread,
	Kaffe_GetEnv,
	NULL
};

/*
 * Setup the Kaffe VM.
 */
JavaVM Kaffe_JavaVM = {
	&Kaffe_JNIInvokeInterface,
};

JavaVMInitArgs Kaffe_JavaVMInitArgs = {
	0,		/* Version */
	0,		/* Properties */
	0,		/* Check source */
	THREADSTACKSIZE,/* Native stack size */
	0,		/* Java stack size */
	MIN_HEAPSIZE,	/* Min heap size */
	MAX_HEAPSIZE,	/* Max heap size */
	/*	2,	*/	/* Verify mode ... verify remote by default */
	0,              /* Verify mode ... noverify by default */
	".",		/* Classpath */
	0,		/* Bootclasspath */
	(void*)&vfprintf,/* Vprintf */
	(void*)&exit,	/* Exit */
	(void*)&abort,	/* Abort */
	1,		/* Enable class GC */
	0,		/* Enable verbose GC */
	1,		/* Disable async GC */
	0,		/* Enable verbose class loading */
	0,		/* Enable verbose JIT */
	0,		/* Enable verbose calls */
	ALLOC_HEAPSIZE,	/* Inc heap size */
	0,		/* Class home */
	0,		/* Library home */
};

/*
 * Array of VMs.
 */
JavaVMInitArgs Kaffe_JavaVMArgs[1];
