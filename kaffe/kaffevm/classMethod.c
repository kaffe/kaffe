/*
 * classMethod.c
 * Dictionary of classes, methods and fields.
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#define SDBG(s)
#define	CDBG(s)
#define	MDBG(s)
#define	FDBG(s)
#define	LDBG(s)

#include "config.h"
#include "config-std.h"
#include "config-mem.h"
#include "gtypes.h"
#include "slots.h"
#include "access.h"
#include "object.h"
#include "classMethod.h"
#include "code.h"
#include "file.h"
#include "readClass.h"
#include "baseClasses.h"
#include "thread.h"
#include "itypes.h"
#include "errors.h"
#include "exception.h"
#include "md.h"
#include "external.h"
#include "lookup.h"
#include "support.h"
#include "gc.h"
#include "locks.h"
#include "md.h"

#define	CLASSHASHSZ	256	/* Must be a power of two */
static iLock classHashLock;
static bool chlinit = false;
static classEntry* classEntryPool[CLASSHASHSZ];

#if 0
#define	METHOD_TRUE_NCODE(METH)			(METH)->c.ncode.ncode_start
#define	METHOD_PRE_COMPILED(METH)		((int16)(METH)->localsz < 0)
#define	SET_METHOD_PRE_COMPILED(METH, VAL)	((METH)->localsz = -(VAL))
#endif
#define METHOD_NEEDS_TRAMPOLINE(meth) \
	(!METHOD_TRANSLATED(meth) || (((meth)->accflags & ACC_STATIC) && (meth)->class->state < CSTATE_DOING_INIT))


static Hjava_lang_Class* SerialInterface[1];

extern gcFuncs gcClassObject;

extern void findClass(classEntry*);
extern void verify2(Hjava_lang_Class*);
extern void verify3(Hjava_lang_Class*);

static void internalSetupClass(Hjava_lang_Class*, Utf8Const*, int, int, Hjava_lang_ClassLoader*);

static void buildDispatchTable(Hjava_lang_Class*);
static void buildInterfaceDispatchTable(Hjava_lang_Class*);
static void allocStaticFields(Hjava_lang_Class*);
static void resolveObjectFields(Hjava_lang_Class*);
static void resolveStaticFields(Hjava_lang_Class*);
static void resolveConstants(Hjava_lang_Class*);

#if !defined(ALIGNMENT_OF_SIZE)
#define	ALIGNMENT_OF_SIZE(S)	(S)
#endif

/*
 * Process all the stage of a classes initialisation.  We can provide
 * a state to aim for (so we don't have to do this all at once).  This
 * is called by various parts of the machine in order to load, link
 * and initialise the class.  Putting it all together here makes it a damn
 * sight easier to understand what's happening.
 */
void
processClass(Hjava_lang_Class* class, int tostate)
{
	int i;
	int j;
	int k;
	Method* meth;
	Hjava_lang_Class* nclass;
	Hjava_lang_Class** newifaces;
	static bool init;
	static iLock classLock;

	/* If this class is initialised to the required point, quit now */
	if (class->state >= tostate) {
		return;
	}

	/* Initialise class lock */
	if (init == false) {
		init = true;
		initStaticLock(&classLock);
	}

#define	SET_CLASS_STATE(S)	class->state = (S)
#define	DO_CLASS_STATE(S)	if ((S) > class->state && (S) <= tostate)

	/* For the moment we only allow one thread to initialise any classes
	 * at once.  This is because we've got circular class dependencies
	 * we've got to work out.
	 */

	lockStaticMutex(&classLock);

	DO_CLASS_STATE(CSTATE_PREPARED) {

		/* Check for circular dependent classes */
		if (class->state == CSTATE_DOING_PREPARE) {
			unlockStaticMutex(&classLock);
			throwException(ClassCircularityError);
		}

		/* Allocate any static space required by class and initialise
		 * the space with any constant values.  This isn't necessary
		 * for pre-loaded classes.
		 */
		if (class->state != CSTATE_PRELOADED) {
			allocStaticFields(class);
		}

		SET_CLASS_STATE(CSTATE_DOING_PREPARE);

		/* Load and link the super class */
		if (class->superclass) {
			class->superclass = getClass((uintp)class->superclass, class);
			processClass(class->superclass, CSTATE_LINKED);
			CLASS_FSIZE(class) = CLASS_FSIZE(class->superclass);
		}

		/* Load all the implemented interfaces. */
		j = class->interface_len;
		nclass = class->superclass;
		if (nclass != 0 && nclass != ObjectClass) {
			/* If class is an interface, include the superclass
			 * as well.
			 */
			if (CLASS_IS_INTERFACE(class)) {
				j += 1;
			}
			j += class->superclass->total_interface_len;
		}
		for (i = 0; i < class->interface_len; i++) {
			uintp iface = (uintp)class->interfaces[i];
			class->interfaces[i] = getClass(iface, class);
			j += class->interfaces[i]->total_interface_len;
		}
		class->total_interface_len = j;

		/* We build a list of *all* interfaces this class can use */
		if (class->interface_len != class->total_interface_len) {
			newifaces = (Hjava_lang_Class**)gc_malloc(sizeof(Hjava_lang_Class**) * j, GC_ALLOC_INTERFACE);
			for (i = 0; i < class->interface_len; i++) {
				newifaces[i] = class->interfaces[i];
			}
			nclass = class->superclass;
			if (nclass != 0 && nclass != ObjectClass) {
				if (CLASS_IS_INTERFACE(class)) {
					newifaces[i] = nclass;
					i++;
				}
				for (j = 0; j < nclass->total_interface_len; j++, i++) {
					newifaces[i] = nclass->interfaces[j];
				}
			}
			for (k = 0; k < class->interface_len; k++) {
				nclass = class->interfaces[k];
				for (j = 0; j < nclass->total_interface_len; j++, i++) {
					newifaces[i] = nclass->interfaces[j];
				}
			}
			class->interfaces = newifaces;
		}

		/* This shouldn't be necessary - but it is at the moment!! */
		class->head.dtable = ClassClass->dtable;

		resolveObjectFields(class);
		resolveStaticFields(class);
		/* Build dispatch table.  We must handle interfaces a little
		 * differently since they only have a <clinit> method.
		 */
		if (!CLASS_IS_INTERFACE(class)) {
			buildDispatchTable(class);
		}
		else {
			buildInterfaceDispatchTable(class);
		}

		SET_CLASS_STATE(CSTATE_PREPARED);
	}

	DO_CLASS_STATE(CSTATE_LINKED) {

		/* Okay, flag we're doing link */
		SET_CLASS_STATE(CSTATE_DOING_LINK);

		/* Second stage verification - check the class format is okay */
		verify2(class);

		/* Third stage verification - check the bytecode is okay */
		verify3(class);

		/* And note that it's done */
		SET_CLASS_STATE(CSTATE_LINKED);
	}

	DO_CLASS_STATE(CSTATE_OK) {

		/* If init is in progress return.  This must be the same
		 * thread because we lock the class when we come in here.
		 */
		if (class->state == CSTATE_DOING_INIT) {
			unlockStaticMutex(&classLock);
			return;
		}

		SET_CLASS_STATE(CSTATE_DOING_CONSTINIT);

		/* Initialise the constants */
		resolveConstants(class);

		SET_CLASS_STATE(CSTATE_CONSTINIT);

		if (class->superclass != NULL) {
			processClass(class->superclass, CSTATE_OK);
		}

		SET_CLASS_STATE(CSTATE_DOING_INIT);

SDBG(		dprintf("Initialising %s static %d\n", class->name->data,
			CLASS_FSIZE(class)); 	)
		meth = findMethodLocal(class, init_name, void_signature);
		if (meth != NULL) {
			callMethodA(meth, METHOD_INDIRECTMETHOD(meth), 0, 0, 0);
			/* Since we'll never run this again we might as well
			 * loose it now.
			 */
			METHOD_NATIVECODE(meth) = 0;
			meth->c.ncode.ncode_start = 0;
			meth->c.ncode.ncode_end = 0;
		}
		SET_CLASS_STATE(CSTATE_OK);
	}

	unlockStaticMutex(&classLock);
}

Hjava_lang_Class*
setupClass(Hjava_lang_Class* cl, constIndex c, constIndex s, u2 flags, Hjava_lang_ClassLoader* loader)
{
	constants* pool;

	pool = CLASS_CONSTANTS(cl);

	/* Find the name of the class */
	if (pool->tags[c] != CONSTANT_Class) {
CDBG(		printf("setupClass: not a class.\n");			)
		return (0);
	}

	internalSetupClass(cl, WORD2UTF(pool->data[c]), flags, s, loader);

	return (cl);
}

#if INTERN_UTF8CONSTS
static
int
hashClassName(Utf8Const *name)
{
	int len = (name->length+1) >> 1;  /* Number of shorts */
	uint16 *ptr = (uint16*) name->data;
	int hash = len;
	while (--len >= 0) {
		hash = (hash << 1) ^ *ptr++;
	}
	return (hash);
}
#else
#define hashClassName(NAME) ((NAME)->hash)
#endif

/*
 * Take a prepared class and register it with the class pool.
 */
void
registerClass(classEntry* entry)
{
	lockMutex(entry);

	/*
	 * It is necessary to add the entry->class to the GC in some
	 * manner so anything it references will also be part of the GC.
	 * however, this isn't possible right now.
	 */

	unlockMutex(entry);
}

static
void
internalSetupClass(Hjava_lang_Class* cl, Utf8Const* name, int flags, int su, Hjava_lang_ClassLoader* loader)
{
	cl->name = name;
	CLASS_METHODS(cl) = NULL;
	CLASS_NMETHODS(cl) = 0;
	cl->superclass = (Hjava_lang_Class*)(uintp)su;
	cl->msize = 0;
	CLASS_FIELDS(cl) = 0;
	CLASS_FSIZE(cl) = 0;
	cl->accflags = flags;
	cl->dtable = 0;
        cl->interfaces = 0;
	cl->interface_len = 0;
	cl->state = CSTATE_LOADED;
	cl->loader = loader;
}

Method*
addMethod(Hjava_lang_Class* c, method_info* m)
{
	constIndex nc;
	constIndex sc;
	Method* mt;
	constants* pool;
	Utf8Const* name;
	Utf8Const* signature;
#ifdef DEBUG
	int ni;
#endif

	pool = CLASS_CONSTANTS (c);

	nc = m->name_index;
	if (pool->tags[nc] != CONSTANT_Utf8) {
MDBG(		printf("addMethod: no method name.\n");			)
		return (0);
	}
	sc = m->signature_index;
	if (pool->tags[sc] != CONSTANT_Utf8) {
MDBG(		printf("addMethod: no signature name.\n");		)
		return (0);
	}
	name = WORD2UTF (pool->data[nc]);
	signature = WORD2UTF (pool->data[sc]);
  
#ifdef DEBUG
	/* Search down class for method name - don't allow duplicates */
	for (ni = CLASS_NMETHODS(c), mt = CLASS_METHODS(c); --ni >= 0; ) {
		assert(! equalUtf8Consts (name, mt->name)
		       || ! equalUtf8Consts (signature, mt->signature));
	}
#endif

MDBG(	printf("Adding method %s:%s%s (%x)\n", c->name->data, WORD2UTF(pool->data[nc])->data, WORD2UTF(pool->data[sc])->data, m->access_flags);	)

	mt = &c->methods[c->nmethods++];
	mt->name = name;
	mt->signature = signature;
	mt->class = c;
	mt->accflags = m->access_flags;
	mt->c.bcode.code = 0;
	mt->stacksz = 0;
	mt->localsz = 0;
	mt->exception_table = 0;
	mt->idx = -1;

	/* Mark constructors as such */
	if (equalUtf8Consts (name, constructor_name)) {
		mt->accflags |= ACC_CONSTRUCTOR;
	}

	return (mt);
}

Field*
addField(Hjava_lang_Class* c, field_info* f)
{
	constIndex nc;
	constIndex sc;
	Field* ft;
	constants* pool;
	int index;
	char* sig;

	pool = CLASS_CONSTANTS (c);

	nc = f->name_index;
	if (pool->tags[nc] != CONSTANT_Utf8) {
FDBG(		printf("addField: no field name.\n");			)
		return (0);
	}

	--CLASS_FSIZE(c);
	if (f->access_flags & ACC_STATIC) {
		index = CLASS_NSFIELDS(c)++;
	}
	else {
		index = CLASS_FSIZE(c) + CLASS_NSFIELDS(c);
	}
	ft = &CLASS_FIELDS(c)[index];

FDBG(	printf("Adding field %s:%s\n", c->name, pool->data[nc].v.tstr);	)

	sc = f->signature_index;
	if (pool->tags[sc] != CONSTANT_Utf8) {
FDBG(		printf("addField: no signature name.\n");		)
		return (0);
	}
	ft->name = WORD2UTF(pool->data[nc]);
	ft->accflags = f->access_flags;

	sig = CLASS_CONST_UTF8(c, sc)->data;
	if (sig[0] == 'L' || sig[0] == '[') {
		FIELD_TYPE(ft) = (Hjava_lang_Class*)CLASS_CONST_UTF8(c, sc);
		FIELD_SIZE(ft) = PTR_TYPE_SIZE;
		ft->accflags |= FIELD_UNRESOLVED_FLAG;
	}
	else {
		FIELD_TYPE(ft) = getClassFromSignature(sig, 0);
		FIELD_SIZE(ft) = TYPE_PRIM_SIZE(FIELD_TYPE(ft));
	}

	return (ft);
}

void
setFieldValue(Field* ft, u2 idx)
{
	/* Set value index */
	FIELD_CONSTIDX(ft) = idx;
	ft->accflags |= FIELD_CONSTANT_VALUE;
}

void
finishFields(Hjava_lang_Class *cl)
{
	Field tmp;
	Field* fld;
	int n;

	/* Reverse the instance fields, so they are in "proper" order. */
	fld = CLASS_IFIELDS(cl);
	n = CLASS_NIFIELDS(cl);
	while (n > 1) {
		tmp = fld[0];
		fld[0] = fld[n-1];
		fld[n-1] = tmp;
		fld++;
		n -= 2;
	}
}

void
addMethodCode(Method* m, Code* c)
{
	m->c.bcode.code = c->code;
	m->c.bcode.codelen = c->code_length;
	m->stacksz = c->max_stack;
	m->localsz = c->max_locals;
	m->exception_table = c->exception_table;
}

void
addInterfaces(Hjava_lang_Class* c, int inr, Hjava_lang_Class** inf)
{
	assert(inr > 0);

        c->interfaces = inf;
	c->interface_len = inr;
}

/*
 * Lookup a named class, loading it if necessary.
 * The name is as used in class files, e.g. "java/lang/String".
 */
Hjava_lang_Class*
loadClass(Utf8Const* name, Hjava_lang_ClassLoader* loader)
{
	classEntry* centry;
	Hjava_lang_Class* clazz = NULL;

        centry = lookupClassEntry(name, loader);
	if (centry->class != NULL) {
		goto found;
	}

	/* 
	 * Failed to find class, so must now load it.
	 *
	 * If we use a class loader, we must call out to Java code.
	 * That means we cannot lock centry.  This is okay, however,
	 * since the only way the Java code can get ahold of a class
	 * object is by calling findSystemClass, which calls loadClass
	 * with loader==NULL or by doing a defineClass, which locks centry.
	 */
	if (loader != NULL) {
		Hjava_lang_String* str;

LDBG(		printf("classLoader: loading %s\n", name->data); )
		str = makeReplaceJavaStringFromUtf8(name->data, name->length, '/', '.');
		clazz = (Hjava_lang_Class*)do_execute_java_method((Hjava_lang_Object*)loader, "loadClass", "(Ljava/lang/String;Z)Ljava/lang/Class;", 0, false, str, true).l;
		if (clazz == NULL) {
			throwException(ClassNotFoundException(name->data));
		}
		clazz->centry = centry;
LDBG(		printf("classLoader: done\n");			)
	} 

	/* Lock centry before setting centry->class */
	lockMutex(centry);

	/* Check again in case someone else did it */
	if (centry->class == NULL) {

		if (loader == NULL) {
			/* findClass will set centry->class if it finds it */
			findClass(centry);
			clazz = centry->class;
		} else {
			centry->class = clazz;
		}

		/* We process the class while we're holding the centry lock 
		 * so that other threads will find a processed class if 
		 * centry->class is not null.
		 */
		if (clazz != NULL)
			processClass(clazz, CSTATE_LINKED);
	}

	/* Release lock now class has been entered and processed */
	unlockMutex(centry);

	if (clazz == NULL) {
		throwException(ClassNotFoundException(name->data));
	}

	found:;
	return (centry->class);
}

Hjava_lang_Class*
loadArray(Utf8Const* name, Hjava_lang_ClassLoader* loader)
{
	return (lookupArray(getClassFromSignature(&name->data[1], loader)));
}

void
loadStaticClass(Hjava_lang_Class** class, char* name)
{
	classEntry* centry;

	(*class) = newClass();

        centry = lookupClassEntry(makeUtf8Const(name, -1), 0);
	lockMutex(centry);
	if (centry->class == 0) {
		centry->class = *class;
		findClass(centry);
	}
	unlockMutex(centry);

	processClass(centry->class, CSTATE_LINKED);
}

Hjava_lang_Class*
lookupClass(char* name)
{
	Hjava_lang_Class* class;

	class = loadClass(makeUtf8Const(name, -1), NULL);
	processClass(class, CSTATE_OK);

	return (class);
}

/*
 * Return FIELD_TYPE(FLD), but if !FIELD_RESOLVED, resolve the field first.
 */
Hjava_lang_Class*
resolveFieldType(Field *fld, Hjava_lang_Class* this)
{
	Hjava_lang_Class* clas;
	char* name;

	/* Avoid locking if we can */
	if (FIELD_RESOLVED(fld)) {
		return (FIELD_TYPE(fld));
	}

	/* We lock the class while we retrieve the field name since someone
	 * else may update it while we're doing this.  Once we've got the
	 * name we don't really care.
	 */
	lockMutex(this->centry);
	if (FIELD_RESOLVED(fld)) {
		unlockMutex(this->centry);
		return (FIELD_TYPE(fld));
	}
	name = ((Utf8Const*)fld->type)->data;
	unlockMutex(this->centry);

	clas = getClassFromSignature(name, this->loader);

	FIELD_TYPE(fld) = clas;
	fld->accflags &= ~FIELD_UNRESOLVED_FLAG;

	return (clas);
}

static
void
resolveObjectFields(Hjava_lang_Class* class)
{
	int fsize;
	int align;
	Field* fld;
	int n;
	int offset;

	/* Find start of new fields in this object.  If start is zero, we must
	 * allow for the object headers.
	 */
	offset = CLASS_FSIZE(class);
	if (offset == 0) {
		offset = sizeof(Hjava_lang_Object);
	}

	/* Find the largest alignment in this class */
	align = 1;
	fld = CLASS_IFIELDS(class);
	n = CLASS_NIFIELDS(class);
	for (; --n >= 0; fld++) {
		fsize = FIELD_SIZE(fld);
		/* Work out alignment for this size entity */
		fsize = ALIGNMENT_OF_SIZE(fsize);
		/* If field is bigger than biggest alignment, change
		 * biggest alignment
		 */
		if (fsize > align) {
			align = fsize;
		}
	}

	/* Align start of this class's data */
	offset = ((offset + align - 1) / align) * align;

	/* Now work out where to put each field */
	fld = CLASS_IFIELDS(class);
	n = CLASS_NIFIELDS(class);
	for (; --n >= 0; fld++) {
		fsize = FIELD_SIZE(fld);
		/* Align field */
		align = ALIGNMENT_OF_SIZE(fsize);
		offset = ((offset + align - 1) / align) * align;
		FIELD_OFFSET(fld) = offset;
		offset += fsize;
	}

	CLASS_FSIZE(class) = offset;
}

/*
 * Allocate the space for the static class data.
 */
static
void
allocStaticFields(Hjava_lang_Class* class)
{
	int fsize;
	int align;
	uint8* mem;
	int offset;
	int n;
	Field* fld;

	/* No static fields */
	if (CLASS_NSFIELDS(class) == 0) {
		return;
	}

	/* Calculate size and position of static data */
	offset = 0;
	n = CLASS_NSFIELDS(class);
	fld = CLASS_SFIELDS(class);
	for (; --n >= 0; fld++) {
		fsize = FIELD_SIZE(fld);
		/* Align field offset */
		align = ALIGNMENT_OF_SIZE(fsize);
		offset = ((offset + align - 1) / align) * align;
		FIELD_SIZE(fld) = offset;
		offset += fsize;
	}

	/* Allocate memory required */
	mem = gc_malloc(offset, GC_ALLOC_STATICDATA);

	/* Rewalk the fields, pointing them at the relevant memory and/or
	 * setting any constant values.
	 */
	fld = CLASS_SFIELDS(class);
	n = CLASS_NSFIELDS(class);
	for (; --n >= 0; fld++) {
		offset = FIELD_SIZE(fld);
		FIELD_SIZE(fld) = FIELD_CONSTIDX(fld);	/* Keep idx in size */
		FIELD_ADDRESS(fld) = mem + offset;
	}
}

static
void
resolveStaticFields(Hjava_lang_Class* class)
{
	uint8* mem;
	constants* pool;
	Field* fld;
	int idx;
	int n;

	lockMutex(class->centry);

	pool = CLASS_CONSTANTS(class);
	fld = CLASS_SFIELDS(class);
	n = CLASS_NSFIELDS(class);
	for (; --n >= 0; fld++) {
		if ((fld->accflags & FIELD_CONSTANT_VALUE) != 0) {

			mem = FIELD_ADDRESS(fld);
			idx = FIELD_SIZE(fld);

			switch (CONST_TAG(idx, pool)) {
			case CONSTANT_Integer:
				if (FIELD_TYPE(fld) == booleanClass ||
				    FIELD_TYPE(fld) == byteClass) {
					*(jbyte*)mem = CLASS_CONST_INT(class, idx);
					FIELD_SIZE(fld) = TYPE_PRIM_SIZE(byteClass);
				}
				else if (FIELD_TYPE(fld) == charClass ||
					 FIELD_TYPE(fld) == shortClass) {
					*(jshort*)mem = CLASS_CONST_INT(class, idx);
					FIELD_SIZE(fld) = TYPE_PRIM_SIZE(shortClass);
				}
				else {
					*(jint*)mem = CLASS_CONST_INT(class, idx);
					FIELD_SIZE(fld) = TYPE_PRIM_SIZE(intClass);
				}
				break;

			case CONSTANT_Float:
				*(jint*)mem = CLASS_CONST_INT(class, idx);
				FIELD_SIZE(fld) = TYPE_PRIM_SIZE(floatClass);
				break;

			case CONSTANT_Long:
			case CONSTANT_Double:
				*(jlong*)mem = CLASS_CONST_LONG(class, idx);
				FIELD_SIZE(fld) = TYPE_PRIM_SIZE(longClass);
				break;

			case CONSTANT_String:
				pool->data[idx] = (ConstSlot)Utf8Const2JavaString(WORD2UTF(pool->data[idx]));
				pool->tags[idx] = CONSTANT_ResolvedString;
				/* ... fall through ... */
			case CONSTANT_ResolvedString:
				*(jref*)mem = (jref)CLASS_CONST_DATA(class, idx);
				FIELD_SIZE(fld) = PTR_TYPE_SIZE;
				break;
			}
		}
	}

	unlockMutex(class->centry);
}

static
void
buildDispatchTable(Hjava_lang_Class* class)
{
	Method* meth;
	void** mtab;
	int i;
#if defined(TRANSLATOR)
	int ntramps = 0;
	methodTrampoline* tramp;
#endif

	if (class->superclass != NULL) {
		class->msize = class->superclass->msize;
	}
	else {
		class->msize = 0;
	}

	meth = CLASS_METHODS(class);
	i = CLASS_NMETHODS(class);
	for (; --i >= 0; meth++) {
		Hjava_lang_Class* super = class->superclass;
#if defined(TRANSLATOR)
		if (METHOD_NEEDS_TRAMPOLINE(meth)) {
			ntramps++;
		}
#endif
		if ((meth->accflags & ACC_STATIC)
		    || equalUtf8Consts(meth->name, constructor_name)) {
			meth->idx = -1;
			continue;
		}
		/* Search superclasses for equivalent method name.
		 * If found extract its index nr.
		 */
		for (; super != NULL;  super = super->superclass) {
			int j = CLASS_NMETHODS(super);
			Method* mt = CLASS_METHODS(super);
			for (; --j >= 0;  ++mt) {
				if (equalUtf8Consts (mt->name, meth->name)
				    && equalUtf8Consts (mt->signature,
							meth->signature)) {
					meth->idx = mt->idx;
					goto foundmatch;
				}
			}
		}
		/* No match found so allocate a new index number */
		meth->idx = class->msize++;
		foundmatch:;
	}

#if defined(TRANSLATOR)
	/* Allocate the dispatch table and this class' trampolines all in
	   one block of memory.  This is primarily done for GC reasons in
	   that I didn't want to add another slot on class just for holding
	   the trampolines, but it also works out for space reasons.  */
	class->dtable = (dispatchTable*)gc_malloc(sizeof(dispatchTable) + class->msize * sizeof(void*) + ntramps * sizeof(methodTrampoline), GC_ALLOC_DISPATCHTABLE);
	tramp = (methodTrampoline*) &class->dtable->method[class->msize];
#else
	class->dtable = (dispatchTable*)gc_malloc(sizeof(dispatchTable) + (class->msize * sizeof(void*)), GC_ALLOC_DISPATCHTABLE);
#endif

	class->dtable->class = class;
	mtab = class->dtable->method;

	/* Install inherited methods into dispatch table. */
	if (class->superclass != NULL) {
		Method** super_mtab = (Method**)class->superclass->dtable->method;
		for (i = 0; i < class->superclass->msize; i++) {
			mtab[i] = super_mtab[i];
		}
	}

	meth = CLASS_METHODS(class);
	i = CLASS_NMETHODS(class);
#if defined(TRANSLATOR)
	for (; --i >= 0; meth++) {
		if (METHOD_NEEDS_TRAMPOLINE(meth)) {
#if 0
			if (METHOD_TRANSLATED(meth)) {
				SET_METHOD_PRE_COMPILED(meth, 1);
				METHOD_TRUE_NCODE(meth) = METHOD_NATIVECODE(meth);
			}
#endif
			FILL_IN_TRAMPOLINE(tramp, meth);
			METHOD_NATIVECODE(meth) = (nativecode*)tramp;
			tramp++;
		}
		if (meth->idx >= 0) {
			mtab[meth->idx] = METHOD_NATIVECODE(meth);
		}
	}
	FLUSH_DCACHE(class->dtable, tramp);
#else
	for (;  --i >= 0; meth++) {
		if (meth->idx >= 0) {
			mtab[meth->idx] = meth;
		}
	}
#endif

	/* Check for undefined abstract methods if class is not abstract.
	 * See "Java Language Specification" (1996) section 12.3.2. */
	if ((class->accflags & ACC_ABSTRACT) == 0) {
		for (i = class->msize - 1; i >= 0; i--) {
			if (mtab[i] == NULL) {
				throwException(AbstractMethodError);
			}
		}
	}
}

static
void
buildInterfaceDispatchTable(Hjava_lang_Class* class)
{
#if defined(TRANSLATOR)
	Method* meth;
	int i;

	meth = CLASS_METHODS(class);
	i = CLASS_NMETHODS(class);

	/* Search methods for <clinit> */
	for (; i > 0; i--, meth++) {
		if (equalUtf8Consts(meth->name, init_name)) {
			goto found;
		}
	}
	return;

	found:;
	if (METHOD_NEEDS_TRAMPOLINE(meth)) {
		methodTrampoline* tramp = (methodTrampoline*)gc_malloc(sizeof(methodTrampoline), GC_ALLOC_DISPATCHTABLE);
		FILL_IN_TRAMPOLINE(tramp, meth);
		METHOD_NATIVECODE(meth) = (nativecode*)tramp;
		FLUSH_DCACHE(tramp, tramp+1);
	}
#endif
}

/*
 * Initialise the constants.
 * First we make sure all the constant strings are converted to java strings.
 */
static
void
resolveConstants(Hjava_lang_Class* class)
{
	int idx;
	constants* pool;

	lockMutex(class->centry);

	/* Scan constant pool and convert any constant strings into true
	 * java strings.
	 */
	pool = CLASS_CONSTANTS (class);
	for (idx = 0; idx < pool->size; idx++) {
		switch (pool->tags[idx]) {
		case CONSTANT_String:
			pool->data[idx] = (ConstSlot)Utf8Const2JavaString(WORD2UTF(pool->data[idx]));
			pool->tags[idx] = CONSTANT_ResolvedString;
			break;

		case CONSTANT_Class:
			getClass(idx, class);
			break;
		}
	}

	unlockMutex(class->centry);
}

classEntry*
lookupClassEntry(Utf8Const* name, Hjava_lang_ClassLoader* loader)
{
	classEntry* entry;
	classEntry** entryp;

        if (chlinit == false) {
		chlinit = true;
		initStaticLock(&classHashLock);
        }

	entry = classEntryPool[hashClassName(name) & (CLASSHASHSZ-1)];
	for (; entry != 0; entry = entry->next) {
		if (equalUtf8Consts(name, entry->name) && loader == entry->loader) {
			return (entry);
		}
	}

	/* Failed to find class entry - create a new one */
	entry = gc_malloc_fixed(sizeof(classEntry));
	entry->name = name;
	entry->loader = loader;
	entry->class = 0;
	entry->next = 0;

	/* Lock the class table and insert entry into it (if not already
	   there) */
        lockStaticMutex(&classHashLock);

	entryp = &classEntryPool[hashClassName(name) & (CLASSHASHSZ-1)];
	for (; *entryp != 0; entryp = &(*entryp)->next) {
		if (equalUtf8Consts(name, (*entryp)->name) && loader == (*entryp)->loader) {
			/* Someone else added it - discard ours and return
			   the new one. */
			gc_free_fixed(entry);
			return (*entryp);
		}
	}

	/* Add ours to end of hash */
	*entryp = entry;

	/* We keep an extra reference to the utf8 name so it won't be GCed */
	gc_add_ref(entry->name);

        unlockStaticMutex(&classHashLock);

	return (entry);
}

/*
 * Lookup a named field.
 */
Field*
lookupClassField(Hjava_lang_Class* clp, Utf8Const* name, bool isStatic)
{
	Field* fptr;
	int n;

	/* Search down class for field name */
	if (isStatic) {
		fptr = CLASS_SFIELDS(clp);
		n = CLASS_NSFIELDS(clp);
	}
	else {
		fptr = CLASS_IFIELDS(clp);
		n = CLASS_NIFIELDS(clp);
	}
	while (--n >= 0) {
		if (equalUtf8Consts (name, fptr->name)) {
			/* Resolve field if necessary */
			resolveFieldType(fptr, clp);
			return (fptr);
		}
		fptr++;
	}
FDBG(	printf("Class:field lookup failed %s:%s\n", c, f);		)
	return (0);
}

/*
 * Determine the number of arguments and return values from the
 * method signature.
 */
void
countInsAndOuts(char* str, short* ins, short* outs, char* outtype)
{
	*ins = sizeofSig(&str, false);
	*outtype = str[0];
	*outs = sizeofSig(&str, false);
}

/*
 * Calculate size of data item based on signature.
 */
int
sizeofSig(char** strp, bool want_wide_refs)
{
	int count;
	int c;

	count = 0;
	for (;;) {
		c = sizeofSigItem(strp, want_wide_refs);
		if (c == -1) {
			return (count);
		}
		count += c;
	}
}

/*
 * Calculate size (in words) of a signature item.
 */
int
sizeofSigItem(char** strp, bool want_wide_refs)
{
	int count;
	char* str;

	for (str = *strp; ; str++) {
		switch (*str) {
		case '(':
			continue;
		case 0:
		case ')':
			count = -1;
			break;
		case 'V':
			count = 0;
			break;
		case 'I':
		case 'Z':
		case 'S':
		case 'B':
		case 'C':
		case 'F':
			count = 1;
			break;
		case 'D':
		case 'J':
			count = 2;
			break;
		case '[':
			count = want_wide_refs ? sizeof(void*) / sizeof(int32) : 1;
			arrayofarray:
			str++;
			if (*str == 'L') {
				while (*str != ';') {
					str++;
				}
			}
			else if (*str == '[') {
				goto arrayofarray;
			}
			break;
		case 'L':
			count = want_wide_refs ? sizeof(void*) / sizeof(int32) : 1;
			/* Skip to end of reference */
			while (*str != ';') {
				str++;
			}
			break;
		default:
			count = 0;	/* avoid compiler warning */
			ABORT();
		}

		*strp = str + 1;
		return (count);
	}
}

/*
 * Find (or create) an array class with component type C.
 */
Hjava_lang_Class*
lookupArray(Hjava_lang_Class* c)
{
	Utf8Const *arr_name;
	char sig[CLASSMAXSIG];  /* FIXME! unchecked fixed buffer! */
	classEntry* centry;
	Hjava_lang_Class* arr_class;
	int arr_flags;

	/* Build signature for array type */
	if (CLASS_IS_PRIMITIVE (c)) {
		arr_class = CLASS_ARRAY_CACHE(c);
		if (arr_class) {
			return (arr_class);
		}
		sprintf (sig, "[%c", CLASS_PRIM_SIG(c));
	}
	else {
		char* cname = CLASS_CNAME (c);
		sprintf (sig, cname[0] == '[' ? "[%s" : "[L%s;", cname);
	}
	arr_name = makeUtf8Const(sig, -1);
	centry = lookupClassEntry(arr_name, c->loader);

	if (centry->class != 0) {
#if !INTERN_UTF8CONSTS
		gc_free(arr_name);
#endif
		goto found;
	}

	/* Lock class entry */
	lockMutex(centry);

	/* Incase someone else did it */
	if (centry->class != 0) {
		unlockMutex(centry);
#if !INTERN_UTF8CONSTS
		gc_free(arr_name);
#endif
		goto found;
	}

	arr_class = newClass();
	centry->class = arr_class;
	/*
	 * This is what Sun's JDK returns for A[].class.getModifiers();
	 */
	arr_flags = ACC_ABSTRACT | ACC_FINAL | ACC_PUBLIC;
	internalSetupClass(arr_class, arr_name, arr_flags, 0, c->loader);
	arr_class->superclass = ObjectClass;
	buildDispatchTable(arr_class);
	CLASS_ELEMENT_TYPE(arr_class) = c;
	if (SerialInterface[0] == 0) {
		SerialInterface[0] = SerialClass;
	}
	addInterfaces(arr_class, 1, SerialInterface);
	arr_class->total_interface_len = arr_class->interface_len;
	arr_class->head.dtable = ClassClass->dtable;
	arr_class->state = CSTATE_OK;
	arr_class->centry = centry;

	unlockMutex(centry);

	found:;
	if (CLASS_IS_PRIMITIVE(c)) {
		CLASS_ARRAY_CACHE(c) = centry->class;
	}

	return (centry->class);
}

#if defined(TRANSLATOR)
/*
 * Find method containing pc.
 */
Method*
findMethodFromPC(uintp pc)
{
	classEntry* entry;
	Method* ptr;
	int ipool;
	int imeth;

	for (ipool = CLASSHASHSZ;  --ipool >= 0; ) {
		for (entry = classEntryPool[ipool];  entry != NULL; entry = entry->next) {
			if (entry->class != 0) {
				imeth = CLASS_NMETHODS(entry->class);
				ptr = CLASS_METHODS(entry->class);
				for (; --imeth >= 0;  ptr++) {
					if (pc >= (uintp)METHOD_NATIVECODE(ptr) && pc < (uintp)ptr->c.ncode.ncode_end) {
						return (ptr);
					}
				}
			}
		}
	}
	return (NULL);
}
#endif
