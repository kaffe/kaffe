/*
 * jni.c
 * Java Native Interface.
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#if 0
#define	NEED_JNIREFS	/* Define to mange local JNI refs */
#endif

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
#include "mem/gc-mem.h"
#include "locks.h"
#include "md.h"
#include "exception.h"
#if defined(TRANSLATOR)
#include "seq.h"
#include "slots.h"
#include "labels.h"
#include "codeproto.h"
#include "basecode.h"
#include "icode.h"
#include "machine.h"
#endif

/*
 * Define the version of JNI we support.
 */
static int java_major_version = 1;
static int java_minor_version = 1;

/*
 * Keep track of how many VM's are active. Right now
 * we only support one at a time.
 */
static int Kaffe_NumVM = 0;

/*
 * If we must manage the JNI references for the native layer then we
 * add extra functions to the JNI calls and returns to manage the
 * referencing.
 */
#if defined(NEED_JNIREFS)
static void addJNIref(jref);
static void removeJNIref(jref);
#define	ADD_REF(O)		addJNIref(O)
#define	REMOVE_REF(O)		removeJNIref(O)
#else
#define	ADD_REF(O)
#define	REMOVE_REF(O)
#endif

/*
 * Define how we get the method to call.
 */
#define	JNI_METHOD_CODE(M)	METHOD_INDIRECTMETHOD(M)

/*
 * Define how we handle exceptions in JNI.
 */
#define	BEGIN_EXCEPTION_HANDLING(X)			\
	vmException ebuf;				\
	ebuf.prev = (vmException*)unhand(getCurrentThread())->exceptPtr;\
	ebuf.meth = (Method*)1;				\
	if (JTHREAD_SETJMP(ebuf.jbuf) != 0) {		\
		unhand(getCurrentThread())->exceptPtr = \
		  (struct Hkaffe_util_Ptr*)ebuf.prev;	\
		return X;				\
	}						\
	unhand(getCurrentThread())->exceptPtr = (struct Hkaffe_util_Ptr*)&ebuf

#define	BEGIN_EXCEPTION_HANDLING_VOID()			\
	vmException ebuf;				\
	ebuf.prev = (vmException*)unhand(getCurrentThread())->exceptPtr;\
	ebuf.meth = (Method*)1;				\
	if (JTHREAD_SETJMP(ebuf.jbuf) != 0) {		\
		unhand(getCurrentThread())->exceptPtr = \
		  (struct Hkaffe_util_Ptr*)ebuf.prev;	\
		return;					\
	}						\
	unhand(getCurrentThread())->exceptPtr = (struct Hkaffe_util_Ptr*)&ebuf

#define	END_EXCEPTION_HANDLING()			\
	unhand(getCurrentThread())->exceptPtr = (struct Hkaffe_util_Ptr*)ebuf.prev

/*
 * Get and set fields.
 */
#define	GET_FIELD(T,O,F)	*(T*)((char*)(O) + FIELD_BOFFSET((Field*)(F)))
#define	SET_FIELD(T,O,F,V)	*(T*)((char*)(O) + FIELD_BOFFSET((Field*)(F))) = (V)
#define	GET_STATIC_FIELD(T,F)	*(T*)FIELD_ADDRESS((Field*)F)
#define	SET_STATIC_FIELD(T,F,V)	*(T*)FIELD_ADDRESS((Field*)F) = (V)

uintp Kaffe_JNI_estart;
uintp Kaffe_JNI_eend;

extern struct JNINativeInterface Kaffe_JNINativeInterface;
extern JavaVMInitArgs Kaffe_JavaVMInitArgs;
extern JavaVM Kaffe_JavaVM;
extern struct JNIEnv_ Kaffe_JNIEnv;

static void Kaffe_JNI_wrapper(Method*, void*);
#if defined(TRANSLATOR)
static void startJNIcall(void);
static void finishJNIcall(void);
static void Kaffe_wrapper(Method* xmeth, void* func, bool use_JNI);
#endif

void Kaffe_JNIExceptionHandler(void);
jint Kaffe_GetVersion(JNIEnv*);
jclass Kaffe_FindClass(JNIEnv*, const char*);
jint Kaffe_ThrowNew(JNIEnv*, jclass, const char*);
jint Kaffe_Throw(JNIEnv* env, jobject obj);

jint
JNI_GetDefaultJavaVMInitArgs(JavaVMInitArgs* args)
{
	if (args->version != ((java_major_version << 16) | java_minor_version)) {
		return (-1);
	}
	memcpy(args, &Kaffe_JavaVMInitArgs, sizeof(JavaVMInitArgs));
	args->version = (java_major_version << 16) | java_minor_version;
	return (0);
}

jint
JNI_CreateJavaVM(JavaVM** vm, JNIEnv** env, JavaVMInitArgs* args)
{
	if (args->version != ((java_major_version << 16) | java_minor_version)) {
		return (-1);
	}

	/* We can only init. one KVM */
	if (Kaffe_NumVM != 0) {
		return (-1);
	}

	/* Setup the machine */
	Kaffe_JavaVMArgs[0] = *args;
	initialiseKaffe();

	/* Setup the JNI Exception handler */
	Kaffe_JNI_estart = (uintp)&Kaffe_GetVersion; /* First routine */
	Kaffe_JNI_eend = (uintp)&Kaffe_JNIExceptionHandler; /* Last routine */

	/* Setup JNI for main thread */
#if defined(NEED_JNIREFS)
	unhand(getCurrentThread())->jnireferences = gc_malloc(sizeof(jnirefs), &gcNormal);
#endif

	/* Return the VM and JNI we're using */
	*vm = &Kaffe_JavaVM;
	*env = (JNIEnv*)&Kaffe_JNIEnv;
	Kaffe_NumVM++;
	return (0);
}

jint
JNI_GetCreatedJavaVMs(JavaVM** vm, jsize buflen, jsize* nvm)
{
	vm[0] = &Kaffe_JavaVM;
	*nvm = Kaffe_NumVM;

	return (0);
}

/*
 * Everything from this point to Kaffe_GetVersion is not
 * exception-aware.  Asynchronous exceptions should not be delivered
 * to them.
 *
 * Everything from Kaffe_GetVersion to Kaffe_JNIExceptionHandler
 * should be bracketed with BEGIN and END _EXCEPTION_HANDLING.
 * Question: what happens when an asynchronous exception occurs at the
 * very start or end of one of these calls.
 */
void
Kaffe_FatalError(JNIEnv* env, const char* mess)
{
	kprintf(stderr, "FATAL ERROR: %s\n", mess);
	exit(1);
}

void
Kaffe_DeleteGlobalRef(JNIEnv* env, jref obj)
{
	gc_rm_ref(obj);
}

void
Kaffe_DeleteLocalRef(JNIEnv* env, jref obj)
{
	REMOVE_REF(obj);
}

jboolean
Kaffe_IsSameObject(JNIEnv* env, jobject obj1, jobject obj2)
{
	if (obj1 == obj2) {
		return (JNI_TRUE);
	}
	else {
		return (JNI_FALSE);
	}
}

void
Kaffe_ReleaseStringChars(JNIEnv* env, jstring data, const jchar* chars)
{
	/* Does nothing */
}

jint
Kaffe_GetVersion(JNIEnv* env)
{
	return ((java_major_version << 16) | java_minor_version);
}

/*
 * take a VM error and throw it as JNI error 
 */
static void
postError(JNIEnv* env, errorInfo* info)
{
	Kaffe_Throw(env, error2Throwable(info));
}

jref
Kaffe_NewGlobalRef(JNIEnv* env, jref obj)
{
	BEGIN_EXCEPTION_HANDLING(0);
	if (!gc_add_ref(obj)) {
		errorInfo info;
		postOutOfMemory(&info);
		postError(env, &info);
	}
	END_EXCEPTION_HANDLING();
	return obj;
}

jclass
Kaffe_DefineClass(JNIEnv* env, jobject loader, const jbyte* buf, jsize len)
{
	Hjava_lang_Class* cls;
	classFile hand;
	errorInfo info;

	BEGIN_EXCEPTION_HANDLING(0);
	hand.base = (void*)buf;
	hand.buf = hand.base;
	hand.size = len;

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

jclass
Kaffe_FindClass(JNIEnv* env, const char* name)
{
	Hjava_lang_Class* cls;
	char *buf;
	errorInfo info;

	BEGIN_EXCEPTION_HANDLING(0);

	buf = checkPtr(KMALLOC(strlen(name) + 1));
	classname2pathname((char*)name, buf);

	if (buf[0] == '[') {
		cls = getClassFromSignature(&buf[1], NULL, &info);
		if (cls != 0) {
			cls = lookupArray(cls, &info);
		}
	}
	else {
		cls = lookupClass(buf, &info);
	}
	KFREE(buf);

	if (cls == 0) {
		postError(env, &info);
	}
	END_EXCEPTION_HANDLING();
	return (cls);
}

jclass
Kaffe_GetSuperClass(JNIEnv* env, jclass cls)
{
	jclass ret;

	BEGIN_EXCEPTION_HANDLING(0);

	ret = ((Hjava_lang_Class*)cls)->superclass;

	END_EXCEPTION_HANDLING();
	return (ret);
}

jbool
Kaffe_IsAssignableFrom(JNIEnv* env, jclass cls1, jclass cls2)
{
	jbool ret;

	BEGIN_EXCEPTION_HANDLING(0);

	if (instanceof(cls2, cls1) != 0) {
		ret = JNI_TRUE;
	}
	else {
		ret = JNI_FALSE;
	}
	END_EXCEPTION_HANDLING();
	return (ret);
}

jint
Kaffe_Throw(JNIEnv* env, jobject obj)
{
	BEGIN_EXCEPTION_HANDLING(0);

	unhand(getCurrentThread())->exceptObj = (struct Hjava_lang_Throwable*)obj;

	END_EXCEPTION_HANDLING();
	return (0);
}

jint
Kaffe_ThrowNew(JNIEnv* env, jclass cls, const char* mess)
{
	Hjava_lang_Object* eobj;

	BEGIN_EXCEPTION_HANDLING(0);

	eobj = execute_java_constructor(NULL, cls,
					"(Ljava/lang/String;)V",
					checkPtr(stringC2Java((char*)mess)));

	unhand(getCurrentThread())->exceptObj = (struct Hjava_lang_Throwable*)eobj;

	END_EXCEPTION_HANDLING();
	return (0);
}

jobject
Kaffe_ExceptionOccured(JNIEnv* env)
{
	jobject obj;

	BEGIN_EXCEPTION_HANDLING(0);

	obj = unhand(getCurrentThread())->exceptObj;

	ADD_REF(obj);
	END_EXCEPTION_HANDLING();
	return (obj);
}

void
Kaffe_ExceptionDescribe(JNIEnv* env)
{
	BEGIN_EXCEPTION_HANDLING_VOID();

	if (unhand(getCurrentThread())->exceptObj != 0) {
		do_execute_java_method(unhand(getCurrentThread())->exceptObj, "printStackTrace", "()V", 0, 0, unhand(getCurrentThread())->exceptObj); 
	}
	END_EXCEPTION_HANDLING();
}

void
Kaffe_ExceptionClear(JNIEnv* env)
{
	BEGIN_EXCEPTION_HANDLING_VOID();

	unhand(getCurrentThread())->exceptObj = 0;

	END_EXCEPTION_HANDLING();
}

jobject
Kaffe_AllocObject(JNIEnv* env, jclass cls)
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

jobject
Kaffe_NewObjectV(JNIEnv* env, jclass cls, jmethodID meth, va_list args)
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

	callMethodV(m, JNI_METHOD_CODE(m), obj, args, &retval);

	ADD_REF(obj);
	END_EXCEPTION_HANDLING();
	return (obj);
}

jobject
Kaffe_NewObject(JNIEnv* env, jclass cls, jmethodID meth, ...)
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

jobject
Kaffe_NewObjectA(JNIEnv* env, jclass cls, jmethodID meth, jvalue* args)
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

	callMethodA(m, JNI_METHOD_CODE(m), obj, args, &retval, 0);

	ADD_REF(obj);
	END_EXCEPTION_HANDLING();
	return (obj);
}

jclass
Kaffe_GetObjectClass(JNIEnv* env, jobject obj)
{
	jclass cls;

	BEGIN_EXCEPTION_HANDLING(0);

	cls = ((Hjava_lang_Object*)obj)->dtable->class;

	END_EXCEPTION_HANDLING();
	return (cls);
}

jbool
Kaffe_IsInstanceOf(JNIEnv* env, jobject obj, jclass cls)
{
	jbool ret;

	BEGIN_EXCEPTION_HANDLING(0);

	if (soft_instanceof((Hjava_lang_Class*)cls, (Hjava_lang_Object*)obj) != 0) {
		ret = JNI_TRUE;
	}
	else {
		ret = JNI_FALSE;
	}

	END_EXCEPTION_HANDLING();
	return (ret);
}

jmethodID
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
		postExceptionMessage(&info, JAVA_LANG(NoSuchMethodError), name);
		postError(env, &info);
		meth = 0;
	}
	END_EXCEPTION_HANDLING();
	
	return (meth);
}

jobject
Kaffe_CallObjectMethodV(JNIEnv* env, jobject obj, jmethodID meth, va_list args)
{
	jvalue retval;
	Hjava_lang_Object* o = (Hjava_lang_Object*)obj;
	Method* m = (Method*)meth;

	BEGIN_EXCEPTION_HANDLING(0);

	if (METHOD_IS_STATIC(m)) {
		throwException(NoSuchMethodError(m->name->data));
	}

	callMethodV(m, (m->idx >= 0 ? o->dtable->method[m->idx]
			: JNI_METHOD_CODE(m)), obj, args, &retval);

	ADD_REF(retval.l);
	END_EXCEPTION_HANDLING();
	return (retval.l);
}

jobject
Kaffe_CallObjectMethod(JNIEnv* env, jobject obj, jmethodID meth, ...)
{
	va_list args;
	jobject ret;

	BEGIN_EXCEPTION_HANDLING(0);

	va_start(args, meth);
	ret = Kaffe_CallObjectMethodV(env, obj, meth, args);
	va_end(args);

	END_EXCEPTION_HANDLING();
	return (ret);
}

jobject
Kaffe_CallObjectMethodA(JNIEnv* env, jobject obj, jmethodID meth, jvalue* args)
{
	jvalue retval;
	Hjava_lang_Object* o = (Hjava_lang_Object*)obj;
	Method* m = (Method*)meth;

	BEGIN_EXCEPTION_HANDLING(0);

	if (METHOD_IS_STATIC(m)) {
		throwException(NoSuchMethodError(m->name->data));
	}

	callMethodA(m, (m->idx >= 0 ? o->dtable->method[m->idx]
			: JNI_METHOD_CODE(m)), obj, args, &retval, 0);

	END_EXCEPTION_HANDLING();
	return (retval.l);
}

jboolean
Kaffe_CallBooleanMethodV(JNIEnv* env, jobject obj, jmethodID meth, va_list args)
{
	jvalue retval;
	Hjava_lang_Object* o = (Hjava_lang_Object*)obj;
	Method* m = (Method*)meth;

	BEGIN_EXCEPTION_HANDLING(0);

	if (METHOD_IS_STATIC(m)) {
		throwException(NoSuchMethodError(m->name->data));
	}

	callMethodV(m, (m->idx >= 0 ? o->dtable->method[m->idx]
			: JNI_METHOD_CODE(m)), obj, args, &retval);

	END_EXCEPTION_HANDLING();
	return ((jboolean) retval.i);
}

jboolean
Kaffe_CallBooleanMethod(JNIEnv* env, jobject obj, jmethodID meth, ...)
{
	va_list args;
	jboolean ret;

	BEGIN_EXCEPTION_HANDLING(0);

	va_start(args, meth);
	ret = Kaffe_CallBooleanMethodV(env, obj, meth, args);
	va_end(args);

	END_EXCEPTION_HANDLING();
	return (ret);
}

jboolean
Kaffe_CallBooleanMethodA(JNIEnv* env, jobject obj, jmethodID meth, jvalue* args)
{
	jvalue retval;
	Hjava_lang_Object* o = (Hjava_lang_Object*)obj;
	Method* m = (Method*)meth;

	BEGIN_EXCEPTION_HANDLING(0);

	if (METHOD_IS_STATIC(m)) {
		throwException(NoSuchMethodError(m->name->data));
	}

	callMethodA(m, (m->idx >= 0 ? o->dtable->method[m->idx]
			: JNI_METHOD_CODE(m)), obj, args, &retval, 0);

	END_EXCEPTION_HANDLING();
	return ((jboolean) retval.i);
}

jbyte
Kaffe_CallByteMethodV(JNIEnv* env, jobject obj, jmethodID meth, va_list args)
{
	jvalue retval;
	Hjava_lang_Object* o = (Hjava_lang_Object*)obj;
	Method* m = (Method*)meth;

	BEGIN_EXCEPTION_HANDLING(0);

	if (METHOD_IS_STATIC(m)) {
		throwException(NoSuchMethodError(m->name->data));
	}

	callMethodV(m, (m->idx >= 0 ? o->dtable->method[m->idx]
			: JNI_METHOD_CODE(m)), obj, args, &retval);

	END_EXCEPTION_HANDLING();
	return ((jbyte) retval.i);
}

jbyte
Kaffe_CallByteMethod(JNIEnv* env, jobject obj, jmethodID meth, ...)
{
	va_list args;
	jbyte ret;

	BEGIN_EXCEPTION_HANDLING(0);

	va_start(args, meth);
	ret = Kaffe_CallByteMethodV(env, obj, meth, args);
	va_end(args);

	END_EXCEPTION_HANDLING();
	return (ret);
}

jbyte
Kaffe_CallByteMethodA(JNIEnv* env, jobject obj, jmethodID meth, jvalue* args)
{
	jvalue retval;
	Hjava_lang_Object* o = (Hjava_lang_Object*)obj;
	Method* m = (Method*)meth;

	BEGIN_EXCEPTION_HANDLING(0);

	if (METHOD_IS_STATIC(m)) {
		throwException(NoSuchMethodError(m->name->data));
	}

	callMethodA(m, (m->idx >= 0 ? o->dtable->method[m->idx]
			: JNI_METHOD_CODE(m)), obj, args, &retval, 0);

	END_EXCEPTION_HANDLING();
	return ((jbyte) retval.i);
}

jchar
Kaffe_CallCharMethodV(JNIEnv* env, jobject obj, jmethodID meth, va_list args)
{
	jvalue retval;
	Hjava_lang_Object* o = (Hjava_lang_Object*)obj;
	Method* m = (Method*)meth;

	BEGIN_EXCEPTION_HANDLING(0);

	if (METHOD_IS_STATIC(m)) {
		throwException(NoSuchMethodError(m->name->data));
	}

	callMethodV(m, (m->idx >= 0 ? o->dtable->method[m->idx]
			: JNI_METHOD_CODE(m)), obj, args, &retval);

	END_EXCEPTION_HANDLING();
	return ((jchar) retval.i);
}

jchar
Kaffe_CallCharMethod(JNIEnv* env, jobject obj, jmethodID meth, ...)
{
	va_list args;
	jchar ret;

	BEGIN_EXCEPTION_HANDLING(0);

	va_start(args, meth);
	ret = Kaffe_CallCharMethodV(env, obj, meth, args);
	va_end(args);

	END_EXCEPTION_HANDLING();
	return (ret);
}

jchar
Kaffe_CallCharMethodA(JNIEnv* env, jobject obj, jmethodID meth, jvalue* args)
{
	jvalue retval;
	Hjava_lang_Object* o = (Hjava_lang_Object*)obj;
	Method* m = (Method*)meth;

	BEGIN_EXCEPTION_HANDLING(0);

	if (METHOD_IS_STATIC(m)) {
		throwException(NoSuchMethodError(m->name->data));
	}

	callMethodA(m, (m->idx >= 0 ? o->dtable->method[m->idx]
			: JNI_METHOD_CODE(m)), obj, args, &retval, 0);

	END_EXCEPTION_HANDLING();
	return ((jchar) retval.i);
}

jshort
Kaffe_CallShortMethodV(JNIEnv* env, jobject obj, jmethodID meth, va_list args)
{
	jvalue retval;
	Hjava_lang_Object* o = (Hjava_lang_Object*)obj;
	Method* m = (Method*)meth;

	BEGIN_EXCEPTION_HANDLING(0);

	if (METHOD_IS_STATIC(m)) {
		throwException(NoSuchMethodError(m->name->data));
	}

	callMethodV(m, (m->idx >= 0 ? o->dtable->method[m->idx]
			: JNI_METHOD_CODE(m)), obj, args, &retval);

	END_EXCEPTION_HANDLING();
	return ((jshort) retval.i);
}

jshort
Kaffe_CallShortMethod(JNIEnv* env, jobject obj, jmethodID meth, ...)
{
	va_list args;
	jshort ret;

	BEGIN_EXCEPTION_HANDLING(0);

	va_start(args, meth);
	ret = Kaffe_CallShortMethodV(env, obj, meth, args);
	va_end(args);

	END_EXCEPTION_HANDLING();
	return (ret);
}

jshort
Kaffe_CallShortMethodA(JNIEnv* env, jobject obj, jmethodID meth, jvalue* args)
{
	jvalue retval;
	Hjava_lang_Object* o = (Hjava_lang_Object*)obj;
	Method* m = (Method*)meth;

	BEGIN_EXCEPTION_HANDLING(0);

	if (METHOD_IS_STATIC(m)) {
		throwException(NoSuchMethodError(m->name->data));
	}

	callMethodA(m, (m->idx >= 0 ? o->dtable->method[m->idx]
			: JNI_METHOD_CODE(m)), obj, args, &retval, 0);

	END_EXCEPTION_HANDLING();
	return ((jshort) retval.i);
}

jint
Kaffe_CallIntMethodV(JNIEnv* env, jobject obj, jmethodID meth, va_list args)
{
	jvalue retval;
	Hjava_lang_Object* o = (Hjava_lang_Object*)obj;
	Method* m = (Method*)meth;

	BEGIN_EXCEPTION_HANDLING(0);

	if (METHOD_IS_STATIC(m)) {
		throwException(NoSuchMethodError(m->name->data));
	}

	callMethodV(m, (m->idx >= 0 ? o->dtable->method[m->idx]
			: JNI_METHOD_CODE(m)), obj, args, &retval);

	END_EXCEPTION_HANDLING();
	return (retval.i);
}

jint
Kaffe_CallIntMethod(JNIEnv* env, jobject obj, jmethodID meth, ...)
{
	va_list args;
	jint ret;

	BEGIN_EXCEPTION_HANDLING(0);

	va_start(args, meth);
	ret = Kaffe_CallIntMethodV(env, obj, meth, args);
	va_end(args);

	END_EXCEPTION_HANDLING();
	return (ret);
}

jint
Kaffe_CallIntMethodA(JNIEnv* env, jobject obj, jmethodID meth, jvalue* args)
{
	jvalue retval;
	Hjava_lang_Object* o = (Hjava_lang_Object*)obj;
	Method* m = (Method*)meth;

	BEGIN_EXCEPTION_HANDLING(0);

	if (METHOD_IS_STATIC(m)) {
		throwException(NoSuchMethodError(m->name->data));
	}

	callMethodA(m, (m->idx >= 0 ? o->dtable->method[m->idx]
			: JNI_METHOD_CODE(m)), obj, args, &retval, 0);

	END_EXCEPTION_HANDLING();
	return (retval.i);
}

jlong
Kaffe_CallLongMethodV(JNIEnv* env, jobject obj, jmethodID meth, va_list args)
{
	jvalue retval;
	Hjava_lang_Object* o = (Hjava_lang_Object*)obj;
	Method* m = (Method*)meth;

	BEGIN_EXCEPTION_HANDLING(0);

	if (METHOD_IS_STATIC(m)) {
		throwException(NoSuchMethodError(m->name->data));
	}

	callMethodV(m, (m->idx >= 0 ? o->dtable->method[m->idx]
			: JNI_METHOD_CODE(m)), obj, args, &retval);

	END_EXCEPTION_HANDLING();
	return (retval.j);
}

jlong
Kaffe_CallLongMethod(JNIEnv* env, jobject obj, jmethodID meth, ...)
{
	va_list args;
	jlong ret;

	BEGIN_EXCEPTION_HANDLING(0);

	va_start(args, meth);
	ret = Kaffe_CallLongMethodV(env, obj, meth, args);
	va_end(args);

	END_EXCEPTION_HANDLING();
	return (ret);
}

jlong
Kaffe_CallLongMethodA(JNIEnv* env, jobject obj, jmethodID meth, jvalue* args)
{
	jvalue retval;
	Hjava_lang_Object* o = (Hjava_lang_Object*)obj;
	Method* m = (Method*)meth;

	BEGIN_EXCEPTION_HANDLING(0);

	if (METHOD_IS_STATIC(m)) {
		throwException(NoSuchMethodError(m->name->data));
	}

	callMethodA(m, (m->idx >= 0 ? o->dtable->method[m->idx]
			: JNI_METHOD_CODE(m)), obj, args, &retval, 0);

	END_EXCEPTION_HANDLING();
	return (retval.j);
}

jfloat
Kaffe_CallFloatMethodV(JNIEnv* env, jobject obj, jmethodID meth, va_list args)
{
	jvalue retval;
	Hjava_lang_Object* o = (Hjava_lang_Object*)obj;
	Method* m = (Method*)meth;

	BEGIN_EXCEPTION_HANDLING(0);

	if (METHOD_IS_STATIC(m)) {
		throwException(NoSuchMethodError(m->name->data));
	}

	callMethodV(m, (m->idx >= 0 ? o->dtable->method[m->idx]
			: JNI_METHOD_CODE(m)), obj, args, &retval);

	END_EXCEPTION_HANDLING();
	return (retval.f);
}

jfloat
Kaffe_CallFloatMethod(JNIEnv* env, jobject obj, jmethodID meth, ...)
{
	va_list args;
	jfloat ret;

	BEGIN_EXCEPTION_HANDLING(0);

	va_start(args, meth);
	ret = Kaffe_CallFloatMethodV(env, obj, meth, args);
	va_end(args);

	END_EXCEPTION_HANDLING();
	return (ret);
}

jfloat
Kaffe_CallFloatMethodA(JNIEnv* env, jobject obj, jmethodID meth, jvalue* args)
{
	jvalue retval;
	Hjava_lang_Object* o = (Hjava_lang_Object*)obj;
	Method* m = (Method*)meth;

	BEGIN_EXCEPTION_HANDLING(0);

	if (METHOD_IS_STATIC(m)) {
		throwException(NoSuchMethodError(m->name->data));
	}

	callMethodA(m, (m->idx >= 0 ? o->dtable->method[m->idx]
			: JNI_METHOD_CODE(m)), obj, args, &retval, 0);

	END_EXCEPTION_HANDLING();
	return (retval.f);
}

jdouble
Kaffe_CallDoubleMethodV(JNIEnv* env, jobject obj, jmethodID meth, va_list args)
{
	jvalue retval;
	Hjava_lang_Object* o = (Hjava_lang_Object*)obj;
	Method* m = (Method*)meth;

	BEGIN_EXCEPTION_HANDLING(0);

	if (METHOD_IS_STATIC(m)) {
		throwException(NoSuchMethodError(m->name->data));
	}

	callMethodV(m, (m->idx >= 0 ? o->dtable->method[m->idx]
			: JNI_METHOD_CODE(m)), obj, args, &retval);

	END_EXCEPTION_HANDLING();
	return (retval.d);
}

jdouble
Kaffe_CallDoubleMethod(JNIEnv* env, jobject obj, jmethodID meth, ...)
{
	va_list args;
	jdouble ret;

	BEGIN_EXCEPTION_HANDLING(0);

	va_start(args, meth);
	ret = Kaffe_CallDoubleMethodV(env, obj, meth, args);
	va_end(args);

	END_EXCEPTION_HANDLING();
	return (ret);
}

jdouble
Kaffe_CallDoubleMethodA(JNIEnv* env, jobject obj, jmethodID meth, jvalue* args)
{
	jvalue retval;
	Hjava_lang_Object* o = (Hjava_lang_Object*)obj;
	Method* m = (Method*)meth;

	BEGIN_EXCEPTION_HANDLING(0);

	if (METHOD_IS_STATIC(m)) {
		throwException(NoSuchMethodError(m->name->data));
	}

	callMethodA(m, (m->idx >= 0 ? o->dtable->method[m->idx]
			: JNI_METHOD_CODE(m)), obj, args, &retval, 0);

	END_EXCEPTION_HANDLING();
	return (retval.d);
}

void
Kaffe_CallVoidMethodV(JNIEnv* env, jobject obj, jmethodID meth, va_list args)
{
	Hjava_lang_Object* o = (Hjava_lang_Object*)obj;
	Method* m = (Method*)meth;

	BEGIN_EXCEPTION_HANDLING_VOID();

	if (METHOD_IS_STATIC(m)) {
		throwException(NoSuchMethodError(m->name->data));
	}

	callMethodV(m, (m->idx >= 0 ? o->dtable->method[m->idx]
			: JNI_METHOD_CODE(m)), obj, args, 0);

	END_EXCEPTION_HANDLING();
}

void
Kaffe_CallVoidMethod(JNIEnv* env, jobject obj, jmethodID meth, ...)
{
	va_list args;

	BEGIN_EXCEPTION_HANDLING_VOID();

	va_start(args, meth);
	Kaffe_CallVoidMethodV(env, obj, meth, args);
	va_end(args);

	END_EXCEPTION_HANDLING();
}

void
Kaffe_CallVoidMethodA(JNIEnv* env, jobject obj, jmethodID meth, jvalue* args)
{
	Hjava_lang_Object* o = (Hjava_lang_Object*)obj;
	Method* m = (Method*)meth;

	BEGIN_EXCEPTION_HANDLING_VOID();

	if (METHOD_IS_STATIC(m)) {
		throwException(NoSuchMethodError(m->name->data));
	}

	callMethodA(m, (m->idx >= 0 ? o->dtable->method[m->idx]
			: JNI_METHOD_CODE(m)), obj, args, 0, 0);

	END_EXCEPTION_HANDLING();
}

jobject
Kaffe_CallNonvirtualObjectMethodV(JNIEnv* env, jobject obj, jclass cls, jmethodID meth, va_list args)
{
	jvalue retval;
	Method* m = (Method*)meth;

	BEGIN_EXCEPTION_HANDLING(0);

	if (METHOD_IS_STATIC(m)) {
		throwException(NoSuchMethodError(m->name->data));
	}

	callMethodV(m, JNI_METHOD_CODE(m), obj, args, &retval);

	ADD_REF(retval.l);
	END_EXCEPTION_HANDLING();
	return (retval.l);
}

jobject
Kaffe_CallNonvirtualObjectMethod(JNIEnv* env, jobject obj, jclass cls, jmethodID meth, ...)
{
	va_list args;
	jobject ret;

	BEGIN_EXCEPTION_HANDLING(0);

	va_start(args, meth);
	ret = Kaffe_CallNonvirtualObjectMethodV(env, obj, cls, meth, args);
	va_end(args);

	END_EXCEPTION_HANDLING();
	return (ret);
}

jobject
Kaffe_CallNonvirtualObjectMethodA(JNIEnv* env, jobject obj, jclass cls, jmethodID meth, jvalue* args)
{
	jvalue retval;
	Method* m = (Method*)meth;

	BEGIN_EXCEPTION_HANDLING(0);

	if (METHOD_IS_STATIC(m)) {
		throwException(NoSuchMethodError(m->name->data));
	}

	callMethodA(m, JNI_METHOD_CODE(m), obj, args, &retval, 0);

	ADD_REF(retval.l);
	END_EXCEPTION_HANDLING();
	return (retval.l);
}

jboolean
Kaffe_CallNonvirtualBooleanMethodV(JNIEnv* env, jobject obj, jclass cls, jmethodID meth, va_list args)
{
	jvalue retval;
	Method* m = (Method*)meth;

	BEGIN_EXCEPTION_HANDLING(0);

	if (METHOD_IS_STATIC(m)) {
		throwException(NoSuchMethodError(m->name->data));
	}

	callMethodV(m, JNI_METHOD_CODE(m), obj, args, &retval);

	END_EXCEPTION_HANDLING();
	return ((jboolean) retval.i);
}

jboolean
Kaffe_CallNonvirtualBooleanMethod(JNIEnv* env, jobject obj, jclass cls, jmethodID meth, ...)
{
	va_list args;
	jboolean ret;

	BEGIN_EXCEPTION_HANDLING(0);

	va_start(args, meth);
	ret = Kaffe_CallNonvirtualBooleanMethodV(env, obj, cls, meth, args);
	va_end(args);

	END_EXCEPTION_HANDLING();
	return (ret);
}

jboolean
Kaffe_CallNonvirtualBooleanMethodA(JNIEnv* env, jobject obj, jclass cls, jmethodID meth, jvalue* args)
{
	jvalue retval;
	Method* m = (Method*)meth;

	BEGIN_EXCEPTION_HANDLING(0);

	if (METHOD_IS_STATIC(m)) {
		throwException(NoSuchMethodError(m->name->data));
	}

	callMethodA(m, JNI_METHOD_CODE(m), obj, args, &retval, 0);

	END_EXCEPTION_HANDLING();
	return ((jboolean) retval.i);
}

jbyte
Kaffe_CallNonvirtualByteMethodV(JNIEnv* env, jobject obj, jclass cls, jmethodID meth, va_list args)
{
	jvalue retval;
	Method* m = (Method*)meth;

	BEGIN_EXCEPTION_HANDLING(0);

	if (METHOD_IS_STATIC(m)) {
		throwException(NoSuchMethodError(m->name->data));
	}

	callMethodV(m, JNI_METHOD_CODE(m), obj, args, &retval);

	END_EXCEPTION_HANDLING();
	return ((jbyte) retval.i);
}

jbyte
Kaffe_CallNonvirtualByteMethod(JNIEnv* env, jobject obj, jclass cls, jmethodID meth, ...)
{
	va_list args;
	jbyte ret;

	BEGIN_EXCEPTION_HANDLING(0);

	va_start(args, meth);
	ret = Kaffe_CallNonvirtualByteMethodV(env, obj, cls, meth, args);
	va_end(args);

	END_EXCEPTION_HANDLING();
	return (ret);
}

jbyte
Kaffe_CallNonvirtualByteMethodA(JNIEnv* env, jobject obj, jclass cls, jmethodID meth, jvalue* args)
{
	jvalue retval;
	Method* m = (Method*)meth;

	BEGIN_EXCEPTION_HANDLING(0);

	if (METHOD_IS_STATIC(m)) {
		throwException(NoSuchMethodError(m->name->data));
	}

	callMethodA(m, JNI_METHOD_CODE(m), obj, args, &retval, 0);

	END_EXCEPTION_HANDLING();
	return ((jbyte) retval.i);
}

jchar
Kaffe_CallNonvirtualCharMethodV(JNIEnv* env, jobject obj, jclass cls, jmethodID meth, va_list args)
{
	jvalue retval;
	Method* m = (Method*)meth;

	BEGIN_EXCEPTION_HANDLING(0);

	if (METHOD_IS_STATIC(m)) {
		throwException(NoSuchMethodError(m->name->data));
	}

	callMethodV(m, JNI_METHOD_CODE(m), obj, args, &retval);

	END_EXCEPTION_HANDLING();
	return ((jchar) retval.i);
}

jchar
Kaffe_CallNonvirtualCharMethod(JNIEnv* env, jobject obj, jclass cls, jmethodID meth, ...)
{
	va_list args;
	jchar ret;

	BEGIN_EXCEPTION_HANDLING(0);

	va_start(args, meth);
	ret = Kaffe_CallNonvirtualCharMethodV(env, obj, cls, meth, args);
	va_end(args);

	END_EXCEPTION_HANDLING();
	return (ret);
}

jchar
Kaffe_CallNonvirtualCharMethodA(JNIEnv* env, jobject obj, jclass cls, jmethodID meth, jvalue* args)
{
	jvalue retval;
	Method* m = (Method*)meth;

	BEGIN_EXCEPTION_HANDLING(0);

	if (METHOD_IS_STATIC(m)) {
		throwException(NoSuchMethodError(m->name->data));
	}

	callMethodA(m, JNI_METHOD_CODE(m), obj, args, &retval, 0);

	END_EXCEPTION_HANDLING();
	return ((jchar) retval.i);
}

jshort
Kaffe_CallNonvirtualShortMethodV(JNIEnv* env, jobject obj, jclass cls, jmethodID meth, va_list args)
{
	jvalue retval;
	Method* m = (Method*)meth;

	BEGIN_EXCEPTION_HANDLING(0);

	if (METHOD_IS_STATIC(m)) {
		throwException(NoSuchMethodError(m->name->data));
	}

	callMethodV(m, JNI_METHOD_CODE(m), obj, args, &retval);

	END_EXCEPTION_HANDLING();
	return ((jshort) retval.i);
}

jshort
Kaffe_CallNonvirtualShortMethod(JNIEnv* env, jobject obj, jclass cls, jmethodID meth, ...)
{
	va_list args;
	jshort ret;

	BEGIN_EXCEPTION_HANDLING(0);

	va_start(args, meth);
	ret = Kaffe_CallNonvirtualShortMethodV(env, obj, cls, meth, args);
	va_end(args);

	END_EXCEPTION_HANDLING();
	return (ret);
}

jshort
Kaffe_CallNonvirtualShortMethodA(JNIEnv* env, jobject obj, jclass cls, jmethodID meth, jvalue* args)
{
	jvalue retval;
	Method* m = (Method*)meth;

	BEGIN_EXCEPTION_HANDLING(0);

	if (METHOD_IS_STATIC(m)) {
		throwException(NoSuchMethodError(m->name->data));
	}

	callMethodA(m, JNI_METHOD_CODE(m), obj, args, &retval, 0);

	END_EXCEPTION_HANDLING();
	return ((jshort) retval.i);
}

jint
Kaffe_CallNonvirtualIntMethodV(JNIEnv* env, jobject obj, jclass cls, jmethodID meth, va_list args)
{
	jvalue retval;
	Method* m = (Method*)meth;

	BEGIN_EXCEPTION_HANDLING(0);

	if (METHOD_IS_STATIC(m)) {
		throwException(NoSuchMethodError(m->name->data));
	}

	callMethodV(m, JNI_METHOD_CODE(m), obj, args, &retval);

	END_EXCEPTION_HANDLING();
	return (retval.i);
}

jint
Kaffe_CallNonvirtualIntMethod(JNIEnv* env, jobject obj, jclass cls, jmethodID meth, ...)
{
	va_list args;
	jint ret;

	BEGIN_EXCEPTION_HANDLING(0);

	va_start(args, meth);
	ret = Kaffe_CallNonvirtualIntMethodV(env, obj, cls, meth, args);
	va_end(args);

	END_EXCEPTION_HANDLING();
	return (ret);
}

jint
Kaffe_CallNonvirtualIntMethodA(JNIEnv* env, jobject obj, jclass cls, jmethodID meth, jvalue* args)
{
	jvalue retval;
	Method* m = (Method*)meth;

	BEGIN_EXCEPTION_HANDLING(0);

	if (METHOD_IS_STATIC(m)) {
		throwException(NoSuchMethodError(m->name->data));
	}

	callMethodA(m, JNI_METHOD_CODE(m), obj, args, &retval, 0);

	END_EXCEPTION_HANDLING();
	return (retval.i);
}

jlong
Kaffe_CallNonvirtualLongMethodV(JNIEnv* env, jobject obj, jclass cls, jmethodID meth, va_list args)
{
	jvalue retval;
	Method* m = (Method*)meth;

	BEGIN_EXCEPTION_HANDLING(0);

	if (METHOD_IS_STATIC(m)) {
		throwException(NoSuchMethodError(m->name->data));
	}

	callMethodV(m, JNI_METHOD_CODE(m), obj, args, &retval);

	END_EXCEPTION_HANDLING();
	return (retval.j);
}

jlong
Kaffe_CallNonvirtualLongMethod(JNIEnv* env, jobject obj, jclass cls, jmethodID meth, ...)
{
	va_list args;
	jlong ret;

	BEGIN_EXCEPTION_HANDLING(0);

	va_start(args, meth);
	ret = Kaffe_CallNonvirtualLongMethodV(env, obj, cls, meth, args);
	va_end(args);

	END_EXCEPTION_HANDLING();
	return (ret);
}

jlong
Kaffe_CallNonvirtualLongMethodA(JNIEnv* env, jobject obj, jclass cls, jmethodID meth, jvalue* args)
{
	jvalue retval;
	Method* m = (Method*)meth;

	BEGIN_EXCEPTION_HANDLING(0);

	if (METHOD_IS_STATIC(m)) {
		throwException(NoSuchMethodError(m->name->data));
	}

	callMethodA(m, JNI_METHOD_CODE(m), obj, args, &retval, 0);

	END_EXCEPTION_HANDLING();
	return (retval.j);
}

jfloat
Kaffe_CallNonvirtualFloatMethodV(JNIEnv* env, jobject obj, jclass cls, jmethodID meth, va_list args)
{
	jvalue retval;
	Method* m = (Method*)meth;

	BEGIN_EXCEPTION_HANDLING(0);

	if (METHOD_IS_STATIC(m)) {
		throwException(NoSuchMethodError(m->name->data));
	}

	callMethodV(m, JNI_METHOD_CODE(m), obj, args, &retval);

	END_EXCEPTION_HANDLING();
	return (retval.f);
}

jfloat
Kaffe_CallNonvirtualFloatMethod(JNIEnv* env, jobject obj, jclass cls, jmethodID meth, ...)
{
	va_list args;
	jfloat ret;

	BEGIN_EXCEPTION_HANDLING(0);

	va_start(args, meth);
	ret = Kaffe_CallNonvirtualFloatMethodV(env, obj, cls, meth, args);
	va_end(args);

	END_EXCEPTION_HANDLING();
	return (ret);
}

jfloat
Kaffe_CallNonvirtualFloatMethodA(JNIEnv* env, jobject obj, jclass cls, jmethodID meth, jvalue* args)
{
	jvalue retval;
	Method* m = (Method*)meth;

	BEGIN_EXCEPTION_HANDLING(0);

	if (METHOD_IS_STATIC(m)) {
		throwException(NoSuchMethodError(m->name->data));
	}

	callMethodA(m, JNI_METHOD_CODE(m), obj, args, &retval, 0);

	END_EXCEPTION_HANDLING();
	return (retval.f);
}

jdouble
Kaffe_CallNonvirtualDoubleMethodV(JNIEnv* env, jobject obj, jclass cls, jmethodID meth, va_list args)
{
	jvalue retval;
	Method* m = (Method*)meth;

	BEGIN_EXCEPTION_HANDLING(0);

	if (METHOD_IS_STATIC(m)) {
		throwException(NoSuchMethodError(m->name->data));
	}

	callMethodV(m, JNI_METHOD_CODE(m), obj, args, &retval);

	END_EXCEPTION_HANDLING();
	return (retval.d);
}

jdouble
Kaffe_CallNonvirtualDoubleMethod(JNIEnv* env, jobject obj, jclass cls, jmethodID meth, ...)
{
	va_list args;
	jdouble ret;

	BEGIN_EXCEPTION_HANDLING(0);

	va_start(args, meth);
	ret = Kaffe_CallNonvirtualDoubleMethodV(env, obj, cls, meth, args);
	va_end(args);

	END_EXCEPTION_HANDLING();
	return (ret);
}

jdouble
Kaffe_CallNonvirtualDoubleMethodA(JNIEnv* env, jobject obj, jclass cls, jmethodID meth, jvalue* args)
{
	jvalue retval;
	Method* m = (Method*)meth;

	BEGIN_EXCEPTION_HANDLING(0);

	if (METHOD_IS_STATIC(m)) {
		throwException(NoSuchMethodError(m->name->data));
	}

	callMethodA(m, JNI_METHOD_CODE(m), obj, args, &retval, 0);

	END_EXCEPTION_HANDLING();
	return (retval.d);
}

void
Kaffe_CallNonvirtualVoidMethodV(JNIEnv* env, jobject obj, jclass cls, jmethodID meth, va_list args)
{
	Method* m = (Method*)meth;

	BEGIN_EXCEPTION_HANDLING_VOID();

	if (METHOD_IS_STATIC(m)) {
		throwException(NoSuchMethodError(m->name->data));
	}

	callMethodV(m, JNI_METHOD_CODE(m), obj, args, 0);

	END_EXCEPTION_HANDLING();
}

void
Kaffe_CallNonvirtualVoidMethod(JNIEnv* env, jobject obj, jclass cls, jmethodID meth, ...)
{
	va_list args;

	BEGIN_EXCEPTION_HANDLING_VOID();

	va_start(args, meth);
	Kaffe_CallNonvirtualVoidMethodV(env, obj, cls, meth, args);
	va_end(args);

	END_EXCEPTION_HANDLING();
}

void
Kaffe_CallNonvirtualVoidMethodA(JNIEnv* env, jobject obj, jclass cls, jmethodID meth, jvalue* args)
{
	Method* m = (Method*)meth;

	BEGIN_EXCEPTION_HANDLING_VOID();

	if (METHOD_IS_STATIC(m)) {
		throwException(NoSuchMethodError(m->name->data));
	}

	callMethodA(m, JNI_METHOD_CODE(m), obj, args, 0, 0);

	END_EXCEPTION_HANDLING();
}

jfieldID
Kaffe_GetFieldID(JNIEnv* env, jclass cls, const char* name, const char* sig)
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

jobject
Kaffe_GetObjectField(JNIEnv* env, jobject obj, jfieldID fld)
{
	jobject nobj;

	BEGIN_EXCEPTION_HANDLING(0);

	nobj = GET_FIELD(jobject, obj, fld);

	ADD_REF(nobj);
	END_EXCEPTION_HANDLING();
	return (nobj);
}

jboolean
Kaffe_GetBooleanField(JNIEnv* env, jobject obj, jfieldID fld)
{
	jboolean ret;

	BEGIN_EXCEPTION_HANDLING(0);

	ret = GET_FIELD(jboolean, obj, fld);

	END_EXCEPTION_HANDLING();
	return (ret);
}

jbyte
Kaffe_GetByteField(JNIEnv* env, jobject obj, jfieldID fld)
{
	jbyte ret;
	BEGIN_EXCEPTION_HANDLING(0);

	ret = GET_FIELD(jbyte, obj, fld);

	END_EXCEPTION_HANDLING();
	return (ret);
}

jchar
Kaffe_GetCharField(JNIEnv* env, jobject obj, jfieldID fld)
{
	jchar ret;
	BEGIN_EXCEPTION_HANDLING(0);

	ret = GET_FIELD(jchar, obj, fld);

	END_EXCEPTION_HANDLING();
	return (ret);
}

jshort
Kaffe_GetShortField(JNIEnv* env, jobject obj, jfieldID fld)
{
	jshort ret;
	BEGIN_EXCEPTION_HANDLING(0);

	ret = GET_FIELD(jshort, obj, fld);

	END_EXCEPTION_HANDLING();
	return (ret);
}

jint
Kaffe_GetIntField(JNIEnv* env, jobject obj, jfieldID fld)
{
	jint ret;
	BEGIN_EXCEPTION_HANDLING(0);

	ret = GET_FIELD(jint, obj, fld);

	END_EXCEPTION_HANDLING();
	return (ret);
}

jlong
Kaffe_GetLongField(JNIEnv* env, jobject obj, jfieldID fld)
{
	jlong ret;
	BEGIN_EXCEPTION_HANDLING(0);

	ret = GET_FIELD(jlong, obj, fld);

	END_EXCEPTION_HANDLING();
	return (ret);
}

jfloat
Kaffe_GetFloatField(JNIEnv* env, jobject obj, jfieldID fld)
{
	jfloat ret;
	BEGIN_EXCEPTION_HANDLING(0);

	ret = GET_FIELD(jfloat, obj, fld);

	END_EXCEPTION_HANDLING();
	return (ret);
}

jdouble
Kaffe_GetDoubleField(JNIEnv* env, jobject obj, jfieldID fld)
{
	jdouble ret;
	BEGIN_EXCEPTION_HANDLING(0);

	ret = GET_FIELD(jdouble, obj, fld);

	END_EXCEPTION_HANDLING();
	return (ret);
}

void
Kaffe_SetObjectField(JNIEnv* env, jobject obj, jfieldID fld, jobject val)
{
	BEGIN_EXCEPTION_HANDLING_VOID();

	SET_FIELD(jobject, obj, fld, val);

	END_EXCEPTION_HANDLING();
}

void
Kaffe_SetBooleanField(JNIEnv* env, jobject obj, jfieldID fld, jbool val)
{
	BEGIN_EXCEPTION_HANDLING_VOID();

	SET_FIELD(jboolean, obj, fld, val);

	END_EXCEPTION_HANDLING();
}

void
Kaffe_SetByteField(JNIEnv* env, jobject obj, jfieldID fld, jbyte val)
{
	BEGIN_EXCEPTION_HANDLING_VOID();

	SET_FIELD(jbyte, obj, fld, val);

	END_EXCEPTION_HANDLING();
}

void
Kaffe_SetCharField(JNIEnv* env, jobject obj, jfieldID fld, jchar val)
{
	BEGIN_EXCEPTION_HANDLING_VOID();

	SET_FIELD(jchar, obj, fld, val);

	END_EXCEPTION_HANDLING();
}

void
Kaffe_SetShortField(JNIEnv* env, jobject obj, jfieldID fld, jshort val)
{
	BEGIN_EXCEPTION_HANDLING_VOID();

	SET_FIELD(jshort, obj, fld, val);

	END_EXCEPTION_HANDLING();
}

void
Kaffe_SetIntField(JNIEnv* env, jobject obj, jfieldID fld, jint val)
{
	BEGIN_EXCEPTION_HANDLING_VOID();

	SET_FIELD(jint, obj, fld, val);

	END_EXCEPTION_HANDLING();
}

void
Kaffe_SetLongField(JNIEnv* env, jobject obj, jfieldID fld, jlong val)
{
	BEGIN_EXCEPTION_HANDLING_VOID();

	SET_FIELD(jlong, obj, fld, val);

	END_EXCEPTION_HANDLING();
}

void
Kaffe_SetFloatField(JNIEnv* env, jobject obj, jfieldID fld, jfloat val)
{
	BEGIN_EXCEPTION_HANDLING_VOID();

	SET_FIELD(jfloat, obj, fld, val);

	END_EXCEPTION_HANDLING();
}

void
Kaffe_SetDoubleField(JNIEnv* env, jobject obj, jfieldID fld, jdouble val)
{
	BEGIN_EXCEPTION_HANDLING_VOID();

	SET_FIELD(jdouble, obj, fld, val);

	END_EXCEPTION_HANDLING();
}

jmethodID
Kaffe_GetStaticMethodID(JNIEnv* env, jclass cls, const char* name, const char* sig)
{
	Method* meth;
	errorInfo info;

	BEGIN_EXCEPTION_HANDLING(0);
	meth = lookupClassMethod((Hjava_lang_Class*)cls, (char*)name, (char*)sig, &info);
	if (meth == 0) {
		postError(env, &info);
	} else if (!METHOD_IS_STATIC(meth)) {
		postExceptionMessage(&info, JAVA_LANG(NoSuchMethodError), name);
		postError(env, &info);
		meth = 0;
	}
	END_EXCEPTION_HANDLING();

	return (meth);
}

jobject
Kaffe_CallStaticObjectMethodV(JNIEnv* env, jclass cls, jmethodID meth, va_list args)
{
	jvalue retval;
	Method* m = (Method*)meth;

	BEGIN_EXCEPTION_HANDLING(0);

	if (!METHOD_IS_STATIC(m)) {
		throwException(NoSuchMethodError(m->name->data));
	}

	callMethodV(m, JNI_METHOD_CODE(m), 0, args, &retval);

	ADD_REF(retval.l);
	END_EXCEPTION_HANDLING();
	return (retval.l);
}

jobject
Kaffe_CallStaticObjectMethod(JNIEnv* env, jclass cls, jmethodID meth, ...)
{
	va_list args;
	jobject ret;

	BEGIN_EXCEPTION_HANDLING(0);

	va_start(args, meth);
	ret = Kaffe_CallStaticObjectMethodV(env, cls, meth, args);
	va_end(args);

	END_EXCEPTION_HANDLING();
	return (ret);
}

jobject
Kaffe_CallStaticObjectMethodA(JNIEnv* env, jclass cls, jmethodID meth, jvalue* args)
{
	jvalue retval;
	Method* m = (Method*)meth;

	BEGIN_EXCEPTION_HANDLING(0);

	if (!METHOD_IS_STATIC(m)) {
		throwException(NoSuchMethodError(m->name->data));
	}

	callMethodA(m, JNI_METHOD_CODE(m), 0, args, &retval, 0);

	ADD_REF(retval.l);
	END_EXCEPTION_HANDLING();
	return (retval.l);
}

jboolean
Kaffe_CallStaticBooleanMethodV(JNIEnv* env, jclass cls, jmethodID meth, va_list args)
{
	jvalue retval;
	Method* m = (Method*)meth;

	BEGIN_EXCEPTION_HANDLING(0);

	if (!METHOD_IS_STATIC(m)) {
		throwException(NoSuchMethodError(m->name->data));
	}

	callMethodV(m, JNI_METHOD_CODE(m), 0, args, &retval);

	END_EXCEPTION_HANDLING();
	return ((jboolean) retval.i);
}

jboolean
Kaffe_CallStaticBooleanMethod(JNIEnv* env, jclass cls, jmethodID meth, ...)
{
	va_list args;
	jboolean ret;

	BEGIN_EXCEPTION_HANDLING(0);

	va_start(args, meth);
	ret = Kaffe_CallStaticBooleanMethodV(env, cls, meth, args);
	va_end(args);

	END_EXCEPTION_HANDLING();
	return (ret);
}

jboolean
Kaffe_CallStaticBooleanMethodA(JNIEnv* env, jclass cls, jmethodID meth, jvalue* args)
{
	jvalue retval;
	Method* m = (Method*)meth;

	BEGIN_EXCEPTION_HANDLING(0);

	if (!METHOD_IS_STATIC(m)) {
		throwException(NoSuchMethodError(m->name->data));
	}

	callMethodA(m, JNI_METHOD_CODE(m), 0, args, &retval, 0);

	END_EXCEPTION_HANDLING();
	return ((jboolean) retval.i);
}

jbyte
Kaffe_CallStaticByteMethodV(JNIEnv* env, jclass cls, jmethodID meth, va_list args)
{
	jvalue retval;
	Method* m = (Method*)meth;

	BEGIN_EXCEPTION_HANDLING(0);

	if (!METHOD_IS_STATIC(m)) {
		throwException(NoSuchMethodError(m->name->data));
	}

	callMethodV(m, JNI_METHOD_CODE(m), 0, args, &retval);

	END_EXCEPTION_HANDLING();
	return ((jbyte) retval.i);
}

jbyte
Kaffe_CallStaticByteMethod(JNIEnv* env, jclass cls, jmethodID meth, ...)
{
	va_list args;
	jbyte ret;

	BEGIN_EXCEPTION_HANDLING(0);

	va_start(args, meth);
	ret = Kaffe_CallStaticByteMethodV(env, cls, meth, args);
	va_end(args);

	END_EXCEPTION_HANDLING();
	return (ret);
}

jbyte
Kaffe_CallStaticByteMethodA(JNIEnv* env, jclass cls, jmethodID meth, jvalue* args)
{
	jvalue retval;
	Method* m = (Method*)meth;

	BEGIN_EXCEPTION_HANDLING(0);

	if (!METHOD_IS_STATIC(m)) {
		throwException(NoSuchMethodError(m->name->data));
	}

	callMethodA(m, JNI_METHOD_CODE(m), 0, args, &retval, 0);

	END_EXCEPTION_HANDLING();
	return ((jbyte) retval.i);
}

jchar
Kaffe_CallStaticCharMethodV(JNIEnv* env, jclass cls, jmethodID meth, va_list args)
{
	jvalue retval;
	Method* m = (Method*)meth;

	BEGIN_EXCEPTION_HANDLING(0);

	if (!METHOD_IS_STATIC(m)) {
		throwException(NoSuchMethodError(m->name->data));
	}

	callMethodV(m, JNI_METHOD_CODE(m), 0, args, &retval);

	END_EXCEPTION_HANDLING();
	return ((jchar) retval.i);
}

jchar
Kaffe_CallStaticCharMethod(JNIEnv* env, jclass cls, jmethodID meth, ...)
{
	va_list args;
	jchar ret;

	BEGIN_EXCEPTION_HANDLING(0);

	va_start(args, meth);
	ret = Kaffe_CallStaticCharMethodV(env, cls, meth, args);
	va_end(args);

	END_EXCEPTION_HANDLING();
	return (ret);
}

jchar
Kaffe_CallStaticCharMethodA(JNIEnv* env, jclass cls, jmethodID meth, jvalue* args)
{
	jvalue retval;
	Method* m = (Method*)meth;

	BEGIN_EXCEPTION_HANDLING(0);

	if (!METHOD_IS_STATIC(m)) {
		throwException(NoSuchMethodError(m->name->data));
	}

	callMethodA(m, JNI_METHOD_CODE(m), 0, args, &retval, 0);

	END_EXCEPTION_HANDLING();
	return ((jchar) retval.i);
}

jshort
Kaffe_CallStaticShortMethodV(JNIEnv* env, jclass cls, jmethodID meth, va_list args)
{
	jvalue retval;
	Method* m = (Method*)meth;

	BEGIN_EXCEPTION_HANDLING(0);

	if (!METHOD_IS_STATIC(m)) {
		throwException(NoSuchMethodError(m->name->data));
	}

	callMethodV(m, JNI_METHOD_CODE(m), 0, args, &retval);

	END_EXCEPTION_HANDLING();
	return ((jshort) retval.i);
}

jshort
Kaffe_CallStaticShortMethod(JNIEnv* env, jclass cls, jmethodID meth, ...)
{
	va_list args;
	jshort ret;

	BEGIN_EXCEPTION_HANDLING(0);

	va_start(args, meth);
	ret = Kaffe_CallStaticShortMethodV(env, cls, meth, args);
	va_end(args);

	END_EXCEPTION_HANDLING();
	return (ret);
}

jshort
Kaffe_CallStaticShortMethodA(JNIEnv* env, jclass cls, jmethodID meth, jvalue* args)
{
	jvalue retval;
	Method* m = (Method*)meth;

	BEGIN_EXCEPTION_HANDLING(0);

	if (!METHOD_IS_STATIC(m)) {
		throwException(NoSuchMethodError(m->name->data));
	}

	callMethodA(m, JNI_METHOD_CODE(m), 0, args, &retval, 0);

	END_EXCEPTION_HANDLING();
	return ((jshort) retval.i);
}

jint
Kaffe_CallStaticIntMethodV(JNIEnv* env, jclass cls, jmethodID meth, va_list args)
{
	jvalue retval;
	Method* m = (Method*)meth;

	BEGIN_EXCEPTION_HANDLING(0);

	if (!METHOD_IS_STATIC(m)) {
		throwException(NoSuchMethodError(m->name->data));
	}

	callMethodV(m, JNI_METHOD_CODE(m), 0, args, &retval);

	END_EXCEPTION_HANDLING();
	return (retval.i);
}

jint
Kaffe_CallStaticIntMethod(JNIEnv* env, jclass cls, jmethodID meth, ...)
{
	va_list args;
	jint ret;

	BEGIN_EXCEPTION_HANDLING(0);

	va_start(args, meth);
	ret = Kaffe_CallStaticIntMethodV(env, cls, meth, args);
	va_end(args);

	END_EXCEPTION_HANDLING();
	return (ret);
}

jint
Kaffe_CallStaticIntMethodA(JNIEnv* env, jclass cls, jmethodID meth, jvalue* args)
{
	jvalue retval;
	Method* m = (Method*)meth;

	BEGIN_EXCEPTION_HANDLING(0);

	if (!METHOD_IS_STATIC(m)) {
		throwException(NoSuchMethodError(m->name->data));
	}

	callMethodA(m, JNI_METHOD_CODE(m), 0, args, &retval, 0);

	END_EXCEPTION_HANDLING();
	return (retval.i);
}

jlong
Kaffe_CallStaticLongMethodV(JNIEnv* env, jclass cls, jmethodID meth, va_list args)
{
	jvalue retval;
	Method* m = (Method*)meth;

	BEGIN_EXCEPTION_HANDLING(0);

	if (!METHOD_IS_STATIC(m)) {
		throwException(NoSuchMethodError(m->name->data));
	}

	callMethodV(m, JNI_METHOD_CODE(m), 0, args, &retval);

	END_EXCEPTION_HANDLING();
	return (retval.j);
}

jlong
Kaffe_CallStaticLongMethod(JNIEnv* env, jclass cls, jmethodID meth, ...)
{
	va_list args;
	jlong ret;

	BEGIN_EXCEPTION_HANDLING(0);

	va_start(args, meth);
	ret = Kaffe_CallStaticLongMethodV(env, cls, meth, args);
	va_end(args);

	END_EXCEPTION_HANDLING();
	return (ret);
}

jlong
Kaffe_CallStaticLongMethodA(JNIEnv* env, jclass cls, jmethodID meth, jvalue* args)
{
	jvalue retval;
	Method* m = (Method*)meth;

	BEGIN_EXCEPTION_HANDLING(0);

	if (!METHOD_IS_STATIC(m)) {
		throwException(NoSuchMethodError(m->name->data));
	}

	callMethodA(m, JNI_METHOD_CODE(m), 0, args, &retval, 0);

	END_EXCEPTION_HANDLING();
	return (retval.j);
}

jfloat
Kaffe_CallStaticFloatMethodV(JNIEnv* env, jclass cls, jmethodID meth, va_list args)
{
	jvalue retval;
	Method* m = (Method*)meth;

	BEGIN_EXCEPTION_HANDLING(0);

	if (!METHOD_IS_STATIC(m)) {
		throwException(NoSuchMethodError(m->name->data));
	}

	callMethodV(m, JNI_METHOD_CODE(m), 0, args, &retval);

	END_EXCEPTION_HANDLING();
	return (retval.f);
}

jfloat
Kaffe_CallStaticFloatMethod(JNIEnv* env, jclass cls, jmethodID meth, ...)
{
	va_list args;
	jfloat ret;

	BEGIN_EXCEPTION_HANDLING(0);

	va_start(args, meth);
	ret = Kaffe_CallStaticFloatMethodV(env, cls, meth, args);
	va_end(args);

	END_EXCEPTION_HANDLING();
	return (ret);
}

jfloat
Kaffe_CallStaticFloatMethodA(JNIEnv* env, jclass cls, jmethodID meth, jvalue* args)
{
	jvalue retval;
	Method* m = (Method*)meth;

	BEGIN_EXCEPTION_HANDLING(0);

	if (!METHOD_IS_STATIC(m)) {
		throwException(NoSuchMethodError(m->name->data));
	}

	callMethodA(m, JNI_METHOD_CODE(m), 0, args, &retval, 0);

	END_EXCEPTION_HANDLING();
	return (retval.f);
}

jdouble
Kaffe_CallStaticDoubleMethodV(JNIEnv* env, jclass cls, jmethodID meth, va_list args)
{
	jvalue retval;
	Method* m = (Method*)meth;

	BEGIN_EXCEPTION_HANDLING(0);

	if (!METHOD_IS_STATIC(m)) {
		throwException(NoSuchMethodError(m->name->data));
	}

	callMethodV(m, JNI_METHOD_CODE(m), 0, args, &retval);

	END_EXCEPTION_HANDLING();
	return (retval.d);
}

jdouble
Kaffe_CallStaticDoubleMethod(JNIEnv* env, jclass cls, jmethodID meth, ...)
{
	va_list args;
	jdouble ret;

	BEGIN_EXCEPTION_HANDLING(0);

	va_start(args, meth);
	ret = Kaffe_CallStaticDoubleMethodV(env, cls, meth, args);
	va_end(args);

	END_EXCEPTION_HANDLING();
	return (ret);
}

jdouble
Kaffe_CallStaticDoubleMethodA(JNIEnv* env, jclass cls, jmethodID meth, jvalue* args)
{
	jvalue retval;
	Method* m = (Method*)meth;

	BEGIN_EXCEPTION_HANDLING(0);

	if (!METHOD_IS_STATIC(m)) {
		throwException(NoSuchMethodError(m->name->data));
	}

	callMethodA(m, JNI_METHOD_CODE(m), 0, args, &retval, 0);

	END_EXCEPTION_HANDLING();
	return (retval.d);
}

void
Kaffe_CallStaticVoidMethodV(JNIEnv* env, jclass cls, jmethodID meth, va_list args)
{
	jvalue retval;
	Method* m = (Method*)meth;

	BEGIN_EXCEPTION_HANDLING_VOID();

	if (!METHOD_IS_STATIC(m)) {
		throwException(NoSuchMethodError(m->name->data));
	}

	callMethodV(m, JNI_METHOD_CODE(m), 0, args, &retval);

	END_EXCEPTION_HANDLING();
}

void
Kaffe_CallStaticVoidMethod(JNIEnv* env, jclass cls, jmethodID meth, ...)
{
	va_list args;

	BEGIN_EXCEPTION_HANDLING_VOID();

	va_start(args, meth);
	Kaffe_CallStaticVoidMethodV(env, cls, meth, args);
	va_end(args);

	END_EXCEPTION_HANDLING();
}

void
Kaffe_CallStaticVoidMethodA(JNIEnv* env, jclass cls, jmethodID meth, jvalue* args)
{
	Method* m = (Method*)meth;

	BEGIN_EXCEPTION_HANDLING_VOID();

	if (!METHOD_IS_STATIC(m)) {
		throwException(NoSuchMethodError(m->name->data));
	}

	callMethodA(m, JNI_METHOD_CODE(m), 0, args, 0, 0);

	END_EXCEPTION_HANDLING();
}

jfieldID
Kaffe_GetStaticFieldID(JNIEnv* env, jclass cls, const char* name, const char* sig)
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

jobject
Kaffe_GetStaticObjectField(JNIEnv* env, jclass cls, jfieldID fld)
{
	jobject obj;

	BEGIN_EXCEPTION_HANDLING(0);

	obj = GET_STATIC_FIELD(jobject, fld);

	ADD_REF(obj);
	END_EXCEPTION_HANDLING();
	return (obj);
}

jboolean
Kaffe_GetStaticBooleanField(JNIEnv* env, jclass cls, jfieldID fld)
{
	jboolean ret;
	BEGIN_EXCEPTION_HANDLING(0);

	ret = GET_STATIC_FIELD(jboolean, fld);

	END_EXCEPTION_HANDLING();
	return (ret);
}

jbyte
Kaffe_GetStaticByteField(JNIEnv* env, jclass cls, jfieldID fld)
{
	jbyte ret;
	BEGIN_EXCEPTION_HANDLING(0);

	ret = GET_STATIC_FIELD(jbyte, fld);

	END_EXCEPTION_HANDLING();
	return (ret);
}

jchar
Kaffe_GetStaticCharField(JNIEnv* env, jclass cls, jfieldID fld)
{
	jchar ret;
	BEGIN_EXCEPTION_HANDLING(0);

	ret = GET_STATIC_FIELD(jchar, fld);

	END_EXCEPTION_HANDLING();
	return (ret);
}

jshort
Kaffe_GetStaticShortField(JNIEnv* env, jclass cls, jfieldID fld)
{
	jint ret;
	BEGIN_EXCEPTION_HANDLING(0);

	ret = GET_STATIC_FIELD(jshort, fld);

	END_EXCEPTION_HANDLING();
	return (ret);
}

jint
Kaffe_GetStaticIntField(JNIEnv* env, jclass cls, jfieldID fld)
{
	jint ret;
	BEGIN_EXCEPTION_HANDLING(0);

	ret = GET_STATIC_FIELD(jint, fld);

	END_EXCEPTION_HANDLING();
	return (ret);
}

jlong
Kaffe_GetStaticLongField(JNIEnv* env, jclass cls, jfieldID fld)
{
	jlong ret;
	BEGIN_EXCEPTION_HANDLING(0);

	ret = GET_STATIC_FIELD(jlong, fld);

	END_EXCEPTION_HANDLING();
	return (ret);
}

jfloat
Kaffe_GetStaticFloatField(JNIEnv* env, jclass cls, jfieldID fld)
{
	jfloat ret;
	BEGIN_EXCEPTION_HANDLING(0);

	ret = GET_STATIC_FIELD(jfloat, fld);

	END_EXCEPTION_HANDLING();
	return (ret);
}

jdouble
Kaffe_GetStaticDoubleField(JNIEnv* env, jclass cls, jfieldID fld)
{
	jdouble ret;
	BEGIN_EXCEPTION_HANDLING(0);

	ret = GET_STATIC_FIELD(jdouble, fld);

	END_EXCEPTION_HANDLING();
	return (ret);
}

void
Kaffe_SetStaticObjectField(JNIEnv* env, jclass cls, jfieldID fld, jobject val)
{
	BEGIN_EXCEPTION_HANDLING_VOID();

	SET_STATIC_FIELD(jobject, fld, val);

	END_EXCEPTION_HANDLING();
}

void
Kaffe_SetStaticBooleanField(JNIEnv* env, jclass cls, jfieldID fld, jbool val)
{
	BEGIN_EXCEPTION_HANDLING_VOID();

	SET_STATIC_FIELD(jboolean, fld, val);

	END_EXCEPTION_HANDLING();
}

void
Kaffe_SetStaticByteField(JNIEnv* env, jclass cls, jfieldID fld, jbyte val)
{
	BEGIN_EXCEPTION_HANDLING_VOID();

	SET_STATIC_FIELD(jbyte, fld, val);

	END_EXCEPTION_HANDLING();
}

void
Kaffe_SetStaticCharField(JNIEnv* env, jclass cls, jfieldID fld, jchar val)
{
	BEGIN_EXCEPTION_HANDLING_VOID();

	SET_STATIC_FIELD(jchar, fld, val);

	END_EXCEPTION_HANDLING();
}

void
Kaffe_SetStaticShortField(JNIEnv* env, jclass cls, jfieldID fld, jshort val)
{
	BEGIN_EXCEPTION_HANDLING_VOID();

	SET_STATIC_FIELD(jshort, fld, val);

	END_EXCEPTION_HANDLING();
}

void
Kaffe_SetStaticIntField(JNIEnv* env, jclass cls, jfieldID fld, jint val)
{
	BEGIN_EXCEPTION_HANDLING_VOID();

	SET_STATIC_FIELD(jint, fld, val);

	END_EXCEPTION_HANDLING();
}

void
Kaffe_SetStaticLongField(JNIEnv* env, jclass cls, jfieldID fld, jlong val)
{
	BEGIN_EXCEPTION_HANDLING_VOID();

	SET_STATIC_FIELD(jlong, fld, val);

	END_EXCEPTION_HANDLING();
}

void
Kaffe_SetStaticFloatField(JNIEnv* env, jclass cls, jfieldID fld, jfloat val)
{
	BEGIN_EXCEPTION_HANDLING_VOID();

	SET_STATIC_FIELD(jfloat, fld, val);

	END_EXCEPTION_HANDLING();
}

void
Kaffe_SetStaticDoubleField(JNIEnv* env, jclass cls, jfieldID fld, jdouble val)
{
	BEGIN_EXCEPTION_HANDLING_VOID();

	SET_STATIC_FIELD(jdouble, fld, val);

	END_EXCEPTION_HANDLING();
}

jstring
Kaffe_NewString(JNIEnv* env, const jchar* data, jsize len)
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

jsize
Kaffe_GetStringLength(JNIEnv* env, jstring data)
{
	jsize ret;
	BEGIN_EXCEPTION_HANDLING(0);

	ret = STRING_SIZE((Hjava_lang_String*)data);
	END_EXCEPTION_HANDLING();
	return (ret);
}

const jchar*
Kaffe_GetStringChars(JNIEnv* env, jstring data, jboolean* copy)
{
	jchar* ret;
	BEGIN_EXCEPTION_HANDLING(0);

	if (copy != NULL) {
		*copy = JNI_FALSE;
	}
	ret = STRING_DATA(((Hjava_lang_String*)data));

	END_EXCEPTION_HANDLING();
	return (ret);
}

jstring
Kaffe_NewStringUTF(JNIEnv* env, const char* data)
{
	Hjava_lang_String* str;
	Utf8Const* utf8;
	int len;

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

jsize
Kaffe_GetStringUTFLength(JNIEnv* env, jstring data)
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

const jbyte*
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

void
Kaffe_ReleaseStringUTFChars(JNIEnv* env, jstring data, const jbyte* chars)
{
	BEGIN_EXCEPTION_HANDLING_VOID();

	KFREE((void*)chars);

	END_EXCEPTION_HANDLING();
}

jsize
Kaffe_GetArrayLength(JNIEnv* env, jarray arr)
{
	jsize ret;
	BEGIN_EXCEPTION_HANDLING(0);

	ret = obj_length((HArrayOfObject*)arr);

	END_EXCEPTION_HANDLING();
	return (ret);
}

jarray
Kaffe_NewObjectArray(JNIEnv* env, jsize len, jclass cls, jobject init)
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

jobject
Kaffe_GetObjectArrayElement(JNIEnv* env, jarray arr, jsize elem)
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

void
Kaffe_SetObjectArrayElement(JNIEnv* env, jarray arr, jsize elem, jobject val)
{
	BEGIN_EXCEPTION_HANDLING_VOID();

	if (elem >= obj_length((HArrayOfObject*)arr)) {
		throwException(ArrayIndexOutOfBoundsException);
	}
	unhand_array((HArrayOfObject*)arr)->body[elem] = (Hjava_lang_Object*)val;

	END_EXCEPTION_HANDLING();
}

jarray
Kaffe_NewBooleanArray(JNIEnv* env, jsize len)
{
	jarray arr;

	BEGIN_EXCEPTION_HANDLING(0);

	arr = newArray(_Jv_booleanClass, len);

	ADD_REF(arr);
	END_EXCEPTION_HANDLING();
	return (arr);
}

jarray
Kaffe_NewByteArray(JNIEnv* env, jsize len)
{
	jarray arr;

	BEGIN_EXCEPTION_HANDLING(0);

	arr = newArray(_Jv_byteClass, len);

	ADD_REF(arr);
	END_EXCEPTION_HANDLING();
	return (arr);
}

jarray
Kaffe_NewCharArray(JNIEnv* env, jsize len)
{
	jarray arr;

	BEGIN_EXCEPTION_HANDLING(0);

	arr = newArray(_Jv_charClass, len);

	ADD_REF(arr);
	END_EXCEPTION_HANDLING();
	return (arr);
}

jarray
Kaffe_NewShortArray(JNIEnv* env, jsize len)
{
	jarray arr;

	BEGIN_EXCEPTION_HANDLING(0);

	arr = newArray(_Jv_shortClass, len);

	ADD_REF(arr);
	END_EXCEPTION_HANDLING();
	return (arr);
}

jarray
Kaffe_NewIntArray(JNIEnv* env, jsize len)
{
	jarray arr;

	BEGIN_EXCEPTION_HANDLING(0);

	arr = newArray(_Jv_intClass, len);

	ADD_REF(arr);
	END_EXCEPTION_HANDLING();
	return (arr);
}

jarray
Kaffe_NewLongArray(JNIEnv* env, jsize len)
{
	jarray arr;

	BEGIN_EXCEPTION_HANDLING(0);

	arr = newArray(_Jv_longClass, len);

	ADD_REF(arr);
	END_EXCEPTION_HANDLING();
	return (arr);
}

jarray
Kaffe_NewFloatArray(JNIEnv* env, jsize len)
{
	jarray arr;

	BEGIN_EXCEPTION_HANDLING(0);

	arr = newArray(_Jv_floatClass, len);

	ADD_REF(arr);
	END_EXCEPTION_HANDLING();
	return (arr);
}

jarray
Kaffe_NewDoubleArray(JNIEnv* env, jsize len)
{
	jarray arr;

	BEGIN_EXCEPTION_HANDLING(0);

	arr = newArray(_Jv_doubleClass, len);

	ADD_REF(arr);
	END_EXCEPTION_HANDLING();
	return (arr);
}

jboolean*
Kaffe_GetBooleanArrayElements(JNIEnv* env, jarray arr, jbool* iscopy)
{
	jboolean* ret;
	BEGIN_EXCEPTION_HANDLING(0);

	if (iscopy != NULL) {
		*iscopy = JNI_FALSE;
	}
	ret = unhand_array((HArrayOfBoolean*)arr)->body;

	END_EXCEPTION_HANDLING();
	return (ret);
}

jbyte*
Kaffe_GetByteArrayElements(JNIEnv* env, jarray arr, jbool* iscopy)
{
	jbyte* ret;
	BEGIN_EXCEPTION_HANDLING(0);

	if (iscopy != NULL) {
		*iscopy = JNI_FALSE;
	}
	ret = unhand_array((HArrayOfByte*)arr)->body;

	END_EXCEPTION_HANDLING();
	return (ret);
}

jchar*
Kaffe_GetCharArrayElements(JNIEnv* env, jarray arr, jbool* iscopy)
{
	jchar* ret;
	BEGIN_EXCEPTION_HANDLING(0);

	if (iscopy != NULL) {
		*iscopy = JNI_FALSE;
	}
	ret = unhand_array((HArrayOfChar*)arr)->body;

	END_EXCEPTION_HANDLING();
	return (ret);
}

jshort*
Kaffe_GetShortArrayElements(JNIEnv* env, jarray arr, jbool* iscopy)
{
	jshort* ret;
	BEGIN_EXCEPTION_HANDLING(0);

	if (iscopy != NULL) {
		*iscopy = JNI_FALSE;
	}
	ret = unhand_array((HArrayOfShort*)arr)->body;

	END_EXCEPTION_HANDLING();
	return (ret);
}

jint*
Kaffe_GetIntArrayElements(JNIEnv* env, jarray arr, jbool* iscopy)
{
	jint* ret;
	BEGIN_EXCEPTION_HANDLING(0);

	if (iscopy != NULL) {
		*iscopy = JNI_FALSE;
	}
	ret = unhand_array((HArrayOfInt*)arr)->body;

	END_EXCEPTION_HANDLING();
	return (ret);
}

jlong*
Kaffe_GetLongArrayElements(JNIEnv* env, jarray arr, jbool* iscopy)
{
	jlong* ret;
	BEGIN_EXCEPTION_HANDLING(0);

	if (iscopy != NULL) {
		*iscopy = JNI_FALSE;
	}
	ret = unhand_array((HArrayOfLong*)arr)->body;

	END_EXCEPTION_HANDLING();
	return (ret);
}

jfloat*
Kaffe_GetFloatArrayElements(JNIEnv* env, jarray arr, jbool* iscopy)
{
	jfloat* ret;
	BEGIN_EXCEPTION_HANDLING(0);

	if (iscopy != NULL) {
		*iscopy = JNI_FALSE;
	}
	ret = unhand_array((HArrayOfFloat*)arr)->body;

	END_EXCEPTION_HANDLING();
	return (ret);
}

jdouble*
Kaffe_GetDoubleArrayElements(JNIEnv* env, jarray arr, jbool* iscopy)
{
	jdouble* ret;
	BEGIN_EXCEPTION_HANDLING(0);

	if (iscopy != NULL) {
		*iscopy = JNI_FALSE;
	}
	ret = unhand_array((HArrayOfDouble*)arr)->body;

	END_EXCEPTION_HANDLING();
	return (ret);
}

void
Kaffe_ReleaseBooleanArrayElements(JNIEnv* env, jarray arr, jbool* elems, jint mode)
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

void
Kaffe_ReleaseByteArrayElements(JNIEnv* env, jarray arr, jbyte* elems, jint mode)
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

void
Kaffe_ReleaseCharArrayElements(JNIEnv* env, jarray arr, jchar* elems, jint mode)
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

void
Kaffe_ReleaseShortArrayElements(JNIEnv* env, jarray arr, jshort* elems, jint mode)
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

void
Kaffe_ReleaseIntArrayElements(JNIEnv* env, jarray arr, jint* elems, jint mode)
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

void
Kaffe_ReleaseLongArrayElements(JNIEnv* env, jarray arr, jlong* elems, jint mode)
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

void
Kaffe_ReleaseFloatArrayElements(JNIEnv* env, jarray arr, jfloat* elems, jint mode)
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

void
Kaffe_ReleaseDoubleArrayElements(JNIEnv* env, jarray arr, jdouble* elems, jint mode)
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

void
Kaffe_GetBooleanArrayRegion(JNIEnv* env, jarray arr, jsize start, jsize len, jbool* data)
{
	BEGIN_EXCEPTION_HANDLING_VOID();

	if (start >= obj_length((HArrayOfBoolean*)arr) || start + len > obj_length((HArrayOfBoolean*)arr)) {
		throwException(ArrayIndexOutOfBoundsException);
	}
	memcpy(data, &unhand_array((HArrayOfBoolean*)arr)->body[start], len * sizeof(jboolean));

	END_EXCEPTION_HANDLING();
}

void
Kaffe_GetByteArrayRegion(JNIEnv* env, jarray arr, jsize start, jsize len, jbyte* data)
{
	BEGIN_EXCEPTION_HANDLING_VOID();

	if (start >= obj_length((HArrayOfByte*)arr) || start + len > obj_length((HArrayOfByte*)arr)) {
		throwException(ArrayIndexOutOfBoundsException);
	}
	memcpy(data, &unhand_array((HArrayOfByte*)arr)->body[start], len * sizeof(jbyte));

	END_EXCEPTION_HANDLING();
}

void
Kaffe_GetCharArrayRegion(JNIEnv* env, jarray arr, jsize start, jsize len, jchar* data)
{
	BEGIN_EXCEPTION_HANDLING_VOID();

	if (start >= obj_length((HArrayOfChar*)arr) || start + len > obj_length((HArrayOfChar*)arr)) {
		throwException(ArrayIndexOutOfBoundsException);
	}
	memcpy(data, &unhand_array((HArrayOfChar*)arr)->body[start], len * sizeof(jchar));

	END_EXCEPTION_HANDLING();
}

void
Kaffe_GetShortArrayRegion(JNIEnv* env, jarray arr, jsize start, jsize len, jshort* data)
{
	BEGIN_EXCEPTION_HANDLING_VOID();

	if (start >= obj_length((HArrayOfShort*)arr) || start + len > obj_length((HArrayOfShort*)arr)) {
		throwException(ArrayIndexOutOfBoundsException);
	}
	memcpy(data, &unhand_array((HArrayOfShort*)arr)->body[start], len * sizeof(jshort));

	END_EXCEPTION_HANDLING();
}

void
Kaffe_GetIntArrayRegion(JNIEnv* env, jarray arr, jsize start, jsize len, jint* data)
{
	BEGIN_EXCEPTION_HANDLING_VOID();

	if (start >= obj_length((HArrayOfInt*)arr) || start + len > obj_length((HArrayOfInt*)arr)) {
		throwException(ArrayIndexOutOfBoundsException);
	}
	memcpy(data, &unhand_array((HArrayOfInt*)arr)->body[start], len * sizeof(jint));

	END_EXCEPTION_HANDLING();
}

void
Kaffe_GetLongArrayRegion(JNIEnv* env, jarray arr, jsize start, jsize len, jlong* data)
{
	BEGIN_EXCEPTION_HANDLING_VOID();

	if (start >= obj_length((HArrayOfLong*)arr) || start + len > obj_length((HArrayOfLong*)arr)) {
		throwException(ArrayIndexOutOfBoundsException);
	}
	memcpy(data, &unhand_array((HArrayOfLong*)arr)->body[start], len * sizeof(jlong));

	END_EXCEPTION_HANDLING();
}

void
Kaffe_GetFloatArrayRegion(JNIEnv* env, jarray arr, jsize start, jsize len, jfloat* data)
{
	BEGIN_EXCEPTION_HANDLING_VOID();

	if (start >= obj_length((HArrayOfFloat*)arr) || start + len > obj_length((HArrayOfFloat*)arr)) {
		throwException(ArrayIndexOutOfBoundsException);
	}
	memcpy(data, &unhand_array((HArrayOfFloat*)arr)->body[start], len * sizeof(jfloat));

	END_EXCEPTION_HANDLING();
}

void
Kaffe_GetDoubleArrayRegion(JNIEnv* env, jarray arr, jsize start, jsize len, jdouble* data)
{
	BEGIN_EXCEPTION_HANDLING_VOID();

	if (start >= obj_length((HArrayOfDouble*)arr) || start + len > obj_length((HArrayOfDouble*)arr)) {
		throwException(ArrayIndexOutOfBoundsException);
	}
	memcpy(data, &unhand_array((HArrayOfDouble*)arr)->body[start], len * sizeof(jdouble));

	END_EXCEPTION_HANDLING();
}

void
Kaffe_SetBooleanArrayRegion(JNIEnv* env, jarray arr, jsize start, jsize len, jbool* data)
{
	BEGIN_EXCEPTION_HANDLING_VOID();

	if (start >= obj_length((HArrayOfBoolean*)arr) || start+len > obj_length((HArrayOfBoolean*)arr)) {
		throwException(ArrayIndexOutOfBoundsException);
	}
	memcpy(&unhand_array((HArrayOfBoolean*)arr)->body[start], data, len * sizeof(jboolean));

	END_EXCEPTION_HANDLING();
}

void
Kaffe_SetByteArrayRegion(JNIEnv* env, jarray arr, jsize start, jsize len, jbyte* data)
{
	BEGIN_EXCEPTION_HANDLING_VOID();

	if (start >= obj_length((HArrayOfByte*)arr) || start+len > obj_length((HArrayOfByte*)arr)) {
		throwException(ArrayIndexOutOfBoundsException);
	}
	memcpy(&unhand_array((HArrayOfByte*)arr)->body[start], data, len * sizeof(jbyte));

	END_EXCEPTION_HANDLING();
}

void
Kaffe_SetCharArrayRegion(JNIEnv* env, jarray arr, jsize start, jsize len, jchar* data)
{
	BEGIN_EXCEPTION_HANDLING_VOID();

	if (start >= obj_length((HArrayOfChar*)arr) || start+len > obj_length((HArrayOfChar*)arr)) {
		throwException(ArrayIndexOutOfBoundsException);
	}
	memcpy(&unhand_array((HArrayOfChar*)arr)->body[start], data, len * sizeof(jchar));

	END_EXCEPTION_HANDLING();
}

void
Kaffe_SetShortArrayRegion(JNIEnv* env, jarray arr, jsize start, jsize len, jshort* data)
{
	BEGIN_EXCEPTION_HANDLING_VOID();

	if (start >= obj_length((HArrayOfShort*)arr) || start+len > obj_length((HArrayOfShort*)arr)) {
		throwException(ArrayIndexOutOfBoundsException);
	}
	memcpy(&unhand_array((HArrayOfShort*)arr)->body[start], data, len * sizeof(jshort));

	END_EXCEPTION_HANDLING();
}

void
Kaffe_SetIntArrayRegion(JNIEnv* env, jarray arr, jsize start, jsize len, jint* data)
{
	BEGIN_EXCEPTION_HANDLING_VOID();

	if (start >= obj_length((HArrayOfInt*)arr) || start+len > obj_length((HArrayOfInt*)arr)) {
		throwException(ArrayIndexOutOfBoundsException);
	}
	memcpy(&unhand_array((HArrayOfInt*)arr)->body[start], data, len * sizeof(jint));

	END_EXCEPTION_HANDLING();
}

void
Kaffe_SetLongArrayRegion(JNIEnv* env, jarray arr, jsize start, jsize len, jlong* data)
{
	BEGIN_EXCEPTION_HANDLING_VOID();

	if (start >= obj_length((HArrayOfLong*)arr) || start+len > obj_length((HArrayOfLong*)arr)) {
		throwException(ArrayIndexOutOfBoundsException);
	}
	memcpy(&unhand_array((HArrayOfLong*)arr)->body[start], data, len * sizeof(jlong));

	END_EXCEPTION_HANDLING();
}

void
Kaffe_SetFloatArrayRegion(JNIEnv* env, jarray arr, jsize start, jsize len, jfloat* data)
{
	BEGIN_EXCEPTION_HANDLING_VOID();

	if (start >= obj_length((HArrayOfFloat*)arr) || start+len > obj_length((HArrayOfFloat*)arr)) {
		throwException(ArrayIndexOutOfBoundsException);
	}
	memcpy(&unhand_array((HArrayOfFloat*)arr)->body[start], data, len * sizeof(jfloat));

	END_EXCEPTION_HANDLING();
}

void
Kaffe_SetDoubleArrayRegion(JNIEnv* env, jarray arr, jsize start, jsize len, jdouble* data)
{
	BEGIN_EXCEPTION_HANDLING_VOID();

	if (start >= obj_length((HArrayOfDouble*)arr) || start+len > obj_length((HArrayOfDouble*)arr)) {
		throwException(ArrayIndexOutOfBoundsException);
	}
	memcpy(&unhand_array((HArrayOfDouble*)arr)->body[start], data, len * sizeof(jdouble));

	END_EXCEPTION_HANDLING();
}

jint
Kaffe_RegisterNatives(JNIEnv* env, jclass cls, const JNINativeMethod* methods, jint nmethods)
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
			if (strcmp(meth[i].name->data, methods[j].name) == 0 &&
			    strcmp(METHOD_SIGD(&meth[i]), methods[j].signature) == 0 &&
			    (meth[i].accflags & ACC_NATIVE) != 0) {
				Kaffe_JNI_wrapper(&meth[i], methods[j].fnPtr); 
				goto found;
			}
		}
		/* Failed to find method */
		throwException(NoSuchMethodError(methods[j].name));

		found:;
	}

	END_EXCEPTION_HANDLING();

	return (0);
}

jint
Kaffe_UnregisterNatives(JNIEnv* env, jclass cls)
{
	/* We do not support unloading */
	return (-1);
}

jint
Kaffe_MonitorEnter(JNIEnv* env, jobject obj)
{
	BEGIN_EXCEPTION_HANDLING(0);

	/* We should never throw out of a JNI call */
	lockObject(obj);

	END_EXCEPTION_HANDLING();
	return (0);
}

jint
Kaffe_MonitorExit(JNIEnv* env, jobject obj)
{
	BEGIN_EXCEPTION_HANDLING(0);

	unlockObject(obj);

	END_EXCEPTION_HANDLING();
	return (0);
}

/*
 * Handle exceptions which fall back to the JNI layer.
 */
void
Kaffe_JNIExceptionHandler(void)
{
	vmException* frame;

	frame = (vmException*)unhand(getCurrentThread())->exceptPtr;
	if (frame) {
		/* Worry about window around BEGIN and END exception
		 * handling, as well as functions which only delay
		 * external exceptions.
		 */
		JTHREAD_LONGJMP(frame->jbuf, 1);
	}
}

/*
 * Functions past this point don't bother with jni exceptions.
 */
jint
Kaffe_GetJavaVM(JNIEnv* env, JavaVM** vm)
{
	(*vm) = &Kaffe_JavaVM;
	return (0);
}

jint
Kaffe_DestroyJavaVM(JavaVM* vm)
{
	/* Does nothing */
	return (0);
}

jint
Kaffe_AttachCurrentThread(JavaVM* vm, JNIEnv** env, ThreadAttachArgs* args)
{
	(*env) = (JNIEnv*)&Kaffe_JNIEnv;
	return (0);
}

jint
Kaffe_DetachCurrentThread(JavaVM* vm)
{
	/* Right now, calling this from main2 is what prevents us from 
	   exiting there */
	exitThread();	
	return (0);
}

static
void
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
static
void
Kaffe_wrapper(Method* xmeth, void* func, bool use_JNI)
{
	errorInfo info;
	int count;
	nativeCodeInfo ncode;
	SlotInfo* tmp;
	bool success = true;
	int j;
	int an;
	int iLockRoot;

	isStatic = METHOD_IS_STATIC(xmeth) ? 1 : 0;
	count = sizeofSigMethod(xmeth, false);
	count += 1 - isStatic;

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
		globalMethod = xmeth;
	}
#endif

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
		begin_func_sync();
		call_soft(startJNIcall);
		end_func_sync();

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
		pusharg_ref_const((void*)&Kaffe_JNIEnv, 0);

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
			count++;
			an++;
			break;
		case 'D':
			pusharg_double(local(an), count);
			count++;
			an++;
			break;
		}
		count++;
		an++;
	}

#else

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
		pusharg_ref_const((void*)&Kaffe_JNIEnv, 0);
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
		break;
	}

	end_function();
	ret();

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
#if defined(KAFFE_PROFILER)
	if (profFlag) {
		profiler_click_t end;

		profiler_get_clicks(end);
		xmeth->jitClicks = end - xmeth->jitClicks;
		globalMethod = 0;
	}
#endif

	leaveTranslator();
	if (!success) {
		throwError(&info);
	}
}
static
void
Kaffe_JNI_wrapper(Method* xmeth, void* func)
{
	Kaffe_wrapper(xmeth, func, true);
}

void
Kaffe_KNI_wrapper(Method* xmeth, void* func)
{
	    if ((xmeth->accflags & ACC_SYNCHRONISED)
#if defined(KAFFE_PROFILER)
	    || profFlag
#endif
	       ) {
		    Kaffe_wrapper(xmeth, func, false);
	    }
	    else {
		    SET_METHOD_NATIVECODE(xmeth, func);
	    }
}
#endif
#if defined(INTERPRETER)
/*
 * Wrap up a native function in a calling wrapper.  The interpreter
 * lets the callMethod[AV] macros functions handle the JNI specifics.
 */
static
void
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
static
void
startJNIcall(void)
{
#if defined(NEED_JNIREFS)
	jnirefs* table;

	table = gc_malloc(sizeof(jnirefs), &gcNormal);
	table->prev = unhand(getCurrentThread())->jnireferences;
	unhand(getCurrentThread())->jnireferences = table;
#endif
	/* No pending exception when we enter JNI routine */
	unhand(getCurrentThread())->exceptObj = 0;
}

static
void
finishJNIcall(void)
{
	jref eobj;
	Hjava_lang_Thread* ct;

	ct = getCurrentThread();
#if defined(NEED_JNIREFS)
	{
		jnirefs* table;

		table = (jnirefs*)unhand(ct)->jnireferences;
		unhand(ct)->jnireferences = table->prev;
	}
#endif
	/* If we have a pending exception, throw it */
	eobj = unhand(ct)->exceptObj;
	if (eobj != 0) {
		unhand(ct)->exceptObj = 0;
		throwExternalException(eobj);
	}
}
#endif /* TRANSLATOR */

#if defined(NEED_JNIREFS)
static
void
addJNIref(jref obj)
{
	jnirefs* table;
	int idx;

	table = (jnirefs*)unhand(getCurrentThread())->jnireferences;

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

static
void
removeJNIref(jref obj)
{
	int idx;
	jnirefs* table;

	table = (jnirefs*)unhand(getCurrentThread())->jnireferences;

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
	Kaffe_ExceptionOccured,
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
	Kaffe_CallObjectMethod,
	Kaffe_CallObjectMethodV,
	Kaffe_CallObjectMethodA,
	Kaffe_CallBooleanMethod,
	Kaffe_CallBooleanMethodV,
	Kaffe_CallBooleanMethodA,
	Kaffe_CallByteMethod,
	Kaffe_CallByteMethodV,
	Kaffe_CallByteMethodA,
	Kaffe_CallCharMethod,
	Kaffe_CallCharMethodV,
	Kaffe_CallCharMethodA,
	Kaffe_CallShortMethod,
	Kaffe_CallShortMethodV,
	Kaffe_CallShortMethodA,
	Kaffe_CallIntMethod,
	Kaffe_CallIntMethodV,
	Kaffe_CallIntMethodA,
	Kaffe_CallLongMethod,
	Kaffe_CallLongMethodV,
	Kaffe_CallLongMethodA,
	Kaffe_CallFloatMethod,
	Kaffe_CallFloatMethodV,
	Kaffe_CallFloatMethodA,
	Kaffe_CallDoubleMethod,
	Kaffe_CallDoubleMethodV,
	Kaffe_CallDoubleMethodA,
	Kaffe_CallVoidMethod,
	Kaffe_CallVoidMethodV,
	Kaffe_CallVoidMethodA,
	Kaffe_CallNonvirtualObjectMethod,
	Kaffe_CallNonvirtualObjectMethodV,
	Kaffe_CallNonvirtualObjectMethodA,
	Kaffe_CallNonvirtualBooleanMethod,
	Kaffe_CallNonvirtualBooleanMethodV,
	Kaffe_CallNonvirtualBooleanMethodA,
	Kaffe_CallNonvirtualByteMethod,
	Kaffe_CallNonvirtualByteMethodV,
	Kaffe_CallNonvirtualByteMethodA,
	Kaffe_CallNonvirtualCharMethod,
	Kaffe_CallNonvirtualCharMethodV,
	Kaffe_CallNonvirtualCharMethodA,
	Kaffe_CallNonvirtualShortMethod,
	Kaffe_CallNonvirtualShortMethodV,
	Kaffe_CallNonvirtualShortMethodA,
	Kaffe_CallNonvirtualIntMethod,
	Kaffe_CallNonvirtualIntMethodV,
	Kaffe_CallNonvirtualIntMethodA,
	Kaffe_CallNonvirtualLongMethod,
	Kaffe_CallNonvirtualLongMethodV,
	Kaffe_CallNonvirtualLongMethodA,
	Kaffe_CallNonvirtualFloatMethod,
	Kaffe_CallNonvirtualFloatMethodV,
	Kaffe_CallNonvirtualFloatMethodA,
	Kaffe_CallNonvirtualDoubleMethod,
	Kaffe_CallNonvirtualDoubleMethodV,
	Kaffe_CallNonvirtualDoubleMethodA,
	Kaffe_CallNonvirtualVoidMethod,
	Kaffe_CallNonvirtualVoidMethodV,
	Kaffe_CallNonvirtualVoidMethodA,
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
	Kaffe_CallStaticObjectMethod,
	Kaffe_CallStaticObjectMethodV,
	Kaffe_CallStaticObjectMethodA,
	Kaffe_CallStaticBooleanMethod,
	Kaffe_CallStaticBooleanMethodV,
	Kaffe_CallStaticBooleanMethodA,
	Kaffe_CallStaticByteMethod,
	Kaffe_CallStaticByteMethodV,
	Kaffe_CallStaticByteMethodA,
	Kaffe_CallStaticCharMethod,
	Kaffe_CallStaticCharMethodV,
	Kaffe_CallStaticCharMethodA,
	Kaffe_CallStaticShortMethod,
	Kaffe_CallStaticShortMethodV,
	Kaffe_CallStaticShortMethodA,
	Kaffe_CallStaticIntMethod,
	Kaffe_CallStaticIntMethodV,
	Kaffe_CallStaticIntMethodA,
	Kaffe_CallStaticLongMethod,
	Kaffe_CallStaticLongMethodV,
	Kaffe_CallStaticLongMethodA,
	Kaffe_CallStaticFloatMethod,
	Kaffe_CallStaticFloatMethodV,
	Kaffe_CallStaticFloatMethodA,
	Kaffe_CallStaticDoubleMethod,
	Kaffe_CallStaticDoubleMethodV,
	Kaffe_CallStaticDoubleMethodA,
	Kaffe_CallStaticVoidMethod,
	Kaffe_CallStaticVoidMethodV,
	Kaffe_CallStaticVoidMethodA,
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

};

/*
 * Setup the Kaffe JNI environment.
 */
struct JNIEnv_ Kaffe_JNIEnv = {
	&Kaffe_JNINativeInterface,
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
	0,		/* Verify mode */
	".",		/* Classpath */
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
