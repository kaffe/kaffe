#include "config.h"
#include "debug.h"
#include "config-mem.h"
#include "config-hacks.h"
#include "classMethod.h"
#include "errors.h"
#include "access.h"
#include "lookup.h"
#include "baseClasses.h"
#include "stringSupport.h"
#include "gcj.h"
#include "object.h"

static Hjava_lang_Class* kenvFindClassByAddress2(void *clazz, errorInfo *einfo);

/*
 * GCJ code wants to create an object.  
 */
Hjava_lang_Object *
kenvCreateObject(struct Hjava_lang_Class *clazz, int size)
{
	/* !? */
	assert(((CLASS_FSIZE(clazz)+3)&~3) == size); 
	return (newObject(clazz));
}

/*
 * cname is a qualified class name, like
 * [[I or Ljava/lang/String; or LHelloWorld; or [[[Ljava/lang/Object;
 */
Hjava_lang_Class *
kenvFindClass(char *cname, errorInfo *einfo)
{
	char buf[1024];		/* XXX */
	Utf8Const *name;
	Hjava_lang_Class *class;

	/* XXX handle classloaders -- but how? */
        if (cname[0] == '[') {
		/* loadArray does want the "L...;" */
		name = utf8ConstNew(cname, -1);
                class = loadArray(name, 0, einfo);
        } else {
		/* loadClass doesn't want the "L...;" */
		if (*cname == 'L') {
			strcpy(buf, cname+1);
			assert(buf[strlen(buf) - 1] == ';');
			buf[strlen(buf) - 1] = '\0';
		}
		name = utf8ConstNew(buf, -1);
                class = loadClass(name, 0, einfo);
        }
	utf8ConstRelease(name);

DBG(GCJ, dprintf(__FUNCTION__": `%s' --> @%p\n", buf, class); )

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

Hjava_lang_Class*
kenvMakeClass(neutralClassInfo *info, errorInfo *einfo)
{
	Hjava_lang_Class *class = newClass();
	int n;

DBG(GCJ, dprintf("making kaffe class for %s@%p vtab=%p gcj=%p...\n",  
	info->name, class, info->vtable, info->gcjClass);)
	
	if (class == 0) {
		goto oom;
	}

	/* classname2pathname(info->name, buf); ???? */
	class->name = utf8ConstNew(info->name, -1);

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
	 * I copy those such that resolveInterfaces can proceed like
	 * for kaffe classes.
	 */
	class->interface_len = info->interfaceCount;
	n = info->interfaceCount * sizeof(struct Hjava_lang_Class**);
	if (n > 0) {
		class->interfaces = (Hjava_lang_Class**)KMALLOC(n);
		memcpy(class->interfaces, info->interfaces, n);
	}

	class->state = CSTATE_PRELOADED;
	if (class->superclass == 0 && CLASS_IS_INTERFACE(class)) {	
		/* gcj set superclass to zero for interfaces, but we 
		 * set it to ObjectClass 
		 */
		class->superclass = ObjectClass;
	}

	SET_CLASS_GCJ(class);

	/* anchor this class */
	if (!gc_add_ref(class)) {
		postOutOfMemory(einfo);
		return (0);
	}

	return (class);
oom:
	postOutOfMemory(einfo);
	return (0);
}

/*
 *
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
DBG(GCJ, dprintf("could not parse signature %s.%s ...\n", info->name, info->signature); )
                return (false);
        }
        mt->class = c;
        mt->accflags = info->accflags;
        mt->c.bcode.code = 0;
        mt->stacksz = 0;
        mt->localsz = 0;
        mt->exception_table = 0;

	if (info->idx == -1) {
		mt->idx = -1;
	} else {
		assert(c->superclass);
		/* assign a new method index unless this method was inherited */
		if (getInheritedMethodIndex(c->superclass, mt) == false) {
			mt->idx = c->superclass->msize + info->idx;
		}
	}
	METHOD_NATIVECODE(mt) = mt->ncode = info->ncode;

        /* Mark constructors as such */
        if (utf8ConstEqual (mt->name, constructor_name)) {
                mt->accflags |= ACC_CONSTRUCTOR;
        }
	mt->accflags |= ACC_TRANSLATED;

        CLASS_NMETHODS(c)++;
DBG(GCJ, dprintf("making kaffe method %s.%s @%p idx=%d ncode=%p...\n", 
	mt->name->data, METHOD_SIGD(mt), 
	mt, mt->idx, METHOD_NATIVECODE(mt));)
        return (true);
}

/* see addField in classMethod.c */
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

	/* need to think about what that would mean */
	assert(!(fld->accflags & FIELD_CONSTANT_VALUE));

	/* The resolved flags have the same meaning as in Kaffe---note that
	 * gcj only resolves types if it can.  Array types, for instance,
	 * are stored as unresolved field refs.  
	 */
	if ((fld->accflags & FIELD_UNRESOLVED_FLAG) == 0) {
		/* can't use kenvFindClassByAddress2 here cause we must avoid
		 * attempts to process this class---especially if the
		 * field points to a type of itself.
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
			utf8ConstNew(info->type, -1);
		if (ksig[0] == 'L' || ksig[0] == '[') {
			assert(info->bsize == PTR_TYPE_SIZE);
		} /* else unresolved primitive type */
	}
	FIELD_SIZE(fld) = info->bsize;
	if (fld->accflags & ACC_STATIC) {
		FIELD_ADDRESS(fld) = info->u.addr;
	}

DBG(GCJ, dprintf("making kaffe field #%d %s.%s @%p type=%p flags=0x%x addr=%p\n", 
	info->idx,
	CLASS_CNAME(c), fld->name->data, fld, 
	fld->type,
	fld->accflags, FIELD_ADDRESS(fld));
    )
        CLASS_NFIELDS(c)++;
        if (fld->accflags & ACC_STATIC) {
                CLASS_NSFIELDS(c)++;
        }
	/* Instance fields are inserted in opposite order, so let's switch
	 * them here.
	 */
	if (info->idx == 0) {
DBG(GCJ,
	dprintf("Finishing fields %p CLASS_IFIELDS %p CLASS_SFIELDS %p "
		"CLASS_NFIELDS %d CLASS_NIFIELDS %d CLASS_NSFIELDS %d\n",
		c, CLASS_IFIELDS(c), CLASS_SFIELDS(c),
		CLASS_NFIELDS(c), CLASS_NIFIELDS(c), CLASS_NSFIELDS(c));
    )
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
#if 1
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
static Hjava_lang_Class*
kenvFindClassByAddress2(void *clazz, errorInfo *einfo)
{
	Hjava_lang_Class *kclass;

	/* Check if this class was preloaded.  If so, a surrogate will
	 * be constructed and returned.
	 */
	kclass = gcjFindClassByAddress(clazz, einfo);

	/* XXX either it wasn't preloaded or some other problem occurred
	 * during processing.  XXX FIXME
	 *
	 * If not a precompiled class, it could be an unresolved ref.
	 */
	if (kclass == 0 /* XXX:  && error was class not found */) {
		/* It could be a kaffe class, which was an unresolved symbol  
		 * in the shared module, but for which there should be an entry
		 * in the symbol table of the fixup module which we've loaded
		 * by now
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
DBG(GCJ,dprintf("%s: %p -> %p %s\n", __FUNCTION__, clazz,
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
		fprintf(stderr, "GCJ: Missing needed class @%p\n", gcjclazz);
		throwError(&info);
	}
	return (kclass);
}

/*
 * Find a method that implements an interface by its name and signature
 * Unfortunately, gcj doesn't use the constant-time method to lookup
 * interfaces yet.  They use a cache instead.  Should this become a
 * problem we can use a cache as well---but I hope they'll just follow
 * suit and implement the lookup based on interface class and index.
 */
void* 
kenvFindMethod(Hjava_lang_Class *kclass, 
		const char *mname,
	        const char *msig)
{
	Utf8Const *name, *sig;
	Method *meth; 
	errorInfo info;
	char buf[128], *p = buf;
	int siglen;

	name = utf8ConstNew(mname, -1);
	
	siglen = strlen(msig);
	if (siglen > sizeof (buf)) {
		p = KMALLOC(siglen+1);
	}

	/* Ouch. GCJ keeps signatures in dotted form, we keep them
	 * in slashed form
	 */
	classname2pathname(msig, p);
	sig = utf8ConstNew(p, siglen);

	meth = findMethod(kclass, name, sig, &info);
	if (!meth) {
		throwError(&info);
	}
	utf8ConstRelease(name);
	utf8ConstRelease(sig);

	if (p != buf) {
		KFREE(p);
	}
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
 * Given a kaffe class that is a surrogate gcj class, find the superclass.
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
DBG(GCJ, dprintf("gcjGetClass: class not found at %p\n", jclazz); )
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
