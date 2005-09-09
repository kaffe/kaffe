/*
 * classMethod.c
 * Dictionary of classes, methods and fields.
 *
 * Copyright (c) 1996, 1997, 2004
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

#include "config.h"
#include "debug.h"
#include "config-std.h"
#include "config-mem.h"
#include "config-hacks.h"
#include "defs.h"
#include "gtypes.h"
#include "slots.h"
#include "access.h"
#include "object.h"
#include "errors.h"
#include "code.h"
#include "file.h"
#include "readClass.h"
#include "baseClasses.h"
#include "stringSupport.h"
#include "stackTrace.h"
#include "thread.h"
#include "jthread.h"
#include "itypes.h"
#include "bytecode.h"
#include "exception.h"
#include "classMethod.h"
#include "md.h"
#include "external.h"
#include "lookup.h"
#include "support.h"
#include "stats.h"
#include "gc.h"
#include "locks.h"
#include "md.h"
#include "jni.h"
#include "soft.h"
#include "methodCache.h"
#include "gcj/gcj.h"
#include "xprofiler.h"
#include "debugFile.h"
#include "jvmpi_kaffe.h"
#include "kaffe/jmalloc.h"
#include "methodcalls.h"

#if 0
#define	METHOD_TRUE_NCODE(METH)			(METH)->c.ncode.ncode_start
#define	METHOD_PRE_COMPILED(METH)		((int16)(METH)->localsz < 0)
#define	SET_METHOD_PRE_COMPILED(METH, VAL)	((METH)->localsz = -(VAL))
#endif


/* interfaces supported by arrays */
static Hjava_lang_Class* arr_interfaces[2];

extern bool verify2(Hjava_lang_Class*, errorInfo*);
extern bool verify3(Hjava_lang_Class*, errorInfo*);

static int internalSetupClass(Hjava_lang_Class*, Utf8Const*, int, int, int, Hjava_lang_ClassLoader*, errorInfo *einfo);

static bool buildDispatchTable(Hjava_lang_Class*, errorInfo *info);
static bool buildInterfaceDispatchTable(Hjava_lang_Class*, errorInfo *);
static bool checkForAbstractMethods(Hjava_lang_Class* class, errorInfo *einfo);
static bool prepareInterface(Hjava_lang_Class*, errorInfo*);
static bool computeInterfaceImplementationIndex(Hjava_lang_Class*, errorInfo*);
static bool allocStaticFields(Hjava_lang_Class*, errorInfo *einfo);
static bool resolveObjectFields(Hjava_lang_Class*, errorInfo *einfo);
static bool resolveStaticFields(Hjava_lang_Class*, errorInfo *einfo);
static bool resolveConstants(Hjava_lang_Class*, errorInfo *einfo);
static bool resolveInterfaces(Hjava_lang_Class *class, errorInfo *einfo);
static parsed_signature_t *duplicateParsedSignature (parsed_signature_t *, errorInfo *);

static struct Hjava_security_ProtectionDomain  *defaultProtectionDomain;


void
initialiseSecurity (void)
{
	struct Hjava_lang_Object *codeSource;
	struct Hjava_lang_Object *permissions;
	struct Hjava_lang_Object *allPermission;

	codeSource = execute_java_constructor ("java/security/CodeSource",
					       NULL,
					       NULL,
					       "(Ljava/net/URL;[Ljava/security/cert/Certificate;)V",
					       NULL,
					       NULL);

	permissions = execute_java_constructor ("java/security/Permissions",
						NULL,
						NULL,
						"()V");

	allPermission = execute_java_constructor ("java/security/AllPermission",
						  NULL,
						  NULL,
						  "()V");
	
	do_execute_java_method(NULL, permissions, "add", "(Ljava/security/Permission;)V",
			       NULL, 0, allPermission);
	do_execute_java_method(NULL, permissions, "setReadOnly", "()V", NULL, 0);

	defaultProtectionDomain = (struct Hjava_security_ProtectionDomain *)execute_java_constructor ("java/security/ProtectionDomain",
							    NULL,
							    NULL,
							    "(Ljava/security/CodeSource;Ljava/security/PermissionCollection;)V",
							    codeSource,
							    permissions);
}

#if !defined(ALIGNMENT_OF_SIZE)
#define	ALIGNMENT_OF_SIZE(S)	(S)
#endif

/* set a class's alloc_type field */
static void
determineAllocType(Hjava_lang_Class *class)
{
  if (StringClass != 0 && StringClass == class)
    class->alloc_type = KGC_ALLOC_JAVASTRING;
  else
    if (ClassLoaderClass != 0 && instanceof(ClassLoaderClass, class))
      class->alloc_type = KGC_ALLOC_JAVALOADER;
    else
      class->alloc_type = KGC_ALLOC_FINALIZEOBJECT;
}

/*
 * Process all the stage of a classes initialisation.  We can provide
 * a state to aim for (so we don't have to do this all at once).  This
 * is called by various parts of the machine in order to load, link
 * and initialise the class.  Putting it all together here makes it a damn
 * sight easier to understand what's happening.
 *
 * Returns true if processing was successful, false otherwise.
 */
bool
processClass(Hjava_lang_Class* class, int tostate, errorInfo *einfo)
{
	Method* meth;
	classEntry *ce;
	Hjava_lang_Class* nclass;
	bool success = true;	/* optimistic */
#if !(defined(NDEBUG) || !defined(KAFFE_VMDEBUG))
	int i;
	static int depth;
#endif /* !(defined(NDEBUG) || !defined(KAFFE_VMDEBUG)) */
	static Method *object_fin;

	/* If this class is initialised to the required point, quit now */
	if (class->state >= tostate) {
		return (true);
	}

#define	SET_CLASS_STATE(S)	class->state = (S)
#define	DO_CLASS_STATE(S)	if ((S) > class->state && (S) <= tostate)

	/* For the moment we only allow one thread to initialise any classes
	 * at once.  This is because we've got circular class dependencies
	 * we've got to work out.
	 */

	/*
	 * Get the entry for this class, we'll need to update its state along
	 * the way.
	 */
	ce = lookupClassEntryInternal(class->name, class->loader);

	lockClass(class);

DBG(RESERROR,
	/* show calls to processClass when debugging resolution errors */
	depth++;
	for (i = 0; i < depth; dprintf("  "), i++);
	dprintf("%p entering process class %s %d->%d\n",
		KTHREAD(current)(), class->name->data,
		class->state, tostate);
    );

retry:
	/* If the initialization of that class failed once before, don't
	 * bother and report that no definition for this class exists.
	 * We must do that after the retry label so that threads waiting
	 * on other threads performing a particular initialization step
	 * can learn that things went wrong.
	 */
	if (class->state == CSTATE_FAILED) {
		postExceptionMessage(einfo,
				     JAVA_LANG(NoClassDefFoundError),
				     "%s",
				     class->name->data);
		einfo->type |= KERR_NO_CLASS_FOUND; /* for the verifier */
		success = false;
		goto done;
	}

	DO_CLASS_STATE(CSTATE_LOADED_SUPER) {

		setClassMappingState(ce, NMS_LOADING);
		
		class->processingThread = THREAD_NATIVE();
		
		/* Load and link the super class */
		if( class->superclass )
		{
			/*
			 * propagate failures in super class loading and
			 * processing.  Since getClass might involve an
			 * upcall to a classloader, we must release the
			 * classLock here.
			 */
			unlockClass(class);
			
#if defined(HAVE_GCJ_SUPPORT)
			if( CLASS_GCJ(class) )
			{
				class->superclass
					= gcjGetClass((void*)class->superclass,
						      einfo);
			}
			else
#endif
			{
				class->superclass =
					getClass((constIndex)(uintp)class->superclass,
						 class,
						 einfo);
			}
			
			lockClass(class);
			if( class->superclass == 0 )
			{
				success = false;
				goto done;
			}
			KGC_addWeakRef(main_collector, 
				       class->superclass, 
				       (void **)&(class->superclass));
			if( !(class->accflags & ACC_INTERFACE) &&
			    (class->superclass->accflags & ACC_INTERFACE)) {
				postExceptionMessage(
					einfo,
					JAVA_LANG(
						IncompatibleClassChangeError),
					"Super class, %s, is an interface.",
					class->superclass->name->data);
				success = false;
				goto done;
			}
			/* that's pretty much obsolete. */
			assert(class->superclass->state >= CSTATE_DOING_LINK);
			classMappingLoaded(ce, class);
			/* Copy initial field size and gc layout.
			 * Later, as this class's fields are resolved, they
			 * are added to the superclass's layout.
			 */
			CLASS_FSIZE(class) = CLASS_FSIZE(class->superclass);
			class->gc_layout = class->superclass->gc_layout;
		}
		if( class->superclass )
		{
			assert(class->superclass->state >= CSTATE_DOING_LINK);
		}
		
	}
	
	DO_CLASS_STATE(CSTATE_VERIFIED) {
		/*
		 * Second stage verification - check the class format is okay
		 */
		success =  verify2(class, einfo);
		if (success == false) {
			goto done;
		}

		SET_CLASS_STATE(CSTATE_VERIFIED);
	}
	
	DO_CLASS_STATE(CSTATE_PREPARED) {

		if( (class->loader == 0) && !gc_add_ref(class) )
		{
			postOutOfMemory(einfo);
			success = false;
			goto done;
		}

		/* Allocate any static space required by class and initialise
		 * the space with any constant values.  This isn't necessary
		 * for pre-loaded classes.
		 */
		if (class->state != CSTATE_PRELOADED
		    && !allocStaticFields(class, einfo)) {
			success = false;
			goto done;
		}

		SET_CLASS_STATE(CSTATE_DOING_PREPARE);
		class->processingThread = THREAD_NATIVE();

#if defined(HAVE_GCJ_SUPPORT)
		if (CLASS_GCJ(class)) {
			success = gcjProcessClass(class, class->gcjPeer, einfo);
			if (success == false) {
				goto done;
			}
		}
#else
/* #warning No GCJ Support */
#endif
		success = resolveObjectFields(class, einfo);
		if (success == false) {
			goto done;
		}

		success = resolveStaticFields(class, einfo);
		if (success == false) {
			goto done;
		}

		success = resolveInterfaces(class, einfo);
		if (success == false) {
			goto done;
		}

		/* Build dispatch table.  We must handle interfaces a little
		 * differently since they only have a <clinit> method.
		 */
		if (!CLASS_IS_INTERFACE(class)) {

			success = buildDispatchTable(class, einfo);
			if (success == false) {
				goto done;
			}

			success = buildInterfaceDispatchTable(class, einfo);
			if (success == false) {
				goto done;
			}

			success = checkForAbstractMethods(class, einfo);
			if (success == false) {
				goto done;
			}

			success = computeInterfaceImplementationIndex(class,
								      einfo);
		} else {
			success = prepareInterface(class, einfo);
		}

		if (success == false) {
			goto done;
		}

		SET_CLASS_STATE(CSTATE_PREPARED);
		
		setClassMappingState(ce, NMS_DONE);
		
#if defined(ENABLE_JVMPI)
		if( JVMPI_EVENT_ISENABLED(JVMPI_EVENT_CLASS_LOAD) )
		{
			JVMPI_Method *jvmpi_methods;
			JVMPI_Field *jvmpi_fields;
			JVMPI_Event ev;
			
			jvmpi_methods = alloca(sizeof(JVMPI_Method) *
					       CLASS_NMETHODS(class));
			jvmpi_fields = alloca(sizeof(JVMPI_Field) *
					      (class->nsfields +
					       CLASS_NFIELDS(class)));
			ev.u.class_load.methods = jvmpi_methods;
			ev.u.class_load.statics = &jvmpi_fields[0];
			ev.u.class_load.instances =
				&jvmpi_fields[class->nsfields];
			jvmpiFillClassLoad(&ev, class);
			jvmpiPostEvent(&ev);
		}
#endif
	}

	assert((class == ObjectClass) || (class->superclass != NULL));

	DO_CLASS_STATE(CSTATE_LINKED) {
		
		if (class->state == CSTATE_DOING_LINK) {
			if (THREAD_NATIVE() == class->processingThread) {
				goto done;
			} else {
				while (class->state == CSTATE_DOING_LINK) {
					waitOnClass(class);
					goto retry;
				}
			}
		}
		
		SET_CLASS_STATE(CSTATE_DOING_LINK);
		
		/* Third stage verification - check the bytecode is okay */
		success = verify3(class, einfo);
		if (success == false) {
			goto done;
		}

		SET_CLASS_STATE(CSTATE_LINKED);
	}

	/* NB: the reason that CONSTINIT is a separate state is that
	 * CONSTINIT depends on StringClass, which isn't available during
	 * initialization when we bring the base classes to the LINKED state.
	 */
	DO_CLASS_STATE(CSTATE_CONSTINIT) {
#if defined(HAVE_GCJ_SUPPORT)
		int i;
                Field *fld;

		if (CLASS_GCJ(class)) {
			success = gcjProcessClassConstants(class,
							   class->gcjPeer,
							   einfo);
			if (success == false) {
				goto done;
			}
		}

		/* We must resolve field types eagerly so that class gc
		 * will mark static fields of gcj classes.  That walking
		 * depends on whether the UNRESOLVED_FLAG is clear.
		 */
		fld = CLASS_FIELDS(class);
		for (i = 0; i < CLASS_NFIELDS(class); fld++, i++) {
			if (resolveFieldType(fld, class, einfo) == 0) {
				success = false;
				goto done;
			}
		}

#endif /* defined(HAVE_GCJ_SUPPORT) */

		/* Initialise the constants */
		success = resolveConstants(class, einfo);
		if (success == false) {
			goto done;
		}

		/* And note that it's done */
		SET_CLASS_STATE(CSTATE_CONSTINIT);
	}

	DO_CLASS_STATE(CSTATE_USABLE) {

		/* If somebody's already processing the super class,
		 * check whether it's us.  If so, return.
		 * Else, wait for the other thread to complete and
		 * start over to reevaluate the situation.
		 */
		if (class->state == CSTATE_DOING_SUPER) {
			if (THREAD_NATIVE() == class->processingThread) {
				goto done;
			} else {
				while (class->state == CSTATE_DOING_SUPER) {
					waitOnClass(class);
					goto retry;
				}
			}
		}

		SET_CLASS_STATE(CSTATE_DOING_SUPER);

		/* Now determine the method used to finalize this object.
		 * If the finalizer is empty, we set class->finalizer to null.
		 * Find finalizer first without calling findMethod.
		 */
		meth = NULL;
		for (nclass = class; nclass != 0; nclass = nclass->superclass) {
			meth = findMethodLocal(nclass, final_name, void_signature);
			if (meth != NULL) {
				break;
			}
		}

		/* every class must have one since java.lang.Object has one */
		if (meth == NULL) {
			postException(einfo, JAVA_LANG(InternalError));
			success = false;
			goto done;
		}

		/* is it empty?  This test should work even if an
		 * object has been finalized before this class is
		 * loaded. If Object.finalize() is empty, save a pointer
		 * to the method itself, and check meth against it in
		 * the future.
		 */
		if ((meth->c.bcode.codelen == 1
		     && meth->c.bcode.code[0] == RETURN)) {
			if (!object_fin && meth->class == ObjectClass) {
				object_fin = meth;
			}
			class->finalizer = NULL;
		} else if (meth == object_fin) {
			class->finalizer = NULL;
		} else {
			class->finalizer = meth;
		}

		determineAllocType(class);

		if (class->superclass != NULL) {
			class->processingThread = THREAD_NATIVE();

			/* We must not hold the class lock here because we
			 * might call out into the superclass's initializer
			 * here!
			 */
			unlockClass(class);
			success = processClass(class->superclass,
					       CSTATE_COMPLETE,
					       einfo);
			lockClass(class);
			if (success == false) {
				if (class->superclass->state == CSTATE_FAILED)
					SET_CLASS_STATE(CSTATE_FAILED);
				goto done;
			}
		}

#if defined(KAFFE_XDEBUGGING)
		if( machine_debug_file )
		{
			addDebugInfo(machine_debug_file,
				     DIA_Class, class,
				     DIA_DONE);
		}
#endif
		
		SET_CLASS_STATE(CSTATE_USABLE);
	}

	DO_CLASS_STATE(CSTATE_COMPLETE) {
		jthrowable exc = NULL;
		jthrowable excpending;

		/* If we need a successfully initialized class here, but its
		 * initializer failed, return false as well
		 */
		if (class->state == CSTATE_FAILED) {
			postExceptionMessage(einfo,
				JAVA_LANG(NoClassDefFoundError),
				"%s", class->name->data);
			success = false;
			goto done;
		}

DBG(STATICINIT, dprintf("Initialising %s static %d\n", class->name->data,
			CLASS_FSIZE(class)); 	);
		meth = findMethodLocal(class, init_name, void_signature);
		if (meth == NULL) {
			SET_CLASS_STATE(CSTATE_COMPLETE);
			goto done;
		}

		if (class->state == CSTATE_DOING_INIT) {
			if (THREAD_NATIVE() == class->processingThread) {
				goto done;
			} else {
				while (class->state == CSTATE_DOING_INIT) {
					waitOnClass(class);
					goto retry;
				}
			}
		}

		SET_CLASS_STATE(CSTATE_DOING_INIT);
		class->processingThread = THREAD_NATIVE();

		/* give classLock up for the duration of this call */
		unlockClass(class);

		/* We use here an exception safe call method to be able
		 * to catch possible exceptions which may occur.
		 */
		excpending = THREAD_DATA()->exceptObj;
		THREAD_DATA()->exceptObj = NULL;

		KaffeVM_safeCallMethodA(meth, METHOD_NATIVECODE(meth), NULL, NULL, NULL, 0);
		exc = THREAD_DATA()->exceptObj;
		THREAD_DATA()->exceptObj = excpending;

		lockClass(class);

		class->processingThread = NULL;
		
		if (exc != 0) {
			if( soft_instanceof(javaLangException, exc) )
			{
				/* this is special-cased in throwError */
				einfo->type = (KERR_INITIALIZER_ERROR |
					       KERR_NO_CLASS_FOUND);
				einfo->throwable = exc;
			}
			else
			{
				/* Should be an error... */
				einfo->type = (KERR_RETHROW |
					       KERR_NO_CLASS_FOUND);
				einfo->throwable = exc;
			}
			
			/*
			 * we return false here because COMPLETE fails
			 */
			success = false;
			SET_CLASS_STATE(CSTATE_FAILED);
		} else {
			SET_CLASS_STATE(CSTATE_COMPLETE);
		}

		/* Since we'll never run this again we might as well
		 * lose it now. However, if there was an exception, keep
		 * it so the stack trace doesn't lose the <clinit> frame.
		 */
#if defined(TRANSLATOR) && (defined (MD_UNREGISTER_JIT_EXCEPTION_INFO) || defined (JIT3))
#if defined(MD_UNREGISTER_JIT_EXCEPTION_INFO)
		if (exc == 0) {
			MD_UNREGISTER_JIT_EXCEPTION_INFO (meth->c.ncode.ncode_start,
							  METHOD_NATIVECODE(meth),
							  meth->c.ncode.ncode_end);
		}
#endif
#endif
		if (
#if defined(JIT3)
		    (exc == 0) &&
#endif
#if defined(KAFFE_XPROFILER)
		    !xProfFlag &&
#endif
		    1) {
			_SET_METHOD_NATIVECODE(meth, NULL);
			meth->c.ncode.ncode_start = NULL;
			meth->c.ncode.ncode_end = NULL;
		}
	}

done:
	/* If anything ever goes wrong with this class, we declare it dead
	 * and will respond with NoClassDefFoundErrors to any future attempts
	 * to access that class.
	 * NB: this does not include when a static initializer failed.
	 */
	if (success == false && class->state != CSTATE_FAILED) {
		SET_CLASS_STATE(CSTATE_FAILED);

		if( ce->state != NMS_DONE )
		{
			setClassMappingState(ce, NMS_EMPTY);
		}
	}

	/* wake up any waiting threads */
	broadcastOnClass(class);
	unlockClass(class);

DBG(RESERROR,
	for (i = 0; i < depth; dprintf("  "), i++);
	depth--;
	dprintf("%p leaving process class %s -> %s\n",
		KTHREAD(current)(), class->name->data,
		success ? "success" : "failure");
    );
	return (success);
}

static int
expandMethods(Hjava_lang_Class *cl, Method *imeth, errorInfo *einfo)
{
	Method *new_methods = NULL;
	int retval = 0;
	
	/*
	if( !CLASS_IS_ABSTRACT(cl) )
	{
		postExceptionMessage(einfo,
				     JAVA_LANG(ClassFormatError),
				     "(class: %s, method: %s signature: %s) "
				     "Abstract method in non-abstract class",
				     cl->name->data,
				     imeth->name->data,
				     imeth->parsed_sig->signature->data);
	}
	else
	*/
	if( (new_methods = gc_realloc(CLASS_METHODS(cl),
				      sizeof(Method) *
				      (CLASS_NMETHODS(cl) + 1), KGC_ALLOC_METHOD)) )
	{
		int i;
		
		i = CLASS_NMETHODS(cl);
		CLASS_METHODS(cl) = new_methods;
		utf8ConstAddRef(imeth->name);
		new_methods[i] = *imeth;

		/* Allocate a new parsed_signature_t for the method. We can't use the one
		 * of the implemented method as destroyClass would then try to free it
		 * twice.
		 */
		new_methods[i].parsed_sig = duplicateParsedSignature (imeth->parsed_sig, einfo);
		if (new_methods[i].parsed_sig == NULL)
		{
			gc_free (new_methods);
			return 0;
		}

		new_methods[i].ndeclared_exceptions = -1;
		new_methods[i].declared_exceptions_u.remote_exceptions =
			imeth;
		new_methods[i].class = cl;

		CLASS_NMETHODS(cl) = i + 1;

		retval = 1;
	}
	else
	{
		gc_free(new_methods);
		postOutOfMemory(einfo);
	}
	return( retval );
}

static int
expandInterfaces(Hjava_lang_Class *root_class,
		 Hjava_lang_Class *class,
		 errorInfo *einfo)
{
	int i, j, k, success = 1;
	
	/*
	 * Check to make sure all the interface methods are implemented,
	 * otherwise, we'll need to add a slot.
	 */
	for( i = 0; (i < class->interface_len) && success; i++ )
	{
		Hjava_lang_Class *iface;
		
		iface = class->interfaces[i];
		if( !expandInterfaces(root_class, iface, einfo) )
		{
			success = 0;
			break;
		}
		for( j = 0; (j < CLASS_NMETHODS(iface)) && success; j++ )
		{
			Hjava_lang_Class *cl;
			int foundit = 0;
			Method *imeth;
			
			imeth = &CLASS_METHODS(iface)[j];
			/* Igore statics */
			if( imeth->accflags & ACC_STATIC )
				continue;
			/* Search for the corresponding slot. */
			for( cl = root_class;
			     cl && !foundit;
			     cl = cl->superclass )
			{
				for( k = 0; k < CLASS_NMETHODS(cl); k++ )
				{
					Method *cmeth;
					
					cmeth = &CLASS_METHODS(cl)[k];
					if( (cmeth->name == imeth->name) &&
					    (cmeth->parsed_sig->signature ==
					     imeth->parsed_sig->signature) )
					{
						foundit = 1;
						break;
					}
				}
			}
			if( !foundit )
			{
				/* No impl, add a slot */
				success = expandMethods(root_class,
							imeth,
							einfo);
			}
		}
	}
	return( success );
}

static bool
resolveInterfaces(Hjava_lang_Class *class, errorInfo *einfo)
{
	int i, j, k;
	int totalilen;
	Hjava_lang_Class** newifaces;
	Hjava_lang_Class* nclass;
	bool success = true;	/* optimistic */

	/* Load all the implemented interfaces. */
	j = class->interface_len;
	nclass = class->superclass;
	if (nclass != 0 && nclass != ObjectClass) {
		/* If class is an interface, its superclass must
		 * be java.lang.Object or the class file is broken.
		 */
		if (CLASS_IS_INTERFACE(class)) {
			postException(einfo, JAVA_LANG(VerifyError));
			success = false;
			goto done;
		}
		j += class->superclass->total_interface_len;
	}
	for (i = 0; i < class->interface_len; i++) {
		uintp iface = (uintp)class->interfaces[i];
		unlockClass(class);

#if defined(HAVE_GCJ_SUPPORT)
		if (CLASS_GCJ(class)) {
			nclass = gcjGetClass((void*)iface, einfo);
		} else {
			nclass = getClass(iface, class, einfo);
		}
#else
		nclass = getClass(iface, class, einfo);
#endif /* HAVE_GCJ_SUPPORT */

		class->interfaces[i] = nclass;

		lockClass(class);
		if (class->interfaces[i] == 0) {
			success = false;
			goto done;
		}
		if (!(class->interfaces[i]->accflags & ACC_INTERFACE)) {
			postExceptionMessage(
				einfo,
				JAVA_LANG(IncompatibleClassChangeError),
				"Class, %s, used as interface by %s",
				class->interfaces[i]->name->data,
				class->name->data);
			success = false;
			goto done;
		}
		if (instanceof(class, class->interfaces[i])) {
			postExceptionMessage(
				einfo,
				JAVA_LANG(ClassCircularityError),
				"%s",
				class->name->data);
			success = false;
			goto done;
		}
		j += class->interfaces[i]->total_interface_len;
	}
	totalilen = j;

	/* We build a list of *all* interfaces this class can use */
	if (class->interface_len != j) {
		newifaces = (Hjava_lang_Class**)gc_malloc(sizeof(Hjava_lang_Class**) * j, KGC_ALLOC_INTERFACE);
		if (newifaces == 0) {
			postOutOfMemory(einfo);
			success = false;
			goto done;
		}
		for (i = 0; i < class->interface_len; i++) {
			newifaces[i] = class->interfaces[i];
		}
		nclass = class->superclass;
		if (nclass != 0 && nclass != ObjectClass) {
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
		if (class->interfaces != NULL)
		  gc_rm_ref(class->interfaces);
		class->interfaces = newifaces;
		if (!gc_add_ref(class->interfaces)) {
			postOutOfMemory(einfo);
			success = false;
			goto done;
		}	
	}

	/* don't set total_interface_len before interfaces to avoid
	 * having walkClass attempting to walk interfaces
	 */
	class->total_interface_len = totalilen;
	
	if( !CLASS_IS_INTERFACE(class) )
	{
		success = expandInterfaces(class, class, einfo);
	}

done:
	return (success);
}

/**
 * Check if a class name is in a set of packages.
 *
 * XXX Move somewhere else...
 *
 * @param plist The null terminated list of packages to check against.
 * @param name The class name to check.
 * @return True if the class name is in one of the packages, false otherwise.
 */
static int
inPackageSet(const char **plist, Utf8Const *name)
{
	unsigned int name_len, lpc, retval = 0;
	
	name_len = strlen(name->data);
	for( lpc = 0; plist[lpc] && !retval; lpc++ )
	{
		unsigned int len;

		len = strlen(plist[lpc]);
		if( (name_len > len) &&
		    strncmp(name->data, plist[lpc], len) == 0 )
		{
			retval = 1;
		}
	}
	return( retval );
}

/**
 * The set of restricted packages that a user defined class loader can't add
 * classes to.
 */
static const char *restrictedPackages[] = {
	"java/",
	"kaffe/",
	NULL
};

static int
internalSetupClass(Hjava_lang_Class* cl, Utf8Const* name, int flags,
		   int this_index, int su, Hjava_lang_ClassLoader* loader,
		   struct _errorInfo *einfo)
{
	if( (loader != NULL) &&
	    inPackageSet(restrictedPackages, name) ) {
		/*
		 * Can't allow users to add classes to the bootstrap
		 * packages.
		 */
		postExceptionMessage(einfo,
				     JAVA_LANG(SecurityException),
				     "Prohibited package: %s",
				     name->data);
		return 0;
	}
	if( cl->name == NULL ) {
		utf8ConstAssign(cl->name, name);
	}
	else if( !utf8ConstEqual(cl->name, name) ) {
		postExceptionMessage(einfo,
				     JAVA_LANG(ClassFormatError),
				     "%s (wrong name: %s)",
				     name->data,
				     cl->name->data);
		return 0;
	}
	cl->packageLength = findPackageLength(name->data);
	CLASS_METHODS(cl) = NULL;
	CLASS_NMETHODS(cl) = 0;
	assert(cl->superclass == 0);
	cl->superclass = (Hjava_lang_Class*)(uintp)su;
	cl->msize = 0;
	CLASS_FIELDS(cl) = NULL;
	CLASS_FSIZE(cl) = 0;
	cl->accflags = flags;
	cl->vtable = NULL;
        cl->interfaces = NULL;
	cl->interface_len = 0;
	assert(cl->state < CSTATE_LOADED);
	cl->state = CSTATE_LOADED;
	cl->loader = loader;
	cl->this_index = this_index;
	cl->inner_classes = NULL;
	cl->nr_inner_classes = 0;
	cl->this_inner_index = -1;

	/* set protection domain to the default */
	if (loader == NULL)
	{
		cl->protectionDomain = defaultProtectionDomain;
	}

	return 1;
}

Hjava_lang_Class*
setupClass(Hjava_lang_Class* cl, constIndex c, constIndex s,
	   u2 flags, Hjava_lang_ClassLoader* loader,
	   errorInfo* einfo)
{
	constants* pool;

	pool = CLASS_CONSTANTS(cl);

	/* Find the name of the class */
	if (pool->tags[c] != CONSTANT_Class) {
                postExceptionMessage(einfo, JAVA_LANG(ClassFormatError),
				     "this class constant pool index is bogus");
		return (NULL);
	}

	if (!internalSetupClass(cl, WORD2UTF(pool->data[c]), flags, c, s,
				loader, einfo))
		return (NULL);
	return (cl);
}


/*
 * add source file name to be printed in exception backtraces
 */
bool
addSourceFile(Hjava_lang_Class* c, int idx, errorInfo *einfo)
{
	constants* pool;
	const char* sourcefile;
	const char* basename;
	bool success = true;

	pool = CLASS_CONSTANTS (c);

	if (pool->tags[idx] != CONSTANT_Utf8) {
		postExceptionMessage(einfo, JAVA_LANG(ClassFormatError),
				     "invalid sourcefile index: %d",
				     idx);
		return false;
	}
	
	sourcefile = WORD2UTF (pool->data[idx])->data;
	basename = strrchr(sourcefile, '/');
	if (basename == 0) {
		basename = sourcefile;
	} else {
		basename++;
	}
	c->sourcefile = gc_malloc(strlen(basename) + 1, KGC_ALLOC_CLASSMISC);
	if (c->sourcefile != 0) {
		strcpy(c->sourcefile, basename);
	} else {
		success = false;
		postOutOfMemory(einfo);
	}
	/* we should be able to drop this utf8 here */
	utf8ConstRelease(WORD2UTF (pool->data[idx]));
	pool->data[idx] = 0;
	return (success);
}

/*
 * Read in InnerClasses declares for a class
 */
bool
addInnerClasses(Hjava_lang_Class* c, size_t len UNUSED, classFile* fp,
		errorInfo *einfo)
{
	int i;
	u2 nr;
	innerClass *ic;

	if (! checkBufSize(fp, 2, CLASS_CNAME(c), einfo))
		return false;

	readu2(&nr, fp);
	if (nr == 0) {
		return true;
	}

	if (! checkBufSize(fp, (u2)(nr*(2*4)), CLASS_CNAME(c), einfo))
	    return false;

	ic = gc_malloc(sizeof(innerClass) * nr, KGC_ALLOC_CLASSMISC);
	if (!ic) {
		postOutOfMemory(einfo);
		return false;
	}

	c->nr_inner_classes = nr;
	c->inner_classes = ic;

	for (i = 0; i < nr; i++, ic++) {
		u2 dummy;

		readu2(&ic->inner_class, fp);
		readu2(&ic->outer_class, fp);
		readu2(&dummy, fp);
		readu2(&ic->inner_class_accflags, fp);

		if (c->this_index && ic->inner_class == c->this_index) {
		    c->accflags = (c->accflags & ~ACC_MASK) | (ic->inner_class_accflags & ACC_MASK);
		    c->this_inner_index = i;
		}
	}
	return true;
}

int
startMethods(Hjava_lang_Class* this, u2 methct, errorInfo *einfo)
{
	if (methct == 0)
	{
		this->methods = NULL;
	}
	else
	{
		this->methods = gc_malloc(sizeof(Method)*(methct), KGC_ALLOC_METHOD);
		if (this->methods == NULL)
		{
			postOutOfMemory(einfo);
			return false;
		}
	}
	KGC_WRITE(this, this->methods);

	CLASS_NMETHODS(this) = 0; /* updated in addMethod */
	return true;
}

Method*
addMethod(Hjava_lang_Class* c,
	  u2 access_flags,
	  u2 name_index,
	  u2 signature_index,
	  errorInfo *einfo)
{
	constIndex nc;
	constIndex sc;
	Method* mt;
	constants* pool;
	Utf8Const* name;
	Utf8Const* signature;
#ifdef KAFFE_VMDEBUG
	int ni;
#endif

	pool = CLASS_CONSTANTS (c);

	nc = name_index;
	if (pool->tags[nc] != CONSTANT_Utf8) {
		/* XXX fill in einfo! */
DBG(RESERROR,	dprintf("addMethod: no method name.\n");		);
		postExceptionMessage(einfo,
				     JAVA_LANG(ClassFormatError),
				     "No method name");
		return (NULL);
	}
	sc = signature_index;
	if (pool->tags[sc] != CONSTANT_Utf8) {
		/* XXX fill in einfo! */
DBG(RESERROR,	dprintf("addMethod: no signature name.\n");	);
		postExceptionMessage(einfo,
				     JAVA_LANG(ClassFormatError),
				     "No signature for method: %s",
				     WORD2UTF (pool->data[nc])->data);
		return (NULL);
	}
	name = WORD2UTF (pool->data[nc]);
	signature = WORD2UTF (pool->data[sc]);

#ifdef KAFFE_VMDEBUG
	/* Search down class for method name - don't allow duplicates */
	for (ni = CLASS_NMETHODS(c), mt = CLASS_METHODS(c); --ni >= 0; ) {
		assert(! utf8ConstEqual (name, mt->name)
		       || ! utf8ConstEqual (signature, METHOD_SIG(mt)));
	}
#endif

DBG(CLASSFILE,
	dprintf("Adding method %s:%s%s (%x)\n", c->name->data, name->data, signature->data, access_flags);
    );

	mt = &CLASS_METHODS(c)[CLASS_NMETHODS(c)];
	utf8ConstAssign(mt->name, name);
	METHOD_PSIG(mt) = parseSignature(signature, einfo);
	if (METHOD_PSIG(mt) == NULL) {
		return NULL;
	}
	mt->class = c;
	/* Warning: ACC_CONSTRUCTION match ACC_STRICT */
	mt->accflags = access_flags & ACC_MASK;
	mt->c.bcode.code = NULL;
	mt->stacksz = 0;
	mt->localsz = 0;
	mt->exception_table = NULL;
	mt->idx = -1;

	/* Mark constructors as such */
	if (utf8ConstEqual (name, constructor_name)) {
		mt->accflags |= ACC_CONSTRUCTOR;
	}

	CLASS_NMETHODS(c)++;
	return (mt);
}

Field*
addField(Hjava_lang_Class* c,
	 u2 access_flags,
	 u2 name_index,
	 u2 signature_index,
	 errorInfo* einfo)
{
	constIndex nc;
	constIndex sc;
	Field* ft;
	constants* pool;
	int i;
	const char* sig;

	pool = CLASS_CONSTANTS (c);

	nc = name_index;
	if (pool->tags[nc] != CONSTANT_Utf8) {
DBG(RESERROR,	dprintf("addField: no field name.\n");			);
		postExceptionMessage(einfo,
				     JAVA_LANG(ClassFormatError),
				     "No field name");
		return (NULL);
	}

	--CLASS_FSIZE(c);	/* holds field count initially */
	if (access_flags & ACC_STATIC) {
		i = CLASS_NSFIELDS(c);
	}
	else {
		i = CLASS_FSIZE(c) + CLASS_NSFIELDS(c);
	}
	ft = &CLASS_FIELDS(c)[i];
	ft->clazz = c;
	
DBG(CLASSFILE,
	dprintf("Adding field %s:%s\n",
		CLASS_CNAME(c), CLASS_CONST_UTF8(c, nc)->data);
    );

	sc = signature_index;
	if (pool->tags[sc] != CONSTANT_Utf8) {
DBG(RESERROR,	dprintf("addField: no signature name.\n");		);
		postExceptionMessage(einfo,
				     JAVA_LANG(ClassFormatError),
				     "No signature name for field: %s",
				     CLASS_CONST_UTF8(c, nc)->data);
		CLASS_NFIELDS(c)++;
		return (NULL);
	}
	utf8ConstAssign(ft->name, WORD2UTF(pool->data[nc]));
	utf8ConstAssign(ft->signature, CLASS_CONST_UTF8(c, sc));
	ft->accflags = access_flags;

	sig = ft->signature->data;
	if (sig[0] == 'L' || sig[0] == '[') {
		/* This `type' field is used to hold a utf8Const describing
		 * the type of the field.  This utf8Const will be replaced
		 * with a pointer to an actual class type in resolveFieldType
		 * Between now and then, we add a reference to it.
		 */
		ft->accflags |= FIELD_UNRESOLVED_FLAG;
		FIELD_SIZE(ft) = PTR_TYPE_SIZE;
	}
	else {
		/* NB: since this class is primitive, getClassFromSignature
		 * will not fail.  Hence it's okay to pass errorInfo as NULL
		 */
		FIELD_TYPE(ft) = getClassFromSignature(sig, NULL, NULL);
		FIELD_SIZE(ft) = TYPE_PRIM_SIZE(FIELD_TYPE(ft));
	}

	CLASS_NFIELDS(c)++;
	if (access_flags & ACC_STATIC) {
		CLASS_NSFIELDS(c)++;
	}
	return (ft);
}

void
setFieldValue(Hjava_lang_Class* this UNUSED, Field* ft, u2 idx)
{
	/* Set value index */
	FIELD_CONSTIDX(ft) = idx;
	ft->accflags |= FIELD_CONSTANT_VALUE;
}

int
startFields(Hjava_lang_Class* this, u2 fieldct, errorInfo *einfo)
{
	CLASS_NFIELDS(this) = 0; /* updated in addField() */
	CLASS_FSIZE(this) = fieldct;
	if (fieldct == 0) {
		CLASS_FIELDS(this) = (Field*)0;
	}
	else {
		CLASS_FIELDS(this) = (Field*) gc_malloc(sizeof(Field) * fieldct,
							KGC_ALLOC_FIELD);
		if (!CLASS_FIELDS(this)) {
			postOutOfMemory(einfo);
			return false;
		}
	}
	KGC_WRITE(this, CLASS_FIELDS(this)); /* XXX */
	
	return true;
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
	assert(m != 0);
	assert(c != 0);
	assert(c->code != 0);
	assert(c->code_length != 0);
	
	m->c.bcode.code = c->code;
	m->c.bcode.codelen = c->code_length;
	m->stacksz = c->max_stack;
	m->localsz = c->max_locals;
	m->exception_table = c->exception_table;
}

void
addInterfaces(Hjava_lang_Class* c, u2 inr, Hjava_lang_Class** inf)
{
	assert(c);
	assert(inr > 0);

        c->interfaces = inf;
	c->interface_len = inr;

	KGC_WRITE(c, c->interfaces); /* XXX */
}

static
Hjava_lang_Class *userLoadClass(classEntry *ce,
				Hjava_lang_ClassLoader *loader,
				errorInfo *einfo)
{
	Hjava_lang_Class *retval = NULL;
	Hjava_lang_String *jname;
	jthrowable excpending;
	Method *meth;
	Hjava_lang_Class *class_of_loader;
	
	/*
	 * If an exception is already pending, for instance because we're
	 * resolving one that has occurred, save it and clear it for the
	 * upcall.
	 */
	excpending = THREAD_DATA()->exceptObj;
	THREAD_DATA()->exceptObj = NULL;
	
	class_of_loader = OBJECT_CLASS((Hjava_lang_Object *)loader);

	if( (jname = utf8Const2JavaReplace(ce->name, '/', '.')) )
	  {
	    jvalue jretval;
	    jvalue args[1];
	    jthrowable excobj;

	    /* If we fail to resolve loadClass then we just have to set retval to NULL.
	     * einfo is already set.
	     */
	    if ((meth = lookupClassMethod(class_of_loader, "loadClass", 
					 "(Ljava/lang/String;)Ljava/lang/Class;",
					 einfo)) == NULL)
	      {
		retval = NULL;
		goto userload_done;
	      }
	    
	    if (METHOD_IS_STATIC(meth))
	      {
		postExceptionMessage(einfo, "java/lang/NoSuchMethodError",
				     "loadClass is wrongly a static method in %s", CLASS_CNAME(class_of_loader));
		retval = NULL;
		goto userload_done;
	      }

	    /* ClassLoader is not an interface so we may use NATIVECODE directly. */
	    args[0].l = jname;
	    KaffeVM_safeCallMethodA(meth, METHOD_NATIVECODE(meth), loader, args, &jretval, false);
	    retval = jretval.l;

	    /*
	     * Check whether an exception occurred.  If one was pending,
	     * the new exception will override this one.
	     */
	    excobj = THREAD_DATA()->exceptObj;
	    THREAD_DATA()->exceptObj = NULL;
	    
	    if( excobj != 0 )
	      {
			/* There was an exception. */
			einfo->type = KERR_RETHROW;
			einfo->throwable = excobj;
			if( soft_instanceof(javaLangClassNotFoundException,
					    (Hjava_lang_Object *)
					    einfo->throwable) )
			{
				/* Set this for the verifier. */
				einfo->type |= KERR_NO_CLASS_FOUND;
			}
			/* We must clear the return value as safeCallMethod does not
			 * handle this.
			 */
			retval = NULL;
	      }
	    else if( retval == NULL )
	      {
		/* No class returned. */
		postExceptionMessage(einfo,
				     JAVA_LANG(ClassNotFoundException),
				     "%s",
				     ce->name->data);
		/* Set this for the verifier. */
		einfo->type |= KERR_NO_CLASS_FOUND;
	      }
	    else if( !utf8ConstEqual(retval->name, ce->name) )
	      {
		/*
		 * Its a valid class, but the name differs from the one
		 * that was requested.
		 */
		postExceptionMessage(
				     einfo,
				     JAVA_LANG(ClassNotFoundException),
				     "Bad class name (expect: %s, get: %s)",
				     ce->name->data,
				     retval->name->data);
		/* Set this for the verifier. */
		einfo->type |= KERR_NO_CLASS_FOUND;
		retval = NULL;
	      }
	    else
	      {
		retval = classMappingLoaded(ce, retval);
	      }
	  }
	else
	  {
	    postOutOfMemory(einfo);
	  }
	
 userload_done:
	/* rethrow pending exception */
	if( excpending != NULL )
	{
	  THREAD_DATA()->exceptObj = excpending;
	}

	return retval;
}

Hjava_lang_Class *loadClass(Utf8Const *name,
			    Hjava_lang_ClassLoader *loader,
			    errorInfo *einfo)
{
	Hjava_lang_Class *retval = NULL;
	classEntry *ce;

	if( (ce = lookupClassEntry(name, loader, einfo)) )
	{
		if( classMappingSearch(ce, &retval, einfo) )
		{
			if( retval == NULL )
			{
				/* Loading is this thread's responsibility. */
				if( loader )
				{
DBG(VMCLASSLOADER,
	/* Announce when VM calls class loaders.. */
	dprintf("Calling user-defined class loader %s - loadClass(%s)\n",
		CLASS_CNAME(OBJECT_CLASS(&loader->base)),
		ce->name->data);
);

					/* Use a user defined loader. */
					retval = userLoadClass(ce,
							       loader,
							       einfo);
				}
				else
				{
DBG(VMCLASSLOADER,
	/* Announce when VM calls class loaders.. */
	dprintf("Calling internal class loader for %s\n",
		ce->name->data);
);

					/* Use the primordial loader. */
					retval = findClass(ce, einfo);
				}
			}
			else
			{
				/* Class is already loaded. */
			}
			if( !retval )
			{
				/* No joy, update state. */
				setClassMappingState(ce, NMS_EMPTY);
			}
			else if( processClass(retval, CSTATE_LINKED, einfo)
				 == false )
			{
				retval = NULL;
			}
		}
		else
		{
			/* Class circularity, or some other error. */
		}
	}
	else
	{
		/* No memory? */
	}
	return( retval );
}

Hjava_lang_Class*
loadArray(Utf8Const* name, Hjava_lang_ClassLoader* loader, errorInfo *einfo)
{
	Hjava_lang_Class *clazz;

	clazz = getClassFromSignature(&name->data[1], loader, einfo);
	if (clazz != 0)
	{
		if( (clazz = lookupArray(clazz, einfo)) )
		{
			return( clazz );
		}
		else
		{
			/* XXX Is it always class not found? */
			discardErrorInfo(einfo);
			postExceptionMessage(einfo,
					     JAVA_LANG(ClassNotFoundException),
					     "%s",
					     name->data);
		}
	}
	else
	{
		discardErrorInfo(einfo);
		postExceptionMessage(einfo,
				     JAVA_LANG(ClassNotFoundException),
				     "%s",
				     name->data);
	}
	return (NULL);
}

/*
 * Load a class to whose Class object we refer globally.
 * This is used only for essential classes, so let's bail if this fails.
 */
void
loadStaticClass(Hjava_lang_Class** class, const char* name)
{
	Hjava_lang_Class *clazz;
	errorInfo info;
	Utf8Const *utf8;
	classEntry* centry;
	bool refAdded;

	utf8 = utf8ConstNew(name, -1);
	if (!utf8) goto bad;
	centry = lookupClassEntry(utf8, NULL, &info);
	if (!centry) goto bad;

	utf8ConstRelease(utf8);
	lockStaticMutex(&centry->slock);
	if (centry->data.cl == 0) {
		centry->state = NMS_LOADING;

DBG(VMCLASSLOADER,
	/* Announce when VM calls class loaders.. */
	dprintf("Calling internal class loader for startup class %s\n",
		name);
);

		clazz = findClass(centry, &info);
		if (clazz == NULL) {
			goto bad;
		}
		/* we won't ever want to lose these classes */
		refAdded = gc_add_ref(clazz);
		if (!refAdded) {
			goto bad;
		}

		(*class) = centry->data.cl = clazz;
	}
	unlockStaticMutex(&centry->slock);
	if (!(*class))
		(*class) = centry->data.cl;

	if (processClass(centry->data.cl, CSTATE_LINKED, &info) == true) {
		assert(centry->state == NMS_DONE);
		return;
	}

bad:
	dprintf("Couldn't find or load essential class `%s' %s %s\n",
			name, info.classname, (char*)info.mess);
	KAFFEVM_ABORT();
}

/*
 * Look a class up by name.
 */
Hjava_lang_Class*
lookupClass(const char* name, Hjava_lang_ClassLoader* loader, errorInfo *einfo)
{
	Hjava_lang_Class* class;
	Utf8Const *utf8;

	utf8 = utf8ConstNew(name, -1);
	if (!utf8) {
		postOutOfMemory(einfo);
		return NULL;
	}
	class = loadClass(utf8, loader, einfo);
	utf8ConstRelease(utf8);
	if (class != NULL) {
		if (processClass(class, CSTATE_COMPLETE, einfo) == true) {
			return (class);
		}
	}
	return (NULL);
}

/*
 * Return FIELD_TYPE(FLD), but if !FIELD_RESOLVED, resolve the field first.
 */
Hjava_lang_Class*
resolveFieldType(Field *fld, Hjava_lang_Class* this, errorInfo *einfo)
{
	Hjava_lang_Class* clas;
	const char* name;

	/* Avoid locking if we can */
	if (FIELD_RESOLVED(fld)) {
		return (FIELD_TYPE(fld));
	}

	/* We must lock the class while we retrieve the field name and
	 * while we resolve it - or we may release the utf8's field type twice
	 */
	lockClass(this);
	if (FIELD_RESOLVED(fld)) {
		unlockClass(this);
		return (FIELD_TYPE(fld));
	}
	name = fld->signature->data;

	if( (clas = getClassFromSignature(name, this->loader, einfo)) )
	{
		FIELD_TYPE(fld) = clas;
		fld->accflags &= ~FIELD_UNRESOLVED_FLAG;
	}
	unlockClass(this);

#if defined(KAFFE_XDEBUGGING)
	if( machine_debug_file )
	{
		addDebugInfo(machine_debug_file,
			     DIA_Class, this,
			     DIA_DONE);
	}
#endif
	
	return (clas);
}

static
bool
resolveObjectFields(Hjava_lang_Class* class, errorInfo *einfo)
{
	int fsize;
	int align;
	Field* fld;
	int nbits, nifields;
	int offset;
	int maxalign;
	int oldoffset;
	int *map;
	jboolean is_reference;

	/* Find start of new fields in this object.  If start is zero, we must
	 * allow for the object headers.
	 */
	offset = CLASS_FSIZE(class);
	oldoffset = offset;	/* remember initial offset */
	if (offset == 0) {
		offset = sizeof(Hjava_lang_Object);
	}

	/* Find the largest alignment in this class */
	maxalign = 1;
	fld = CLASS_IFIELDS(class);
	nifields = CLASS_NIFIELDS(class);
	for (; --nifields >= 0; fld++) {
		fsize = FIELD_SIZE(fld);
		/* Work out alignment for this size entity */
		fsize = ALIGNMENT_OF_SIZE(fsize);
		/* If field is bigger than biggest alignment, change
		 * biggest alignment
		 */
		if (fsize > maxalign) {
			maxalign = fsize;
		}
	}
#if 0
	/* Align start of this class's data */
	if (oldoffset == 0) {
		offset = ((offset + maxalign - 1) / maxalign) * maxalign;
	}
#endif

	/* Now work out where to put each field */
	fld = CLASS_IFIELDS(class);
	nifields = CLASS_NIFIELDS(class);
	for (; --nifields >= 0; fld++) {
		fsize = FIELD_SIZE(fld);
		/* Align field */
		align = ALIGNMENT_OF_SIZE(fsize);
		offset = ((offset + align - 1) / align) * align;
		FIELD_BOFFSET(fld) = offset;
		offset += fsize;
	}

	CLASS_FSIZE(class) = offset;

	/* recall old offset */
	offset = oldoffset;

	/* Check whether the class is java.lang.ref.Reference */
	is_reference =
	   (strcmp(CLASS_CNAME(class), "java/lang/ref/Reference") == 0);

	/* Now that we know how big that object is going to be, create
	 * a bitmap to help the gc scan the object.  The first part is
	 * inherited from the superclass.
	 */
	map = BITMAP_NEW(CLASS_FSIZE(class)/ALIGNMENTOF_VOIDP, KGC_ALLOC_CLASSMISC);
	if (map == 0) {
		postOutOfMemory(einfo);
		return (false);
	}

	if (offset > 0) {
		nbits = offset/ALIGNMENTOF_VOIDP;
		BITMAP_COPY(map, class->gc_layout, nbits);
	} else {
		/* The walkObject routine marks the class object explicitly.
		 * We assume that the header does not contain anything ELSE
		 * that must be marked.
		 */
		offset = sizeof(Hjava_lang_Object);
		nbits = offset/ALIGNMENTOF_VOIDP;
	}
	class->gc_layout = map;

#if 0
	/* Find and align start of object */
	if (oldoffset == 0) {
		offset = ((offset + maxalign - 1) / maxalign) * maxalign;
	}
#endif
	nbits = offset/ALIGNMENTOF_VOIDP;

DBG(GCPRECISE,
	dprintf("GCLayout for %s:\n", CLASS_CNAME(class));
    );

	/* Now work out the gc layout */
	fld = CLASS_IFIELDS(class);
	nifields = CLASS_NIFIELDS(class);
	for (; --nifields >= 0; fld++) {
		fsize = FIELD_SIZE(fld);
		/* Align field */
		align = ALIGNMENT_OF_SIZE(fsize);
		offset += (align - (offset % align)) % align;
		nbits = offset/ALIGNMENTOF_VOIDP;

		/* paranoia */
		assert(FIELD_BOFFSET(fld) == offset);

		/* Set bit if this field is a reference type, except if
		 * it's a kaffe.util.Ptr (PTRCLASS).  */
		if (!FIELD_RESOLVED(fld)) {
			Utf8Const *sig = fld->signature;
			if ((sig->data[0] == 'L' || sig->data[0] == '[') &&
			    strcmp(sig->data, PTRCLASSSIG) &&
			    (!is_reference || strcmp(FIELD_NAME(fld), "referent") != 0)) {
				BITMAP_SET(map, nbits);
			}
		} else {
			if (FIELD_ISREF(fld) ) {
				BITMAP_SET(map, nbits);
			}
		}
DBG(GCPRECISE,
		dprintf(" offset=%3d nbits=%2d ", offset, nbits);
		BITMAP_DUMP(map, nbits+1)
		dprintf(" fsize=%3d (%s)\n", fsize, fld->name->data);
    );
		offset += fsize;
	}
	return (true);
}

/*
 * Allocate the space for the static class data.
 */
static
bool
allocStaticFields(Hjava_lang_Class* class, errorInfo *einfo)
{
	int fsize;
	int align;
	uint8* mem;
	int offset;
	int n;
	Field* fld;

	/* No static fields */
	if (CLASS_NSFIELDS(class) == 0) {
		return (true);
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

	assert(offset > 0);
	/* Allocate memory required */
	mem = gc_malloc((unsigned int)offset, KGC_ALLOC_STATICDATA);
	if (mem == NULL) {
		postOutOfMemory(einfo);
		return (false);
	}
	CLASS_STATICDATA(class) = mem;

	/* Rewalk the fields, pointing them at the relevant memory and/or
	 * setting any constant values.
	 */
	fld = CLASS_SFIELDS(class);
	n = CLASS_NSFIELDS(class);
	for (; --n >= 0; fld++) {
		offset = FIELD_SIZE(fld);
		FIELD_SIZE(fld) = FIELD_CONSTIDX(fld);	/* Keep idx in size */
#if defined(HAVE_GCJ_SUPPORT)
		/* Check whether gcj code refers to this field.  If so,
		 * we'll have storage for this field in a fixup module.
		 * gcjGetFieldAddr retrieves the address for the storage
		 *
		 * NB: Don't confuse this with the case where class is a gcj
		 * class.  In that case, this function is not even invoked!
		 */
		FIELD_ADDRESS(fld) =
			gcjGetFieldAddr(CLASS_CNAME(class), fld->name->data);

		/* not a field for which gcj provides storage */
		if (FIELD_ADDRESS(fld) == 0) {
			FIELD_ADDRESS(fld) = mem + offset;
		}
#else
		FIELD_ADDRESS(fld) = mem + offset;
#endif
	}
	return (true);
}

static
bool
resolveStaticFields(Hjava_lang_Class* class, errorInfo *einfo)
{
	uint8* mem;
	constants* pool;
	Utf8Const* utf8;
	Field* fld;
	unsigned int idx;
	int n;

	/* No locking here, assume class is already locked. */
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
				{
					Hjava_lang_String *st;

					utf8 = WORD2UTF(pool->data[idx]);
					st = utf8Const2Java(utf8);
					utf8ConstRelease(utf8);
					if (!st) {
						postOutOfMemory(einfo);
						return false;
					}
					pool->data[idx] = (ConstSlot)st;
					pool->tags[idx] = CONSTANT_ResolvedString;
				}
				/* ... fall through ... */
			case CONSTANT_ResolvedString:
				*(jref*)mem = (jref)CLASS_CONST_DATA(class, idx);
				FIELD_SIZE(fld) = PTR_TYPE_SIZE;
				break;
			default:
				break;
			}
		}
	}
	return true;
}

/*
 * Check whether there exists a method with the same name and signature
 * ``meth'' in class ``clazz'' or any of its superclasses.
 * If so, return set the ``meth'''s index to its index and return true.
 * Otherwise return false.
 */
bool
getInheritedMethodIndex(Hjava_lang_Class *super, Method *meth)
{
	/* Search superclasses for equivalent method name.
	 * If found extract its index nr.
	 */
	for (; super != NULL;  super = super->superclass) {
		int j = CLASS_NMETHODS(super);
		Method* mt = CLASS_METHODS(super);
		for (; --j >= 0;  ++mt) {
			/* skip methods that are private or static */
			if ((mt->accflags & (ACC_PRIVATE|ACC_STATIC)) != 0)
				continue;

			/* skip inaccessible methods */
			if (!checkAccess (meth->class, super, mt->accflags))
				continue;

			if (utf8ConstEqual (mt->name, meth->name) &&
			    utf8ConstEqual (METHOD_SIG(mt), METHOD_SIG(meth)))
			{
				meth->idx = mt->idx;
				return (true);
			}
		}
	}
	return (false);
}

/**
 * Get start of native code of a method
 *
 * @param method a method
 * @return pointer to start of code
 */
struct _jitCodeHeader*
getMethodCodeStart(Method * method) 
{
  return method->c.ncode.ncode_start;
}

/**
 * Set start of native code of a method
 *
 * @param method a method
 * @param start pointer to start of code
 */
void
setMethodCodeStart(Method * method, struct _jitCodeHeader* start)
{
  method->c.ncode.ncode_start = start;
}

static
bool
buildDispatchTable(Hjava_lang_Class* class, errorInfo *einfo)
{
	Method* meth;
	void** mtab;
	int i;
	Hjava_lang_Class *cc;

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

		/* Do not assign dtable indices for static, private
		 * and constructor methods.
	         */
		if (METHOD_IS_STATIC(meth) || METHOD_IS_PRIVATE(meth)
		    || utf8ConstEqual(meth->name, constructor_name))
		{
			meth->idx = -1;
			continue;
		}

		/* Search superclasses for equivalent method name.
		 * If found extract its index nr.
		 */
		if (getInheritedMethodIndex(super, meth) == false) {

			/* No match found so allocate a new index number ---
			 * except if the method or class is final in which
			 * case it doesn't need one.
			 */

			if (METHOD_IS_FINAL(meth) || CLASS_IS_FINAL(class)) {
				meth->idx = -1;
			} else {
				meth->idx = class->msize++;
			}
		}
	}

	class->vtable = (dispatchTable*)gc_malloc(sizeof(dispatchTable) +
		class->msize * sizeof(void*), KGC_ALLOC_DISPATCHTABLE);

	if (class->vtable == 0) {
		postOutOfMemory(einfo);
		return (false);
	}

	class->vtable->class = class;
	mtab = class->vtable->method;

	/* now build a trampoline for each and every method */
	meth = CLASS_METHODS(class);
	i = CLASS_NMETHODS(class);

	for (; --i >= 0; meth++) {
		void **where;

		/*
		 * Build trampoline and set the method's native code to
		 * point to this trampoline.
		 */
		where = (void**)PMETHOD_NATIVECODE(meth);
		if (engine_buildTrampoline(meth, where, einfo) == 0) {
			return (false);
		}
	}

	/* trampolines are also needed for all virtual inherited
	 * methods so they can be patched up independently
	 */
	for (cc = class->superclass; cc != 0; cc = cc->superclass) {
		meth = CLASS_METHODS(cc);
		i = CLASS_NMETHODS(cc);
		for (; --i >= 0; meth++) {
			void **where;

			/* skip static and overridden methods */
			if (meth->idx < 0 || mtab[meth->idx] != 0) {
				continue;
			}

			/* else it's an inherited method.  Build a trampoline,
			 * but do not update METHOD_NATIVECODE.
			 */
			where = &mtab[meth->idx];
			if (engine_buildTrampoline(meth, where, einfo) == 0) {
				return (false);
			}
		}
	}
	return (true);
}

/*
 * Build the table used by this class for dispatching the interfaces
 * it implements.
 */
static
bool
buildInterfaceDispatchTable(Hjava_lang_Class* class, errorInfo *einfo)
{
	int i, j;

	/* Construct two tables:
	 * This first table maps interfaces to indices in itable.
	 * The itable maps the indices for each interface method to the
	 * index in the dispatch table that corresponds to this interface
	 * method.  If a class does not implement an interface it declared,
	 * or if it attempts to implement it with improper methods, the
	 * dispatch table will have a 0xffffffff index.
	 * (XXX is this 64bit safe?)
	 *
	 * This will cause a NoSuchMethodError
	 * to be thrown later, in soft_lookupinterfacemethod.
	 */
	/* don't bother if we don't implement any interfaces */
	if (class->total_interface_len == 0) {
		return (true);
	}

	class->if2itable = gc_malloc((class->total_interface_len + 1) * sizeof(short), KGC_ALLOC_CLASSMISC);

	if (class->if2itable == 0) {
		postOutOfMemory(einfo);
		return (false);
	}

	/* first count how many indices we need. We need at least one entry to put
	 * the pointer to the class.
	 */
	j = 1;
	for (i = 0; i < class->total_interface_len; i++) {
		class->if2itable[i] = j;
		j += 1;		/* add one word to store interface class */
		j += class->interfaces[i]->msize;
	}
	/* This last entry is to specify the total length of the table. */
	class->if2itable[class->total_interface_len] = j;
	class->itable2dtable = gc_malloc(j * sizeof(void *), KGC_ALLOC_INTERFACE_TABLE);
	if (class->itable2dtable == 0) {
		postOutOfMemory(einfo);
		return (false);
	}
	if (!gc_add_ref(class->itable2dtable)) {
		postOutOfMemory(einfo);
		return false;
	}
	class->itable2dtable[0] = class;
	j = 1;
	for (i = 0; i < class->total_interface_len; i++) {
		int inm = CLASS_NMETHODS(class->interfaces[i]);
		Method *imeth = CLASS_METHODS(class->interfaces[i]);

		/* store interface as first word for type inclusion test */
		class->itable2dtable[j++] = class->interfaces[i];
		for (; inm--; imeth++) {
			Hjava_lang_Class* ncl;
			Method *cmeth = NULL;

			/* ignore static methods in interface --- can an
			 * interface have any beside <clinit>?
			 */
			if (imeth->accflags & ACC_STATIC) {
				continue;
			}

			/* Search the actual method that implements this
			 * interface method by name.
			 */
			for (ncl = class; ncl != NULL;  ncl = ncl->superclass) {
				int k = CLASS_NMETHODS(ncl);
				cmeth = CLASS_METHODS(ncl);
				for (; --k >= 0;  ++cmeth) {
					if (utf8ConstEqual (cmeth->name,
							    imeth->name) &&
					     utf8ConstEqual (METHOD_SIG(cmeth),
							     METHOD_SIG(imeth)))
					{
						goto found;
					}
				}
			}
			/* not found */
			cmeth = NULL;

		    found:;

			/* constructors and static methods cannot implement
			 * interface methods
			 */
			if (cmeth && (METHOD_IS_STATIC(cmeth) ||
				      METHOD_IS_CONSTRUCTOR(cmeth)))
			{
				cmeth = NULL;
			}

			/* cmeth == 0 if
			 * This class does not implement the interface at all.
			 * Or if the class attempts to implement an interface
			 * method with a static method or constructor.
			 */
			if (cmeth == 0) {
				class->itable2dtable[j] = (void *)-1;
			} else {
				if (engine_buildTrampoline(cmeth,
					    class->itable2dtable + j,
					    einfo) == 0)
				{
					return (false);
				}
			}
			j++;
		}
	}
	return (true);
}

/*
 * Compute the interface implementation index for this class.
 * The impl_index is an index for each class that is used to index the
 * implementor table of the interface when dispatching an interface
 * method.  See soft_lookupinterfacemethod in soft.c
 * It points directly at the first itable2dtable index for the section
 * that describes the interface (which is what's now redundantly stored
 * in if2itable[x].)
 *
 * This allows interface lookup in constant time.
 */
static bool
computeInterfaceImplementationIndex(Hjava_lang_Class* clazz, errorInfo* einfo)
{
	int j, k;
	unsigned int i;
	int found_i;
	bool rc = false;
	Hjava_lang_Class** ifcs;

	/* find an impl_index for this class
	 * Note that we only find a suitable impl_index with regard to the
	 * interfaces this class implements, not a globally unique one.
	 *
	 * In other words, two classes implementing disjoint sets of
	 * interfaces may end up with the same impl_index.  For this reason,
	 * the impl_index at this point cannot be used to implement the
	 * equivalent of instanceof <interface>
	 *
	 * We assume that a verifier design is possible that will ensure
	 * that a run-time object indeed implements an interface at the
	 * time when soft_lookupinterfacemethod is invoked.  We do not have
	 * such a verifier at this point.
	 */

	if (clazz->total_interface_len == 0) {
		return (true);
	}

	/* We are manipulating the implementor tables of all implemented
	 * interfaces.  We must lock them.  To avoid deadlock, lets lock
	 * them in ascending order.
	 */
	ifcs = KMALLOC(clazz->total_interface_len * sizeof(Hjava_lang_Class*));
	memcpy(ifcs, clazz->interfaces,
		clazz->total_interface_len * sizeof(Hjava_lang_Class*));

	/* this is bubble-sort */
	do {
		k = 0;
		for (j = 0; j < clazz->total_interface_len - 1; j++) {
			Hjava_lang_Class* iface_j = ifcs[j];
			Hjava_lang_Class* iface_j1 = ifcs[j+1];
			if ((uintp)iface_j > (uintp)iface_j1) {
				k = 1;
				ifcs[j] = iface_j1;
				ifcs[j+1] = iface_j;
			}
		}
	} while (k);

	for (j = 0; j < clazz->total_interface_len; j++) {
		lockClass(ifcs[j]);
	}

	i = 0;
	do {
		found_i = 1;
		for (j = 0; j < clazz->total_interface_len; j++) {
			Hjava_lang_Class* iface = clazz->interfaces[j];
	    		uintp len = 0;

			if (iface->implementors != NULL) {
				/* This is how many entries follow, so the
				 * array has a[0] + 1 elements. We have to convert
				 * the value from pointer type (which is at least 16 bits
				 * as previously).
				 */
			        len = (uintp)iface->implementors[0];
			}
			if (i >= len || iface->implementors[i+1] == NULL) {
				continue;	/* this one would work */
			} else {
				found_i = 0;
				break;
			}
		}
		i++;
	} while (!found_i);

	/* 'i' is a suitable index --- note we incremented i before leaving
	 * the loop above.
	 */
	clazz->impl_index = i;

	/* Now patch all interfaces to point back to their itable2dtable
	 * regions in the dispatch table of this implementing class.
	 */
	for (j = 0; j < clazz->total_interface_len; j++) {
		Hjava_lang_Class* iface = clazz->interfaces[j];
		uintp len;

		/* make sure the implementor table is big enough */
		if (iface->implementors == NULL || i > (uintp)iface->implementors[0]) {
			if (iface->implementors == NULL) {
				len = (i + 1) + 4; /* 4 is slack only */
				iface->implementors = (void ***)gc_malloc(len * sizeof(void **), KGC_ALLOC_CLASSMISC);
			} else {
				/* double in size */
				len = (uintp)(iface->implementors[0] + 1) * 2;
				if (len <= i) {
					len = (i + 1) + 4;
				}
				iface->implementors = (void ***)gc_realloc(
					iface->implementors,
					len * sizeof(void **), KGC_ALLOC_CLASSMISC);
			}

			if (iface->implementors == NULL) {
				postOutOfMemory(einfo);
				goto done;
			}
			/* NB: we assume KMALLOC/KREALLOC zero memory out */
			iface->implementors[0] = (void *)(len - 1);

			/* New entries are magically marked as unused by the GC
			 * as it fills the memory with 0.
			 */
		}

		assert(i < (uintp)iface->implementors[0] + 1);
		iface->implementors[i] = &(clazz->itable2dtable[clazz->if2itable[j]]);
	}
	rc = true;

done:
	for (j = clazz->total_interface_len - 1; j >= 0; j--) {
		unlockClass(ifcs[j]);
	}
	KFREE(ifcs);
	return (rc);
}

/* Check for undefined abstract methods if class is not abstract.
 * See "Java Language Specification" (1996) section 12.3.2.
 *
 * Returns true if the class is abstract or if no abstract methods were
 * found, false otherwise.
 */
static
bool
checkForAbstractMethods(Hjava_lang_Class* class, errorInfo *einfo)
{
	int i;
	void **mtab = class->vtable->method;

	if ((class->accflags & ACC_ABSTRACT) == 0) {
		for (i = 0; i < class->msize; i++) {
			if (mtab[i] == NULL) {
				postException(einfo,
					JAVA_LANG(AbstractMethodError));
				return (false);
			}
		}
	}
	return (true);
}

/*
 * This functions simply assign indices to the virtual methods in an
 * interface.
 *
 * In addition, if the interface has a <clinit> method, it builds a
 * trampoline for it.
 */
static
bool
/* ARGSUSED */
prepareInterface(Hjava_lang_Class* class, errorInfo *einfo)
{
	Method* meth;
	int i;

	meth = CLASS_METHODS(class);
	class->msize = 0;

	/* enumerate indices and store them in meth->idx */
	for (i = 0; i < CLASS_NMETHODS(class); i++, meth++) {
		if (meth->accflags & ACC_STATIC) {
			meth->idx = -1;
			/* Handle <clinit> */
			if (utf8ConstEqual(meth->name, init_name)) {
				void **where;
				where = (void**)PMETHOD_NATIVECODE(meth);

				if (engine_buildTrampoline(meth, where, einfo) == 0) {
					return (false);
				}
			}
		}
		else {
			meth->idx = class->msize++;
		}
	}
	return (true);
}

/*
 * convert a CONSTANT_String entry in the constant poool
 * from utf8 to java.lang.String
 */
Hjava_lang_String*
resolveString(Hjava_lang_Class* clazz, int idx, errorInfo *info)
{
	Utf8Const* utf8;
	Hjava_lang_String* str = NULL;
	constants* pool;

	pool = CLASS_CONSTANTS(clazz);

	lockClass(clazz);
	switch (pool->tags[idx]) {
	case CONSTANT_String:
		utf8 = WORD2UTF(pool->data[idx]);
		str = utf8Const2Java(utf8);
		if (!str) {
			postOutOfMemory(info);
			break;
		}
		pool->data[idx] = (ConstSlot)str;
		pool->tags[idx] = CONSTANT_ResolvedString;
		utf8ConstRelease(utf8);
		break;

	case CONSTANT_ResolvedString:	/* somebody else resolved it */
		str = (Hjava_lang_String*)pool->data[idx];
		break;

	default:
		assert(!!!"Neither String nor ResolvedString?");
	}
	unlockClass(clazz);
	return (str);
}

#undef EAGER_LOADING
/*
 * Initialise the constants.
 * First we make sure all the constant strings are converted to java strings.
 *
 * This code removed:
 * There seems to be no need to be so eager in loading
 * referenced classes or even resolving strings.
 */
static
bool
resolveConstants(Hjava_lang_Class* class UNUSED, errorInfo *einfo UNUSED)
{
	bool success = true;
#ifdef EAGER_LOADING
	iLock* lock;
	int idx;
	constants* pool;
	Utf8Const* utf8;

	lockClass(class);

	/* Scan constant pool and convert any constant strings into true
	 * java strings.
	 */
	pool = CLASS_CONSTANTS (class);
	for (idx = 0; idx < pool->size; idx++) {
		switch (pool->tags[idx]) {
		case CONSTANT_String:
			utf8 = WORD2UTF(pool->data[idx]);
			/* XXX: unchecked malloc */
			pool->data[idx] = (ConstSlot)utf8Const2Java(utf8);
			pool->tags[idx] = CONSTANT_ResolvedString;
			utf8ConstRelease(utf8);
			break;

		case CONSTANT_Class:
			if (getClass(idx, class, einfo) == 0) {
				success = false;
				goto done;
			}
			break;
		}
	}

done:
	unlockClass(this);
#endif	/* EAGER_LOADING */
	return (success);
}

/*
 * Lookup a named field.  Do not search super classes. Do not resolve the field.
 */
static Field*
lookupClassFieldLocal(Hjava_lang_Class* clp, Utf8Const* name, bool isStatic)
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
		if (utf8ConstEqual (name, fptr->name)) {
			return (fptr);
		}
		fptr++;
	}

	return (NULL);
}

/*
 * Lookup a named field. Search superclasses and resolve the field.
 */
Field*
lookupClassField(Hjava_lang_Class* clp, Utf8Const* name, bool isStatic, errorInfo *einfo)
{
	Field *fptr;
	Hjava_lang_Class *c;

	for (c = clp; c; c = c->superclass) {
		fptr = lookupClassFieldLocal(c, name, isStatic);
		if (fptr) {
			/* Resolve field if necessary */
			if (resolveFieldType(fptr, c, einfo) == 0) {
				return (NULL);
			}
			return (fptr);
		}
	}

	if (isStatic) {
		int i = clp->total_interface_len;
		Hjava_lang_Class **cp = &clp->interfaces[0];

		while (--i >= 0) {
			fptr = lookupClassFieldLocal (*cp, name, true);

			if (fptr) {
				if (resolveFieldType(fptr, *cp, einfo) == 0) {
					return (NULL);
				}
				return (fptr);
			}
			cp++;
		}	

	}

DBG(RESERROR,
	dprintf("lookupClassField for %s failed %s:%s\n",
		isStatic?"static":"non-static",clp->name->data, name->data);
    );
	postExceptionMessage(einfo, JAVA_LANG(NoSuchFieldError), "%s", name->data);
	return (NULL);
}

/*
 * Determine the number of arguments and return values from the
 * method signature.
 */
void
countInsAndOuts(const char* str, short* ins, short* outs, char* outtype)
{
	*ins = sizeofSig(&str, false);
	*outtype = str[0];
	*outs = sizeofSig(&str, false);
}

/*
 * Calculate size (in words) of a signature item.
 */
int
sizeofSigChar(char ch, bool want_wide_refs)
{
	switch (ch) {
	case 'V':
		return 0;
	case 'I':
	case 'Z':
	case 'S':
	case 'B':
	case 'C':
	case 'F':
		return 1;
		break;
	case 'D':
	case 'J':
		return 2;
		break;
	case '[':
	case 'L':
		return want_wide_refs ? sizeof(void*) / sizeof(int32) : 1;
	default:
		break;
	}

	return -1;
}

/*
 * Calculate size (in words) of a signature item and move *strp so
 * that it points to the next element of the signature
 */
int
sizeofSigItem(const char** strp, bool want_wide_refs)
{
	int count;
	const char* str;

	for (str = *strp; ; str++) {
		count = sizeofSigChar(*str, want_wide_refs);
		if (count == -1) {
			switch (*str) {
			case '(':
				continue;
			case 0:
			case ')':
				break;
			default:
				count = -1;	/* avoid compiler warning */
				KAFFEVM_ABORT();
			}
		} else {
			while (*str == '[')
				++str;
			if (*str == 'L') {
				while (*str != ';') {
					str++;
				}
			}
		}

		*strp = str + 1;
		return (count);
	}
}

/*
 * Calculate size of data item based on signature and move *strp so
 * that it points to the next element of the signature.
 */
int
sizeofSig(const char** strp, bool want_wide_refs)
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
 * Calculate size (in words) of a Class.
 */
int
sizeofSigClass(Hjava_lang_Class* clazz, bool want_wide_refs)
{
	return sizeofSigChar(CLASS_IS_PRIMITIVE(clazz)
			     ? CLASS_PRIM_SIG(clazz)
			     : 'L', want_wide_refs);
}

/*
 * Calculate size (in words) of the signature of a Method.
 */
int
sizeofSigMethod(Method *meth, bool want_wide_refs)
{
	int i = 0, args = METHOD_NARGS(meth), size = 0;

	while (i < args) {
		size += sizeofSigChar(*METHOD_ARG_TYPE(meth, i),
				      want_wide_refs);
		++i;
	}

	return (size);
}

/**
 * Count the number of arguments in a signature.
 */
int
countArgsInSignature(const char *signature)
{
	int nargs = 0;

	while (sizeofSigItem(&signature, false) != -1) {
		++nargs;
	}

	return (nargs);
}

/**
 * Duplicates a parsed signature.
 */
static parsed_signature_t*
duplicateParsedSignature(parsed_signature_t *orig, errorInfo *einfo)
{
	parsed_signature_t *ret;
	size_t sizeOfSignature;

	sizeOfSignature = sizeof(*ret) + orig->nargs * sizeof(ret->ret_and_args[0]);

	ret = (parsed_signature_t *)gc_malloc (sizeOfSignature, KGC_ALLOC_CLASSMISC);

	if (ret == NULL) {
		postOutOfMemory(einfo);
		return NULL;
	}

	memcpy (ret, orig, sizeOfSignature);

	utf8ConstAddRef (PSIG_UTF8(ret));

	return ret;
}

/*
 * Parse a method signature and return an array of pointers to classes.
 */
parsed_signature_t*
parseSignature(Utf8Const *signature, errorInfo *einfo)
{
	int count;
	parsed_signature_t *sig;
	const char *sig_iter;
	int nargs;

	nargs = countArgsInSignature(signature->data);
	sig = (parsed_signature_t*)gc_malloc(sizeof(*sig) +
					     nargs * sizeof(sig->ret_and_args[0]), KGC_ALLOC_CLASSMISC);
	if (sig == NULL) {
		postOutOfMemory(einfo);
		return (NULL);
	}
	utf8ConstAssign(PSIG_UTF8(sig), signature);
	PSIG_NARGS(sig) = nargs;

	sig_iter = signature->data+1; /* skip '(' */
	for (count = 0; count < nargs; ++count) {
		PSIG_ARG(sig, count) = sig_iter - signature->data;
		sizeofSigItem(&sig_iter, false);
	}
	++sig_iter; /* skip `)' */
	PSIG_RET(sig) = sig_iter - signature->data;

	PSIG_RNARGS(sig) = KaffeVM_countRealNumberOfArgs(sig); 

	return sig;
}

/*
 * Find (or create) an array class with component type C.
 */
Hjava_lang_Class*
lookupArray(Hjava_lang_Class* c, errorInfo *einfo)
{
	Utf8Const *arr_name;
	char *sig;
	classEntry* centry;
	Hjava_lang_Class* arr_class;
	int arr_flags;

	/* If we couldn't resolve the element type, there's no way we can
	 * construct the array type.
	 */
	if (c == 0) {
		return (NULL);
	}

	/* Build signature for array type */
	if (CLASS_IS_PRIMITIVE (c)) {
		if (c == voidClass) {
			postException(einfo, JAVA_LANG(VerifyError));
			return (NULL);
		}

		arr_class = CLASS_ARRAY_CACHE(c);
		if (arr_class) {
			return (arr_class);
		}
		sig = checkPtr(KMALLOC(3));
		sprintf (sig, "[%c", CLASS_PRIM_SIG(c));
	}
	else {
		const char* cname = CLASS_CNAME (c);
		sig = checkPtr(KMALLOC(strlen(cname) + 4));

		if (cname[0] == '[') {
			sprintf (sig, "[%s", cname);
		} else {
			sprintf (sig, "[L%s;", cname);
		}
	}
	arr_name = utf8ConstNew(sig, -1);	/* release before returning */
	KFREE(sig);

	if (!arr_name) {
		postOutOfMemory(einfo);
		return (NULL);
	}
	centry = lookupClassEntry(arr_name, c->loader, einfo);
	if (centry == 0) {
		utf8ConstRelease(arr_name);
		return (NULL);
	}

	if (centry->data.cl != 0) {
		goto found;
	}

	/* Lock class entry */
	lockStaticMutex(&centry->slock);

	/* Incase someone else did it */
	if (centry->data.cl != 0) {
		unlockStaticMutex(&centry->slock);
		goto found;
	}

	arr_class = newClass();
	if (arr_class == 0) {
		postOutOfMemory(einfo);
		centry->data.cl = c = NULL;
		goto bail;
	}

	/* anchor arrays created without classloader */
	if (c->loader == 0) {
		if (!gc_add_ref(arr_class)) {
			postOutOfMemory(einfo);
			centry->data.cl = c = NULL;
			goto bail;
		}
	}

	/*
	 * See JDC Bug: 4208179. The story is that the Spec leaves the
	 * flags "unspecified" and the JDK actually sets them.... most
	 * of the time.
	 */
	arr_flags = ACC_ABSTRACT | ACC_FINAL;
	if (c->accflags & ACC_PUBLIC) {
		arr_flags |= ACC_PUBLIC;
	}
	internalSetupClass(arr_class, arr_name, arr_flags, 0, 0, c->loader, NULL);

	/* use protection domain of element class for array classes */
	arr_class->protectionDomain = c->protectionDomain;

	arr_class->superclass = ObjectClass;
	if (buildDispatchTable(arr_class, einfo) == false) {
		centry->data.cl = c = NULL;
		goto bail;
	}
	CLASS_ELEMENT_TYPE(arr_class) = c;

	/* Add the interfaces that arrays implement.  Note that addInterface
	 * will hold on to arr_interfaces, so it must be a static variable.
	 */
	if (arr_interfaces[0] == 0) {
		arr_interfaces[0] = SerialClass;
		arr_interfaces[1] = CloneClass;
	}
	addInterfaces(arr_class, 2, arr_interfaces);

	arr_class->total_interface_len = arr_class->interface_len;
	arr_class->head.vtable = getClassVtable();
	arr_class->state = CSTATE_COMPLETE;
	arr_class->centry = centry;

	/* mark the corresponding centry as done */
	centry->data.cl = arr_class;
	centry->state = NMS_DONE;

bail:
	unlockStaticMutex(&centry->slock);

	found:;
	if (c && CLASS_IS_PRIMITIVE(c)) {
		CLASS_ARRAY_CACHE(c) = centry->data.cl;
	}
	
#if defined(KAFFE_XDEBUGGING)
	if( (machine_debug_file != NULL) && (centry->data.cl != NULL) ) {
		addDebugInfo(machine_debug_file,
			     DIA_Array, centry->data.cl,
			     DIA_DONE);
	}
#endif

	utf8ConstRelease(arr_name);
	return (centry->data.cl);
}

int findPackageLength(const char *name)
{
	int retval;

	for( retval = strlen(name) - 1;
	     (name[retval] != '/') && (retval > 0);
	     retval-- );
	return( retval );
}
