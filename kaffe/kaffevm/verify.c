/*
 * verify.c
 * Perform stages 2 & 3 of class verification.  Stage 1 is performed
 *  when the class is being loaded (so isn't here) and stage 4 is performed
 *  as the method is being executed.
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 *
 *
 * verify2() was originally created by someone in Transvirtual Technologies.  however,
 * it did almost nothing (only a shrivel of the stuff needed by pass 2...
 * specifically part 3 of of pass 2, which has been modified),
 * so questions regarding pass 2 should be sent to:
 *     Rob Gonzalez <rob@kaffe.org>
 */

#include "config.h"
#include "config-std.h"
#include "constants.h"
#include "classMethod.h"
#include "baseClasses.h"
#include "exception.h"
#include "errors.h"
#include "jni.h"
#include "debug.h"
#include "utf8const.h"


/*
 * Returns whether the given class is "trusted" (i.e. does not require verification).
 */
static
bool
isTrustedClass(Hjava_lang_Class* class) {
	// recall (from main.c): -verifyremote (default) ==> verifyMode = 2
	//                       -verify                 ==> verifyMode = 3
	//                       -noverify               ==> verifyMode = 0
	return ((class->loader == 0 && (Kaffe_JavaVMArgs[0].verifyMode & 1) == 0) ||
		(class->loader != 0 && (Kaffe_JavaVMArgs[0].verifyMode & 2) == 0));
}


/*********************************************************************************
 * Pass 2 Verification
 *********************************************************************************/

static bool checkConstructor(Method*, errorInfo*);
static bool checkMethodStaticConstraints(Method*, errorInfo*);
static bool checkAbstractMethod(Method*, errorInfo*);

// perhaps this should go in classMethod.[ch]...
static bool isMethodVoid(Method* method)
{
	char* sig = (char*)method->parsed_sig->signature->data;
	int i = strlen(sig);
	
	// TODO: i > 2 won't be necessary here after we put parsing into pass 2 verification
	return (i > 2) && (sig[i-2] == ')' && sig[i-1] == 'V');
}


/*
 * For debugging purposes.  Prints out the value of the specified constant pool entry.
 */
#ifdef KAFFE_VMDEBUG
static
void
printConstantPool(Hjava_lang_Class* class)
{
	int idx;
	constants *pool = CLASS_CONSTANTS(class);
	
	DBG(VERIFY2, dprintf("    CONSTANT POOL FOR %s\n", class->name->data); );
	
	for (idx = 1; idx < pool->size; idx++) {
		DBG(VERIFY2, dprintf("      %d", idx); );
		
		switch (pool->tags[idx]) {
		case CONSTANT_Utf8:
			DBG(VERIFY2, dprintf("   UTF8: %s\n", CONST_UTF2CHAR(idx, pool)) );
			break;
			
			
		case CONSTANT_Long:
		case CONSTANT_Double:
			idx++;
		case CONSTANT_Integer:
		case CONSTANT_Float:
			DBG(VERIFY2, dprintf("   NUMERICAL\n"); );
			break;
			
			
		case CONSTANT_ResolvedString:
		case CONSTANT_ResolvedClass:
			DBG(VERIFY2, dprintf("   RESOLVED: %s\n",
					     ((Hjava_lang_Class*)pool->data[idx])->name->data); );
			break;
			
			
			
		case CONSTANT_Class:
			DBG(VERIFY2, dprintf("   UNRESOLVED CLASS: %s\n",
					     WORD2UTF(pool->data[idx])->data); );
			break;
			
		case CONSTANT_String:
			DBG(VERIFY2, dprintf("   STRING: %s\n",
					     WORD2UTF(pool->data[idx])->data); );
			break;
			
			
			
		case CONSTANT_Fieldref:
			DBG(VERIFY2, dprintf("   FIELDREF: %s  --type--  %s\n",
					     CONST_UTF2CHAR(FIELDREF_NAME(idx, pool), pool),
					     CONST_UTF2CHAR(FIELDREF_TYPE(idx, pool), pool)); );
			break;
			
		case CONSTANT_Methodref:
			DBG(VERIFY2, dprintf("   METHODREF: %s  --type--  %s\n",
					     CONST_UTF2CHAR(METHODREF_NAME(idx, pool), pool),
					     CONST_UTF2CHAR(METHODREF_SIGNATURE(idx, pool), pool)); );
			break;
			
			
		case CONSTANT_InterfaceMethodref:
			DBG(VERIFY2, dprintf("   INTERFACEMETHODREF: %s  --type--  %s\n",
					     CONST_UTF2CHAR(INTERFACEMETHODREF_NAME(idx, pool), pool),
					     CONST_UTF2CHAR(INTERFACEMETHODREF_SIGNATURE(idx, pool), pool)); );
			break;
			
			
		case CONSTANT_NameAndType:
			DBG(VERIFY2, dprintf("   NAMEANDTYPE: %s  --and--  %s\n",
					     CONST_UTF2CHAR(NAMEANDTYPE_NAME(idx, pool), pool),
					     CONST_UTF2CHAR(NAMEANDTYPE_SIGNATURE(idx, pool), pool)); );
			break;
			
		default:
			// should never get here...
			break;
		}
	}	
}
#endif


/*
 * Verify pass 2:  Check the internal consistency of the class file
 *  but do not check the bytecode.  If at any point we find an error in the
 *  class file format, we abort and throw a ClassFormatError or VerifyError
 *  (depending on what the Sun Java VM would throw given the specific error).
 *
 * According to the JVML spec for Java 2, this should do the following:
 *  1. ensure that every class (except java/lang/Object) has a direct superclass
 *
 *  2. a.  ensure that final classes are not subclassed 
 *     b.  ensure that final methods are not overriden
 *
 *  3. ensure that constant pool satisfies the documented static constraints
 *     - i.e. each CONSTANT_Class_info structure in constant pool contains in its
 *       name_index item a valid constant pool index for a CONSTANT_Utf8_info structure,
 *       etc.
 *
 *  4. ensure that all field references and method references in the constant pool have
 *     (this is basically parsing type signatures for validity):
 *     - valid names
 *     - valid classes
 *     - a valid type desriptor
 *
 * However, the JVML spec also specifies a bunch of other rules that make sense to be
 * checked here (i.e. interfaces must be abstract)
 */
bool
verify2(Hjava_lang_Class* class, errorInfo *einfo)
{
	constants* pool;
	int idx;
	int tag;
	
	
	if (isTrustedClass(class))
		return(true);
	
	
	DBG(VERIFY2, dprintf("\nPass 2 Verifying class %s\n", class->name->data); );
	
	
	/* this is commented out because Sun's Java runtime environment doesn't enforce the restrictions
	 * placed upon interfaces by the specification.
	 * GJC also doesn't follow this!
	 *
	if (CLASS_IS_INTERFACE(class)) {
		// JVML spec p. 96: if a class has ACC_INTERFACE flag set, it must also have
		// ACC_ABSTRACT set and may have ACC_PUBLIC set.  it may not have any other flags
		// (in the given table) set.
		
		if (!CLASS_IS_ABSTRACT(class)) {
			postExceptionMessage(einfo, JAVA_LANG(ClassFormatError),
					     "interfaces must have their abstract flag set (in interface \"%s\")",
					     class->name->data);
			return(false);
		} else if (CLASS_IS_FINAL(class)) {
			postExceptionMessage(einfo, JAVA_LANG(ClassFormatError),
					     "interfaces cannot be final (in interface \"%s\")",
					     class->name->data);
			return(false);
		}
	}
	*/
	
	
	// java/lang/Object does not have a superclass, is not final, and is not an interface,
	// so we skip all of those checks
	if(strcmp(OBJECTCLASS, class->name->data)) {
		if (class->superclass == NULL) {
			/***********************************************************
			 * 1 - every class except java/lang/Object has a superclass
			 ***********************************************************/
			postExceptionMessage(einfo, JAVA_LANG(ClassFormatError),
					     "class \"%s\" does not have a superclass",
					     class->name->data);
			return(false);
		} else if (CLASS_IS_FINAL(class->superclass)) {
			/*********************************************************
			 * 2a: check that final classes are not inherited from
			 *********************************************************/
		
			postExceptionMessage(einfo, JAVA_LANG(VerifyError),
					     "class \"%s\" cannot inherit from final class \"%s\"",
					     class->name->data,
					     class->superclass->name->data);
			return(false);
		} else if (CLASS_IS_INTERFACE(class)) {
			// we separate this from the rest of the method checking because the only requirement
			// of methods in an interface is that they be public, abstract, and nothing else.
			
			Method* method;
			int n;
		
			for (n = CLASS_NMETHODS(class), method = CLASS_METHODS(class);
			     n > 0;
			     --n, ++method) {
				
				// if it's <clinit> (init_name) then it doesn't have to be public.
				// compilers often insert a <clinit> function to initialize an
				// interfaces public static fields, if there are any.
				if (strcmp(init_name->data, method->name->data)) {
					if (!METHOD_IS_PUBLIC(method)) {
						postExceptionMessage(einfo, JAVA_LANG(ClassFormatError),
								     "interface methods must be declared public (method \"%s\" in interface \"%s\")",
								     method->name->data,
								     class->name->data);
						return(false);
					} else if (!METHOD_IS_ABSTRACT(method)) {
						postExceptionMessage(einfo, JAVA_LANG(ClassFormatError),
								     "interface methods must be declared abstract (method \"%s\" in interface \"%s\")",
								     method->name->data,
								     class->name->data);
						return(false);
					}
				}
				
				if (checkMethodStaticConstraints(method, einfo) == false) {
					// something else is wrong...exception raised elsewhere
					return(false);
				}
			}
		} else {
			/*********************************************************
			 * 2b: check that final methods are not overridden
			 *********************************************************/
			
			Hjava_lang_Class *superclass;
			Method* curMethod;
			Method* method;
			int n, m;
			
			
			for (n = CLASS_NMETHODS(class), curMethod = CLASS_METHODS(class);
			     n > 0;
			     --n, ++curMethod) {
			
				// check static constraints on each method
			
				if (METHOD_IS_CONSTRUCTOR(curMethod) && checkConstructor(curMethod, einfo) == false) {
					return(false);
				} else if (checkMethodStaticConstraints(curMethod, einfo) == false) {
					return(false);
				}
			}
			
			
			// cycle through superclass hiarchy and make sure final methods aren't overriden
			for (superclass = class->superclass; superclass != NULL; superclass = superclass->superclass) {
				
				// cycle through methods in current super class
				for (m = CLASS_NMETHODS(superclass), method = CLASS_METHODS(superclass);
				     m > 0;
				     --m, ++method) {
				
					if (METHOD_IS_FINAL(method) &&
					    
					    // the following exceptions come from testing against Sun's JVM behavior
					    (strcmp(init_name->data, method->name->data) &&
					     strcmp("this", method->name->data))) {
						
						// make sure the method in question was not overriden in the current class
						for (n = CLASS_NMETHODS(class), curMethod = CLASS_METHODS(class);
						     n > 0;
						     --n, ++curMethod) {
						
							/* TODO: we should really put all the methods of class into a hash table
							 *       that we can access through name and signature.  this would make
							 *       this check more efficient (is this done in the interpretter?  if so
							 *       we may think about setting up the hashes here and just keeping them
							 *       around for later).
							 */
							if (utf8ConstEqual(curMethod->name, method->name)
							    && utf8ConstEqual(METHOD_SIG(curMethod), METHOD_SIG(method)))
								{
									postExceptionMessage(einfo, JAVA_LANG(VerifyError),
											     "final method \"%s\" declared in class \"%s\" is overriden in class \"%s\"",
											     method->name->data,
											     superclass->name->data,
											     class->name->data);
									return(false);
								}
						}
					}
				}
			}
		}
	}
	
	
	/*********************************************************
	 * 3 - Check class constant pool is consistent
	 *
	 * This is the only section of pass 2 that was available
	 * under Transvirtual, though even this has been modified.
	 *********************************************************/
	// error message for step 3
#define POOLERROR \
	postExceptionMessage(einfo, JAVA_LANG(ClassFormatError), "malformed constant pool in class \"%s\"", class->name->data); \
	return (false)
	
	
	pool = CLASS_CONSTANTS(class);
	
	// Constant pool loaded - check it's integrity.
	for (idx = 1; idx < pool->size; idx++) {
		switch (pool->tags[idx]) {
		case CONSTANT_Fieldref:
		case CONSTANT_Methodref:
		case CONSTANT_InterfaceMethodref:
			tag = CONST_TAG(FIELDREF_CLASS(idx, pool), pool);
			if (tag != CONSTANT_Class && tag != CONSTANT_ResolvedClass) {
				POOLERROR;
			}
			if (CONST_TAG(FIELDREF_NAMEANDTYPE(idx, pool), pool) != CONSTANT_NameAndType) {
				POOLERROR;
			}
			break;
			
		case CONSTANT_NameAndType:
			if (CONST_TAG(NAMEANDTYPE_NAME(idx, pool), pool) != CONSTANT_Utf8) {
				POOLERROR;
			}
			if (CONST_TAG(NAMEANDTYPE_SIGNATURE(idx, pool), pool) != CONSTANT_Utf8) {
				POOLERROR;
			}
			break;
			
			
			// the following tags are always legal, so no furthur checks need be performed
		case CONSTANT_Long:
		case CONSTANT_Double:
			idx++;
		case CONSTANT_Integer:
		case CONSTANT_Float:
			
		case CONSTANT_Utf8:
			
		case CONSTANT_ResolvedString:
		case CONSTANT_ResolvedClass:
			
			/* CONSTANT_Class and CONSTANT_String
			 * have already been re-written to avoid the
			 * extra indirection added by Java.  This is to fit
			 * with the precompiled format.  Here we will not
			 * get an error.
			 */
		case CONSTANT_Class:
		case CONSTANT_String:
			break;
			
			
		default:
			// undefined tag
			POOLERROR;
			break;
		}
	}
#undef POOLERROR	
	DBG(VERIFY2, printConstantPool(class));

	
	/**************************************************************
	 * 4 - ensure that all field and method references in
	 *     constant pool have valid names, classes, and type
	 *     descriptor
	 *
	 * From the JVM spec pp.141-2:
	 *  Note that when it looks at field and method references,
	 *  this pass does not check to make sure that the given
	 *  field or method actually exists in the given class, nor
	 *  does it check that the type descriptors given refer to
	 *  real classes.  It checks only that these items are well
	 *  formed.
	 **************************************************************/
	
	/* TODO: we still need a parser that checks whether the
	 *       type descriptors are valid.  when the parser's done, this
	 *       loop will check that the field and method type descriptors are cool.
	 
	// Constant pool loaded - check the integrity of the type references
	pool = CLASS_CONSTANTS(class);
	for (idx = 1; idx < pool->size; idx++) {
		switch (pool->tags[idx]) {
		case CONSTANT_Long:
		case CONSTANT_Double:
			idx++;
		case CONSTANT_Integer:
		case CONSTANT_Float:
			
		case CONSTANT_Utf8:
			
		case CONSTANT_ResolvedString:
		case CONSTANT_ResolvedClass:
			
		case CONSTANT_Class:
		case CONSTANT_String:

		case CONSTANT_NameAndType:
			break;
			
			
			
		case CONSTANT_Fieldref:
			// todo
			break;
			
		case CONSTANT_Methodref:
		case CONSTANT_InterfaceMethodref:
			// todo
			break;
			
		default:
			// we'll never get here, because of pass 3
			postExceptionMessage(einfo, JAVA_LANG(InternalError),
					     "step 4 of pass 2 verification has screwed up while processing class \"%s\"",
					     class->name->data);
			return(false);
		}
	}
	*/
	
	return (true);
}



/*
 * makes sure a constructor's flags are valid.
 */
static bool checkConstructor(Method* method, errorInfo* einfo)
{
	if (METHOD_IS_STATIC(method)) {
		postExceptionMessage(einfo, JAVA_LANG(ClassFormatError),
				     "class %s: constructor cannot be static",
				     method->class->name->data);
		return false;
	} else if (METHOD_IS_FINAL(method)) {
		postExceptionMessage(einfo, JAVA_LANG(ClassFormatError),
				     "class %s: constructor cannot be final",
				     method->class->name->data);
		return false;
	} else if (!isMethodVoid(method)) {
		postExceptionMessage(einfo, JAVA_LANG(ClassFormatError),
				     "class %s: constructor does not have void return type",
				     method->class->name->data);
		return false;
	} else if (checkMethodStaticConstraints(method, einfo) == false) {
		return false;
	}
	
	// TODO: make sure constructor has a valid signature
	
	return(true);
}


/*
 * ensures that all access flags are legal (more specifically, that the combination
 * of access flags in a method is legal).
 *
 * also checks code-length constraints of the method.
 */
static
bool
checkMethodStaticConstraints(Method* method, errorInfo* einfo)
{
	if (METHOD_IS_PUBLIC(method)) {
		if (METHOD_IS_PROTECTED(method)) {
			postExceptionMessage(einfo, JAVA_LANG(ClassFormatError),
					     "%s.%s: method cannot be both public and protected",
					     method->class->name->data,
					     method->name->data);
			return(false);
		} else if(METHOD_IS_PRIVATE(method)) {
			postExceptionMessage(einfo, JAVA_LANG(ClassFormatError),
					     "%s.%s: method cannot be both public and private",
					     method->class->name->data,
					     method->name->data);
			return(false);
		}
	} else if (METHOD_IS_PROTECTED(method) && METHOD_IS_PRIVATE(method)) {
		postExceptionMessage(einfo, JAVA_LANG(ClassFormatError),
				     "%s.%s: method cannot be both protected and private",
				     method->class->name->data,
				     method->name->data);
		return(false);
	}
	
	
	if (METHOD_IS_ABSTRACT(method)) {
		if (checkAbstractMethod(method, einfo) == false) {
			return(false);
		}
	} else if (!METHOD_IS_NATIVE(method)) {
		// code length static constraints
		
		if (METHOD_BYTECODE_LEN(method) == 0) {
			postExceptionMessage(einfo, JAVA_LANG(ClassFormatError),
					     "%s.%s: method's code length cannot be zero",
					     method->class->name->data,
					     method->name->data);
			return(false);
		} else if (METHOD_BYTECODE_LEN(method) >= 65536) {
			postExceptionMessage(einfo, JAVA_LANG(ClassFormatError),
					     "%s.%s: method's code length must be less than 65536 bytes",
					     method->class->name->data,
					     method->name->data);
			return(false);
		}
	}
	
	
	return(true);
}



/*
 * Given a method with its ACC_ABSTRACT flag set, this checks the the rest of the access flags
 * are set appropriately.  that is, it cannot be final, native, private, static, strictfp, or synchronized.
 *
 * also, we check that an abstract method doesn't have a code attribute.
 */
static
bool
checkAbstractMethod(Method* method, errorInfo* einfo)
{
#define ABSTRACT_METHOD_ERROR(_MSG) \
	postExceptionMessage(einfo, JAVA_LANG(ClassFormatError), \
			     "in method \"%s.%s\": abstract methods cannot be %s", \
			     method->class->name->data, \
			     method->name->data, _MSG); \
	return(false)
	
	
	/* This is commented out because Sun's verifier doesn't care if an abstract method
	 * is in an abstract class.
	 * 
	// ensure that only abstract classes may have abstract methods
	if (!(CLASS_IS_INTERFACE(method->class) || CLASS_IS_ABSTRACT(method->class))) {
		postExceptionMessage(einfo, JAVA_LANG(ClassFormatError),
				     "in method \"%s.%s\": only abstract classes may have abstract methods",
				     method->class->name->data,
				     method->name->data);
		return(false);
	}
	*/
	
	
	// constructors cannot be abstract
	if (METHOD_IS_CONSTRUCTOR(method)) {
		if (CLASS_IS_INTERFACE(method->class)) {
			postExceptionMessage(einfo, JAVA_LANG(ClassFormatError),
					     "in method \"%s.%s\": an interface cannot have a constructor <init>",
					     method->class->name->data,
					     method->name->data);
			return(false);
		} else {
			postExceptionMessage(einfo, JAVA_LANG(ClassFormatError),
					     "in method \"%s.%s\": constructors cannot be abstract",
					     method->class->name->data,
					     method->name->data);
			return(false);
		}
	}
	
	
	// ensure the abstract method has no code
	if (METHOD_BYTECODE_LEN(method) > 0) {
		postExceptionMessage(einfo, JAVA_LANG(ClassFormatError),
				     "in method \"%s.%s\": abstract methods cannot have a Code attribute",
				     method->class->name->data,
				     method->name->data);
		return(false);
	}
	
	
	// enforce access flag rules of the JVML spec. for abstract methods
	if (METHOD_IS_PRIVATE(method))           { ABSTRACT_METHOD_ERROR("private");      }
	else if (METHOD_IS_FINAL(method))        { ABSTRACT_METHOD_ERROR("final");        }
	else if (METHOD_IS_NATIVE(method))       { ABSTRACT_METHOD_ERROR("native");       }
	else if (METHOD_IS_STATIC(method))       { ABSTRACT_METHOD_ERROR("static");       }
	else if (METHOD_IS_STRICT(method))       { ABSTRACT_METHOD_ERROR("strictfp");     }
	
	/* not enforced by Sun's verifier
	 *
	else if (METHOD_IS_SYNCHRONISED(method)) { ABSTRACT_METHOD_ERROR("synchronized"); }
	*/
	
	
	return(true);
	
#undef ABSTRACT_METHOD_ERROR
}



/*
 * Verify pass 3:  Check the consistency of the bytecode.
 */
bool
verify3(Hjava_lang_Class* class, errorInfo *einfo)
{
	if ((class->loader == 0 && (Kaffe_JavaVMArgs[0].verifyMode & 1) == 0) ||
	    (class->loader != 0 && (Kaffe_JavaVMArgs[0].verifyMode & 2) == 0)) {
		return (true);
	}

	/* Run bytecode verifier - for reasons only known to the Sun gods
	 * you are suppose to verify the bytecodes for each method at link
	 * time rather than when you run the damn thing.  It's much more
	 * useful to run the verifier on demand because it creates lots of
	 * info which helps the JIT code generator.  I suppose to conform
	 * to the spec we'll just have to run it twice - once here for
	 * compatibility reasons, and once on demand to get the JIT info.
	 * Hey ho ...
	 */

		/* NOT YET !!! */
	return (true);
}
