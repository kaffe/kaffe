/*
 * jni.c
 * Java Native Interface.
 *
 * Copyright (c) 1996, 1997, 2004
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * Copyright (c) 2004
 * 	The Kaffe.org's developers. See ChangeLog for details.
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
#include "jvmpi_kaffe.h"
#include "jni_i.h"
#include "jni_funcs.h"
#include "native-wrapper.h"
#include "kaffe_jni.h"

extern struct JNINativeInterface Kaffe_JNINativeInterface;
extern KaffeVM_Arguments Kaffe_JavaVMInitArgs;
extern JavaVM Kaffe_JavaVM;

static jint Kaffe_GetVersion(JNIEnv*);
static jclass Kaffe_FindClass(JNIEnv*, const char*);
static jint Kaffe_ThrowNew(JNIEnv*, jclass, const char*);
static jint Kaffe_Throw(JNIEnv* env, jobject obj);

#if defined(NEED_JNIREFS)
void
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

void
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


static jint
Kaffe_GetVersion(JNIEnv* UNUSED env)
{
       return Kaffe_JavaVMArgs.version;
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
Kaffe_FindClass(JNIEnv UNUSED *env, const char* name)
{
	jstring nameString;
	Utf8Const* utf8;
	jvalue retval;

	BEGIN_EXCEPTION_HANDLING(0);

	/* We accepts slashes, but Class.forName() does not */
	utf8 = checkPtr(utf8ConstNew(name, -1));
	nameString = utf8Const2JavaReplace(utf8, '/', '.');
	utf8ConstRelease(utf8);
	checkPtr(nameString);

	do_execute_java_class_method(&retval, "java.lang.Class", NULL,
		"forName", "(Ljava/lang/String;)Ljava/lang/Class;", nameString);
	ADD_REF(retval.l);

	END_EXCEPTION_HANDLING();
	return (retval.l);
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
		do_execute_java_method(NULL, thread_data->exceptObj, "printStackTrace", "()V",
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

	callMethodV(m, METHOD_NATIVECODE(m), obj, args, &retval);

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

	callMethodA(m, METHOD_NATIVECODE(m), obj, args, &retval, 0);

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
	
	return ((jmethodID)meth);
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
	return ((jfieldID)fld);
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

	return ((jmethodID)meth);
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

	return ((jfieldID)fld);
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
				meth[i].accflags |= ACC_JNI;
				engine_create_wrapper(&meth[i], methodArray[j].fnPtr); 
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
Kaffe_AttachCurrentThread(JavaVM* vm UNUSED, void** penv, JavaVMAttachArgs* args UNUSED)
{
	if (KTHREAD(attach_current_thread) (false)) {
		KSEM(init)(&THREAD_DATA()->sem);
		attachFakedThreadInstance ("test attach", false);
		*penv = THREAD_JNIENV();
		return 0;
	}
	return -1;
}

static jint
Kaffe_AttrachCurrentThreadAsDaemon(JavaVM* vm UNUSED, void** penv, JavaVMAttachArgs* args UNUSED)
{
	if (KTHREAD(attach_current_thread) (true)) {
		KSEM(init)(&THREAD_DATA()->sem);
		attachFakedThreadInstance ("daemon attach", true);
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
	KaffeJNI_FromReflectedMethod,
	KaffeJNI_FromReflectedField,
	KaffeJNI_ToReflectedMethod,
	Kaffe_GetSuperClass,
	Kaffe_IsAssignableFrom,
	KaffeJNI_ToReflectedField,
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
	KaffeJNI_GetObjectField,
	KaffeJNI_GetBooleanField,
	KaffeJNI_GetByteField,
	KaffeJNI_GetCharField,
	KaffeJNI_GetShortField,
	KaffeJNI_GetIntField,
	KaffeJNI_GetLongField,
	KaffeJNI_GetFloatField,
	KaffeJNI_GetDoubleField,
	KaffeJNI_SetObjectField,
	KaffeJNI_SetBooleanField,
	KaffeJNI_SetByteField,
	KaffeJNI_SetCharField,
	KaffeJNI_SetShortField,
	KaffeJNI_SetIntField,
	KaffeJNI_SetLongField,
	KaffeJNI_SetFloatField,
	KaffeJNI_SetDoubleField,
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
	KaffeJNI_GetStaticObjectField,
	KaffeJNI_GetStaticBooleanField,
	KaffeJNI_GetStaticByteField,
	KaffeJNI_GetStaticCharField,
	KaffeJNI_GetStaticShortField,
	KaffeJNI_GetStaticIntField,
	KaffeJNI_GetStaticLongField,
	KaffeJNI_GetStaticFloatField,
	KaffeJNI_GetStaticDoubleField,
	KaffeJNI_SetStaticObjectField,
	KaffeJNI_SetStaticBooleanField,
	KaffeJNI_SetStaticByteField,
	KaffeJNI_SetStaticCharField,
	KaffeJNI_SetStaticShortField,
	KaffeJNI_SetStaticIntField,
	KaffeJNI_SetStaticLongField,
	KaffeJNI_SetStaticFloatField,
	KaffeJNI_SetStaticDoubleField,
	KaffeJNI_NewString,
	KaffeJNI_GetStringLength,
	KaffeJNI_GetStringChars,
	KaffeJNI_ReleaseStringChars,
	KaffeJNI_NewStringUTF,
	KaffeJNI_GetStringUTFLength,
	KaffeJNI_GetStringUTFChars,
	KaffeJNI_ReleaseStringUTFChars,
	Kaffe_GetArrayLength,
	KaffeJNI_NewObjectArray,
	KaffeJNI_GetObjectArrayElement,
	KaffeJNI_SetObjectArrayElement,
	KaffeJNI_NewBooleanArray,
	KaffeJNI_NewByteArray,
	KaffeJNI_NewCharArray,
	KaffeJNI_NewShortArray,
	KaffeJNI_NewIntArray,
	KaffeJNI_NewLongArray,
	KaffeJNI_NewFloatArray,
	KaffeJNI_NewDoubleArray,
	KaffeJNI_GetBooleanArrayElements,
	KaffeJNI_GetByteArrayElements,
	KaffeJNI_GetCharArrayElements,
	KaffeJNI_GetShortArrayElements,
	KaffeJNI_GetIntArrayElements,
	KaffeJNI_GetLongArrayElements,
	KaffeJNI_GetFloatArrayElements,
	KaffeJNI_GetDoubleArrayElements,
	KaffeJNI_ReleaseBooleanArrayElements,
	KaffeJNI_ReleaseByteArrayElements,
	KaffeJNI_ReleaseCharArrayElements,
	KaffeJNI_ReleaseShortArrayElements,
	KaffeJNI_ReleaseIntArrayElements,
	KaffeJNI_ReleaseLongArrayElements,
	KaffeJNI_ReleaseFloatArrayElements,
	KaffeJNI_ReleaseDoubleArrayElements,
	KaffeJNI_GetBooleanArrayRegion,
	KaffeJNI_GetByteArrayRegion,
	KaffeJNI_GetCharArrayRegion,
	KaffeJNI_GetShortArrayRegion,
	KaffeJNI_GetIntArrayRegion,
	KaffeJNI_GetLongArrayRegion,
	KaffeJNI_GetFloatArrayRegion,
	KaffeJNI_GetDoubleArrayRegion,
	KaffeJNI_SetBooleanArrayRegion,
	KaffeJNI_SetByteArrayRegion,
	KaffeJNI_SetCharArrayRegion,
	KaffeJNI_SetShortArrayRegion,
	KaffeJNI_SetIntArrayRegion,
	KaffeJNI_SetLongArrayRegion,
	KaffeJNI_SetFloatArrayRegion,
	KaffeJNI_SetDoubleArrayRegion,
	Kaffe_RegisterNatives,
	Kaffe_UnregisterNatives,
	Kaffe_MonitorEnter,
	Kaffe_MonitorExit,
	Kaffe_GetJavaVM,
	KaffeJNI_GetStringRegion,
	KaffeJNI_GetStringUTFRegion,
	KaffeJNI_GetPrimitiveArrayCritical,
	KaffeJNI_ReleasePrimitiveArrayCritical,
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
	Kaffe_AttrachCurrentThreadAsDaemon
};

/*
 * Setup the Kaffe VM.
 */
JavaVM Kaffe_JavaVM = {
	&Kaffe_JNIInvokeInterface,
};

KaffeVM_Arguments Kaffe_JavaVMInitArgs = {
	0,		/* Version */
	0,		/* Properties */
	0,		/* Check source */
	THREADSTACKSIZE,/* Native stack size */
	0,		/* Java stack size */
	MIN_HEAPSIZE,	/* Min heap size */
	MAX_HEAPSIZE,	/* Max heap size */
	/*	2,	*/	/* Verify mode ... verify remote by default */
	0,		/* Verify mode ... noverify by default */
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
KaffeVM_Arguments Kaffe_JavaVMArgs;
