/* jni.h
   Copyright (C) 2001, 2004  Free Software Foundation, Inc.

This file is part of GNU Classpath.

GNU Classpath is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2, or (at your option)
any later version.
 
GNU Classpath is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
General Public License for more details.

You should have received a copy of the GNU General Public License
along with GNU Classpath; see the file COPYING.  If not, write to the
Free Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
02111-1307 USA.

Linking this library statically or dynamically with other modules is
making a combined work based on this library.  Thus, the terms and
conditions of the GNU General Public License cover the whole
combination.

As a special exception, the copyright holders of this library give you
permission to link this library with independent modules to produce an
executable, regardless of the license terms of these independent
modules, and to copy and distribute the resulting executable under
terms of your choice, provided that you also meet, for each linked
independent module, the terms and conditions of the license of that
module.  An independent module is a module which is not derived from
or based on this library.  If you modify this library, you may extend
this exception to your version of the library, but you are not
obligated to do so.  If you do not wish to do so, delete this
exception statement from your version. */

/* This file is based on jni.h from libgcj */

#ifndef __CLASSPATH_JNI_H__
#define __CLASSPATH_JNI_H__

#include <stdarg.h>

/* Linkage and calling conventions. */
#if defined (_WIN32) || defined (__WIN32__) || defined (WIN32)

#define JNIIMPORT        __declspec(dllimport)
#define JNIEXPORT        __declspec(dllexport)

#define JNICALL          __stdcall

#else /* !( _WIN32 || __WIN32__ || WIN32) */

#define JNIIMPORT
#define JNIEXPORT
#define JNICALL

#endif /* !( _WIN32 || __WIN32__ || WIN32) */

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */
  
#include "kaffe/jni_md.h" 

typedef jint jsize;
  
#ifdef __cplusplus
}
#endif

/* 
 * Before jni.h is #included within a typical JVM, the source code should 
 * #define _JNI_VM_INTERNAL_TYPES_DEFINED and provide the real declarations
 * for 'jobject', 'jfieldID', 'jMethodID' and other implementation types.
 * If _JNI_VM_INTERNAL_TYPES_DEFINED is not defined, the following 
 * declares the old versions of the types.
 */
#ifndef _JNI_VM_INTERNAL_TYPES_DEFINED
/*
typedef void *jobject;
typedef void *jfieldID;
typedef void *jmethodID;
*/
struct _jfieldID;
struct _jmethodID;
typedef struct _jfieldID *jfieldID;
typedef struct _jmethodID *jmethodID;
#endif 
   
#ifdef __cplusplus
  
class _jobject {};
class _jclass : public _jobject {};
class _jthrowable : public _jobject {};
class _jstring : public _jobject {};
class _jarray : public _jobject {};
class _jbooleanArray : public _jarray {};
class _jbyteArray : public _jarray {};
class _jcharArray : public _jarray {};
class _jshortArray : public _jarray {};
class _jintArray : public _jarray {};
class _jlongArray : public _jarray {};
class _jfloatArray : public _jarray {};
class _jdoubleArray : public _jarray {};
class _jobjectArray : public _jarray {};

typedef _jobject *jobject;
typedef _jclass *jclass;
typedef _jthrowable *jthrowable;
typedef _jstring *jstring;
typedef _jarray *jarray;
typedef _jbooleanArray *jbooleanArray;
typedef _jbyteArray *jbyteArray;
typedef _jcharArray *jcharArray;
typedef _jshortArray *jshortArray;
typedef _jintArray *jintArray;
typedef _jlongArray *jlongArray;
typedef _jfloatArray *jfloatArray;
typedef _jdoubleArray *jdoubleArray;
typedef _jobjectArray *jobjectArray;

typedef struct _Jv_JNIEnv JNIEnv;
typedef struct _Jv_JavaVM JavaVM;

#else /* __cplusplus */

 
typedef void *jobject;
typedef jobject jclass;
typedef jobject jstring;
typedef jobject jarray;
typedef jobject jthrowable;
typedef jobject jobjectArray;
typedef jobject jbyteArray;
typedef jobject jshortArray;
typedef jobject jintArray;
typedef jobject jlongArray;
typedef jobject jbooleanArray;
typedef jobject jcharArray;
typedef jobject jfloatArray;
typedef jobject jdoubleArray;
  
/* Dummy defines.  */
typedef const struct JNINativeInterface *JNIEnv;
typedef const struct JNIInvokeInterface *JavaVM;

#endif /* __cplusplus */

#define _Jv_va_list va_list

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

typedef union jvalue
{
  jboolean z;
  jbyte    b;
  jchar    c;
  jshort   s;
  jint     i;
  jlong    j;
  jfloat   f;
  jdouble  d;
  jobject  l;
} jvalue;
      
/* Used for jboolean type  */
#define JNI_TRUE  1
#define JNI_FALSE 0

/* Used when releasing array elements.  */
#define JNI_COMMIT 1
#define JNI_ABORT  2

/* FIXME: Could be wrong */
typedef jobject jweak;  

/* Version numbers.  */
#define JNI_VERSION_1_1 0x00010001 /* JNI version 1.1 */
#define JNI_VERSION_1_2 0x00010002 /* JNI version 1.2 */
#define JNI_VERSION_1_4 0x00010004 /* JNI version 1.4 */
    
/* Error codes */
#define JNI_OK            0
#define JNI_ERR          (-1)
#define JNI_EDETACHED    (-2)
#define JNI_EVERSION     (-3)
  
/* These functions might be defined in libraries which we load; the
   JNI implementation calls them at the appropriate times.  */
JNIEXPORT jint JNICALL JNI_OnLoad (JavaVM *, void *);
JNIEXPORT void JNICALL JNI_OnUnload (JavaVM *, void *);
    
/* These functions are called by user code to start using the
   invocation API.  */
JNIEXPORT jint JNICALL JNI_GetDefaultJavaVMInitArgs (void *);
JNIEXPORT jint JNICALL JNI_CreateJavaVM (JavaVM **, void **, void *);
JNIEXPORT jint JNICALL JNI_GetCreatedJavaVMs(JavaVM **, jsize, jsize *);


/* This structure is used when registering native methods.  */
typedef struct
{
  char *name;
  char *signature;
  void *fnPtr;
} JNINativeMethod;

struct JNINativeInterface
{
  void *reserved0;					       /* 0 */
  void *reserved1;					       /* 1 */
  void *reserved2;					       /* 2 */
  void *reserved3;					       /* 3 */

  jint     (JNICALL *GetVersion)                   (JNIEnv *);                               /* 4 */
  jclass   (JNICALL *DefineClass)                  (JNIEnv *, const char *, jobject, const jbyte *, jsize); /* 5 */
  jclass   (JNICALL *FindClass)                    (JNIEnv *, const char *);                 /* 6 */

  jmethodID (JNICALL *FromReflectedMethod)	   (JNIEnv *, jobject);                      /* 7 */
  jfieldID  (JNICALL *FromReflectedField)	   (JNIEnv *, jobject);                      /* 8 */
  jobject   (JNICALL *ToReflectedMethod)	   (JNIEnv *, jclass, jmethodID, jboolean);  /* 9 */

  jclass   (JNICALL *GetSuperclass)                (JNIEnv *, jclass);                       /* 10 */
  jboolean (JNICALL *IsAssignableFrom)             (JNIEnv *, jclass, jclass);               /* 11 */

  jobject  (JNICALL *ToReflectedField)		   (JNIEnv *, jclass, jfieldID, jboolean);   /* 12 */

  jint     (JNICALL *Throw)                        (JNIEnv *, jthrowable);                   /* 13 */
  jint     (JNICALL *ThrowNew)                     (JNIEnv *, jclass, const char *);         /* 14 */
  jthrowable (JNICALL *ExceptionOccurred)          (JNIEnv *);                               /* 15 */
  void     (JNICALL *ExceptionDescribe)            (JNIEnv *);                               /* 16 */
  void     (JNICALL *ExceptionClear)               (JNIEnv *);                               /* 17 */
  void     (JNICALL *FatalError)                   (JNIEnv *, const char *);                 /* 18 */

  jint     (JNICALL *PushLocalFrame)		   (JNIEnv *, jint);                         /* 19 */
  jobject  (JNICALL *PopLocalFrame)		   (JNIEnv *, jobject);                      /* 20 */

  jobject  (JNICALL *NewGlobalRef)                 (JNIEnv *, jobject);                      /* 21 */
  void     (JNICALL *DeleteGlobalRef)              (JNIEnv *, jobject);                      /* 22 */
  void     (JNICALL *DeleteLocalRef)               (JNIEnv *, jobject);                      /* 23 */
  jboolean (JNICALL *IsSameObject)                 (JNIEnv *, jobject, jobject);             /* 24 */

  jobject  (JNICALL *NewLocalRef)		   (JNIEnv *, jobject);                      /* 25 */
  jint     (JNICALL *EnsureLocalCapacity)	   (JNIEnv *, jint);                         /* 26 */

  jobject  (JNICALL *AllocObject)                  (JNIEnv *, jclass);                       /* 27 */
  jobject (JNICALL *NewObject)			   (JNIEnv *, jclass, jmethodID, ...);       /* 28 */
  jobject (JNICALL *NewObjectV)			   (JNIEnv *, jclass, jmethodID, _Jv_va_list); /* 29 */
  jobject (JNICALL *NewObjectA)			   (JNIEnv *, jclass, jmethodID, jvalue *);  /* 30 */

  jclass   (JNICALL *GetObjectClass)               (JNIEnv *, jobject);                      /* 31 */
  jboolean (JNICALL *IsInstanceOf)                 (JNIEnv *, jobject, jclass);              /* 32 */
  jmethodID (JNICALL *GetMethodID)                 (JNIEnv *, jclass, const char *, const char *); /* 33 */

  jobject 	(JNICALL *CallObjectMethod)	   (JNIEnv *, jobject, jmethodID, ...);         /* 34 */
  jobject 	(JNICALL *CallObjectMethodV)	   (JNIEnv *, jobject, jmethodID, _Jv_va_list); /* 35 */
  jobject 	(JNICALL *CallObjectMethodA)	   (JNIEnv *, jobject, jmethodID, jvalue *);    /* 36 */
  jboolean 	(JNICALL *CallBooleanMethod)	   (JNIEnv *, jobject, jmethodID, ...);         /* 37 */
  jboolean 	(JNICALL *CallBooleanMethodV)	   (JNIEnv *, jobject, jmethodID, _Jv_va_list); /* 38 */
  jboolean 	(JNICALL *CallBooleanMethodA)	   (JNIEnv *, jobject, jmethodID, jvalue *);    /* 39 */
  jbyte 	(JNICALL *CallByteMethod)	   (JNIEnv *, jobject, jmethodID, ...);         /* 40 */
  jbyte 	(JNICALL *CallByteMethodV)	   (JNIEnv *, jobject, jmethodID, _Jv_va_list); /* 41 */
  jbyte 	(JNICALL *CallByteMethodA)	   (JNIEnv *, jobject, jmethodID, jvalue *);    /* 42 */
  jchar 	(JNICALL *CallCharMethod)	   (JNIEnv *, jobject, jmethodID, ...);         /* 43 */
  jchar 	(JNICALL *CallCharMethodV)	   (JNIEnv *, jobject, jmethodID, _Jv_va_list); /* 44 */
  jchar 	(JNICALL *CallCharMethodA)	   (JNIEnv *, jobject, jmethodID, jvalue *);    /* 45 */
  jshort 	(JNICALL *CallShortMethod)	   (JNIEnv *, jobject, jmethodID, ...);         /* 46 */
  jshort 	(JNICALL *CallShortMethodV)	   (JNIEnv *, jobject, jmethodID, _Jv_va_list); /* 47 */
  jshort 	(JNICALL *CallShortMethodA)	   (JNIEnv *, jobject, jmethodID, jvalue *);    /* 48 */
  jint 		(JNICALL *CallIntMethod)	   (JNIEnv *, jobject, jmethodID, ...);         /* 49 */
  jint 		(JNICALL *CallIntMethodV)	   (JNIEnv *, jobject, jmethodID, _Jv_va_list); /* 50 */
  jint 		(JNICALL *CallIntMethodA)	   (JNIEnv *, jobject, jmethodID, jvalue *);    /* 51 */
  jlong 	(JNICALL *CallLongMethod)	   (JNIEnv *, jobject, jmethodID, ...);         /* 52 */
  jlong 	(JNICALL *CallLongMethodV)	   (JNIEnv *, jobject, jmethodID, _Jv_va_list); /* 53 */
  jlong 	(JNICALL *CallLongMethodA)	   (JNIEnv *, jobject, jmethodID, jvalue *);    /* 54 */
  jfloat 	(JNICALL *CallFloatMethod)	   (JNIEnv *, jobject, jmethodID, ...);         /* 55 */
  jfloat 	(JNICALL *CallFloatMethodV)	   (JNIEnv *, jobject, jmethodID, _Jv_va_list); /* 56 */
  jfloat 	(JNICALL *CallFloatMethodA)	   (JNIEnv *, jobject, jmethodID, jvalue *);    /* 57 */
  jdouble 	(JNICALL *CallDoubleMethod)	   (JNIEnv *, jobject, jmethodID, ...);         /* 58 */
  jdouble 	(JNICALL *CallDoubleMethodV)	   (JNIEnv *, jobject, jmethodID, _Jv_va_list); /* 59 */
  jdouble 	(JNICALL *CallDoubleMethodA)	   (JNIEnv *, jobject, jmethodID, jvalue *);    /* 60 */
  void  	(JNICALL *CallVoidMethod)	   (JNIEnv *, jobject, jmethodID, ...);         /* 61 */
  void  	(JNICALL *CallVoidMethodV)	   (JNIEnv *, jobject, jmethodID, _Jv_va_list); /* 62 */
  void  	(JNICALL *CallVoidMethodA)	   (JNIEnv *, jobject, jmethodID, jvalue *);    /* 63 */

  jobject   (JNICALL *CallNonvirtualObjectMethod)  (JNIEnv *, jobject, jclass, jmethodID, ...);          /* 64 */
  jobject   (JNICALL *CallNonvirtualObjectMethodV) (JNIEnv *, jobject, jclass, jmethodID, _Jv_va_list);  /* 65 */
  jobject   (JNICALL *CallNonvirtualObjectMethodA) (JNIEnv *, jobject, jclass, jmethodID, jvalue *);     /* 66 */
  jboolean  (JNICALL *CallNonvirtualBooleanMethod) (JNIEnv *, jobject, jclass, jmethodID, ...);          /* 67 */
  jboolean  (JNICALL *CallNonvirtualBooleanMethodV) (JNIEnv *, jobject, jclass, jmethodID, _Jv_va_list); /* 68 */
  jboolean  (JNICALL *CallNonvirtualBooleanMethodA) (JNIEnv *, jobject, jclass, jmethodID, jvalue *);    /* 69 */
  jbyte     (JNICALL *CallNonvirtualByteMethod)	   (JNIEnv *, jobject, jclass, jmethodID, ...);          /* 70 */
  jbyte     (JNICALL *CallNonvirtualByteMethodV)   (JNIEnv *, jobject, jclass, jmethodID, _Jv_va_list);  /* 71 */
  jbyte     (JNICALL *CallNonvirtualByteMethodA)   (JNIEnv *, jobject, jclass, jmethodID, jvalue *);     /* 72 */
  jchar     (JNICALL *CallNonvirtualCharMethod)	   (JNIEnv *, jobject, jclass, jmethodID, ...);          /* 73 */
  jchar     (JNICALL *CallNonvirtualCharMethodV)   (JNIEnv *, jobject, jclass, jmethodID, _Jv_va_list);  /* 74 */
  jchar     (JNICALL *CallNonvirtualCharMethodA)   (JNIEnv *, jobject, jclass, jmethodID, jvalue *);     /* 75 */
  jshort    (JNICALL *CallNonvirtualShortMethod)   (JNIEnv *, jobject, jclass, jmethodID, ...);          /* 76 */
  jshort    (JNICALL *CallNonvirtualShortMethodV)  (JNIEnv *, jobject, jclass, jmethodID, _Jv_va_list);  /* 77 */
  jshort    (JNICALL *CallNonvirtualShortMethodA)  (JNIEnv *, jobject, jclass, jmethodID, jvalue *);     /* 78 */
  jint 	    (JNICALL *CallNonvirtualIntMethod)	   (JNIEnv *, jobject, jclass, jmethodID, ...);          /* 79 */
  jint 	    (JNICALL *CallNonvirtualIntMethodV)	   (JNIEnv *, jobject, jclass, jmethodID, _Jv_va_list);  /* 80 */
  jint 	    (JNICALL *CallNonvirtualIntMethodA)	   (JNIEnv *, jobject, jclass, jmethodID, jvalue *);     /* 81 */
  jlong     (JNICALL *CallNonvirtualLongMethod)	   (JNIEnv *, jobject, jclass, jmethodID, ...);          /* 82 */
  jlong     (JNICALL *CallNonvirtualLongMethodV)   (JNIEnv *, jobject, jclass, jmethodID, _Jv_va_list);  /* 83 */
  jlong     (JNICALL *CallNonvirtualLongMethodA)   (JNIEnv *, jobject, jclass, jmethodID, jvalue *);     /* 84 */
  jfloat    (JNICALL *CallNonvirtualFloatMethod)   (JNIEnv *, jobject, jclass, jmethodID, ...);          /* 85 */
  jfloat    (JNICALL *CallNonvirtualFloatMethodV)  (JNIEnv *, jobject, jclass, jmethodID, _Jv_va_list);  /* 86 */
  jfloat    (JNICALL *CallNonvirtualFloatMethodA)  (JNIEnv *, jobject, jclass, jmethodID, jvalue *);     /* 87 */
  jdouble   (JNICALL *CallNonvirtualDoubleMethod)  (JNIEnv *, jobject, jclass, jmethodID, ...);          /* 88 */
  jdouble   (JNICALL *CallNonvirtualDoubleMethodV) (JNIEnv *, jobject, jclass, jmethodID, _Jv_va_list);  /* 89 */
  jdouble   (JNICALL *CallNonvirtualDoubleMethodA) (JNIEnv *, jobject, jclass, jmethodID, jvalue *);     /* 90 */
  void      (JNICALL *CallNonvirtualVoidMethod)	   (JNIEnv *, jobject, jclass, jmethodID, ...);          /* 91 */
  void      (JNICALL *CallNonvirtualVoidMethodV)   (JNIEnv *, jobject, jclass, jmethodID, _Jv_va_list);  /* 92 */
  void      (JNICALL *CallNonvirtualVoidMethodA)   (JNIEnv *, jobject, jclass, jmethodID, jvalue *);     /* 93 */

  jfieldID      (JNICALL *GetFieldID)              (JNIEnv *, jclass, const char *, const char *);  /* 94 */

  jobject  (JNICALL *GetObjectField)               (JNIEnv *, jobject, jfieldID);                   /* 95 */
  jboolean (JNICALL *GetBooleanField)              (JNIEnv *, jobject, jfieldID);                   /* 96 */
  jbyte    (JNICALL *GetByteField)                 (JNIEnv *, jobject, jfieldID);                   /* 97 */
  jchar    (JNICALL *GetCharField)                 (JNIEnv *, jobject, jfieldID);                   /* 98 */
  jshort   (JNICALL *GetShortField)                (JNIEnv *, jobject, jfieldID);                   /* 99 */
  jint     (JNICALL *GetIntField)                  (JNIEnv *, jobject, jfieldID);                   /* 100 */
  jlong    (JNICALL *GetLongField)                 (JNIEnv *, jobject, jfieldID);                   /* 101 */
  jfloat   (JNICALL *GetFloatField)                (JNIEnv *, jobject, jfieldID);                   /* 102 */
  jdouble  (JNICALL *GetDoubleField)               (JNIEnv *, jobject, jfieldID);                   /* 103 */

  void		(JNICALL *SetObjectField)	   (JNIEnv *, jobject, jfieldID, jobject);          /* 104 */
  void		(JNICALL *SetBooleanField)	   (JNIEnv *, jobject, jfieldID, jboolean);         /* 105 */
  void		(JNICALL *SetByteField)		   (JNIEnv *, jobject, jfieldID, jbyte);            /* 106 */
  void		(JNICALL *SetCharField)		   (JNIEnv *, jobject, jfieldID, jchar);            /* 107 */
  void		(JNICALL *SetShortField)	   (JNIEnv *, jobject, jfieldID, jshort);           /* 108 */
  void		(JNICALL *SetIntField)		   (JNIEnv *, jobject, jfieldID, jint);             /* 109 */
  void		(JNICALL *SetLongField)		   (JNIEnv *, jobject, jfieldID, jlong);            /* 110 */
  void		(JNICALL *SetFloatField)	   (JNIEnv *, jobject, jfieldID, jfloat);           /* 111 */
  void		(JNICALL *SetDoubleField)	   (JNIEnv *, jobject, jfieldID, jdouble);          /* 112 */

  jmethodID (JNICALL *GetStaticMethodID)           (JNIEnv *, jclass, const char *, const char *);  /* 113 */

  jobject 	(JNICALL *CallStaticObjectMethod)  (JNIEnv *, jclass, jmethodID, ...);              /* 114 */
  jobject 	(JNICALL *CallStaticObjectMethodV) (JNIEnv *, jclass, jmethodID, _Jv_va_list);      /* 115 */
  jobject 	(JNICALL *CallStaticObjectMethodA) (JNIEnv *, jclass, jmethodID, jvalue *);         /* 116 */
  jboolean 	(JNICALL *CallStaticBooleanMethod) (JNIEnv *, jclass, jmethodID, ...);              /* 117 */
  jboolean 	(JNICALL *CallStaticBooleanMethodV) (JNIEnv *, jclass, jmethodID, _Jv_va_list);     /* 118 */
  jboolean 	(JNICALL *CallStaticBooleanMethodA) (JNIEnv *, jclass, jmethodID, jvalue *);        /* 119 */
  jbyte 	(JNICALL *CallStaticByteMethod)    (JNIEnv *, jclass, jmethodID, ...);              /* 120 */
  jbyte 	(JNICALL *CallStaticByteMethodV)   (JNIEnv *, jclass, jmethodID, _Jv_va_list);      /* 121 */
  jbyte 	(JNICALL *CallStaticByteMethodA)   (JNIEnv *, jclass, jmethodID, jvalue *);         /* 122 */
  jchar 	(JNICALL *CallStaticCharMethod)    (JNIEnv *, jclass, jmethodID, ...);              /* 123 */
  jchar 	(JNICALL *CallStaticCharMethodV)   (JNIEnv *, jclass, jmethodID, _Jv_va_list);      /* 124 */
  jchar 	(JNICALL *CallStaticCharMethodA)   (JNIEnv *, jclass, jmethodID, jvalue *);         /* 125 */
  jshort 	(JNICALL *CallStaticShortMethod)   (JNIEnv *, jclass, jmethodID, ...);              /* 126 */
  jshort 	(JNICALL *CallStaticShortMethodV)  (JNIEnv *, jclass, jmethodID, _Jv_va_list);      /* 127 */
  jshort 	(JNICALL *CallStaticShortMethodA)  (JNIEnv *, jclass, jmethodID, jvalue *);         /* 128 */
  jint 		(JNICALL *CallStaticIntMethod) 	   (JNIEnv *, jclass, jmethodID, ...);              /* 129 */
  jint 		(JNICALL *CallStaticIntMethodV)    (JNIEnv *, jclass, jmethodID, _Jv_va_list);      /* 130 */
  jint 		(JNICALL *CallStaticIntMethodA)    (JNIEnv *, jclass, jmethodID, jvalue *);         /* 131 */
  jlong 	(JNICALL *CallStaticLongMethod)    (JNIEnv *, jclass, jmethodID, ...);              /* 132 */
  jlong 	(JNICALL *CallStaticLongMethodV)   (JNIEnv *, jclass, jmethodID, _Jv_va_list);      /* 133 */
  jlong 	(JNICALL *CallStaticLongMethodA)   (JNIEnv *, jclass, jmethodID, jvalue *);         /* 134 */
  jfloat 	(JNICALL *CallStaticFloatMethod)   (JNIEnv *, jclass, jmethodID, ...);              /* 135 */
  jfloat 	(JNICALL *CallStaticFloatMethodV)  (JNIEnv *, jclass, jmethodID, _Jv_va_list);      /* 136 */
  jfloat 	(JNICALL *CallStaticFloatMethodA)  (JNIEnv *, jclass, jmethodID, jvalue *);         /* 137 */
  jdouble 	(JNICALL *CallStaticDoubleMethod)  (JNIEnv *, jclass, jmethodID, ...);              /* 138 */
  jdouble 	(JNICALL *CallStaticDoubleMethodV) (JNIEnv *, jclass, jmethodID, _Jv_va_list);      /* 139 */
  jdouble 	(JNICALL *CallStaticDoubleMethodA) (JNIEnv *, jclass, jmethodID, jvalue *);         /* 140 */
  void  	(JNICALL *CallStaticVoidMethod)    (JNIEnv *, jclass, jmethodID, ...);              /* 141 */
  void  	(JNICALL *CallStaticVoidMethodV)   (JNIEnv *, jclass, jmethodID, _Jv_va_list);      /* 142 */
  void  	(JNICALL *CallStaticVoidMethodA)   (JNIEnv *, jclass, jmethodID, jvalue *);         /* 143 */

  jfieldID      (JNICALL *GetStaticFieldID)        (JNIEnv *, jclass, const char *, const char *);  /* 144 */

  jobject	(JNICALL *GetStaticObjectField)	   (JNIEnv *, jclass, jfieldID);                    /* 145 */
  jboolean	(JNICALL *GetStaticBooleanField)   (JNIEnv *, jclass, jfieldID);                    /* 146 */
  jbyte		(JNICALL *GetStaticByteField)	   (JNIEnv *, jclass, jfieldID);                    /* 147 */
  jchar		(JNICALL *GetStaticCharField)	   (JNIEnv *, jclass, jfieldID);                    /* 148 */
  jshort	(JNICALL *GetStaticShortField)	   (JNIEnv *, jclass, jfieldID);                    /* 149 */
  jint		(JNICALL *GetStaticIntField)	   (JNIEnv *, jclass, jfieldID);                    /* 150 */
  jlong		(JNICALL *GetStaticLongField)	   (JNIEnv *, jclass, jfieldID);                    /* 151 */
  jfloat	(JNICALL *GetStaticFloatField)	   (JNIEnv *, jclass, jfieldID);                    /* 152 */
  jdouble	(JNICALL *GetStaticDoubleField)	   (JNIEnv *, jclass, jfieldID);                    /* 153 */

  void 		(JNICALL *SetStaticObjectField)	   (JNIEnv *, jclass, jfieldID, jobject);           /* 154 */
  void 		(JNICALL *SetStaticBooleanField)   (JNIEnv *, jclass, jfieldID, jboolean);          /* 155 */
  void 		(JNICALL *SetStaticByteField)	   (JNIEnv *, jclass, jfieldID, jbyte);             /* 156 */
  void 		(JNICALL *SetStaticCharField)	   (JNIEnv *, jclass, jfieldID, jchar);             /* 157 */
  void 		(JNICALL *SetStaticShortField)	   (JNIEnv *, jclass, jfieldID, jshort);            /* 158 */
  void 		(JNICALL *SetStaticIntField)	   (JNIEnv *, jclass, jfieldID, jint);              /* 159 */
  void 		(JNICALL *SetStaticLongField)	   (JNIEnv *, jclass, jfieldID, jlong);             /* 160 */
  void 		(JNICALL *SetStaticFloatField)	   (JNIEnv *, jclass, jfieldID, jfloat);            /* 161 */
  void 		(JNICALL *SetStaticDoubleField)	   (JNIEnv *, jclass, jfieldID, jdouble);           /* 162 */

  jstring  (JNICALL *NewString)                    (JNIEnv *, const jchar *, jsize);                /* 163 */
  jsize    (JNICALL *GetStringLength)              (JNIEnv *, jstring);                             /* 164 */
  const jchar * (JNICALL *GetStringChars)          (JNIEnv *, jstring, jboolean *);                 /* 165 */
  void     (JNICALL *ReleaseStringChars)           (JNIEnv *, jstring, const jchar *);              /* 166 */
  jstring  (JNICALL *NewStringUTF)                 (JNIEnv *, const char *);                        /* 167 */
  jsize    (JNICALL *GetStringUTFLength)           (JNIEnv *, jstring);                             /* 168 */
  const char * (JNICALL *GetStringUTFChars)       (JNIEnv *, jstring, jboolean *);                 /* 169 */
  void     (JNICALL *ReleaseStringUTFChars)        (JNIEnv *, jstring, const char *);               /* 170 */
  jsize    (JNICALL *GetArrayLength)               (JNIEnv *, jarray);                              /* 171 */
  jobjectArray   (JNICALL *NewObjectArray)         (JNIEnv *, jsize, jclass, jobject);              /* 172 */
  jobject  (JNICALL *GetObjectArrayElement)        (JNIEnv *, jobjectArray, jsize);                 /* 173 */
  void     (JNICALL *SetObjectArrayElement)        (JNIEnv *, jobjectArray, jsize, jobject);        /* 174 */

  jbooleanArray (JNICALL *NewBooleanArray)	   (JNIEnv *, jsize);                               /* 175 */
  jbyteArray    (JNICALL *NewByteArray)		   (JNIEnv *, jsize);                               /* 176 */
  jcharArray    (JNICALL *NewCharArray)		   (JNIEnv *, jsize);                               /* 177 */
  jshortArray   (JNICALL *NewShortArray)	   (JNIEnv *, jsize);                               /* 178 */
  jintArray     (JNICALL *NewIntArray)		   (JNIEnv *, jsize);                               /* 179 */
  jlongArray    (JNICALL *NewLongArray)		   (JNIEnv *, jsize);                               /* 180 */
  jfloatArray   (JNICALL *NewFloatArray)	   (JNIEnv *, jsize);                               /* 181 */
  jdoubleArray  (JNICALL *NewDoubleArray)	   (JNIEnv *, jsize);                               /* 182 */

  jboolean *	(JNICALL *GetBooleanArrayElements) (JNIEnv *, jbooleanArray, jboolean *);           /* 183 */
  jbyte *	(JNICALL *GetByteArrayElements)	   (JNIEnv *, jbyteArray, jboolean *);              /* 184 */
  jchar *	(JNICALL *GetCharArrayElements)	   (JNIEnv *, jcharArray, jboolean *);              /* 185 */
  jshort *	(JNICALL *GetShortArrayElements)   (JNIEnv *, jshortArray, jboolean *);             /* 186 */
  jint *	(JNICALL *GetIntArrayElements)	   (JNIEnv *, jintArray, jboolean *);               /* 187 */
  jlong *	(JNICALL *GetLongArrayElements)	   (JNIEnv *, jlongArray, jboolean *);              /* 188 */
  jfloat *	(JNICALL *GetFloatArrayElements)   (JNIEnv *, jfloatArray, jboolean *);             /* 189 */
  jdouble *	(JNICALL *GetDoubleArrayElements)  (JNIEnv *, jdoubleArray, jboolean *);            /* 190 */

  void		(JNICALL *ReleaseBooleanArrayElements) (JNIEnv *, jbooleanArray, jboolean *, jint); /* 191 */
  void		(JNICALL *ReleaseByteArrayElements)    (JNIEnv *, jbyteArray, jbyte *, jint);       /* 192 */
  void		(JNICALL *ReleaseCharArrayElements)    (JNIEnv *, jcharArray, jchar *, jint);       /* 193 */
  void		(JNICALL *ReleaseShortArrayElements)   (JNIEnv *, jshortArray, jshort *, jint);     /* 194 */
  void		(JNICALL *ReleaseIntArrayElements)     (JNIEnv *, jintArray, jint *, jint);         /* 195 */
  void		(JNICALL *ReleaseLongArrayElements)    (JNIEnv *, jlongArray, jlong *, jint);       /* 196 */
  void		(JNICALL *ReleaseFloatArrayElements)   (JNIEnv *, jfloatArray, jfloat *, jint);     /* 197 */
  void		(JNICALL *ReleaseDoubleArrayElements)  (JNIEnv *, jdoubleArray, jdouble *, jint);   /* 198 */

  void 		(JNICALL *GetBooleanArrayRegion)   (JNIEnv *, jbooleanArray,jsize, jsize, jboolean *);  /* 199 */
  void 		(JNICALL *GetByteArrayRegion)	   (JNIEnv *, jbyteArray, jsize, jsize, jbyte *);       /* 200 */
  void 		(JNICALL *GetCharArrayRegion)	   (JNIEnv *, jcharArray, jsize, jsize, jchar *);       /* 201 */
  void 		(JNICALL *GetShortArrayRegion)	   (JNIEnv *, jshortArray, jsize, jsize, jshort *);     /* 202 */
  void 		(JNICALL *GetIntArrayRegion)	   (JNIEnv *, jintArray, jsize, jsize, jint *);         /* 203 */
  void 		(JNICALL *GetLongArrayRegion)	   (JNIEnv *, jlongArray, jsize, jsize, jlong *);       /* 204 */
  void 		(JNICALL *GetFloatArrayRegion)	   (JNIEnv *, jfloatArray, jsize, jsize, jfloat *);     /* 205 */
  void 		(JNICALL *GetDoubleArrayRegion)	   (JNIEnv *, jdoubleArray, jsize, jsize, jdouble *);   /* 206 */

  void 		(JNICALL *SetBooleanArrayRegion)   (JNIEnv *, jbooleanArray, jsize, jsize, jboolean *); /* 207 */
  void 		(JNICALL *SetByteArrayRegion)	   (JNIEnv *, jbyteArray, jsize, jsize, jbyte *);       /* 208 */
  void 		(JNICALL *SetCharArrayRegion)	   (JNIEnv *, jcharArray, jsize, jsize, jchar *);       /* 209 */
  void 		(JNICALL *SetShortArrayRegion)	   (JNIEnv *, jshortArray, jsize, jsize, jshort *);     /* 210 */
  void 		(JNICALL *SetIntArrayRegion)	   (JNIEnv *, jintArray, jsize, jsize, jint *);         /* 211 */
  void 		(JNICALL *SetLongArrayRegion)	   (JNIEnv *, jlongArray, jsize, jsize, jlong *);       /* 212 */
  void 		(JNICALL *SetFloatArrayRegion)	   (JNIEnv *, jfloatArray, jsize, jsize, jfloat *);     /* 213 */
  void 		(JNICALL *SetDoubleArrayRegion)	   (JNIEnv *, jdoubleArray, jsize, jsize, jdouble *);   /* 214 */

  jint     (JNICALL *RegisterNatives)              (JNIEnv *, jclass, const JNINativeMethod *, jint);   /* 215 */
  jint     (JNICALL *UnregisterNatives)            (JNIEnv *, jclass);                                  /* 216 */
  jint     (JNICALL *MonitorEnter)                 (JNIEnv *, jobject);                                 /* 217 */
  jint     (JNICALL *MonitorExit)                  (JNIEnv *, jobject);                                 /* 218 */
  jint     (JNICALL *GetJavaVM)                    (JNIEnv *, JavaVM **);                               /* 219 */

  /* ---- JNI 1.2 functions ---- */
  void	   (JNICALL *GetStringRegion)	           (JNIEnv *, jstring, jsize, jsize, jchar *);          /* 220 */
  void     (JNICALL *GetStringUTFRegion)	   (JNIEnv *, jstring, jsize, jsize, char *);           /* 221 */

  void * (JNICALL *GetPrimitiveArrayCritical)      (JNIEnv *, jarray, jboolean *);                      /* 222 */
  void   (JNICALL *ReleasePrimitiveArrayCritical)  (JNIEnv *, jarray, void *, jint);                    /* 223 */

  const jchar * (JNICALL *GetStringCritical)       (JNIEnv *, jstring, jboolean *);                     /* 224 */
  void          (JNICALL *ReleaseStringCritical)   (JNIEnv *, jstring, const jchar *);                  /* 225 */

  jweak  (JNICALL *NewWeakGlobalRef)               (JNIEnv *, jobject);                                 /* 226 */
  void   (JNICALL *DeleteWeakGlobalRef)            (JNIEnv *, jweak);                                   /* 227 */

  jboolean	(JNICALL *ExceptionCheck)	   (JNIEnv *);                                          /* 228 */

  /* ---- JNI 1.4 functions ---- */
  jobject       (JNICALL *NewDirectByteBuffer)     (JNIEnv *, void *, jlong);                           /* 229 */
  void *        (JNICALL *GetDirectBufferAddress)  (JNIEnv *, jobject);                                 /* 230 */
  long          (JNICALL *GetDirectBufferCapacity) (JNIEnv *, jobject);                                 /* 231 */

};

#ifdef __cplusplus
} /* Extern "C" */

struct _Jv_JNIEnv
{
  /* The method table.  */
  const struct JNINativeInterface *p;

  jint GetVersion ()
  { return p->GetVersion (this); }

  jclass DefineClass (const char* char0, jobject obj0, const jbyte * val1, jsize val2)
  { return p->DefineClass (this, char0, obj0, val1, val2); }

  jclass FindClass (const char * val0)
  { return p->FindClass (this, val0); }

  jmethodID FromReflectedMethod (jobject obj0)
  { return p->FromReflectedMethod (this, obj0); }

  jfieldID FromReflectedField (jobject obj0)
  { return p->FromReflectedField (this, obj0); }

  jobject ToReflectedMethod (jclass cl0, jmethodID meth1, jboolean val2)
  { return p->ToReflectedMethod (this, cl0, meth1, val2); }

  jclass GetSuperclass (jclass cl0)
  { return p->GetSuperclass (this, cl0); }

  jboolean IsAssignableFrom (jclass cl0, jclass cl1)
  { return p->IsAssignableFrom (this, cl0, cl1); }

  jobject ToReflectedField (jclass cl0, jfieldID fld1, jboolean val2)
  { return p->ToReflectedField (this, cl0, fld1, val2); }

  jint Throw (jthrowable val0)
  { return p->Throw (this, val0); }

  jint ThrowNew (jclass cl0, const char * val1)
  { return p->ThrowNew (this, cl0, val1); }

  jthrowable ExceptionOccurred ()
  { return p->ExceptionOccurred (this); }

  void ExceptionDescribe ()
  { p->ExceptionDescribe (this); }

  void ExceptionClear ()
  { p->ExceptionClear (this); }

  void FatalError (const char * val0)
  { p->FatalError (this, val0); }

  jint PushLocalFrame (jint val0)
  { return p->PushLocalFrame (this, val0); }

  jobject PopLocalFrame (jobject obj0)
  { return p->PopLocalFrame (this, obj0); }

  jobject NewGlobalRef (jobject obj0)
  { return p->NewGlobalRef (this, obj0); }

  void DeleteGlobalRef (jobject obj0)
  { p->DeleteGlobalRef (this, obj0); }

  void DeleteLocalRef (jobject obj0)
  { p->DeleteLocalRef (this, obj0); }

  jboolean IsSameObject (jobject obj0, jobject obj1)
  { return p->IsSameObject (this, obj0, obj1); }

  jobject NewLocalRef (jobject obj0)
  { return p->NewLocalRef (this, obj0); }

  jint EnsureLocalCapacity (jint val0)
  { return p->EnsureLocalCapacity (this, val0); }

  jobject AllocObject (jclass cl0)
  { return p->AllocObject (this, cl0); }

  jobject NewObject (jclass cl0, jmethodID meth1, ...)
  {
    _Jv_va_list args;
    va_start (args, meth1);
    jobject result = p->NewObjectV (this, cl0, meth1, args);
    va_end (args);
    return result;
  }

  jobject NewObjectV (jclass cl0, jmethodID meth1, _Jv_va_list val2)
  { return p->NewObjectV (this, cl0, meth1, val2); }

  jobject NewObjectA (jclass cl0, jmethodID meth1, jvalue * val2)
  { return p->NewObjectA (this, cl0, meth1, val2); }

  jclass GetObjectClass (jobject obj0)
  { return p->GetObjectClass (this, obj0); }

  jboolean IsInstanceOf (jobject obj0, jclass cl1)
  { return p->IsInstanceOf (this, obj0, cl1); }

  jmethodID GetMethodID (jclass cl0, const char * val1, const char * val2)
  { return p->GetMethodID (this, cl0, val1, val2); }

  jobject CallObjectMethod (jobject obj0, jmethodID meth1, ...)
  {
    _Jv_va_list args;
    va_start (args, meth1);
    jobject result = p->CallObjectMethodV (this, obj0, meth1, args);
    va_end (args);
    return result;
  }

  jobject CallObjectMethodV (jobject obj0, jmethodID meth1, _Jv_va_list val2)
  { return p->CallObjectMethodV (this, obj0, meth1, val2); }

  jobject CallObjectMethodA (jobject obj0, jmethodID meth1, jvalue * val2)
  { return p->CallObjectMethodA (this, obj0, meth1, val2); }

  jboolean CallBooleanMethod (jobject obj0, jmethodID meth1, ...)
  {
    _Jv_va_list args;
    va_start (args, meth1);
    jboolean result = p->CallBooleanMethodV (this, obj0, meth1, args);
    va_end (args);
    return result;
  }

  jboolean CallBooleanMethodV (jobject obj0, jmethodID meth1, _Jv_va_list val2)
  { return p->CallBooleanMethodV (this, obj0, meth1, val2); }

  jboolean CallBooleanMethodA (jobject obj0, jmethodID meth1, jvalue * val2)
  { return p->CallBooleanMethodA (this, obj0, meth1, val2); }

  jbyte CallByteMethod (jobject obj0, jmethodID meth1, ...)
  {
    _Jv_va_list args;
    va_start (args, meth1);
    jbyte result = p->CallByteMethodV (this, obj0, meth1, args);
    va_end (args);
    return result;
  }

  jbyte CallByteMethodV (jobject obj0, jmethodID meth1, _Jv_va_list val2)
  { return p->CallByteMethodV (this, obj0, meth1, val2); }

  jbyte CallByteMethodA (jobject obj0, jmethodID meth1, jvalue * val2)
  { return p->CallByteMethodA (this, obj0, meth1, val2); }

  jchar CallCharMethod (jobject obj0, jmethodID meth1, ...)
  {
    _Jv_va_list args;
    va_start (args, meth1);
    jchar result = p->CallCharMethodV (this, obj0, meth1, args);
    va_end (args);
    return result;
  }

  jchar CallCharMethodV (jobject obj0, jmethodID meth1, _Jv_va_list val2)
  { return p->CallCharMethodV (this, obj0, meth1, val2); }

  jchar CallCharMethodA (jobject obj0, jmethodID meth1, jvalue * val2)
  { return p->CallCharMethodA (this, obj0, meth1, val2); }

  jshort CallShortMethod (jobject obj0, jmethodID meth1, ...)
  {
    _Jv_va_list args;
    va_start (args, meth1);
    jshort result = p->CallShortMethodV (this, obj0, meth1, args);
    va_end (args);
    return result;
  }

  jshort CallShortMethodV (jobject obj0, jmethodID meth1, _Jv_va_list val2)
  { return p->CallShortMethodV (this, obj0, meth1, val2); }

  jshort CallShortMethodA (jobject obj0, jmethodID meth1, jvalue * val2)
  { return p->CallShortMethodA (this, obj0, meth1, val2); }

  jint CallIntMethod (jobject obj0, jmethodID meth1, ...)
  {
    _Jv_va_list args;
    va_start (args, meth1);
    jint result = p->CallIntMethodV (this, obj0, meth1, args);
    va_end (args);
    return result;
  }

  jint CallIntMethodV (jobject obj0, jmethodID meth1, _Jv_va_list val2)
  { return p->CallIntMethodV (this, obj0, meth1, val2); }

  jint CallIntMethodA (jobject obj0, jmethodID meth1, jvalue * val2)
  { return p->CallIntMethodA (this, obj0, meth1, val2); }

  jlong CallLongMethod (jobject obj0, jmethodID meth1, ...)
  {
    _Jv_va_list args;
    va_start (args, meth1);
    jlong result = p->CallLongMethodV (this, obj0, meth1, args);
    va_end (args);
    return result;
  }

  jlong CallLongMethodV (jobject obj0, jmethodID meth1, _Jv_va_list val2)
  { return p->CallLongMethodV (this, obj0, meth1, val2); }

  jlong CallLongMethodA (jobject obj0, jmethodID meth1, jvalue * val2)
  { return p->CallLongMethodA (this, obj0, meth1, val2); }

  jfloat CallFloatMethod (jobject obj0, jmethodID meth1, ...)
  {
    _Jv_va_list args;
    va_start (args, meth1);
    jfloat result = p->CallFloatMethodV (this, obj0, meth1, args);
    va_end (args);
    return result;
  }

  jfloat CallFloatMethodV (jobject obj0, jmethodID meth1, _Jv_va_list val2)
  { return p->CallFloatMethodV (this, obj0, meth1, val2); }

  jfloat CallFloatMethodA (jobject obj0, jmethodID meth1, jvalue * val2)
  { return p->CallFloatMethodA (this, obj0, meth1, val2); }

  jdouble CallDoubleMethod (jobject obj0, jmethodID meth1, ...)
  {
    _Jv_va_list args;
    va_start (args, meth1);
    jdouble result = p->CallDoubleMethodV (this, obj0, meth1, args);
    va_end (args);
    return result;
  }

  jdouble CallDoubleMethodV (jobject obj0, jmethodID meth1, _Jv_va_list val2)
  { return p->CallDoubleMethodV (this, obj0, meth1, val2); }

  jdouble CallDoubleMethodA (jobject obj0, jmethodID meth1, jvalue * val2)
  { return p->CallDoubleMethodA (this, obj0, meth1, val2); }

  void CallVoidMethod (jobject obj0, jmethodID meth1, ...)
  {
    _Jv_va_list args;
    va_start (args, meth1);
    p->CallVoidMethodV (this, obj0, meth1, args);
    va_end (args);
  }

  void CallVoidMethodV (jobject obj0, jmethodID meth1, _Jv_va_list val2)
  { p->CallVoidMethodV (this, obj0, meth1, val2); }

  void CallVoidMethodA (jobject obj0, jmethodID meth1, jvalue * val2)
  { p->CallVoidMethodA (this, obj0, meth1, val2); }

  jobject CallNonvirtualObjectMethod (jobject obj0, jclass cl1, jmethodID meth2, ...)
  {
    _Jv_va_list args;
    va_start (args, meth2);
    jobject result = p->CallNonvirtualObjectMethodV (this, obj0, cl1, meth2, args);
    va_end (args);
    return result;
  }

  jobject CallNonvirtualObjectMethodV (jobject obj0, jclass cl1, jmethodID meth2, _Jv_va_list val3)
  { return p->CallNonvirtualObjectMethodV (this, obj0, cl1, meth2, val3); }

  jobject CallNonvirtualObjectMethodA (jobject obj0, jclass cl1, jmethodID meth2, jvalue * val3)
  { return p->CallNonvirtualObjectMethodA (this, obj0, cl1, meth2, val3); }

  jboolean CallNonvirtualBooleanMethod (jobject obj0, jclass cl1, jmethodID meth2, ...)
  {
    _Jv_va_list args;
    va_start (args, meth2);
    jboolean result = p->CallNonvirtualBooleanMethodV (this, obj0, cl1, meth2, args);
    va_end (args);
    return result;
  }

  jboolean CallNonvirtualBooleanMethodV (jobject obj0, jclass cl1, jmethodID meth2, _Jv_va_list val3)
  { return p->CallNonvirtualBooleanMethodV (this, obj0, cl1, meth2, val3); }

  jboolean CallNonvirtualBooleanMethodA (jobject obj0, jclass cl1, jmethodID meth2, jvalue * val3)
  { return p->CallNonvirtualBooleanMethodA (this, obj0, cl1, meth2, val3); }

  jbyte CallNonvirtualByteMethod (jobject obj0, jclass cl1, jmethodID meth2, ...)
  {
    _Jv_va_list args;
    va_start (args, meth2);
    jbyte result = p->CallNonvirtualByteMethodV (this, obj0, cl1, meth2, args);
    va_end (args);
    return result;
  }

  jbyte CallNonvirtualByteMethodV (jobject obj0, jclass cl1, jmethodID meth2, _Jv_va_list val3)
  { return p->CallNonvirtualByteMethodV (this, obj0, cl1, meth2, val3); }

  jbyte CallNonvirtualByteMethodA (jobject obj0, jclass cl1, jmethodID meth2, jvalue * val3)
  { return p->CallNonvirtualByteMethodA (this, obj0, cl1, meth2, val3); }

  jchar CallNonvirtualCharMethod (jobject obj0, jclass cl1, jmethodID meth2, ...)
  {
    _Jv_va_list args;
    va_start (args, meth2);
    jchar result = p->CallNonvirtualCharMethodV (this, obj0, cl1, meth2, args);
    va_end (args);
    return result;
  }

  jchar CallNonvirtualCharMethodV (jobject obj0, jclass cl1, jmethodID meth2, _Jv_va_list val3)
  { return p->CallNonvirtualCharMethodV (this, obj0, cl1, meth2, val3); }

  jchar CallNonvirtualCharMethodA (jobject obj0, jclass cl1, jmethodID meth2, jvalue * val3)
  { return p->CallNonvirtualCharMethodA (this, obj0, cl1, meth2, val3); }

  jshort CallNonvirtualShortMethod (jobject obj0, jclass cl1, jmethodID meth2, ...)
  {
    _Jv_va_list args;
    va_start (args, meth2);
    jshort result = p->CallNonvirtualShortMethodV (this, obj0, cl1, meth2, args);
    va_end (args);
    return result;
  }

  jshort CallNonvirtualShortMethodV (jobject obj0, jclass cl1, jmethodID meth2, _Jv_va_list val3)
  { return p->CallNonvirtualShortMethodV (this, obj0, cl1, meth2, val3); }

  jshort CallNonvirtualShortMethodA (jobject obj0, jclass cl1, jmethodID meth2, jvalue * val3)
  { return p->CallNonvirtualShortMethodA (this, obj0, cl1, meth2, val3); }

  jint CallNonvirtualIntMethod (jobject obj0, jclass cl1, jmethodID meth2, ...)
  {
    _Jv_va_list args;
    va_start (args, meth2);
    jint result = p->CallNonvirtualIntMethodV (this, obj0, cl1, meth2, args);
    va_end (args);
    return result;
  }

  jint CallNonvirtualIntMethodV (jobject obj0, jclass cl1, jmethodID meth2, _Jv_va_list val3)
  { return p->CallNonvirtualIntMethodV (this, obj0, cl1, meth2, val3); }

  jint CallNonvirtualIntMethodA (jobject obj0, jclass cl1, jmethodID meth2, jvalue * val3)
  { return p->CallNonvirtualIntMethodA (this, obj0, cl1, meth2, val3); }

  jlong CallNonvirtualLongMethod (jobject obj0, jclass cl1, jmethodID meth2, ...)
  {
    _Jv_va_list args;
    va_start (args, meth2);
    jlong result = p->CallNonvirtualLongMethodV (this, obj0, cl1, meth2, args);
    va_end (args);
    return result;
  }

  jlong CallNonvirtualLongMethodV (jobject obj0, jclass cl1, jmethodID meth2, _Jv_va_list val3)
  { return p->CallNonvirtualLongMethodV (this, obj0, cl1, meth2, val3); }

  jlong CallNonvirtualLongMethodA (jobject obj0, jclass cl1, jmethodID meth2, jvalue * val3)
  { return p->CallNonvirtualLongMethodA (this, obj0, cl1, meth2, val3); }

  jfloat CallNonvirtualFloatMethod (jobject obj0, jclass cl1, jmethodID meth2, ...)
  {
    _Jv_va_list args;
    va_start (args, meth2);
    jfloat result = p->CallNonvirtualFloatMethodV (this, obj0, cl1, meth2, args);
    va_end (args);
    return result;
  }

  jfloat CallNonvirtualFloatMethodV (jobject obj0, jclass cl1, jmethodID meth2, _Jv_va_list val3)
  { return p->CallNonvirtualFloatMethodV (this, obj0, cl1, meth2, val3); }

  jfloat CallNonvirtualFloatMethodA (jobject obj0, jclass cl1, jmethodID meth2, jvalue * val3)
  { return p->CallNonvirtualFloatMethodA (this, obj0, cl1, meth2, val3); }

  jdouble CallNonvirtualDoubleMethod (jobject obj0, jclass cl1, jmethodID meth2, ...)
  {
    _Jv_va_list args;
    va_start (args, meth2);
    jdouble result = p->CallNonvirtualDoubleMethodV (this, obj0, cl1, meth2, args);
    va_end (args);
    return result;
  }

  jdouble CallNonvirtualDoubleMethodV (jobject obj0, jclass cl1, jmethodID meth2, _Jv_va_list val3)
  { return p->CallNonvirtualDoubleMethodV (this, obj0, cl1, meth2, val3); }

  jdouble CallNonvirtualDoubleMethodA (jobject obj0, jclass cl1, jmethodID meth2, jvalue * val3)
  { return p->CallNonvirtualDoubleMethodA (this, obj0, cl1, meth2, val3); }

  void CallNonvirtualVoidMethod (jobject obj0, jclass cl1, jmethodID meth2, ...)
  {
    _Jv_va_list args;
    va_start (args, meth2);
    p->CallNonvirtualVoidMethodV (this, obj0, cl1, meth2, args);
    va_end (args);
  }

  void CallNonvirtualVoidMethodV (jobject obj0, jclass cl1, jmethodID meth2, _Jv_va_list val3)
  { p->CallNonvirtualVoidMethodV (this, obj0, cl1, meth2, val3); }

  void CallNonvirtualVoidMethodA (jobject obj0, jclass cl1, jmethodID meth2, jvalue * val3)
  { p->CallNonvirtualVoidMethodA (this, obj0, cl1, meth2, val3); }

  jfieldID GetFieldID (jclass cl0, const char * val1, const char * val2)
  { return p->GetFieldID (this, cl0, val1, val2); }

  jobject GetObjectField (jobject obj0, jfieldID fld1)
  { return p->GetObjectField (this, obj0, fld1); }

  jboolean GetBooleanField (jobject obj0, jfieldID fld1)
  { return p->GetBooleanField (this, obj0, fld1); }

  jbyte GetByteField (jobject obj0, jfieldID fld1)
  { return p->GetByteField (this, obj0, fld1); }

  jchar GetCharField (jobject obj0, jfieldID fld1)
  { return p->GetCharField (this, obj0, fld1); }

  jshort GetShortField (jobject obj0, jfieldID fld1)
  { return p->GetShortField (this, obj0, fld1); }

  jint GetIntField (jobject obj0, jfieldID fld1)
  { return p->GetIntField (this, obj0, fld1); }

  jlong GetLongField (jobject obj0, jfieldID fld1)
  { return p->GetLongField (this, obj0, fld1); }

  jfloat GetFloatField (jobject obj0, jfieldID fld1)
  { return p->GetFloatField (this, obj0, fld1); }

  jdouble GetDoubleField (jobject obj0, jfieldID fld1)
  { return p->GetDoubleField (this, obj0, fld1); }

  void SetObjectField (jobject obj0, jfieldID fld1, jobject obj2)
  { p->SetObjectField (this, obj0, fld1, obj2); }

  void SetBooleanField (jobject obj0, jfieldID fld1, jboolean val2)
  { p->SetBooleanField (this, obj0, fld1, val2); }

  void SetByteField (jobject obj0, jfieldID fld1, jbyte val2)
  { p->SetByteField (this, obj0, fld1, val2); }

  void SetCharField (jobject obj0, jfieldID fld1, jchar val2)
  { p->SetCharField (this, obj0, fld1, val2); }

  void SetShortField (jobject obj0, jfieldID fld1, jshort val2)
  { p->SetShortField (this, obj0, fld1, val2); }

  void SetIntField (jobject obj0, jfieldID fld1, jint val2)
  { p->SetIntField (this, obj0, fld1, val2); }

  void SetLongField (jobject obj0, jfieldID fld1, jlong val2)
  { p->SetLongField (this, obj0, fld1, val2); }

  void SetFloatField (jobject obj0, jfieldID fld1, jfloat val2)
  { p->SetFloatField (this, obj0, fld1, val2); }

  void SetDoubleField (jobject obj0, jfieldID fld1, jdouble val2)
  { p->SetDoubleField (this, obj0, fld1, val2); }

  jmethodID GetStaticMethodID (jclass cl0, const char * val1, const char * val2)
  { return p->GetStaticMethodID (this, cl0, val1, val2); }

  jobject CallStaticObjectMethod (jclass cl0, jmethodID meth1, ...)
  {
    _Jv_va_list args;
    va_start (args, meth1);
    jobject result = p->CallStaticObjectMethodV (this, cl0, meth1, args);
    va_end (args);
    return result;
  }

  jobject CallStaticObjectMethodV (jclass cl0, jmethodID meth1, _Jv_va_list val2)
  { return p->CallStaticObjectMethodV (this, cl0, meth1, val2); }

  jobject CallStaticObjectMethodA (jclass cl0, jmethodID meth1, jvalue * val2)
  { return p->CallStaticObjectMethodA (this, cl0, meth1, val2); }

  jboolean CallStaticBooleanMethod (jclass cl0, jmethodID meth1, ...)
  {
    _Jv_va_list args;
    va_start (args, meth1);
    jboolean result = p->CallStaticBooleanMethodV (this, cl0, meth1, args);
    va_end (args);
    return result;
  }

  jboolean CallStaticBooleanMethodV (jclass cl0, jmethodID meth1, _Jv_va_list val2)
  { return p->CallStaticBooleanMethodV (this, cl0, meth1, val2); }

  jboolean CallStaticBooleanMethodA (jclass cl0, jmethodID meth1, jvalue * val2)
  { return p->CallStaticBooleanMethodA (this, cl0, meth1, val2); }

  jbyte CallStaticByteMethod (jclass cl0, jmethodID meth1, ...)
  {
    _Jv_va_list args;
    va_start (args, meth1);
    jbyte result = p->CallStaticByteMethodV (this, cl0, meth1, args);
    va_end (args);
    return result;
  }

  jbyte CallStaticByteMethodV (jclass cl0, jmethodID meth1, _Jv_va_list val2)
  { return p->CallStaticByteMethodV (this, cl0, meth1, val2); }

  jbyte CallStaticByteMethodA (jclass cl0, jmethodID meth1, jvalue * val2)
  { return p->CallStaticByteMethodA (this, cl0, meth1, val2); }

  jchar CallStaticCharMethod (jclass cl0, jmethodID meth1, ...)
  {
    _Jv_va_list args;
    va_start (args, meth1);
    jchar result = p->CallStaticCharMethodV (this, cl0, meth1, args);
    va_end (args);
    return result;
  }

  jchar CallStaticCharMethodV (jclass cl0, jmethodID meth1, _Jv_va_list val2)
  { return p->CallStaticCharMethodV (this, cl0, meth1, val2); }

  jchar CallStaticCharMethodA (jclass cl0, jmethodID meth1, jvalue * val2)
  { return p->CallStaticCharMethodA (this, cl0, meth1, val2); }

  jshort CallStaticShortMethod (jclass cl0, jmethodID meth1, ...)
  {
    _Jv_va_list args;
    va_start (args, meth1);
    jshort result = p->CallStaticShortMethodV (this, cl0, meth1, args);
    va_end (args);
    return result;
  }

  jshort CallStaticShortMethodV (jclass cl0, jmethodID meth1, _Jv_va_list val2)
  { return p->CallStaticShortMethodV (this, cl0, meth1, val2); }

  jshort CallStaticShortMethodA (jclass cl0, jmethodID meth1, jvalue * val2)
  { return p->CallStaticShortMethodA (this, cl0, meth1, val2); }

  jint CallStaticIntMethod (jclass cl0, jmethodID meth1, ...)
  {
    _Jv_va_list args;
    va_start (args, meth1);
    jint result = p->CallStaticIntMethodV (this, cl0, meth1, args);
    va_end (args);
    return result;
  }

  jint CallStaticIntMethodV (jclass cl0, jmethodID meth1, _Jv_va_list val2)
  { return p->CallStaticIntMethodV (this, cl0, meth1, val2); }

  jint CallStaticIntMethodA (jclass cl0, jmethodID meth1, jvalue * val2)
  { return p->CallStaticIntMethodA (this, cl0, meth1, val2); }

  jlong CallStaticLongMethod (jclass cl0, jmethodID meth1, ...)
  {
    _Jv_va_list args;
    va_start (args, meth1);
    jlong result = p->CallStaticLongMethodV (this, cl0, meth1, args);
    va_end (args);
    return result;
  }

  jlong CallStaticLongMethodV (jclass cl0, jmethodID meth1, _Jv_va_list val2)
  { return p->CallStaticLongMethodV (this, cl0, meth1, val2); }

  jlong CallStaticLongMethodA (jclass cl0, jmethodID meth1, jvalue * val2)
  { return p->CallStaticLongMethodA (this, cl0, meth1, val2); }

  jfloat CallStaticFloatMethod (jclass cl0, jmethodID meth1, ...)
  {
    _Jv_va_list args;
    va_start (args, meth1);
    jfloat result = p->CallStaticFloatMethodV (this, cl0, meth1, args);
    va_end (args);
    return result;
  }

  jfloat CallStaticFloatMethodV (jclass cl0, jmethodID meth1, _Jv_va_list val2)
  { return p->CallStaticFloatMethodV (this, cl0, meth1, val2); }

  jfloat CallStaticFloatMethodA (jclass cl0, jmethodID meth1, jvalue * val2)
  { return p->CallStaticFloatMethodA (this, cl0, meth1, val2); }

  jdouble CallStaticDoubleMethod (jclass cl0, jmethodID meth1, ...)
  {
    _Jv_va_list args;
    va_start (args, meth1);
    jdouble result = p->CallStaticDoubleMethodV (this, cl0, meth1, args);
    va_end (args);
    return result;
  }

  jdouble CallStaticDoubleMethodV (jclass cl0, jmethodID meth1, _Jv_va_list val2)
  { return p->CallStaticDoubleMethodV (this, cl0, meth1, val2); }

  jdouble CallStaticDoubleMethodA (jclass cl0, jmethodID meth1, jvalue * val2)
  { return p->CallStaticDoubleMethodA (this, cl0, meth1, val2); }

  void CallStaticVoidMethod (jclass cl0, jmethodID meth1, ...)
  {
    _Jv_va_list args;
    va_start (args, meth1);
    p->CallStaticVoidMethodV (this, cl0, meth1, args);
    va_end (args);
  }

  void CallStaticVoidMethodV (jclass cl0, jmethodID meth1, _Jv_va_list val2)
  { p->CallStaticVoidMethodV (this, cl0, meth1, val2); }

  void CallStaticVoidMethodA (jclass cl0, jmethodID meth1, jvalue * val2)
  { p->CallStaticVoidMethodA (this, cl0, meth1, val2); }

  jfieldID GetStaticFieldID (jclass cl0, const char * val1, const char * val2)
  { return p->GetStaticFieldID (this, cl0, val1, val2); }

  jobject GetStaticObjectField (jclass cl0, jfieldID fld1)
  { return p->GetStaticObjectField (this, cl0, fld1); }

  jboolean GetStaticBooleanField (jclass cl0, jfieldID fld1)
  { return p->GetStaticBooleanField (this, cl0, fld1); }

  jbyte GetStaticByteField (jclass cl0, jfieldID fld1)
  { return p->GetStaticByteField (this, cl0, fld1); }

  jchar GetStaticCharField (jclass cl0, jfieldID fld1)
  { return p->GetStaticCharField (this, cl0, fld1); }

  jshort GetStaticShortField (jclass cl0, jfieldID fld1)
  { return p->GetStaticShortField (this, cl0, fld1); }

  jint GetStaticIntField (jclass cl0, jfieldID fld1)
  { return p->GetStaticIntField (this, cl0, fld1); }

  jlong GetStaticLongField (jclass cl0, jfieldID fld1)
  { return p->GetStaticLongField (this, cl0, fld1); }

  jfloat GetStaticFloatField (jclass cl0, jfieldID fld1)
  { return p->GetStaticFloatField (this, cl0, fld1); }

  jdouble GetStaticDoubleField (jclass cl0, jfieldID fld1)
  { return p->GetStaticDoubleField (this, cl0, fld1); }

  void SetStaticObjectField (jclass cl0, jfieldID fld1, jobject obj2)
  { p->SetStaticObjectField (this, cl0, fld1, obj2); }

  void SetStaticBooleanField (jclass cl0, jfieldID fld1, jboolean val2)
  { p->SetStaticBooleanField (this, cl0, fld1, val2); }

  void SetStaticByteField (jclass cl0, jfieldID fld1, jbyte val2)
  { p->SetStaticByteField (this, cl0, fld1, val2); }

  void SetStaticCharField (jclass cl0, jfieldID fld1, jchar val2)
  { p->SetStaticCharField (this, cl0, fld1, val2); }

  void SetStaticShortField (jclass cl0, jfieldID fld1, jshort val2)
  { p->SetStaticShortField (this, cl0, fld1, val2); }

  void SetStaticIntField (jclass cl0, jfieldID fld1, jint val2)
  { p->SetStaticIntField (this, cl0, fld1, val2); }

  void SetStaticLongField (jclass cl0, jfieldID fld1, jlong val2)
  { p->SetStaticLongField (this, cl0, fld1, val2); }

  void SetStaticFloatField (jclass cl0, jfieldID fld1, jfloat val2)
  { p->SetStaticFloatField (this, cl0, fld1, val2); }

  void SetStaticDoubleField (jclass cl0, jfieldID fld1, jdouble val2)
  { p->SetStaticDoubleField (this, cl0, fld1, val2); }

  jstring NewString (const jchar * val0, jsize val1)
  { return p->NewString (this, val0, val1); }

  jsize GetStringLength (jstring val0)
  { return p->GetStringLength (this, val0); }

  const jchar * GetStringChars (jstring val0, jboolean * val1)
  { return p->GetStringChars (this, val0, val1); }

  void ReleaseStringChars (jstring val0, const jchar * val1)
  { p->ReleaseStringChars (this, val0, val1); }

  jstring NewStringUTF (const char * val0)
  { return p->NewStringUTF (this, val0); }

  jsize GetStringUTFLength (jstring val0)
  { return p->GetStringUTFLength (this, val0); }

  const char * GetStringUTFChars (jstring val0, jboolean * val1)
  { return p->GetStringUTFChars (this, val0, val1); }

  void ReleaseStringUTFChars (jstring val0, const char * val1)
  { p->ReleaseStringUTFChars (this, val0, val1); }

  jsize GetArrayLength (jarray val0)
  { return p->GetArrayLength (this, val0); }

  jobjectArray NewObjectArray (jsize val0, jclass cl1, jobject obj2)
  { return p->NewObjectArray (this, val0, cl1, obj2); }

  jobject GetObjectArrayElement (jobjectArray val0, jsize val1)
  { return p->GetObjectArrayElement (this, val0, val1); }

  void SetObjectArrayElement (jobjectArray val0, jsize val1, jobject obj2)
  { p->SetObjectArrayElement (this, val0, val1, obj2); }

  jbooleanArray NewBooleanArray (jsize val0)
  { return p->NewBooleanArray (this, val0); }

  jbyteArray NewByteArray (jsize val0)
  { return p->NewByteArray (this, val0); }

  jcharArray NewCharArray (jsize val0)
  { return p->NewCharArray (this, val0); }

  jshortArray NewShortArray (jsize val0)
  { return p->NewShortArray (this, val0); }

  jintArray NewIntArray (jsize val0)
  { return p->NewIntArray (this, val0); }

  jlongArray NewLongArray (jsize val0)
  { return p->NewLongArray (this, val0); }

  jfloatArray NewFloatArray (jsize val0)
  { return p->NewFloatArray (this, val0); }

  jdoubleArray NewDoubleArray (jsize val0)
  { return p->NewDoubleArray (this, val0); }

  jboolean * GetBooleanArrayElements (jbooleanArray val0, jboolean * val1)
  { return p->GetBooleanArrayElements (this, val0, val1); }

  jbyte * GetByteArrayElements (jbyteArray val0, jboolean * val1)
  { return p->GetByteArrayElements (this, val0, val1); }

  jchar * GetCharArrayElements (jcharArray val0, jboolean * val1)
  { return p->GetCharArrayElements (this, val0, val1); }

  jshort * GetShortArrayElements (jshortArray val0, jboolean * val1)
  { return p->GetShortArrayElements (this, val0, val1); }

  jint * GetIntArrayElements (jintArray val0, jboolean * val1)
  { return p->GetIntArrayElements (this, val0, val1); }

  jlong * GetLongArrayElements (jlongArray val0, jboolean * val1)
  { return p->GetLongArrayElements (this, val0, val1); }

  jfloat * GetFloatArrayElements (jfloatArray val0, jboolean * val1)
  { return p->GetFloatArrayElements (this, val0, val1); }

  jdouble * GetDoubleArrayElements (jdoubleArray val0, jboolean * val1)
  { return p->GetDoubleArrayElements (this, val0, val1); }

  void ReleaseBooleanArrayElements (jbooleanArray val0, jboolean * val1, jint val2)
  { p->ReleaseBooleanArrayElements (this, val0, val1, val2); }

  void ReleaseByteArrayElements (jbyteArray val0, jbyte * val1, jint val2)
  { p->ReleaseByteArrayElements (this, val0, val1, val2); }

  void ReleaseCharArrayElements (jcharArray val0, jchar * val1, jint val2)
  { p->ReleaseCharArrayElements (this, val0, val1, val2); }

  void ReleaseShortArrayElements (jshortArray val0, jshort * val1, jint val2)
  { p->ReleaseShortArrayElements (this, val0, val1, val2); }

  void ReleaseIntArrayElements (jintArray val0, jint * val1, jint val2)
  { p->ReleaseIntArrayElements (this, val0, val1, val2); }

  void ReleaseLongArrayElements (jlongArray val0, jlong * val1, jint val2)
  { p->ReleaseLongArrayElements (this, val0, val1, val2); }

  void ReleaseFloatArrayElements (jfloatArray val0, jfloat * val1, jint val2)
  { p->ReleaseFloatArrayElements (this, val0, val1, val2); }

  void ReleaseDoubleArrayElements (jdoubleArray val0, jdouble * val1, jint val2)
  { p->ReleaseDoubleArrayElements (this, val0, val1, val2); }

  void GetBooleanArrayRegion (jbooleanArray val0, jsize val1, jsize val2, jboolean * val3)
  { p->GetBooleanArrayRegion (this, val0, val1, val2, val3); }

  void GetByteArrayRegion (jbyteArray val0, jsize val1, jsize val2, jbyte * val3)
  { p->GetByteArrayRegion (this, val0, val1, val2, val3); }

  void GetCharArrayRegion (jcharArray val0, jsize val1, jsize val2, jchar * val3)
  { p->GetCharArrayRegion (this, val0, val1, val2, val3); }

  void GetShortArrayRegion (jshortArray val0, jsize val1, jsize val2, jshort * val3)
  { p->GetShortArrayRegion (this, val0, val1, val2, val3); }

  void GetIntArrayRegion (jintArray val0, jsize val1, jsize val2, jint * val3)
  { p->GetIntArrayRegion (this, val0, val1, val2, val3); }

  void GetLongArrayRegion (jlongArray val0, jsize val1, jsize val2, jlong * val3)
  { p->GetLongArrayRegion (this, val0, val1, val2, val3); }

  void GetFloatArrayRegion (jfloatArray val0, jsize val1, jsize val2, jfloat * val3)
  { p->GetFloatArrayRegion (this, val0, val1, val2, val3); }

  void GetDoubleArrayRegion (jdoubleArray val0, jsize val1, jsize val2, jdouble * val3)
  { p->GetDoubleArrayRegion (this, val0, val1, val2, val3); }

  void SetBooleanArrayRegion (jbooleanArray val0, jsize val1, jsize val2, jboolean * val3)
  { p->SetBooleanArrayRegion (this, val0, val1, val2, val3); }

  void SetByteArrayRegion (jbyteArray val0, jsize val1, jsize val2, jbyte * val3)
  { p->SetByteArrayRegion (this, val0, val1, val2, val3); }

  void SetCharArrayRegion (jcharArray val0, jsize val1, jsize val2, jchar * val3)
  { p->SetCharArrayRegion (this, val0, val1, val2, val3); }

  void SetShortArrayRegion (jshortArray val0, jsize val1, jsize val2, jshort * val3)
  { p->SetShortArrayRegion (this, val0, val1, val2, val3); }

  void SetIntArrayRegion (jintArray val0, jsize val1, jsize val2, jint * val3)
  { p->SetIntArrayRegion (this, val0, val1, val2, val3); }

  void SetLongArrayRegion (jlongArray val0, jsize val1, jsize val2, jlong * val3)
  { p->SetLongArrayRegion (this, val0, val1, val2, val3); }

  void SetFloatArrayRegion (jfloatArray val0, jsize val1, jsize val2, jfloat * val3)
  { p->SetFloatArrayRegion (this, val0, val1, val2, val3); }

  void SetDoubleArrayRegion (jdoubleArray val0, jsize val1, jsize val2, jdouble * val3)
  { p->SetDoubleArrayRegion (this, val0, val1, val2, val3); }

  jint RegisterNatives (jclass cl0, const JNINativeMethod * val1, jint val2)
  { return p->RegisterNatives (this, cl0, val1, val2); }

  jint UnregisterNatives (jclass cl0)
  { return p->UnregisterNatives (this, cl0); }

  jint MonitorEnter (jobject obj0)
  { return p->MonitorEnter (this, obj0); }

  jint MonitorExit (jobject obj0)
  { return p->MonitorExit (this, obj0); }

  jint GetJavaVM (JavaVM ** val0)
  { return p->GetJavaVM (this, val0); }

  void GetStringRegion (jstring val0, jsize val1, jsize val2, jchar * val3)
  { p->GetStringRegion (this, val0, val1, val2, val3); }

  void GetStringUTFRegion (jstring val0, jsize val1, jsize val2, char * val3)
  { p->GetStringUTFRegion (this, val0, val1, val2, val3); }

  void * GetPrimitiveArrayCritical (jarray val0, jboolean * val1)
  { return p->GetPrimitiveArrayCritical (this, val0, val1); }

  void ReleasePrimitiveArrayCritical (jarray val0, void * val1, jint val2)
  { p->ReleasePrimitiveArrayCritical (this, val0, val1, val2); }

  const jchar * GetStringCritical (jstring val0, jboolean * val1)
  { return p->GetStringCritical (this, val0, val1); }

  void ReleaseStringCritical (jstring val0, const jchar * val1)
  { p->ReleaseStringCritical (this, val0, val1); }

  jweak NewWeakGlobalRef (jobject obj0)
  { return p->NewWeakGlobalRef (this, obj0); }

  void DeleteWeakGlobalRef (jweak val0)
  { p->DeleteWeakGlobalRef (this, val0); }

  jboolean ExceptionCheck ()
  { return p->ExceptionCheck (this); }

  jobject NewDirectByteBuffer (void * val1, jlong val2)
  { return p->NewDirectByteBuffer (this, val1, val2); }

  void * GetDirectBufferAddress (jobject val1)
  { return p->GetDirectBufferAddress (this, val1); }

  long GetDirectBufferCapacity (jobject val1)
  { return p->GetDirectBufferCapacity (this, val1); }

};
#endif /* __cplusplus */

/*
 * Invocation API.
 */

struct JNIInvokeInterface
{
  void *reserved0;
  void *reserved1;
  void *reserved2;

  jint (JNICALL *DestroyJavaVM)               (JavaVM *);
  jint (JNICALL *AttachCurrentThread)         (JavaVM *, void **, void *);
  jint (JNICALL *DetachCurrentThread)         (JavaVM *);
  jint (JNICALL *GetEnv)                      (JavaVM *, void **, jint);
  jint (JNICALL *AttachCurrentThreadAsDaemon) (JavaVM *, void **, void *);

};

#ifdef __cplusplus

struct _Jv_JavaVM
{
  const struct JNIInvokeInterface *functions;

  jint DestroyJavaVM ()
  { return functions->DestroyJavaVM (this); }

  jint AttachCurrentThread (void **penv, void *args)
  { return functions->AttachCurrentThread (this, penv, args); }

  jint DetachCurrentThread ()
  { return functions->DetachCurrentThread (this); }

  jint GetEnv (void **penv, jint version)
  { return functions->GetEnv (this, penv, version); }

  jint AttachCurrentThreadAsDaemon (void **penv, void *args)
  { return functions->AttachCurrentThreadAsDaemon (this, penv, args); }
};
#endif /* __cplusplus */

typedef struct JavaVMAttachArgs
{
  jint version;			/* Must be JNI_VERSION_1_2.  */
  char *name;			/* The name of the thread (or NULL).  */
  jobject group;		/* Global ref of a ThreadGroup object
				   (or NULL).  */
} JavaVMAttachArgs;

typedef struct JavaVMOption
{
  char *optionString;
  void *extraInfo;
} JavaVMOption;

typedef struct JavaVMInitArgs
{
  /* Must be JNI_VERSION_1_2.  */
  jint version;

  /* Number of options.  */
  jint nOptions;

  /* Options to the VM.  */
  JavaVMOption *options;

  /* Whether we should ignore unrecognized options.  */
  jboolean ignoreUnrecognized;
} JavaVMInitArgs;


/* Keep c-font-lock-extra-types in alphabetical order. */
/* Local Variables: */
/* c-font-lock-extra-types: ("\\sw+_t" 
   "JNIEnv" "JNINativeMethod" "JavaVM" "JavaVMOption" "jarray"
   "jboolean" "jbooleanArray" "jbyte" "jbyteArray" "jchar"  "jcharArray" 
   "jclass" "jdouble" "jdoubleArray" "jfieldID" "jfloat" "jfloatArray"
   "jint" "jintArray" "jlong" "jlongArray" "jmethodID" "jobject" "jstring" "jthrowable" 
   "jvalue" "jweak") */
/* End: */
#endif /* __CLASSPATH_JNI_H__ */
