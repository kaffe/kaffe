/*
 * jni.c
 * Java Native Interface.
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * Copyright (c) 2004-2005
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
#include "stackTrace.h"

extern struct JNINativeInterface Kaffe_JNINativeInterface;
extern KaffeVM_Arguments Kaffe_JavaVMInitArgs;
static JavaVM Kaffe_JavaVM;

JavaVM*
KaffeJNI_GetKaffeVM(void)
{
  return &Kaffe_JavaVM;
}

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
KaffeJNI_FatalError(JNIEnv* env UNUSED, const char* mess)
{
	kprintf(stderr, "FATAL ERROR: %s\n", mess);
	abort();
}

static jint
Kaffe_GetVersion(JNIEnv* env UNUSED)
{
       return Kaffe_JavaVMArgs.version;
}

/*
 * take a VM error and throw it as JNI error 
 */
static void
postError(JNIEnv* env, errorInfo* info)
{
	(*env)->Throw(env, error2Throwable(info));
}

static jclass
Kaffe_DefineClass(JNIEnv* env, const char *name UNUSED, jobject loader,
		  const jbyte* buf, jsize len)
{
	Hjava_lang_Class* cls;
	classFile hand;
	errorInfo info;
	jobject loader_local;

	BEGIN_EXCEPTION_HANDLING(NULL);

	loader_local = unveil(loader); /* save clobbered reg.  */

	classFileInit(&hand, NULL, (unsigned char*)buf, (size_t)len,
		      CP_BYTEARRAY);

	cls = newClass();
	if (cls == 0) {
		postOutOfMemory(&info);
	} else {
		cls = readClass(cls, &hand, loader_local, &info);
	}
	if (cls == 0) {
		postError(env, &info);
	}

	END_EXCEPTION_HANDLING();
	return (cls);
}

static jclass
Kaffe_FindClass(JNIEnv *env UNUSED, const char* name)
{
	stackTraceInfo          *trace;
	Utf8Const* utf8;
	Hjava_lang_ClassLoader  *loader;
	Hjava_lang_Class	*clazz;
	errorInfo		einfo;
	int			i;
	char*			pathname;

	BEGIN_EXCEPTION_HANDLING(NULL);

	/* convert name to the form used inside the vm */
	pathname = checkPtr(KMALLOC(strlen (name) + 1));
	classname2pathname (name, pathname);

	/* create a new utf8 constant */
	utf8 = utf8ConstNew(pathname, -1);

	/* free the internal form of name */
	KFREE(pathname);

	/* bail out if we could not create the utf8 constant */
	if (utf8 == NULL)
	{
		postOutOfMemory (&einfo);
		throwError (&einfo);
	}

	/* Quote from the JNI documentation:
	 *
	 * "In the Java 2 Platform, FindClass locates the class loader associated with the current native method.
	 *  If the native code belongs to a system class, no class loader will be involved. Otherwise, the proper
	 *  class loader will be invoked to load and link the named class. When FindClass is called through the
	 *  Invocation Interface, there is no current native method or its associated class loader. In that case,
	 *  the result of ClassLoader.getBaseClassLoader is used."
	 *
	 * So we ...
	 */

	/* ... get the stacktrace ... */
	trace = (stackTraceInfo *)buildStackTrace (NULL);
	if (trace == NULL)
	{
		postOutOfMemory (&einfo);
		goto error_out;
	}

	/* ... find the first java method on the stack ... */
	for (i=0; trace[i].meth != ENDOFSTACK; i++)
	{
		if ((trace[i].meth != NULL) &&
		    (trace[i].meth->class != NULL))
			break;
	}

	/* ... determine the loader to be used ... */
	if (trace[i].meth == ENDOFSTACK) {
		jvalue retval;

		do_execute_java_class_method (&retval, "java/lang/ClassLoader",
					      NULL,
					      "getSystemClassLoader",
					      "()Ljava/lang/ClassLoader;");

		loader = (Hjava_lang_ClassLoader *)retval.l;
	} else {
		loader = trace[i].meth->class->loader;
	}

	/* ... and finally call loadArray or loadClass.
	 *
	 * Why don't we delegate to Class.forName? Our implementation of
	 * VMClassLoader.loadClass denies access to classes in internal
	 * packages like kaffe.lang or gnu.classpath (for security reasons).
	 * However, when FindClass is invoked from the native method of a
	 * class defined by the bootstrap loader, that is not correct.
	 * Therefore, we call loadClass / loadArray directly, which corresponds
	 * to what getClass() does. 
	 */
	if (utf8->data[0] == '[')
	{
		clazz = loadArray (utf8, loader, &einfo);
	}
	else
	{
		clazz = loadClass (utf8, loader, &einfo);
	}

	if (clazz == NULL)
	{
		goto error_out;
	}

	if (processClass (clazz, CSTATE_COMPLETE, &einfo) == false)
	{
		goto error_out;
	}

	ADD_REF(clazz);

	utf8ConstRelease(utf8);
	END_EXCEPTION_HANDLING();
	return (clazz);

error_out:
	utf8ConstRelease(utf8);
	throwError (&einfo);

	/* This is to make gcc silent on one warning */
	return NULL;
}

static jclass
Kaffe_GetSuperClass(JNIEnv* env UNUSED, jclass cls)
{
	jclass clz, cls_local;

	BEGIN_EXCEPTION_HANDLING(NULL);

	cls_local = unveil(cls); /* save clobbered reg.  */
	clz = ((Hjava_lang_Class*)cls_local)->superclass;

	END_EXCEPTION_HANDLING();
	return (clz);
}

static jboolean
Kaffe_IsAssignableFrom(JNIEnv* env UNUSED, jclass cls1, jclass cls2)
{
	jboolean r;
	jclass cls1_local;
	jclass cls2_local;

	BEGIN_EXCEPTION_HANDLING(0);

	cls1_local = unveil(cls1);
	cls2_local = unveil(cls2);

	if (instanceof(cls2_local, cls1_local) != 0) {
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
	jobject obj_local;
	BEGIN_EXCEPTION_HANDLING(0);

	if( obj )
	{
		obj_local = unveil(obj);

		assert(((Hjava_lang_Object *)obj_local)->vtable);

		thread_data->exceptObj =
		  (struct Hjava_lang_Throwable*)obj_local;
	}

	END_EXCEPTION_HANDLING();
	return (0);
}

static jint
Kaffe_ThrowNew(JNIEnv* env UNUSED, jclass cls, const char* mess)
{
	Hjava_lang_Object* eobj;
	jclass cls_local;

	BEGIN_EXCEPTION_HANDLING(0);

	cls_local = unveil(cls);
	eobj = execute_java_constructor(NULL, NULL, cls_local,
					"(Ljava/lang/String;)V",
					checkPtr(stringC2Java(mess)));

	thread_data->exceptObj = (struct Hjava_lang_Throwable*)eobj;

	END_EXCEPTION_HANDLING();
	return (0);
}

static jobject
Kaffe_ExceptionOccurred(JNIEnv* env UNUSED)
{
	jobject obj;

	BEGIN_EXCEPTION_HANDLING(NULL);

	obj = thread_data->exceptObj;

	if (obj != NULL)
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
	const char* cname;
	Hjava_lang_Class* class;
	Hjava_lang_Throwable *eobj;

	BEGIN_EXCEPTION_HANDLING_VOID();

	eobj = thread_data->exceptObj;

	while (eobj != NULL) {
		/* Don't use the java stack printer because the exception
		 * may arise in the IO codec.
		 */
	       Hjava_lang_String *msg;
	       char *realname;

	       class = OBJECT_CLASS(&eobj->base);
	       cname = CLASS_CNAME(class);
	       realname = KMALLOC(strlen(cname)+1);
	       pathname2classname(cname, realname);
			       
	       msg = unhand(eobj)->detailMessage;
	       if (msg != NULL) {
		       char *cmsg = checkPtr(stringJava2C(msg));

		       kprintf(stderr, "%s: %s\n", realname, cmsg);
		       KFREE(cmsg);
		       unhand(eobj)->detailMessage = NULL;
	       } else {
		       kprintf(stderr, "%s\n", realname);
	       }
       	       KFREE(realname);
	       
	       printStackTrace (eobj, NULL, true);

	       if (eobj->cause != eobj) {
		       eobj = eobj->cause;
		       kprintf(stderr, "caused by: ");
	       } else
		       eobj = NULL;
	}
	END_EXCEPTION_HANDLING();
}

static void
Kaffe_ExceptionClear(JNIEnv* env UNUSED)
{
	BEGIN_EXCEPTION_HANDLING_VOID();

	thread_data->exceptObj = NULL;

	END_EXCEPTION_HANDLING();
}

static jobject
Kaffe_AllocObject(JNIEnv* env UNUSED, jclass cls)
{
	jobject obj;
	Hjava_lang_Class* clazz;
	jclass cls_local;

	BEGIN_EXCEPTION_HANDLING(NULL);

	cls_local = unveil(cls);
	clazz = (Hjava_lang_Class*)cls_local;

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
	jclass cls_local;

	BEGIN_EXCEPTION_HANDLING(NULL);

	cls_local = unveil(cls);
	clazz = (Hjava_lang_Class*)cls_local;

	if (CLASS_IS_INTERFACE(clazz) || CLASS_IS_ABSTRACT(clazz) || !METHOD_IS_CONSTRUCTOR(m)) {
		throwException(InstantiationException(clazz->name->data));
	}
	obj = newObject(clazz);

	KaffeVM_callMethodV(m, METHOD_NATIVECODE(m), obj, args, &retval);

	ADD_REF(obj);
	END_EXCEPTION_HANDLING();
	return (obj);
}

static jobject
Kaffe_NewObject(JNIEnv* env UNUSED, jclass cls, jmethodID meth, ...)
{
	jobject obj;
	va_list args;
	jclass cls_local;

	BEGIN_EXCEPTION_HANDLING(NULL);

	cls_local = unveil(cls);

	va_start(args, meth);
	obj = Kaffe_NewObjectV(env, cls_local, meth, args);
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
	jclass cls_local;

	BEGIN_EXCEPTION_HANDLING(NULL);

	cls_local = unveil(cls);
	clazz = (Hjava_lang_Class*)cls_local;

	if (CLASS_IS_INTERFACE(clazz) || CLASS_IS_ABSTRACT(clazz) || !METHOD_IS_CONSTRUCTOR(m)) {
		throwException(InstantiationException(clazz->name->data));
	}
	obj = newObject(clazz);

	KaffeVM_callMethodA(m, METHOD_NATIVECODE(m), obj, args, &retval, 0);

	ADD_REF(obj);
	END_EXCEPTION_HANDLING();
	return (obj);
}

static jclass
Kaffe_GetObjectClass(JNIEnv* env UNUSED, jobject obj)
{
	jclass cls;
	jobject obj_local;

	BEGIN_EXCEPTION_HANDLING(NULL);

	obj_local = unveil(obj);
	cls = ((Hjava_lang_Object*)obj_local)->vtable->class;

	END_EXCEPTION_HANDLING();
	return (cls);
}

static jboolean
Kaffe_IsInstanceOf(JNIEnv* env UNUSED, jobject obj, jclass cls)
{
	jboolean r;
	jobject obj_local;
	jclass cls_local;

	BEGIN_EXCEPTION_HANDLING(0);

	obj_local = unveil(obj);
	cls_local = unveil(cls);
	if (soft_instanceof((Hjava_lang_Class*)cls_local,
			    (Hjava_lang_Object*)obj_local) != 0) {
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
	jclass cls_local;

	BEGIN_EXCEPTION_HANDLING(NULL);
	cls_local = unveil(cls);

	meth = lookupClassMethod((Hjava_lang_Class*)cls_local, name, sig,
				 &info);
	if (meth == NULL) {
		postError(env, &info);
	}
	else if (METHOD_IS_STATIC(meth)) {
		postExceptionMessage(&info, JAVA_LANG(NoSuchMethodError),
				     "%s", name);
		postError(env, &info);
		meth = NULL;
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
	jclass cls_local;

	BEGIN_EXCEPTION_HANDLING(NULL);

	cls_local = unveil(cls);
	utf8 = checkPtr(utf8ConstNew(name, -1));
	fld = lookupClassField((Hjava_lang_Class*)cls_local, utf8, false,
			       &info);
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
	jclass cls_local;

	BEGIN_EXCEPTION_HANDLING(NULL);

	cls_local = unveil(cls);
	meth = lookupClassMethod((Hjava_lang_Class*)cls_local, name, sig,
				 &info);
	if (meth == NULL) {
		postError(env, &info);
	} else if (!METHOD_IS_STATIC(meth)) {
		postExceptionMessage(&info, JAVA_LANG(NoSuchMethodError), "%s", name);
		postError(env, &info);
		meth = NULL;
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
	jclass cls_local;

	BEGIN_EXCEPTION_HANDLING(NULL);

	cls_local = unveil(cls);
	utf8 = checkPtr(utf8ConstNew(name, -1));
	fld = lookupClassField((Hjava_lang_Class*)cls_local, utf8, true, &info);
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
	jarray arr_local;
	BEGIN_EXCEPTION_HANDLING(0);

	arr_local = unveil(arr);
	len = obj_length((HArrayOfObject*)arr_local);

	END_EXCEPTION_HANDLING();
	return (len);
}

static jint
Kaffe_RegisterNatives(JNIEnv* env UNUSED, jclass cls, const JNINativeMethod* methodArray, jint nmethods)
{
	Method* meth;
	int nmeth;
	int i, j;
	jclass cls_local;

	BEGIN_EXCEPTION_HANDLING(0);

	cls_local = unveil(cls);

	meth = CLASS_METHODS((Hjava_lang_Class*)cls_local);
	nmeth = CLASS_NMETHODS((Hjava_lang_Class*)cls_local);

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
	jobject obj_local;

	BEGIN_EXCEPTION_HANDLING(0);

	/* We should never throw out of a JNI call */
	obj_local = unveil(obj);
	lockObject(obj_local);

	END_EXCEPTION_HANDLING();
	return (0);
}

static jint
Kaffe_MonitorExit(JNIEnv* env UNUSED, jobject obj)
{
	jobject obj_local;

	BEGIN_EXCEPTION_HANDLING(0);

	obj_local = unveil(obj);
	unlockObject(obj_local);

	END_EXCEPTION_HANDLING();
	return (0);
}

/*
 * Functions past this point don't bother with jni exceptions.
 */

JavaVM*
Kaffe_GetKaffeVM(void)
{
  return &Kaffe_JavaVM;
}

static jint
Kaffe_GetJavaVM(JNIEnv* env UNUSED, JavaVM** vm)
{
	(*vm) = &Kaffe_JavaVM;
	return (0);
}

static jint
Kaffe_AttachCurrentThread(JavaVM* vm UNUSED, void** penv, void* args UNUSED)
{
	if (KTHREAD(attach_current_thread) (false)) {
		KSEM(init)(&THREAD_DATA()->sem);
		KaffeVM_attachFakedThreadInstance ("test attach", false);
		*penv = THREAD_JNIENV();
		return 0;
	}
	return -1;
}

static jint
Kaffe_AttachCurrentThreadAsDaemon(JavaVM* vm UNUSED, void** penv, void* args UNUSED)
{
	if (KTHREAD(attach_current_thread) (true)) {
		KSEM(init)(&THREAD_DATA()->sem);
		KaffeVM_attachFakedThreadInstance ("daemon attach", true);
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
	if (!KaffeJNI_IsSameObject(je, currentVM, vm))
		return (JNI_EDETACHED);

	/* Is the requested version of the interface known? */
	switch (interface_id) {
	case JNI_VERSION_1_1:
		(*penv) = je;
		return (JNI_OK);
	case JNI_VERSION_1_2:
		(*penv) = je;
		return (JNI_OK);
	case JNI_VERSION_1_4:
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
	case JVMPI_VERSION_1_2:
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
	KaffeJNI_FatalError,
	KaffeJNI_PushLocalFrame,
	KaffeJNI_PopLocalFrame,
	KaffeJNI_NewGlobalRef,
	KaffeJNI_DeleteGlobalRef,
	KaffeJNI_DeleteLocalRef,
	KaffeJNI_IsSameObject,
	KaffeJNI_NewLocalRef,
	KaffeJNI_EnsureLocalCapacity,
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
	KaffeJNI_GetStringChars,
	KaffeJNI_ReleaseStringChars,
	KaffeJNI_NewWeakGlobalRef, //
	KaffeJNI_DeleteWeakGlobalRef, //
	Kaffe_ExceptionCheck,
	KaffeJNI_NewDirectByteBuffer,
	KaffeJNI_GetDirectBufferAddress,
	KaffeJNI_GetDirectBufferCapacity

};

/*
 * Setup the Kaffe invoke interface.
 */
const struct JNIInvokeInterface Kaffe_JNIInvokeInterface = {
	NULL,
	NULL,
	NULL,
	KaffeJNI_DestroyJavaVM,
	Kaffe_AttachCurrentThread,
	Kaffe_DetachCurrentThread,
	Kaffe_GetEnv,
	Kaffe_AttachCurrentThreadAsDaemon
};

/*
 * Setup the Kaffe VM.
 */
static JavaVM Kaffe_JavaVM = {
	&Kaffe_JNIInvokeInterface,
};

KaffeVM_Arguments Kaffe_JavaVMInitArgs = {
	0,		/* Version */
	NULL,		/* Properties */
	0,		/* Check source */
	THREADSTACKSIZE,/* Native stack size */
	0,		/* Java stack size */
	MIN_HEAPSIZE,	/* Min heap size */
	MAX_HEAPSIZE,	/* Max heap size */
	/*	2,	*/	/* Verify mode ... verify remote by default */
	0,		/* Verify mode ... noverify by default */
	".",		/* Classpath */
	NULL,		/* Bootclasspath */
	(void*)&vfprintf,/* Vprintf */
	&exit,	/* Exit */
	&abort,	/* Abort */
	1,		/* Enable class GC */
	0,		/* Enable verbose GC */
	1,		/* Disable async GC */
	0,		/* Enable verbose class loading */
	0,		/* Enable verbose JIT */
	0,		/* Enable verbose calls */
	ALLOC_HEAPSIZE,	/* Inc heap size */
	NULL,		/* Class home */
	NULL,		/* Library home */
	NULL,           /* No profiler */
	NULL            /* No arguments to profiler */
};

/*
 * Array of VMs.
 */
KaffeVM_Arguments Kaffe_JavaVMArgs;
