/*
 * gcj-glue.c
 *
 * Copyright (c) 1996, 1997, 1998, 1999
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 *
 * Written by Godmar Back <gback@cs.utah.edu>
 */

#include "config.h"
#include "debug.h"
#include "config-mem.h"
#include "classMethod.h"
#include "errors.h"
#include "access.h"
#include "lookup.h"
#include "baseClasses.h"
#include "stringSupport.h"
#include "gcj.h"
#include "jit3/machine.h"
#include "object.h"

#if defined(HAVE_GCJ_SUPPORT) && defined(JIT3) && defined(TRANSLATOR)

/*
 * GCJ code wants to create an object.  
 */
Hjava_lang_Object *
kenvCreateObject(struct Hjava_lang_Class *clazz, int size)
{
	/* !? --- I think that may be i386 specific -- XXX optimize me */
	assert(((CLASS_FSIZE(clazz)+3)&~3) == size); 
	return (newObject(clazz));
}

/* Given a const char * type name as a class name, create a
 * pathname.  I.e., makes a utf8const "[Ljava/lang/String;" from
 * [Ljava.lang.String
 */
static 
Utf8Const *
makePathUtf8FromClassChar(const char *name)
{
	int n;
	char buf[256], *b = buf;
	Utf8Const *utf8;

	n = strlen(name);
	if (n > sizeof(buf) - 1) {
		b = KMALLOC(n + 1);
	}
	classname2pathname(name, b);
	utf8 = utf8ConstNew(b, n);
	if (b != buf) {
		KFREE(b);
	}
	return (utf8);
}

/*
 * cname is a qualified class name, like
 * [[I or Ljava.lang.String; or LHelloWorld; or [[[Ljava.lang.Object;
 * or a qualified path name where there's a slash instead of the dot
 */
Hjava_lang_Class *
kenvFindClass(char *cname, errorInfo *einfo)
{
	char buf[1024];			/* XXX */
	Utf8Const *name;
	Hjava_lang_Class *class;

	/* XXX handle classloaders -- but how? */
        if (cname[0] == '[') {
		/* loadArray does want the "L...;" */
		name = makePathUtf8FromClassChar(cname);
                class = loadArray(name, 0, einfo);
        } else {
		/* loadClass doesn't want the "L...;" */
		if (*cname == 'L') {
			strcpy(buf, cname+1);
			assert(buf[strlen(buf) - 1] == ';');
			buf[strlen(buf) - 1] = '\0';
		} else {
			/* else could be a primitive class such as "long" */
			strcpy(buf, cname);
		}
		name = makePathUtf8FromClassChar(buf);
                class = loadClass(name, 0, einfo);
        }
	utf8ConstRelease(name);

DBG(GCJ, dprintf(__FUNCTION__": `%s' --> @%p\n", cname, class); )

	return (class);
}

void 
kenvPostClassNotFound(const char *utf8name, errorInfo *einfo)
{
	postNoClassDefFoundError(einfo, utf8name);
}

void
kenvProcessClass(struct Hjava_lang_Class *class)
{
	errorInfo info;

DBG(GCJ, dprintf("processing kaffe class for %s@%p from st=%d to %d...\n",  
	CLASS_CNAME(class), class, class->state, CSTATE_COMPLETE);
    )
	if (processClass(class, CSTATE_COMPLETE, &info) == false) {
		throwError(&info);
	}
}

/*
 * Create a kaffe class from the gcj info
 */
Hjava_lang_Class*
kenvMakeClass(neutralClassInfo *info, errorInfo *einfo)
{
	int n;
	classEntry *centry;
	Hjava_lang_Class *class = newClass();

DBG(GCJ, dprintf("making kaffe class for %s@%p vtab=%p gcj=%p...\n",  
	info->name, class, info->vtable, info->gcjClass);)
	
	if (class == 0) {
		goto oom;
	}

	class->name = makePathUtf8FromClassChar(info->name);

	/* We must take care to not free some of these fields when unloading 
	 * fully or partially loaded gcj classes (if that'll ever be supported) 
	 * because they may still point in gcj-produced static data.
	 */

	/* We assume the same vtable layout here: first a pointer to the
	 * class, then a zero, then the vtable
	 */
	class->dtable = info->vtable;

	/* write kaffe class in vtable.class field in gcj space if class has
	 * a dtable --- interfaces don't have one.
	 */
	if (class->dtable) {
		class->dtable->class = class;
	}
	class->gcjPeer = info->gcjClass;
	class->accflags = info->accflags;
	class->msize = info->vTableCount;
	class->superclass = info->superclass;	/* either null or pointer to
						   other gcj class */

	if (info->methodCount > 0) {
		class->methods = gc_malloc(sizeof(Method) * info->methodCount, 
					   GC_ALLOC_METHOD);
		if (class->methods == 0) {
			goto oom;
		}
	}

	if (info->fieldCount > 0) {
		class->fields = gc_malloc(sizeof(Field) * info->fieldCount, 
					   GC_ALLOC_FIELD);
		if (class->fields == 0) {
			goto oom;
		}
	}

	/* 
	 * I copy those such that resolveInterfaces can proceed the same
	 * way it does for for kaffe classes.
	 */
	class->interface_len = info->interfaceCount;
	n = info->interfaceCount * sizeof(struct Hjava_lang_Class**);
	if (n > 0) {
		class->interfaces = (Hjava_lang_Class**)KMALLOC(n);
		memcpy(class->interfaces, info->interfaces, n);
	}

	class->state = CSTATE_PRELOADED;
	if (class->superclass == 0 && CLASS_IS_INTERFACE(class)) {	
		/* gcj sets superclass to zero for interfaces, but we 
		 * set it to ObjectClass.
		 */
		class->superclass = ObjectClass;
	}

	SET_CLASS_GCJ(class);

	/* XXX: find a better place to do this:
	 * If we're invoked via loadClass, the caller will anchor us after
	 * we return.  This gc_add_ref call is only necessary if we're
	 * resolving a class referenced from gcj code via its address, 
	 * bypassing the usual name-based lookup path.   XXX
	 */
	if (!gc_add_ref(class)) {
		goto oom;
	}

	/* make sure this class is registered with the class pool 
	 * Same caveat as for call to gc_add_ref above applies.
	 */
        centry = lookupClassEntry(class->name, 0, einfo);
        if (centry == 0) {
                return (0);
        }
	class->centry = centry;
        centry->class = class;

	return (class);
oom:
	postOutOfMemory(einfo);
	return (0);
}

/*
 * Build a Kaffe Method* struct from a GCJ method description
 *
 * NB:  It ain't pretty, and it duplicates a lot of code from 
 * classMethod.c:addMethod()
 */
bool
kenvMakeMethod(neutralMethodInfo* info, Hjava_lang_Class *c, errorInfo *einfo)
{
	Utf8Const *signature;
	static 	char buf[1024];

        Method *mt = &CLASS_METHODS(c)[CLASS_NMETHODS(c)];
	mt->name = utf8ConstNew(info->name, -1);

	classname2pathname(info->signature, buf);
	signature = utf8ConstNew(buf, -1);

        METHOD_PSIG(mt) = parseSignature(signature, einfo);
	utf8ConstRelease(signature);

        if (METHOD_PSIG(mt) == NULL) {
DBG(GCJ, 	
		dprintf("could not parse signature %s.%s ...\n", 
			info->name, info->signature); 
    )
                return (false);
        }
        mt->class = c;
        mt->accflags = info->accflags;
        mt->c.bcode.code = 0;
        mt->stacksz = 0;
        mt->localsz = 0;
        mt->exception_table = 0;

	if (*info->idx == -1) {
		mt->idx = -1;
	} else {
		int supermsize;

		/* avoid accessing superclass when mt->class == ObjectClass */
		if (c == ObjectClass) {
			supermsize = 0;
		} else {
			supermsize = c->superclass->msize;
		}
		assert(c == ObjectClass || c->superclass != 0);

		/* assign a new method index unless this method was inherited */
		if (c->superclass != 0 
		    && getInheritedMethodIndex(c->superclass, mt) == false) 
		{
			mt->idx = supermsize + (*info->idx)++;
		}
	}
	mt->ncode = info->ncode;

	/* Interfaces and abstract class don't have a vtable in gcj, but 
	 * their methods have non-zero indices.  METHOD_NATIVECODE accesses
	 * a location within the dtable if idx != 0.
	 *
	 * XXX: Fix this whole "METHOD_NATIVECODE" mess and come up with
	 * a more viable abstraction instead.
	 */
	if (!CLASS_IS_INTERFACE(mt->class) && !CLASS_IS_ABSTRACT(mt->class)) {
		METHOD_NATIVECODE(mt) = mt->ncode;
	}

        /* Mark constructors as such */
        if (utf8ConstEqual (mt->name, constructor_name)) {
                mt->accflags |= ACC_CONSTRUCTOR;
        }

	/* Assumption 1: gcj always sets the 0x4000 ACC_TRANSLATED flag, 
	 * even in abstract methods.
	 */
	assert(mt->accflags & ACC_TRANSLATED);

	/*
	 * We clear ACC_TRANSLATED flags for methods with no code, such as
	 * abstract methods.
	 */
	if (info->ncode == 0 || (info->accflags & ACC_NATIVE)) {
		mt->accflags &= ~ACC_TRANSLATED;
	}

        CLASS_NMETHODS(c)++;

DBG(GCJ, 
	dprintf("making kaffe method %s.%s @%p idx=%d m->ncode=%p...\n", 
	    mt->name->data, METHOD_SIGD(mt), 
	    mt, mt->idx, mt->ncode);
    )
        return (true);
}

/* 
 * Create a kaffe Field structure from GCJ field description
 *
 * compare addField in classMethod.c 
 */
bool
kenvMakeField(neutralFieldInfo* info, Hjava_lang_Class *c, errorInfo *einfo)
{
	Field *fld;
	Hjava_lang_Class *ftype;

	/* static fields start from the bottom, instance fields from the top */
        if (info->flags & ACC_STATIC) {
                fld = &CLASS_FIELDS(c)[CLASS_NSFIELDS(c)];
        } else {
                fld = &CLASS_FIELDS(c)[info->idx + CLASS_NSFIELDS(c)];
        }
        fld->accflags = info->flags;
	fld->name = utf8ConstNew(info->name, -1);

	/* For now, that's true.  However, see
	 * http://sourceware.cygnus.com/ml/java-discuss/1999-q4/msg00379.html
	 * http://sourceware.cygnus.com/ml/java-discuss/1999-q4/msg00380.html
	 * for how things may change.
	 */
	assert(!(fld->accflags & FIELD_CONSTANT_VALUE));

	/* The resolved flags have the same meaning as in Kaffe---note that
	 * gcj only resolves types if it can do so at link/compile time.  
	 * Array types, for instance, are stored as unresolved field refs.
	 */
	if ((fld->accflags & FIELD_UNRESOLVED_FLAG) == 0) {
		/* can't use kenvFindClassByAddress2 here cause we must avoid
		 * attempts to process this class---especially if the
		 * field points to a type that is the type of its enclosing
		 * object.
		 */
		ftype = gcjFindClassByAddress(info->type, einfo);
		if (!ftype) {
			char *name;
			name = gcjFindUnresolvedClassByAddress(info->type); 
			if (name) {
				ftype = kenvFindClass(name, einfo);
			} else {
				kenvPostClassNotFound(name, einfo);
			}
		}

		if (!ftype) {
			return (false);
		}
		FIELD_TYPE(fld) = ftype;

		if (CLASS_IS_PRIMITIVE(ftype)) {
			assert(info->bsize == TYPE_PRIM_SIZE(ftype));
		} else {
			assert(info->bsize == PTR_TYPE_SIZE);
		}
	} else {
		const char *ksig = info->type;
		FIELD_TYPE(fld) = (Hjava_lang_Class*)
			makePathUtf8FromClassChar(ksig);
		if (ksig[0] == 'L' || ksig[0] == '[') {
			assert(info->bsize == PTR_TYPE_SIZE);
		} /* else unresolved primitive type */
	}
	FIELD_SIZE(fld) = info->bsize;
	if (fld->accflags & ACC_STATIC) {
		FIELD_ADDRESS(fld) = info->u.addr;
	}

DBG(GCJ, 
	dprintf("making kaffe field #%d %s.%s @%p type=%p flags=0x%x addr=%p\n",
		info->idx, CLASS_CNAME(c), fld->name->data, fld, 
		fld->type, fld->accflags, FIELD_ADDRESS(fld));
    )

        CLASS_NFIELDS(c)++;
        if (fld->accflags & ACC_STATIC) {
                CLASS_NSFIELDS(c)++;
        }

	/* Instance fields are inserted in opposite order, so let's switch
	 * them here.  This is what finishFields does.
	 */
	if (info->idx == 0) {
		finishFields(c);
	}
	return (true);
}

const char *
kenvGetClassName(struct Hjava_lang_Class *clazz)
{
	return (CLASS_CNAME(clazz));
}

void* 
kenvMakeJavaString(const char *utf8data, int utf8length, int utf8hash)
{
	Utf8Const *u;
	Hjava_lang_String *s;

	u = utf8ConstNew(utf8data, utf8length);
	/* make sure kaffe and gcj computed the same hash --- unfortunately
	 * gcj only stores the lower 16 bits.
	 */
	assert((u->hash & 0xffff) == utf8hash);
	s = utf8Const2Java(u);
#if 0	/* This is kind of excessive, but turn it on if you need the
	 * addresses of strings
	 */
DBG(GCJ, dprintf("making str `%s' ->%p value->%p v.dtable->%p\n",
		utf8data, s, s->value, ((Hjava_lang_Object*)s->value)->dtable);
    )
#endif
	utf8ConstRelease(u);

	/* Nail these down for now because we do not walk a gcj class's
	 * constant pool from which these strings may be referenced.
	 */
	if (!gc_add_ref(s)) {
		errorInfo info;
		postOutOfMemory(&info);
		throwError(&info);
	}
	return (s);
}

/*
 * Given a pointer to a java::lang::Class, return kaffe's surrogate class 
 */
Hjava_lang_Class*
kenvFindClassByAddress2(void *clazz, errorInfo *einfo)
{
	Hjava_lang_Class *kclass;

	/* Check if this class was preloaded.  If so, a surrogate will
	 * be constructed and returned.
	 */
	kclass = gcjFindClassByAddress(clazz, einfo);

	/* Either it wasn't preloaded or some other problem occurred
	 * during processing.  XXX think about what it would to do if
	 * another problem occurred.
	 *
	 * If it's not a precompiled class, it could be an unresolved ref.
	 */
	if (kclass == 0 /* XXX:  && error was class not found !? */) {
		/* It could be a kaffe class, which was an unresolved symbol  
		 * in the shared module, but for which there should be an entry
		 * in the symbol table of the fixup module which we've loaded
		 * by now.
		 */
		char *name;
		name = gcjFindUnresolvedClassByAddress(clazz);
		if (name) {
			kclass = kenvFindClass(name, einfo);
		} else {
			kenvPostClassNotFound(name, einfo);
		}
	}
        if (kclass && kclass->state < CSTATE_LINKED) {
		if (processClass(kclass, CSTATE_LINKED, einfo) == false)  {
DBG(GCJ, 		dprintf("gcjGetClass: failed to process class %s@%p\n",
				kclass); 
    )
			return (0);
		}        
	}

DBG(GCJ,
	dprintf("%s: %p -> %p %s\n", __FUNCTION__, clazz,
		kclass, kclass ? CLASS_CNAME(kclass) : "<nil>");
    )
	return (kclass);
}

Hjava_lang_Class*
kenvFindClassByAddress(void *gcjclazz)
{
	Hjava_lang_Class* kclass;
	errorInfo info;

	kclass = kenvFindClassByAddress2(gcjclazz, &info);
	if (kclass == 0) {
		dprintf("GCJ: Missing needed class @%p\n", gcjclazz);
		throwError(&info);
	}
	return (kclass);
}

/*
 * Find a method that implements an interface by its name and signature
 * Unfortunately, gcj doesn't use the constant-time method to lookup
 * interfaces yet.  I hope they'll follow suit and implement the lookup 
 * based on interface class and index.
 */

#define HSIZE	1024
struct mCacheEntry {
	Hjava_lang_Class 	*kclass;
	const char 		*mname;
	const char 		*msig;
	Method 			*meth;
} mCache[HSIZE];

#define MHASH(V)   ((((uintp)(V) >> 2) ^ ((uintp)(V) >> 9))%HSIZE)

/* Try to lookup a class/methodname/methodsignature combo in the cache,
 * return location to where Method can be found.  If we have a cache miss,
 * clear that location.
 */
static inline
Method **
tryCache(Hjava_lang_Class *kclass, const char *mname, const char *msig)
{
	/* With this hash function, I'm getting these hit ratios:

	    mpegaudio       : 181945 hits, 28 misses
	    db              : 19297313 hits, 19 misses
	    jack            : 14507926 hits, 40 misses
	    jess            : 706127 hits, 139 misses
	    compress        : 184 hits, 15 misses

	 */

	int h = MHASH(kclass) ^ MHASH(mname);
	struct mCacheEntry *e = mCache + h;

	if (e->kclass != kclass || e->mname != mname || e->msig != msig) {
		e->kclass = kclass;
		e->mname = mname;
		e->msig = msig;
		e->meth = 0;
	}
	return (&(e->meth));
}

/* #define COUNT_CACHE	1 */

#if COUNT_CACHE
static long long cachehits;
static long long cachemisses;
void printInterfaceCacheStats() {
	dprintf("GCJ interface cache: %qd hits, %qd misses\n",
		cachehits, cachemisses);
}
#endif

void* 
kenvFindMethod(Hjava_lang_Class *kclass, 
		const char *mname,
	        const char *msig)
{
	Method *meth; 
	Method **mptr;
	errorInfo info;

	mptr = tryCache(kclass, mname, msig);
	meth = *mptr;
	if (meth) {
#if COUNT_CACHE
		cachehits++;
#endif
		goto found;
	}
#if COUNT_CACHE
	cachemisses++;
#endif

        for (; kclass != 0; kclass = kclass->superclass) {

		int n = CLASS_NMETHODS(kclass);        
		for (meth = CLASS_METHODS(kclass); --n >= 0; ++meth) {

			/* Ouch.  GCJ keeps signatures in dotted form, we 
			 * keep them in slashed form.
			 */
			if (!strcmp(mname, meth->name->data)
			    && comparePath2ClassName(
			    		PSIG_DATA(METHOD_PSIG(meth)), 
					msig))
			{
				*mptr = meth;
			    	goto found;
			}
		}
	}
	postExceptionMessage(&info, JAVA_LANG(NoSuchMethodError), "%s", mname);
	throwError(&info);
	return (0);
found:

DBG(GCJ, dprintf("%s: %s.%s is at %p\n", 
		__FUNCTION__, mname, msig, METHOD_NATIVECODE(meth));
    )
	assert(METHOD_NATIVECODE(meth));
	return (METHOD_NATIVECODE(meth));
}

/*
 * A GCJ trampoline.
 * Invoked only once per method so can be expensive.
 * Note that the msig is only a partial signature: it doesn't have the
 * return type in it.
 */
void *
kenvTranslateMethod(const char *classname, const char *mname, const char *msig)
{
	Method *meth;
	errorInfo info;
	Hjava_lang_Class *kclass, *cc;
	int msiglen = strlen(msig);

	kclass = kenvFindClass((char*)classname, &info);
	if (kclass == 0) {
		throwError(&info);
	}

	/* 
	 * GCJ invokes (static) native methods w/o calling Jv_InitClass
	 * first.  (Jv_InitClass is the first thing a gcj static method
	 * does.)  Hence, make sure the class's static initialized has
	 * been invoked to ensure the static native method is even here.
	 *
	 * XXX need a different approach for static linking here!
	 */
	kenvProcessClass(kclass);

	for (cc = kclass; cc; cc = cc->superclass) {
		int  nm = CLASS_NMETHODS(cc);
		meth = CLASS_METHODS(cc);
		while (nm-- > 0) {
			if (!strcmp(mname, meth->name->data) &&
			    !strncmp(msig, METHOD_SIGD(meth), msiglen)) 
			{
				goto found;
			}
			meth++;
		}
	}
DBG(GCJ,
	dprintf("%s: NoSuchMethod %s.%s.%s\n", __FUNCTION__, 
			classname, mname, msig);
    )

	postExceptionMessage(&info, JAVA_LANG(NoSuchMethodError), 
		"%s.%s.%s", classname, mname, msig);
	throwError(&info);
	return (0);

found:
	assert(meth);
	if (!METHOD_TRANSLATED(meth)) {
		if (translate(meth, &info) == false) {
			throwError(&info);
		}
	}

	return (METHOD_NATIVECODE(meth));
}

/*
 * Find a class by its GCJ address.
 * Exception: if we're asked to find a class with the address
 * "ObjectClass", return the address of ObjectClass???
 *
 * XXX this has only to do with interface/java.lang.Object
 * XXX CLEAN ME UP and possibly eliminate this function.
 */
Hjava_lang_Class* 
gcjGetClass(void *jclazz, errorInfo *einfo)
{
	Hjava_lang_Class* kclass;

	/* Both java.lang.Object and interfaces have their superclass field
	 * set to null.  To distinguish interfaces, we set their field to
	 * ObjectClass in kenvMakeClass
	 */
	if (jclazz == (void*)ObjectClass) {
		return (ObjectClass);
	}

	kclass = kenvFindClassByAddress2(jclazz, einfo);
	if (kclass == 0) {
DBG(GCJ, 	
		dprintf("gcjGetClass: class not found at %p\n", jclazz); 
	)
		return (0);
	}
	return (kclass);
}

void* 
kenvMalloc(int size, struct _errorInfo *einfo)
{
	void *p = KMALLOC(size);
	if (p == 0) {
		postOutOfMemory(einfo); 
	}
	return (p);
}

void 
kenvFree(void * ptr)
{
	KFREE(ptr);
}

#endif /* HAVE_GCJ_SUPPORT */
