/*
 * gcj.h
 *
 * Copyright (c) 1996, 1997, 1998, 1999
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

#ifndef __gcj_h
#define __gcj_h

/* NB: This headers contains only data structures and declarations that
 * can be safely included by both gcj and kaffe files.  As such, we cannot
 * use any non-standard types here
 */

#ifdef __cplusplus

/* only for cplusplus files */
struct fixup_table_t {
        void *symbol;
        enum { CLASS = 1, STATICFIELD = 2, METHODREF = 3, VTABLE = 4 } type;
        char* data1;
        char* data2;
        char* data3;
        void dump();
	char* findClass(void *symbol);
	void* getStaticFieldAddr(const char *clazz, const char *name);
};
extern fixup_table_t *fixupTable;

extern "C" {
#endif

struct kaffe_frame_descriptor {
        int idx;
	int offset;
};

typedef struct {
	int	idx;			/* index running from fCount...0 */
	char* 	name;			/* zero-terminated utf8 */
	void*	type;
	unsigned short flags;
	unsigned short bsize;
	union {
		int 	boffset;
		void* 	addr;
	} u;
} neutralFieldInfo;

typedef struct {
	char* 	name;			/* zero-terminated utf8 */
	char* 	signature;		/* zero-terminated utf8 */
	unsigned short accflags;	/* flags according to java conv. */
	void* 	ncode;			/* where the code is */
	int	*idx;			/* ptr to vtable index of this method */
} neutralMethodInfo;

typedef struct {
	char*	name;			/* zero-terminated utf8 */
	void*	gcjClass;		/* java::lang::Class */
	void*	vtable;			/* _Jv_Vtable */
	void*	superclass;		/* java::lang::Class->superclass */
	int	accflags;		/* flags according to java conv. */
	int	methodCount;		/* # of methods */
	int	fieldCount;		/* # of fields */
	int	vTableCount;		/* # of entries in vtable */
	int	interfaceCount;		/* # of entries in interfaces */
	void**	interfaces;		/* interfaces supported by this class */
} neutralClassInfo;

struct Hjava_lang_Class;
struct Hjava_lang_Object;
struct _jmethodID;
struct _jfieldID;
struct _errorInfo;

/*
 * Methods prefixed with "gcj" are exported by the gcj subsystem for 
 * use by code in kaffevm
 */
extern void * gcjGetFieldAddr(const char *clazz, const char *fldname);

extern void gcjInit(void);
extern void gcjLoadSharedObject(char* sofile);
extern struct Hjava_lang_Class* gcjFindClassByUtf8Name(
	const char* utf8name, struct _errorInfo*);
extern bool gcjProcessClass(struct Hjava_lang_Class* clazz, void *gcjClass,
			    struct _errorInfo* einfo);
extern bool gcjProcessClassConstants(struct Hjava_lang_Class* clazz, 
				     void *gcjClass, struct _errorInfo* einfo);

extern struct Hjava_lang_Class* gcjGetClass(void *jclazz, struct _errorInfo*);

extern char *gcjFindUnresolvedClassByAddress(void *symbol);
extern struct Hjava_lang_Class* gcjFindClassByAddress(void *clazz, 
						      struct _errorInfo*);

extern struct Hjava_lang_Class *gcjFindMatchClass(void *minfo, 
						  struct _errorInfo *einfo);

/*
 * Methods prefixed with "kenv" are implemented in the kaffe glue code
 * to allow it to access functions from the kaffe environment  
 */

extern struct Hjava_lang_Class* kenvFindClass(char *cname, struct _errorInfo *);
extern struct Hjava_lang_Class* kenvFindClassByAddress(void *gcjclazz);
extern const char *kenvGetClassName(struct Hjava_lang_Class *clazz);
extern struct Hjava_lang_Object * kenvCreateObject(
	struct Hjava_lang_Class *clazz, int size);

extern void* kenvFindMethod(struct Hjava_lang_Class *kclass, 
	const char *mname, 
	const char *msig);

extern void *kenvTranslateMethod(const char *classname, 
				 const char *mname, const char *msig);

extern void* kenvMakeJavaString(const char *utf8data, int utf8length, 
				int utf8hash);

extern struct Hjava_lang_Class* kenvMakeClass(neutralClassInfo*, 
	struct _errorInfo*);

extern bool kenvMakeMethod(neutralMethodInfo* info, 
	struct Hjava_lang_Class *, struct _errorInfo *);

extern bool kenvMakeField(neutralFieldInfo* info, 
	struct Hjava_lang_Class *, struct _errorInfo *);

extern void kenvProcessClass(struct Hjava_lang_Class* clazz);
extern void kenvPostClassNotFound(const char *utf8name, struct _errorInfo *);

extern void kenvThrowBadArrayIndex(int idx);
extern struct Hjava_lang_Class* kenvFindClassByAddress2(void *clazz, 
							struct _errorInfo *);

extern void* kenvMalloc(int size, struct _errorInfo *);
extern void kenvFree(void * ptr);

extern bool comparePath2ClassName(const char *cname, const char *pname);

#define GCJ2KAFFE(gcjClass)	((struct Hjava_lang_Class*)((gcjClass)->thread))

extern void gcjInitPrimitiveClasses(void);

#define FIELD_UNRESOLVED_FLAG   0x8000
#define FIELD_CONSTANT_VALUE    0x4000

extern void blockAsyncSignals(void);
extern void unblockAsyncSignals(void);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
