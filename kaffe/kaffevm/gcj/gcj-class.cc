/*
 * gcj-class.cc
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
#include "config-std.h"
#include "gcj.h"

#if defined(HAVE_GCJ_SUPPORT) && defined(TRANSLATOR) && defined(JIT3)
#define private public
#include <java/lang/Object.h>
#include <java/lang/Class.h>
#include <java/lang/reflect/Method.h>
#include <java/lang/reflect/Modifier.h>
#include <gcj/field.h>
#include <gcj/method.h>

static java::lang::Class *preCList;
static char *findPrimitiveClass(void *symbol);

/*
 * Repeat those from libjava/include/java-cpool.h
 */
#define JV_CONSTANT_Undefined (0L)
#define JV_CONSTANT_Utf8 (1L)
#define JV_CONSTANT_Unicode (2L)
#define JV_CONSTANT_Integer (3L)
#define JV_CONSTANT_Float (4L)
#define JV_CONSTANT_Long (5L)
#define JV_CONSTANT_Double (6L)
#define JV_CONSTANT_Class (7L)
#define JV_CONSTANT_String (8L)
#define JV_CONSTANT_Fieldref (9L)
#define JV_CONSTANT_Methodref (10L)
#define JV_CONSTANT_InterfaceMethodref (11L)
#define JV_CONSTANT_NameAndType (12L)
#define JV_CONSTANT_ResolvedFlag (16L)
#define JV_CONSTANT_ResolvedString (16L | 8L)
#define JV_CONSTANT_ResolvedClass  (16L | 7L)


#include <dlfcn.h>
#include <string.h>
#include <assert.h>

fixup_table_t *fixupTable;

/* tell gcj where the static field has been allocated */
void *
fixup_table_t::getStaticFieldAddr(const char *clazz, const char *name)
{
	fixup_table_t	*f = this;
DBG(GCJ,
	if (0) {
	    dprintf("%s: looking for static %s. %s\n", __FUNCTION__, clazz, name);
	}
    )
	while (f->symbol) {
		/* use strstr because we want to find java/lang/String in
		 * Ljava/lang/String;
		 */
		if (f->type == STATICFIELD 
			&& strstr(f->data1, clazz) 
			&& !strcmp(name, f->data2)) 
		{
DBG(GCJ, dprintf("%s: static field %s.%s at %p\n", __FUNCTION__, clazz, name, f->symbol); )
			return (f->symbol);	
		}
		f++;
	}
	return (0);
}

char* 
fixup_table_t::findClass(void *symbol)
{
	fixup_table_t	*f = this;

	while (f->symbol) {
		if (f->symbol == symbol && f->type == CLASS) {
			return (f->data1);
		}
		f++;
	}
	return (0);
}

extern "C" void *
gcj_fixup_trampoline(void **psymbol)
{
DBG(GCJ,
	dprintf("%s (%p)\n", __FUNCTION__, psymbol);
    )
	fixup_table_t	*entry = fixupTable;

	while (entry->symbol) {
		if (entry->symbol == *psymbol) {
			break;
		}
		entry++;
	}
	assert(entry->symbol);

	void *ncode;
	const char *classname = entry->data1;
	const char *mname = entry->data2;
	const char *msig = entry->data3;
	assert(entry->type == fixup_table_t::METHODREF);

DBG(GCJ,
	dprintf("%s: %s.%s.%s\n",
		__FUNCTION__, classname, mname, msig);
    )
	ncode = kenvTranslateMethod(classname, mname, msig);

	/* Atomic write. */
	*psymbol = ncode;

DBG(GCJ,
	dprintf("%s: patch %p with %p\n", __FUNCTION__, psymbol, ncode);
    )

	return (ncode);
}

void 
fixup_table_t::dump()
{
	fixup_table_t	*f = this;
	while (f->symbol) {
		fprintf(stderr, "%13s @%p, d1=%s, d2=%s, d3=%s\n",
			f->type == CLASS ? "CLASS" :
			f->type == STATICFIELD ? "STATICFIELD" :
			f->type == METHODREF ? "METHOD" :
			f->type == VTABLE ? "VTABLE" : "???", 
			f->symbol, f->data1, f->data2, f->data3);
		f++;
    	}
}

extern "C" void *
gcjGetFieldAddr(const char *clazz, const char *name)
{
	if (fixupTable) {
		return (fixupTable->getStaticFieldAddr(clazz, name));
	} else {
		return (0);
	}
}

extern "C" char *
gcjFindUnresolvedClassByAddress(void *symbol)
{
	assert (fixupTable);
	char *name = fixupTable->findClass(symbol);
	if (name == 0) {
		name = findPrimitiveClass(symbol);
	}
	return (name);
}


/*
 * Load any shared objects in the class path.
 */
extern "C" void
gcjLoadSharedObject(char* sofile)
{
#if LIBTOOL_CRAP_THAT_DOESNT_REALLY_WORK
	/* I would like to use loadNativeLibrary here.  
	 * Problem is that this function returns success
	 * even if there is no success.  But what can you expect from
	 * libtool?
	 */
	char	errmsg[256];
	if (loadNativeLibrary(entry->path, errmsg, 255) != 1) {
		fprintf(stderr, "Failed to load `%s'\nError: `%s'\n",
			sofile, errmsg);
	} else {
DBG(GCJ,	dprintf("%s: Loaded `%s'\n", __FUNCTION__, sofile); )
	}
#else
	char buf[1024];	// XXX

	/* We load a fixup .so module first in order to avoid undefined
	 * symbols when loading the actual library.  Use "make_fixup X"
	 * to create the _fixup_X.so shared module
	 */
	strcpy(buf, "_fixup_");
	strcat(buf, sofile);
	blockAsyncSignals();
	void *h = dlopen(buf, RTLD_GLOBAL);
	if (h == 0) {
		fprintf(stderr, 
			"Failed to load fixup `%s'\n"
			"Error: `%s'\n", buf, dlerror());
	}
	fixupTable = (fixup_table_t*)dlsym(h, "_gcjSymbols");
	if (fixupTable == 0) {
		fprintf(stderr, 
			"Failed to find gcj symbols\nError: `%s'\n",
			dlerror());
	}
DBG(GCJ, 
	dprintf("---------------------------------------------------------\n");
	dprintf("Dumping fixup symbol table for module %s\n", sofile);
	fixupTable->dump();
	dprintf("-------end of table for %s---\n", sofile);
    )

	h = dlopen(sofile, RTLD_GLOBAL);
	unblockAsyncSignals();
	if (h == 0) {
		fprintf(stderr, 
			"Failed to load `%s'\nError: `%s'\n",
				sofile, dlerror());
	} else {
DBG(GCJ,	dprintf("%s: Loaded `%s'\n", __FUNCTION__, sofile); )
	}
#endif
}

static void
dumpMethod(_Jv_Method *meth)
{
	fprintf(stderr,"  Method @%p ", meth);
	fprintf(stderr,"name=`%s', sig=`%s', flags=0x%x ", 
		meth->name->data, meth->signature->data, meth->accflags);
	fprintf(stderr,"ncode=%p\n", meth->ncode);
}

static void
dumpField(_Jv_Field *fld)
{
	fprintf(stderr,"  Field @%p ", fld);
	fprintf(stderr,"name=`%s', flags=0x%x\n", 
		fld->name->data, fld->flags);
}

static const char *
poolTag2Name(int i)
{
#define	C(x)	if (i == x) { return #x + sizeof "JV_CONSTANT"; }
	C(JV_CONSTANT_String);
	C(JV_CONSTANT_Undefined);
	C(JV_CONSTANT_Utf8);
	C(JV_CONSTANT_Unicode);
	C(JV_CONSTANT_Integer);
	C(JV_CONSTANT_Float);
	C(JV_CONSTANT_Long);
	C(JV_CONSTANT_Double);
	C(JV_CONSTANT_Class);
	C(JV_CONSTANT_String);
	C(JV_CONSTANT_Fieldref);
	C(JV_CONSTANT_Methodref);
	C(JV_CONSTANT_InterfaceMethodref);
	C(JV_CONSTANT_NameAndType);
	return "UNKNOWN";
}

static void
dumpClass(java::lang::Class *clazz)
{
	fprintf(stderr,"Class `%s'@%p\n", clazz->name->data, clazz);
	fprintf(stderr,"  accflags=0x%x ", clazz->accflags);
	fprintf(stderr,"  superclass=@%p\n", clazz->superclass);

#define PFIELD(x) fprintf(stderr,"  "#x"= %d ", (int)clazz->x)
	PFIELD(size_in_bytes);
	PFIELD(vtable_method_count);
	PFIELD(method_count);
	fprintf(stderr, "\n");
	PFIELD(field_count);
	PFIELD(static_field_count);
	PFIELD(state);
	fprintf(stderr, "\n");

	PFIELD(interface_count);
	for (int i = 0; i < clazz->interface_count; i++) {
		fprintf(stderr, " %p", clazz->interfaces[i]);
	}
	fprintf(stderr, "\n");

	for (int i = 0; i < clazz->method_count; i++) {
		dumpMethod(clazz->methods + i);
	}
	for (int i = 0; i < clazz->field_count; i++) {
		dumpField(clazz->fields + i);
	}

	_Jv_Constants& pool = clazz->constants;
	for (int i = 1; i < pool.size; i++) {
		fprintf(stderr, "  CPool[%d].%s = ", i,
				poolTag2Name(pool.tags[i]));
		switch (pool.tags[i]) {
		case JV_CONSTANT_String:
		case JV_CONSTANT_Class:
		{
			fprintf(stderr, "`%s'\n", pool.data[i].utf8->data);
			break;
		}

		default:
			/* else just print the pointer value for now */
			fprintf(stderr, "%p (?)\n", pool.data[i].utf8);
			break;
		}
	}
}


/*
 * Register a pre-built class.
 */
extern "C" void
_Jv_RegisterClass(java::lang::Class* clazz)
{
DBG(GCJ, dumpClass(clazz);	)

	/* use next field provided by libgcj definition */
	clazz->next = preCList;
	preCList = clazz;
}

static void
fillInClassInfo(neutralClassInfo *info, java::lang::Class *clazz)
{
	info->gcjClass = clazz;
	info->name = clazz->name->data;
	info->vtable = clazz->vtable;
	info->superclass = clazz->superclass;
	info->methodCount = clazz->method_count;
	info->accflags = clazz->accflags;
	info->fieldCount = clazz->field_count;
	info->vTableCount = clazz->vtable_method_count;
	info->interfaceCount = clazz->interface_count;
	info->interfaces = (void**)clazz->interfaces;
}

static void
fillInMethodInfo(neutralMethodInfo *info, struct _Jv_Method *meth)
{
	info->name = meth->name->data;
	info->signature = meth->signature->data;
	info->accflags = meth->accflags;
	info->ncode = meth->ncode;
}

static void
fillInFieldInfo(neutralFieldInfo *info, _Jv_Field *fld)
{
	info->name = fld->name->data;
	if (fld->isResolved()) {
		info->type = fld->type;
	} else {
		info->type = ((_Jv_Utf8Const*)fld->type)->data;
	}
	info->flags = fld->flags;
	info->bsize = fld->bsize;
	info->u.boffset = fld->u.boffset;
	info->u.addr = fld->u.addr;
}

/*
 *
 */
static struct Hjava_lang_Class* 
constructSurrogate(java::lang::Class *clazz, struct _errorInfo *einfo)
{
	neutralClassInfo cinfo;
	struct Hjava_lang_Class *kclass = GCJ2KAFFE(clazz);

	if (kclass != 0) {
		return (kclass);
	}

DBG(GCJ, dprintf("%s: creating surrogate for class `%s'\n", __FUNCTION__, clazz->name->data);
        )
	fillInClassInfo(&cinfo, clazz);
	kclass = kenvMakeClass(&cinfo, einfo);
DBG(GCJ, 
	dprintf("%s: surrogate for class `%s' is @%p\n", __FUNCTION__,
		clazz->name->data, kclass);
	)

	GCJ2KAFFE(clazz) = kclass;
	return (kclass);
}

static bool
makeFields(java::lang::Class *clazz, 
		struct Hjava_lang_Class *kclass, 
		struct _errorInfo *einfo)
{
	for (int i = 0; i < clazz->field_count; i++) {
		neutralFieldInfo finfo;
		fillInFieldInfo(&finfo, clazz->fields + i);
		finfo.idx = clazz->field_count - i - 1;
		if (kenvMakeField(&finfo, kclass, einfo) == false) {
			return (false);
		}
	}
	return (true);
}

static bool
makeMethods(java::lang::Class *clazz, 
		struct Hjava_lang_Class *kclass, 
		struct _errorInfo *einfo)
{
	int vidx = 0;		// virtual index
	int minusone = -1;

	for (int i = 0; i < clazz->method_count; i++) {
		neutralMethodInfo minfo;
		_Jv_Method *meth = clazz->methods + i;
		fillInMethodInfo(&minfo, meth);
		/* A final classes's methods are automatically final and
		 * so are private methods.
		 */
		using namespace java::lang::reflect;

		if ((clazz->accflags & Modifier::FINAL) ||
		    (meth->accflags & Modifier::STATIC) ||
		    (meth->accflags & Modifier::FINAL) ||
		    (meth->accflags & Modifier::PRIVATE) ||
			!strcmp(meth->name->data, "<init>")) 
		{
			minfo.idx = &minusone;
		} else {
			minfo.idx = &vidx;
		}

		/* kenvMakeMethod will increase vidx as appropriate */
		if (kenvMakeMethod(&minfo, kclass, einfo) == false) {
			return (false);
		}
	}
	return (true);
}


extern "C" struct Hjava_lang_Class*
gcjFindClassByAddress(void *clazz, struct _errorInfo *einfo)
{
	java::lang::Class* ptr;
	for (ptr = preCList; ptr != 0; ptr = ptr->next) {
		if (ptr == clazz) {
DBG(GCJ, 		dprintf("%s: found class by j.l.C `%s'\n", 
				__FUNCTION__, ptr->name->data); )
			return (constructSurrogate(ptr, einfo));
		}
	}
	/* XXX fill in einfo here!? */
	return (0);
}

extern "C" bool
comparePath2ClassName(const char *cname, const char *pname) 
{
	register unsigned int a, b;

        while ((a = *cname++), (b = *pname++), a && b) {
		if (a == b || a == '/' && b == '.') {
			continue;
		}
		return (false);
	}
	return (!(a || b));
}

/*
 * This method finds a gcj class by its utf8 name.
 * We find the gcj, remove it from the list of gcj classes,
 * and create the surrogate kaffe class which we return.
 */
extern "C" struct Hjava_lang_Class*
gcjFindClassByUtf8Name(const char* utf8name, struct _errorInfo *einfo)
{
	java::lang::Class* ptr;

	for (ptr = preCList; ptr != 0; ptr = ptr->next) {
		if (comparePath2ClassName(utf8name, ptr->name->data)) {
DBG(GCJ, 		dprintf("%s: found class by name `%s'\n", 
				__FUNCTION__, utf8name); 
    )

			return (constructSurrogate(ptr, einfo));
		}
	}
	kenvPostClassNotFound(utf8name, einfo);
	return (0);
}

/*
 * Process a prebuilt class.
 * We must perform the same steps that the gcj-compiled code expects the
 * libjava/libgcj run-time to do.
 *
 * This is the construction of the kaffe surrogate fields and methods
 * and the resolution of constant pool entries.   The latter is done
 * in gcjProcessClassConstants which is invoked at a later processing
 * stage for bootstrapping reasons.
 */
bool
gcjProcessClass(struct Hjava_lang_Class* kclazz, void *gcjClass, 
	struct _errorInfo *einfo)
{
	java::lang::Class *clazz = (java::lang::Class*)gcjClass;

	if (makeMethods(clazz, kclazz, einfo) == false) {
		return (false);
	}
	if (makeFields(clazz, kclazz, einfo) == false) {
		return (false);
	}
	return (true);
}

bool
gcjProcessClassConstants(struct Hjava_lang_Class* kclazz, void *gcjClass, 
	struct _errorInfo *einfo)
{
	java::lang::Class *clazz = (java::lang::Class*)gcjClass;

DBG(GCJ, dprintf("GCJ: ProcessClassConstants `%s'\n", clazz->name->data); )

	/* from libjava/java/lang/natClassLoader.cc: _Jv_PrepareCompiledClass */
	_Jv_Constants *pool = &clazz->constants;

	/* gcj starts its constant pool at index 1 */
	for (int index = 1; index < pool->size; ++index) {
		switch (pool->tags[index]) {
		case JV_CONSTANT_Class: 
		{
			_Jv_Utf8Const *name = pool->data[index].utf8;

			struct Hjava_lang_Class *kclass;
			kclass = kenvFindClass(name->data, einfo);
			if (kclass == 0) {
				return (false);
			}
			jclass gcjcls;
			gcjcls = (jclass)kenvMalloc(sizeof(java::lang::Class), 
						    einfo);
			if (gcjcls == 0) {
				return (false);
			}

			/* prep up our fake gcjclass --- it better not be used
			 * for anything but to extract the kaffe surrogate
			 */
			gcjcls->name = name;
			GCJ2KAFFE(gcjcls) = kclass;
			/* XXX set gcjPeer in kclass maybe? */
			/* XXX set gcjcls->vtable here? */

			pool->data[index].clazz = gcjcls;
			pool->tags[index] |= JV_CONSTANT_ResolvedFlag;
			break;
		}
		case JV_CONSTANT_String:
		{
			void *str;
			_Jv_Utf8Const *u = pool->data[index].utf8;
			str = kenvMakeJavaString(u->data, u->length, u->hash);
			pool->data[index].o = (jstring)str;
			pool->tags[index] |= JV_CONSTANT_ResolvedFlag;
			break;
		}
		default:
			fprintf(stderr, 
			    "GCJ: Cannot handle constant pool item %d yet\n",
			    	(int)pool->tags[index]);
		}
	}
	return (true);
}

/*
 * Given a pointer as passed in libgcj's exception matcher, return
 * the kaffe class against which we should match.
 */
struct Hjava_lang_Class *
gcjFindMatchClass(void *minfo, struct _errorInfo *einfo)
{
	/* libgcj's exception.cc says:

	     // The match_info is either a (java::lang::Class*) or
	     // match_info is one more than a (Utf8Const*).

	    And then they code in ugly GNU style:

	      if (sizeof(void*) != sizeof(size_t))
		abort();

	      size_t mi = (size_t) match_info;
	      if ((mi & 1) != 0)
		match_info = _Jv_FindClass ((Utf8Const*) (mi - 1), NULL);
	      if (! _Jv_IsInstanceOf ((jobject) info->value,
				      (jclass) match_info))
		return NULL;
	 */

	/* is it the class's utf8 name */
	if ((((int)minfo) & 1) != 0) {
		_Jv_Utf8Const *utf8 = (_Jv_Utf8Const *)((int)minfo - 1);
		return (gcjFindClassByUtf8Name(utf8->data, einfo));
	}


	/* else it's a pointer to the class itself */
	return (kenvFindClassByAddress2(minfo, einfo));
}


/*
 * Initialise kaffe surrogate type for primitive gcj classes.
 * See libgcj/libjava/prims.cc for what libgcj does.
 * We simply set aside some sizeof(java::lang::Class) space
 * and initialize the GCJ2KAFFE and the name field.
 * We also put them in the list of preloaded classes in order for
 * lookup functions to work.
 */
class _Jv_PrimClass {
	int	_dummy[sizeof(java::lang::Class)/sizeof(int)];
}   _Jv_intClass,
    _Jv_longClass,
    _Jv_booleanClass,
    _Jv_charClass,
    _Jv_floatClass,
    _Jv_doubleClass,
    _Jv_byteClass,
    _Jv_shortClass,     
    _Jv_voidClass;

#define TYPENAME(type) type##ClassName = { 0, sizeof #type, #type }
static struct { /* must match _Jv_Utf8Const */ 
    _Jv_ushort hash;
    _Jv_ushort len;
    char	data[10];
} 
    TYPENAME(int),
    TYPENAME(long),
    TYPENAME(boolean),
    TYPENAME(char),
    TYPENAME(float),
    TYPENAME(double),
    TYPENAME(byte),
    TYPENAME(short),
    TYPENAME(void);
#undef TYPENAME

void 
gcjInitPrimitiveClasses(void)
{
#define KCLASS(type) { \
	java::lang::Class* c = (java::lang::Class*)&_Jv_##type##Class;  \
	GCJ2KAFFE(c) = type##Class;   					\
	c->name = (_Jv_Utf8Const*)&type##ClassName;			\
	_Jv_RegisterClass(c);						\
}
	KCLASS(int)
	KCLASS(long)
	KCLASS(boolean)
	KCLASS(char)
	KCLASS(float)
	KCLASS(double)
	KCLASS(byte)
	KCLASS(short)
	KCLASS(void)
#undef KCLASS
	/* check that these flag values are still the same */
	assert (FIELD_UNRESOLVED_FLAG == _Jv_FIELD_UNRESOLVED_FLAG);
	assert (FIELD_CONSTANT_VALUE == _Jv_FIELD_CONSTANT_VALUE);
}

/* 
 * Find a primitive classes name.
 */
static char *
findPrimitiveClass(void *symbol)
{
#define I(type) 				\
	if (symbol == &_Jv_##type##Class) {	\
		return (";" #type);		\
	} 

	I(int) else
	I(long) else
	I(boolean) else
	I(char) else
	I(float) else
	I(double) else
	I(byte) else
	I(short) else
	I(void) else { return (0); }
#undef I
}

#endif /* HAVE_GCJ_SUPPORT */
