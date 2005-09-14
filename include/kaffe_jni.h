#ifndef __KAFFE_JNI_H
#define __KAFFE_JNI_H

typedef struct KaffeVM_Arguments {
        jint            version;

        char**          properties;
        jint            checkSource;
        jint            nativeStackSize;
        jint            javaStackSize;
        jint            minHeapSize;
        jint            maxHeapSize;
        jint            verifyMode;
        const char*     classpath;
        const char*     bootClasspath;
        jint            (*jni_vfprintf)(FILE*, const char*, va_list);
        void            (*exit)(jint);
        void            (*abort)(void);
        jint            enableClassGC;
        jint            enableVerboseGC;
        jint            disableAsyncGC;
        jint            enableVerboseClassloading;
        jint            enableVerboseJIT;
        jint            enableVerboseCall;
        jint            allocHeapSize;
        const char*     classhome;
        const char*     libraryhome;
        const char*     profilerLibname;
        const char*     profilerArguments;
} KaffeVM_Arguments;

extern KaffeVM_Arguments Kaffe_JavaVMArgs;

#endif
