/*
 * verify.c
 * Perform stages 2 & 3 of class verification.  Stage 1 is performed
 *  when the class is being loaded (so isn't here) and stage 4 is performed
 *  as the method is being executed.
 *
 * verify2() was originally created by someone in Transvirtual Technologies.  however,
 * it did almost nothing (only a shrivel of the stuff needed by pass 2...
 * specifically part 3 of of pass 2, which has been modified),
 * so questions regarding pass 2 should be sent to:
 *     Rob Gonzalez <rob@kaffe.org>
 *
 * verify3() was also originally created by someone in Transvirtual, but it only
 * returned true :)  Questions regarding this one can be sent to Rob as well.
 *
 * All other code in this file was added to assist the full versions of verify2() and
 * verify3().
 */

#include "config.h"
#include "config-std.h"
#include "constants.h"
#include "classMethod.h"
#include "baseClasses.h"
#include "lookup.h"
#include "exception.h"
#include "errors.h"
#include "jni.h"
#include "debug.h"
#include "utf8const.h"

// needed for pass 3
#include "bytecode.h"
#include "itypes.h"


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
 * Type Descriptor Parsing Methods
 *********************************************************************************/
static const char* parseBaseTypeDescriptor(const char* sig);
static const char* parseObjectTypeDescriptor(const char* sig);
static const char* parseArrayTypeDescriptor(const char* sig);
static const char* parseFieldTypeDescriptor(const char* sig);
static bool        parseMethodTypeDescriptor(const char* sig);


/*
 * parses a BaseType descriptor (p.100 JVML Spec 2)
 * 
 *   BaseType: B | C | D | F | I | J | S | Z
 *
 * returns the sig pointing right after the BaseType
 * or NULL if an error occurred.
 */
static
const char*
parseBaseTypeDescriptor(const char* sig)
{
	switch(*sig) {
	case 'B': case 'C': case 'D': case 'F': case 'I': case 'J': case 'S': case 'Z':
		return sig + 1;
	default:
		break;
	}
	
	return NULL;
}

/*
 * parses an ObjectType descriptor (p.101 JVML Spec 2)
 *
 *   ObjectType: L<classname>;
 *   <classname> is a string made from unicode characters
 *
 * precondition: *sig == 'L'
 *
 * returns the sig pointing right after the ObjectType
 * or NULL if an error occurred.
 */
static
const char*
parseObjectTypeDescriptor(const char* sig)
{
	for (sig++; sig != '\0' && *sig != ';'; sig++);
	
	if (sig == '\0') return NULL;
	else             return sig + 1;
}

/* parses an ArrayType descriptor (p.101 JVML Spec 2)
 *
 *   ArrayType    : [ComponentType
 *   ComponentType: FieldType
 *
 * precondition: *sig == '['
 *
 * returns the sig pointing right after the ArrayType
 * or NULL if an error occurred.
 */
static
const char*
parseArrayTypeDescriptor(const char* sig)
{
	while (*sig != '\0' && *sig == '[') sig++;
	
	if (*sig == '\0') return NULL;
	else              return parseFieldTypeDescriptor(sig);
}


/*
 * parses a field type descriptor (i.e. its type signature).
 *
 *   FieldType:
 *     BaseType | ObjectType | ArrayType
 *
 * returns the signatures position immediately after the field's
 * type signature, or NULL if any error has occurred.
 */
static
const char*
parseFieldTypeDescriptor(const char* sig)
{
	if (sig == NULL)      return NULL;
	else if (*sig == '[') return parseArrayTypeDescriptor(sig);
	else if (*sig == 'L') return parseObjectTypeDescriptor(sig);
	else                  return parseBaseTypeDescriptor(sig);
}


/*
 * parses a method type descriptor (i.e. its signature).  (p.103 JVML Spec 2)
 *
 *   MethodDescriptor:
 *     ( ParameterDescriptor* ) ReturnDescriptor
 *   ParameterDescriptor:
 *     FieldType
 *   ReturnDescriptor:
 *     FieldType | V
 *
 * returns whether the descriptor is legal
 */
static
bool
parseMethodTypeDescriptor(const char* sig)
{
	if (sig == NULL || *sig != '(') return false;
	
	DBG(VERIFY2, dprintf("        parsing method type descriptor: %s\n", sig); );
	
	// parse the type parameters
	for (sig++; *sig != '\0' && *sig != ')'; sig = parseFieldTypeDescriptor(sig)) {
		DBG(VERIFY2, dprintf("            parameter sig: %s\n", sig); );
	}
	
	if (*sig == '\0') {
		DBG(VERIFY2, dprintf("            error: no ReturnDescriptor\n"); );
		return false;
	}
	sig++;

	DBG(VERIFY2, dprintf("            ReturnDescriptor: %s\n", sig); );	
	if (*sig == 'V')
		return (*(sig + 1) == '\0');
	
	if (parseFieldTypeDescriptor(sig) != NULL) return true;
	
	return false;
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
			DBG(VERIFY2, dprintf("   UNRESOLVED CLASS: %s\n", CLASS_NAMED(idx, pool)); );
			break;
			
		case CONSTANT_String:
			DBG(VERIFY2, dprintf("   STRING: %s\n", CONST_STRING_NAMED(idx, pool)); );
			break;
			
			
			
		case CONSTANT_Fieldref:
			DBG(VERIFY2, dprintf("   FIELDREF: %s  --type--  %s\n",
					     FIELDREF_NAMED(idx, pool), FIELDREF_SIGD(idx, pool)); );
			break;
			
		case CONSTANT_Methodref:
			DBG(VERIFY2, dprintf("   METHODREF: %s  --type--  %s\n",
					     METHODREF_NAMED(idx, pool), METHODREF_SIGD(idx, pool)); );
			break;
			
			
		case CONSTANT_InterfaceMethodref:
			DBG(VERIFY2, dprintf("   INTERFACEMETHODREF: %s  --type--  %s\n",
					     INTERFACEMETHODREF_NAMED(idx, pool), INTERFACEMETHODREF_SIGD(idx, pool)); );
			break;
			
			
		case CONSTANT_NameAndType:
			DBG(VERIFY2, dprintf("   NAMEANDTYPE: %s  --and--  %s\n",
					     NAMEANDTYPE_NAMED(idx, pool), NAMEANDTYPE_SIGD(idx, pool)); );
			break;
			
		default:
			DBG(VERIFY2, dprintf("   *** UNRECOGNIZED CONSTANT POOL ENTRY in class %s *** \n",
					     CLASS_CNAME(class)); );
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
	
	
	DBG(VERIFY2, dprintf("\nPass 2 Verifying class %s\n", CLASS_CNAME(class)); );
	
	
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
					     CLASS_CNAME(class));
			return(false);
		} else if (CLASS_IS_FINAL(class)) {
			postExceptionMessage(einfo, JAVA_LANG(ClassFormatError),
					     "interfaces cannot be final (in interface \"%s\")",
					     CLASS_CNAME(class));
			return(false);
		}
	}
	*/
	
	
	// java/lang/Object does not have a superclass, is not final, and is not an interface,
	// so we skip all of those checks
	if(strcmp(OBJECTCLASS, CLASS_CNAME(class))) {
		if (class->superclass == NULL) {
			/***********************************************************
			 * 1 - every class except java/lang/Object has a superclass
			 ***********************************************************/
			postExceptionMessage(einfo, JAVA_LANG(ClassFormatError),
					     "class \"%s\" does not have a superclass",
					     CLASS_CNAME(class));
			return(false);
		} else if (CLASS_IS_FINAL(class->superclass)) {
			/*********************************************************
			 * 2a: check that final classes are not inherited from
			 *********************************************************/
		
			postExceptionMessage(einfo, JAVA_LANG(VerifyError),
					     "class \"%s\" cannot inherit from final class \"%s\"",
					     CLASS_CNAME(class),
					     CLASS_CNAME(class->superclass));
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
				if (strcmp(init_name->data, METHOD_NAMED(method))) {
					if (!METHOD_IS_PUBLIC(method)) {
						postExceptionMessage(einfo, JAVA_LANG(ClassFormatError),
								     "interface methods must be declared public (method \"%s\" in interface \"%s\")",
								     METHOD_NAMED(method),
								     CLASS_CNAME(class));
						return(false);
					} else if (!METHOD_IS_ABSTRACT(method)) {
						postExceptionMessage(einfo, JAVA_LANG(ClassFormatError),
								     "interface methods must be declared abstract (method \"%s\" in interface \"%s\")",
								     METHOD_NAMED(method),
								     CLASS_CNAME(class));
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
					
					if (METHOD_IS_FINAL(method) && !METHOD_IS_PRIVATE(method) &&
					    
					    // the following exceptions come from testing against Sun's JVM behavior
					    (strcmp(init_name->data, METHOD_NAMED(method)) &&
					     strcmp("this", METHOD_NAMED(method)))) {
						
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
											     METHOD_NAMED(method),
											     CLASS_CNAME(class->superclass),
											     CLASS_CNAME(class));
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
	postExceptionMessage(einfo, JAVA_LANG(ClassFormatError), "malformed constant pool in class \"%s\"", CLASS_CNAME(class)); \
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
	 *   Note that when it looks at field and method references,
	 *   this pass does not check to make sure that the given
	 *   field or method actually exists in the given class, nor
	 *   does it check that the type descriptors given refer to
	 *   real classes.  It checks only that these items are well
	 *   formed.
	 **************************************************************/
	
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
			if (!parseFieldTypeDescriptor(FIELDREF_SIGD(idx, pool))) {
				postExceptionMessage(einfo, JAVA_LANG(ClassFormatError),
						     "malformed field reference type descriptor, \"%s\", in class \"%s\"",
						     CONST_UTF2CHAR(FIELDREF_TYPE(idx, pool), pool),
						     CLASS_CNAME(class));
				return(false);
			}
			break;
			
		case CONSTANT_Methodref:
			if (!parseMethodTypeDescriptor(METHODREF_SIGD(idx, pool))) {
				postExceptionMessage(einfo, JAVA_LANG(ClassFormatError),
						     "malformed method reference type descriptor, \"%s\", in class \"%s\"",
						     CONST_UTF2CHAR(METHODREF_SIGNATURE(idx, pool), pool),
						     CLASS_CNAME(class));
				return(false);
			}
			break;
			
		case CONSTANT_InterfaceMethodref:
			if (!parseMethodTypeDescriptor(INTERFACEMETHODREF_SIGD(idx, pool))) {
				postExceptionMessage(einfo, JAVA_LANG(ClassFormatError),
						     "malformed interface method reference type descriptor, \"%s\", in class \"%s\"",
						     CONST_UTF2CHAR(INTERFACEMETHODREF_SIGNATURE(idx, pool), pool),
						     CLASS_CNAME(class));
				return(false);
			}
			break;
			
		default:
			// we'll never get here, because of pass 3
			postExceptionMessage(einfo, JAVA_LANG(InternalError),
					     "step 4 of pass 2 verification has screwed up while processing class \"%s\"",
					     CLASS_CNAME(class));
			return(false);
		}
	}
	
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
				     CLASS_CNAME(method->class));
		return false;
	} else if (METHOD_IS_FINAL(method)) {
		postExceptionMessage(einfo, JAVA_LANG(ClassFormatError),
				     "class %s: constructor cannot be final",
				     CLASS_CNAME(method->class));
		return false;
	} else if (!isMethodVoid(method)) {
		postExceptionMessage(einfo, JAVA_LANG(ClassFormatError),
				     "class %s: constructor does not have void return type",
				     CLASS_CNAME(method->class));
		return false;
	} else if (checkMethodStaticConstraints(method, einfo) == false) {
		return false;
	}
	
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
					     CLASS_CNAME(method->class),
					     METHOD_NAMED(method));
			return(false);
		} else if(METHOD_IS_PRIVATE(method)) {
			postExceptionMessage(einfo, JAVA_LANG(ClassFormatError),
					     "%s.%s: method cannot be both public and private",
					     CLASS_CNAME(method->class),
					     METHOD_NAMED(method));
			return(false);
		}
	} else if (METHOD_IS_PROTECTED(method) && METHOD_IS_PRIVATE(method)) {
		postExceptionMessage(einfo, JAVA_LANG(ClassFormatError),
				     "%s.%s: method cannot be both protected and private",
				     CLASS_CNAME(method->class),
				     METHOD_NAMED(method));
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
					     CLASS_CNAME(method->class),
					     METHOD_NAMED(method));
			return(false);
		} else if (METHOD_BYTECODE_LEN(method) >= 65536) {
			postExceptionMessage(einfo, JAVA_LANG(ClassFormatError),
					     "%s.%s: method's code length must be less than 65536 bytes",
					     CLASS_CNAME(method->class),
					     METHOD_NAMED(method));
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
			     CLASS_CNAME(method->class), \
			     METHOD_NAMED(method), _MSG); \
	return(false)
	
	
	/* This is commented out because Sun's verifier doesn't care if an abstract method
	 * is in an abstract class.
	 * 
	// ensure that only abstract classes may have abstract methods
	if (!(CLASS_IS_INTERFACE(method->class) || CLASS_IS_ABSTRACT(method->class))) {
		postExceptionMessage(einfo, JAVA_LANG(ClassFormatError),
				     "in method \"%s.%s\": only abstract classes may have abstract methods",
				     CLASS_CNAME(method->class),
				     METHOD_NAMED(method));
		return(false);
	}
	*/
	
	
	// constructors cannot be abstract
	if (METHOD_IS_CONSTRUCTOR(method)) {
		if (CLASS_IS_INTERFACE(method->class)) {
			postExceptionMessage(einfo, JAVA_LANG(ClassFormatError),
					     "in method \"%s.%s\": an interface cannot have a constructor <init>",
					     CLASS_CNAME(method->class),
					     METHOD_NAMED(method));
			return(false);
		} else {
			postExceptionMessage(einfo, JAVA_LANG(ClassFormatError),
					     "in method \"%s.%s\": constructors cannot be abstract",
					     CLASS_CNAME(method->class),
					     METHOD_NAMED(method));
			return(false);
		}
	}
	
	
	// ensure the abstract method has no code
	if (METHOD_BYTECODE_LEN(method) > 0) {
		postExceptionMessage(einfo, JAVA_LANG(ClassFormatError),
				     "in method \"%s.%s\": abstract methods cannot have a Code attribute",
				     CLASS_CNAME(method->class),
				     METHOD_NAMED(method));
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



/************************************************************************************
 * Pass 3 Verification
 *
 * Pass 3 of verification is broken up into two distinct passes.  The first, 3a,
 * checks static constraints of the bytecode (detailed on p. 134 of the JVML Spec 2).
 * The second, 3b, performs data-flow analysis on the bytecode, which checks structural
 * constraints and does type checking.
 *
 * NOTE: according to p.171 of JVML Spec 2, "Performing verification at link time
 *       is attractive in that the checks are performed just once, substantially
 *       reducing the amount of work that must be done at run time.  Other
 *       implementation strategies are possible, provided that they comply with
 *       The Java Language Specification and The Java Virtual Machine Specification."
 *
 *       In other words, we can perform pass 3 verification whenever convenient, and
 *       not necessarily at link time.  This could provide for optimizations in the
 *       future, as verification generates a lot of information that is useful to
 *       the JIT compilers.  At the moment, this same information is generated by
 *       code-analyze.c again later on.
 ************************************************************************************/

// lengths in bytes of all the instructions
// 16 rows of 16
static const uint8 insnLen[256] = {
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
	2, 3, 2, 3, 3, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
	1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
	1, 1, 1, 1, 3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
	1, 1, 1, 1, 1, 1, 1, 1, 1, 3, 3, 3, 3, 3, 3, 3, 
	3, 3, 3, 3, 3, 3, 3, 3, 3, 2, 0, 0, 1, 1, 1, 1, 
	1, 1, 3, 3, 3, 3, 3, 3, 3, 5, 1, 3, 2, 3, 1, 1, 
	3, 3, 1, 1, 1, 4, 3, 3, 5, 5, 1, 1, 1, 1, 1, 1, 
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
};


// these retrieve the word (16 bits) or double world (32 bits) of bytecode starting
// at pc = _PC
#define	WORD(_CODE, _PC)  ((int16)( \
			   (_CODE[(_PC)+0] << 8) | \
			   (_CODE[(_PC)+1])))
#define	DWORD(_CODE, _PC) ((int32)( \
			   (_CODE[(_PC)+0] << 24) | \
			   (_CODE[(_PC)+1] << 16) | \
			   (_CODE[(_PC)+2] << 8) | \
			   (_CODE[(_PC)+3])))


/*
 * basic block header information
 */
typedef struct block_info
{
	// address of start of block
	uint32 startAddr;
	uint32 lastAddr;  // whether it be the address of a GOTO, etc.
	
	// which address this block returns to on a RET...
	// a value of zero is an error
	uint32 retAddr;
	
	
	// status of block...changed (needs to be re-evaluated), visited, etc.
	char status;
	
	
	Hjava_lang_Class** locals;
	
	uint32 stacksz;
	Hjava_lang_Class** opstack;
	
	
	// each block may have a set of retAddr contexts
	struct block_info* next;
} BlockInfo;


// status flags for a basic block.
// these also pertain to the status[] array for the entire instruction array
#define CHANGED          1
#define VISITED          2
#define IS_INSTRUCTION   4

// if the instruction is preceeded by WIDE
#define WIDE_MODDED      8

// used at the instruction status level to find basic blocks
#define START_BLOCK     16
#define END_BLOCK       32



// types for type checking (pass 3b)
#define	TUNSTABLE		((Hjava_lang_Class*)0)

// returnAddress type
#define	TADDR			((Hjava_lang_Class*)2)

#define	TOBJ			(ObjectClass)
#define TNULL			((Hjava_lang_Class*)4)
#define TSTRING			(StringClass)

#define	TVOID			(voidClass)

#define	TINT			(intClass)
#define	TFLOAT			(floatClass)

#define	TLONG			(longClass)
#define	TDOUBLE			(doubleClass)

// used for the second space of LONGs and DOUBLEs
// in local variables or on the operand stack
#define TWIDE                   ((Hjava_lang_Class*)5)


#define IS_OBJECT(_T)         (_T == TOBJ)
#define IS_STRING(_T)         (_T == TSTRING)

#define IS_ADDRESS(_T)        (_T == TADDR)
#define IS_PRIMITIVE_TYPE(_T) (_T == TINT || _T == TFLOAT || _T == TLONG || _T == TDOUBLE)


/*
 * array types
 */
#define TCHARARR		(charArrClass)

// internally, booleans are represented by bytes
#define TBOOLARR                (byteArrClass)
#define TBYTEARR		(byteArrClass)

#define TSHORTARR		(shortArrClass)
#define TINTARR			(intArrClass)
#define TLONGARR		(longArrClass)

#define TFLOATARR		(floatArrClass)
#define TDOUBLEARR		(doubleArrClass)

#define TOBJARR			(objectArrClass)


#define IS_PRIMITIVE_ARRAY(_T) \
           ((_T) == TCHARARR || (_T) == TBYTEARR || (_T) == TSHORTARR || (_T) == TINTARR || (_T) == TLONGARR || \
            (_T) == TFLOATARR || (_T) == TDOUBLEARR)

// for IS_ARRAY we need to make sure that CLASS_IS_ARRAY is passed something legitimate...
#define IS_ARRAY(_T) \
           ((_T) && ((_T) != TUNSTABLE) && ((_T) != TADDR) && ((_T) != TNULL) && ((_T) != TWIDE) && \
	    CLASS_IS_ARRAY(_T))


/***********************************************************************************
 * Methods for Pass 3 Verification
 ***********************************************************************************/
#ifdef KAFFE_VMDEBUG
static void printInstruction(const int opcode);
#endif

static bool              verifyMethod(errorInfo* einfo, Method* method);
static BlockInfo**       verifyMethod3a(errorInfo* einfo,
					Method* method,
					char* status,       // array of status info for all opcodes
					uint32* numBlocks); // number of basic blocks


/*
 * Verify pass 3:  Check the consistency of the bytecode.
 *
 * This is the k-razy step that does data-flow analysis to prove the safety of the code.
 */
bool
verify3(Hjava_lang_Class* class, errorInfo *einfo)
{
	int n;
	Method* method;
	
	// see if verification is turned on, and whether the class we're about to verify requires verification
	//
	// NOTE: we don't skip interfaces here because an interface may contain a <clinit> method with bytecode
	if (isTrustedClass(class))
		return (true);
	
	
	DBG(VERIFY3, dprintf("\nPass 3 Verifying Class \"%s\"\n", CLASS_CNAME(class)); );
	
	for (n = CLASS_NMETHODS(class), method = CLASS_METHODS(class);
	     n > 0;
	     --n, ++method) {
		
		DBG(VERIFY3, dprintf("\n  -----------------------------------\n  considering method %s\n", METHOD_NAMED(method)); );
		
		// if it's abstract or native, no verification necessary
		if (!(METHOD_IS_ABSTRACT(method) || METHOD_IS_NATIVE(method))) {
			DBG(VERIFY3, dprintf("  verifying method %s\n", METHOD_NAMED(method)); );
			
			if (parseMethodTypeDescriptor(METHOD_SIGD(method)) == false) {
				postExceptionMessage(einfo, JAVA_LANG(ClassFormatError),
						     "Method %s.%s has invalid signature, %s",
						     CLASS_CNAME(class), METHOD_NAMED(method), METHOD_SIGD(method));
				return(false);
			}
			else if (verifyMethod(einfo, method) == false) {
				DBG(VERIFY3, dprintf("  FAILURE TO VERIFY METHOD %s.%s\n",
						     CLASS_CNAME(class),
						     METHOD_NAMED(method)); );
				
				// propagate the error...
				return(false);
			}
		}
	}
	
	
	DBG(VERIFY3, dprintf("\nDone Pass 3 Verifying Class \"%s\"\n", CLASS_CNAME(class)); );
	return(true);
}

/*
 * Controls the verification of a single method.  It allocates most of the memory needed for
 * verification (when encountering JSRs, more memory will need to be allocated later),
 * loads the initial arguments, calls pass3a, then calls pass3b and cleans up.
 */
static
bool
verifyMethod(errorInfo *einfo, Method* method)
{
	// to save some typing, etc.
	int codelen  = METHOD_BYTECODE_LEN(method);
	
	char* status = NULL; // the status of each instruction...changed, visited, etc.
                             // used primarily to help find the basic blocks initially
	
	uint32      numBlocks = 0;
	
	/**************************************************************************************************
	 * Memory Allocation
	 **************************************************************************************************/
	DBG(VERIFY3, dprintf("    allocating memory for verification (codelen = %d)...\n", codelen); );
	
        status   = checkPtr((char*)KMALLOC(codelen * sizeof(char)));
	
	// find basic blocks and allocate memory for them
	verifyMethod3a(einfo, method, status, &numBlocks);
	
	DBG(VERIFY3, dprintf("    done allocating memory\n"); );
	
	KFREE(status);
	return(true);
}


/*
 * verifyMethod3a()
 *     check static constraints.  section 4.8.1 of JVML Spec 2.
 *
 * NOTE: we don't check whether execution can fall off the end of method code here as
 *       that would require us to know whether the last statements are reachable.
 *       Sun's verifier, for instance, rejects code with an unreachable NOP at the end!
 *       Thus we check whether execution can fall off the end during the data flow analysis
 *       of pass 3b, structural constraint checking.
 */
BlockInfo**
verifyMethod3a(errorInfo* einfo,
	       Method* method,
	       char* status,      // array of status info for all opcodes
	       uint32* numBlocks) // number of basic blocks
{
#define VERIFY_ERROR(_MSG) \
	postExceptionMessage(einfo, JAVA_LANG(VerifyError), \
			     "in method \"%s.%s\": %s", \
			     CLASS_CNAME(method->class), METHOD_NAMED(method), _MSG); \
	return NULL

#define ENSURE_NON_WIDE \
	if (wide) { \
		VERIFY_ERROR("illegal instruction following wide instruction"); \
	}

#define CHECK_POOL_IDX(_IDX) \
	if (_IDX > pool->size) { \
		VERIFY_ERROR("attempt to access a constant pool index beyond constant pool range"); \
		return(false); \
	}
	
#define GET_WIDX(_IDX, _PC) \
	_IDX = (_PC) + 1; \
	_IDX = WORD(code, _IDX); \
	CHECK_POOL_IDX(_IDX)

#define BRANCH_IN_BOUNDS(_N, _INST) \
	if (_N < 0 || _N >= codelen) { \
		postExceptionMessage(einfo, JAVA_LANG(VerifyError), \
				     "in method \"%s.%s\": %s branches out of method's code", \
				     CLASS_CNAME(method->class), METHOD_NAMED(method), _INST); \
		return NULL; \
	}

	// makes sure the index given for a local variable is within the correct index
#define CHECK_LOCAL_INDEX(_N) \
	if ((_N) >= method->localsz) { \
		VERIFY_ERROR("attempting to access a local variable beyond local array"); \
	} 
	
	
	constants* pool     = CLASS_CONSTANTS(method->class);
	
	// used for looking at method signatures...
	const char* sig;
	
	int codelen         = METHOD_BYTECODE_LEN(method);
	unsigned char* code = METHOD_BYTECODE_CODE(method);
	
	uint32 pc = 0, newpc = 0, n = 0, idx = 0;
#define NEXTPC (pc + insnLen[code[pc]])
	int32 branchoffset = 0;
	int32 low, high;
	
	bool wide;
	//	bool inABlock; // used when calculating the start/return address of each block
	
	uint32 blockCount  = 0;
	BlockInfo** blocks = NULL;
	
	
	DBG(VERIFY3, dprintf("      Verifier Pass 3a: checking static constraints and finding basic blocks...\n"); );
	
	
	// find the start of every instruction and basic block to determine legal branches
	//
	// also, this makes sure that only legal instructions follow the WIDE instruction
	status[0] |= START_BLOCK;
	wide = false;
	pc = 0;
	while(pc < codelen) {
		status[pc] |= IS_INSTRUCTION;
		
		DBG(VERIFY3, dprintf("        instruction: "); printInstruction(code[pc]); dprintf("\n"); );
		
		if (codelen - pc < insnLen[code[pc]]) {
			VERIFY_ERROR("last operand in code array is cut off");
		}
		
		switch(code[pc]) {
		case ALOAD_0: case ASTORE_0:
		case ILOAD_0: case ISTORE_0:
		case FLOAD_0: case FSTORE_0:
			ENSURE_NON_WIDE;
			CHECK_LOCAL_INDEX(0);
			break;
		case ALOAD_1: case ASTORE_1:
		case ILOAD_1: case ISTORE_1:
		case FLOAD_1: case FSTORE_1:
		case LLOAD_0: case LSTORE_0:
		case DLOAD_0: case DSTORE_0:
			ENSURE_NON_WIDE;
			CHECK_LOCAL_INDEX(1);
			break;
		case ALOAD_2: case ASTORE_2:
		case ILOAD_2: case ISTORE_2:
		case FLOAD_2: case FSTORE_2:
		case LLOAD_1: case LSTORE_1:
		case DLOAD_1: case DSTORE_1:
			ENSURE_NON_WIDE;
			CHECK_LOCAL_INDEX(2);
			break;
		case ALOAD_3: case ASTORE_3:
		case ILOAD_3: case ISTORE_3:
		case FLOAD_3: case FSTORE_3:
		case LLOAD_2: case LSTORE_2:
		case DLOAD_2: case DSTORE_2:
			ENSURE_NON_WIDE;
			CHECK_LOCAL_INDEX(3);
			break;
		case LLOAD_3: case LSTORE_3:
		case DLOAD_3: case DSTORE_3:
			ENSURE_NON_WIDE;
			CHECK_LOCAL_INDEX(4);
			break;
			
			
		case GETFIELD:  case PUTFIELD:
		case GETSTATIC: case PUTSTATIC:
			ENSURE_NON_WIDE;
			
			GET_WIDX(n, pc);
			n = CONST_TAG(n, pool);
			if (n != CONSTANT_Fieldref) {
				VERIFY_ERROR("[get/put][field/static] accesses something in the constant pool that is not a CONSTANT_Fieldref");
			}
			break;
			
		case INVOKEVIRTUAL:
		case INVOKESTATIC:
		case INVOKESPECIAL:
			ENSURE_NON_WIDE;
			
			GET_WIDX(idx, pc);
			n = CONST_TAG(idx, pool);
			if (n != CONSTANT_Methodref) {
				VERIFY_ERROR("invoke* accesses something in the constant pool that is not a CONSTANT_Methodref");
			}
			
			sig = METHODREF_SIGD(idx, pool);
			if (*sig == '<') {
				if (!strcmp(constructor_name->data, sig)) {
					if (code[pc] != INVOKESPECIAL) {
						VERIFY_ERROR("only invokespecial can be used to execute <init> methods");
					}
				} else {
					VERIFY_ERROR("no method with a name whose first character is '<' may be called by an invoke instruction");
				}
			}
			
			break;
			
			
			// invokeinterface is a 5 byte instruction.  the first byte is the instruction.
			// the next two are the index into the constant pool for the methodreference.
			// the fourth is the number of parameters expected by the method, and the verifier
			// must check that the actual method signature of the method to be invoked matches
			// this number.  the 5th must be zero.  these are apparently present for historical
			// reasons (yeah Sun :::smirk:::).
		case INVOKEINTERFACE:
			ENSURE_NON_WIDE;
			
			GET_WIDX(idx, pc);
			n = CONST_TAG(idx, pool);
			if (n != CONSTANT_InterfaceMethodref) {
				VERIFY_ERROR("invokeinterface accesses something in the constant pool that is not a CONSTANT_InterfaceMethodref");
			}
			
			sig = INTERFACEMETHODREF_SIGD(idx, pool);
			if (*sig == '<') {
				VERIFY_ERROR("invokeinterface cannot be used to invoke any instruction with a name starting with '<'");
			}
			
			if (code[pc + 3] == 0) {
				VERIFY_ERROR("fourth byte of invokeinterface is zero");
			} else if (code[pc + 4] != 0) {
				VERIFY_ERROR("fifth byte of invokeinterface is not zero");
			}
			
			break;
			
			
		case INSTANCEOF:
		case CHECKCAST:
			ENSURE_NON_WIDE;
			
			GET_WIDX(n, pc);
			n = CONST_TAG(n, pool);
			if (n != CONSTANT_Class && n != CONSTANT_ResolvedClass) {
				VERIFY_ERROR("instanceof/checkcast indexes a constant pool entry that is not type CONSTANT_Class or CONSTANT_ResolvedClass");
			}
			
			break;
			
			
		case MULTIANEWARRAY:
			ENSURE_NON_WIDE;
			
			GET_WIDX(idx, pc);
			n = CONST_TAG(idx, pool);
			if (n != CONSTANT_Class && n != CONSTANT_ResolvedClass) {
				VERIFY_ERROR("multinewarray indexes a constant pool entry that is not type CONSTANT_Class or CONSTANT_ResolvedClass");
			}
			
			// number of dimensions must be <= num dimensions of array type being created
			sig = CLASS_NAMED(idx, pool);
			newpc = code[pc + 3];
			if (newpc == 0) {
				VERIFY_ERROR("dimensions operand of multianewarray must be non-zero");
			}
			for(n = 0; *sig == '['; sig++, n++);
			if (n < newpc) {
				VERIFY_ERROR("dimensions operand of multianewarray is > the number of dimensions in array being created");
			}
			
			break;
			
			
		case NEW:
			ENSURE_NON_WIDE;
			
			GET_WIDX(idx, pc);
			n = CONST_TAG(idx, pool);
			if (n != CONSTANT_Class && n != CONSTANT_ResolvedClass) {
				VERIFY_ERROR("new indexes a constant pool entry that is not type CONSTANT_Class or CONSTANT_ResolvedClass");
			}
			
			// cannot create arrays with NEW
			sig = CLASS_NAMED(idx, pool);
			if (*sig == '[') {
				VERIFY_ERROR("new instruction used to create a new array");
			}
			break;
			
			
		case ANEWARRAY:
			ENSURE_NON_WIDE;
			
			GET_WIDX(idx, pc);
			n = CONST_TAG(idx, pool);
			if (n != CONSTANT_Class && n != CONSTANT_ResolvedClass) {
				VERIFY_ERROR("anewarray indexes a constant pool entry that is not type CONSTANT_Class or CONSTANT_ResolvedClass");
			}
			
			// count the number of dimensions of the array being created...it must be <= 255
			sig = CLASS_NAMED(idx, pool);
			for (n = 0; *sig == '['; sig++, n++);
			if (n > 255) {
				VERIFY_ERROR("anewarray used to create an array of > 255 dimensions");
			}
			
			break;
			
		case NEWARRAY:
			ENSURE_NON_WIDE;
			
			n = code[pc + 1];
			if (n < 4 || n > 11) {
				VERIFY_ERROR("newarray operand must be in the range [4,11]");
			}
			
			break;
			
			
			
			/***********************************************************
			 * Instructions that can be modified by WIDE
			 ***********************************************************/
		case WIDE:
			ENSURE_NON_WIDE;
			wide = true;
			break;
			
			
		case ALOAD: case ASTORE:
		case ILOAD: case ISTORE:
		case FLOAD: case FSTORE:
			if (wide == true) {
				// the WIDE is considered the beginning of the instruction
				status[pc] ^= IS_INSTRUCTION;
				status[pc] |= WIDE_MODDED;
				
				pc++;
				wide = false;
				
				n = DWORD(code, pc);
			}
			else {
				n = pc + 1;
				n = WORD(code, n);
			}
			
			CHECK_LOCAL_INDEX(n);
			break;
			
		case LLOAD: case LSTORE:
		case DLOAD: case DSTORE:
			if (wide == true) {
				// the WIDE is considered the beginning of the instruction
				status[pc] ^= IS_INSTRUCTION;
				status[pc] |= WIDE_MODDED;
				
				pc++;
				wide = false;
				
				n = WORD(code, pc);
			}
			else {
				n = pc + 1;
				n = WORD(code, n);
			}
			
			// makes sure the index given for a local variable is within the correct index
			//
			// REM: longs and doubles take two consecutive local spots
			CHECK_LOCAL_INDEX(n + 1);
			break;
			
			
		case IINC:
			if (wide == true) {
				// the WIDE is considered the beginning of the instruction
				status[pc] ^= IS_INSTRUCTION;
				status[pc] |= WIDE_MODDED;
				
				pc += 2;
				wide = false;
			}
			break;
			
			
			/********************************************************************
			 * BRANCHING INSTRUCTIONS
			 ********************************************************************/
		case GOTO:
			ENSURE_NON_WIDE;
			status[pc] |= END_BLOCK;
			
			n = pc + 1;
			branchoffset = WORD(code, n);
			newpc = pc + branchoffset;
			BRANCH_IN_BOUNDS(newpc, "goto");
			status[newpc] |= START_BLOCK;
			break;
			
		case GOTO_W:
			ENSURE_NON_WIDE;
			status[pc] |= END_BLOCK;
			
			n = pc + 1;
			branchoffset = DWORD(code, n);
			newpc = pc + branchoffset;
			BRANCH_IN_BOUNDS(newpc, "goto_w");
			status[newpc] |= START_BLOCK;
			break;
			
			
		case IF_ACMPEQ:  case IFNONNULL:
		case IF_ACMPNE:  case IFNULL:
		case IF_ICMPEQ:  case IFEQ:
		case IF_ICMPNE:	 case IFNE:
		case IF_ICMPGT:	 case IFGT:
		case IF_ICMPGE:	 case IFGE:
		case IF_ICMPLT:	 case IFLT:
		case IF_ICMPLE:	 case IFLE:
			ENSURE_NON_WIDE;
			status[pc] |= END_BLOCK;
			
			newpc = NEXTPC;
			BRANCH_IN_BOUNDS(newpc, "if<condition> = false");
			status[newpc] |= START_BLOCK;
			
			n            = pc + 1;
			branchoffset = WORD(code, n);
			newpc        = pc + branchoffset;
			BRANCH_IN_BOUNDS(newpc, "if<condition> = true");
			status[newpc] |= START_BLOCK;
			break;
			
			
		case JSR:
			newpc = pc + 1;
			newpc = pc + WORD(code, newpc);
			goto JSR_common;
		case JSR_W:
			newpc = pc + 1;
			newpc = pc + DWORD(code, newpc);
			
		JSR_common:
			ENSURE_NON_WIDE;
			status[pc] |= END_BLOCK;
			
			BRANCH_IN_BOUNDS(newpc, "jsr");
			status[newpc] |= START_BLOCK;
			
			// the next instruction is a target for branching via RET
			pc = NEXTPC;
			BRANCH_IN_BOUNDS(pc, "jsr/ret");
			status[pc] |= START_BLOCK;
			continue;
			
		case RET:
			status[pc] |= END_BLOCK;
			if (wide == true) {
				status[pc] ^= IS_INSTRUCTION;
				status[pc] |= WIDE_MODDED;
				
				wide = false;
				pc += 2;
			}
			pc = NEXTPC;
			// we don't make the status of the next instruction the start of
			// any block as it's uncertain whether we ever get there.
			continue;
			
			
		case LOOKUPSWITCH:
			ENSURE_NON_WIDE;
			status[pc] |= END_BLOCK;
			
			// default branch...between 0 and 3 bytes of padding are added so that the
			// default branch is at an address that is divisible by 4
			n = (pc + 1) % 4;
			if (n) n = pc + 5 - n;
			else   n = pc + 1;
			newpc = pc + DWORD(code, n);
			BRANCH_IN_BOUNDS(newpc, "lookupswitch");
			status[newpc] |= START_BLOCK;
			DBG(VERIFY3,
			    dprintf("          lookupswitch: pc = %d ... instruction = ", newpc);
			    printInstruction(code[newpc]);
			    dprintf("\n");
			    );
			
			// get number of key/target pairs
			n += 4;
			low = DWORD(code, n);
			if (low < 0) {
				VERIFY_ERROR("lookupswitch with npairs < 0");
			}
			
			// make sure all targets are in bounds
			for (n += 4, high = n + 8*low; n < high; n += 8) {
				newpc = pc + DWORD(code, n+4);
				BRANCH_IN_BOUNDS(newpc, "lookupswitch");
				status[newpc] |= START_BLOCK;
				
				DBG(VERIFY3,
				    dprintf("          lookupswitch: pc = %d ... instruction = ", newpc);
				    printInstruction(code[newpc]);
				    dprintf("\n");
				    );
			}
			
			pc = high;
			continue;
			
			
		case TABLESWITCH:
			ENSURE_NON_WIDE;
			status[pc] |= END_BLOCK;
			
			// default branch...between 0 and 3 bytes of padding are added so that the
			// default branch is at an address that is divisible by 4
			n = (pc + 1) % 4;
			if (n) n = pc + 5 - n;
			else   n = pc + 1;
			newpc = pc + DWORD(code, n);
			BRANCH_IN_BOUNDS(newpc, "tableswitch");
			status[newpc] |= START_BLOCK;
			DBG(VERIFY3,
			    dprintf("          tableswitch: pc = %d ... instruction = ", newpc);
			    printInstruction(code[newpc]);
			    dprintf("\n");
			    );
			
			// get the high and low values of the table
			low  = DWORD(code, n + 4);
			high = DWORD(code, n + 8);
			if (high < low) {
				VERIFY_ERROR("tableswitch high val <= low val");
			}
			n += 12;
			
			// high and low are used as temps in this loop that checks
			// the validity of all the branches in the table
			for (high = n + 4*(high - low + 1); n < high; n += 4) {
				newpc = pc + DWORD(code, n);
				BRANCH_IN_BOUNDS(newpc, "tableswitch");
				status[newpc] |= START_BLOCK;
				
				DBG(VERIFY3,
				    dprintf("          tableswitch: pc = %d ... instruction = ", newpc);
				    printInstruction(code[newpc]);
				    dprintf("\n");
				    );
			}
			
			pc = high;
			continue;
			
			
			// the rest of the ways to end a block
		case RETURN:
		case ARETURN:
		case IRETURN:
		case FRETURN:
		case LRETURN:
		case DRETURN:
		case ATHROW:
			ENSURE_NON_WIDE;
			status[pc] |= END_BLOCK;
			break;
			
			
		default:
			if (wide == true) {
				VERIFY_ERROR("illegal instruction following wide instruction");
			}
		}
		
		pc = NEXTPC;
	}
	
	
	DBG(VERIFY3, dprintf("      Verifier Pass 3a: second pass to locate illegal branches and count blocks..."); );
	
	// newpc is going to stand for the PC of the previous instruction
	for (newpc = 0, pc = 0; pc < codelen; pc++) {
		if (status[pc] & IS_INSTRUCTION) {
			if (status[pc] & START_BLOCK) {
				blockCount++;
				
				if (newpc < pc) {
					// make sure that the previous instruction is
					// marked as the end of a block (it would only
					// have been marked so if it were some kind of
					// branch).
					status[newpc] |= END_BLOCK;
				}
			}
			
			newpc = pc;
		}
		else if (status[pc] & START_BLOCK) {
			VERIFY_ERROR("branch into middle of instruction");
		}
	}
	
	
	DBG(VERIFY3, dprintf("      done, %d blocks found.\n", blockCount); );
	
	
	blocks = NULL;
	/* To be included with pass 3b ...
	 *
	DBG(VERIFY3, dprintf("      Verifier Pass 3a: third pass to allocate memory for basic blocks...\n"); );
	
	blocks = checkPtr((BlockInfo**)KMALLOC(blockCount * sizeof(BlockInfo*)));
	
	for (inABlock = true, n = 0, pc = 0; pc < codelen; pc++) {
		if (status[pc] & START_BLOCK) {
			blocks[n] = createBlock(method);
			blocks[n]->startAddr = pc;
			n++;
			
			inABlock = true;
			
			
			DBG(VERIFY3, dprintf("        setting blocks[%d]->startAddr = %d\n",
					     n-1, blocks[n-1]->startAddr); );
		}
		
		if (inABlock && (status[pc] & END_BLOCK)) {
			blocks[n-1]->lastAddr = pc;
			
			inABlock = false;
			
			
			DBG(VERIFY3, dprintf("        setting blocks[%d]->lastAddr = %d\n",
					     n-1, blocks[n-1]->lastAddr); );
		}
	}
	*/
	
	
	DBG(VERIFY3, dprintf("      Verifier Pass 3a: done\n"); );
	
	*numBlocks = blockCount;
	return blocks;
	
	
#undef NEXTPC
#undef BRANCH_IN_BOUNDS
#undef ENSURE_NON_WIDE
#undef VERIFY_ERROR
}




// for debugging
#ifdef KAFFE_VMDEBUG
static
void
printInstruction(const int opcode)
{
#define PRINT(_OP) dprintf("%s", _OP); return;
	
	switch(opcode) {
	case 0:   PRINT("NOP");
		
	case 1:   PRINT("ACONST-null");
		
	case 2:   PRINT("ICONST_M1");
	case 3:   PRINT("ICONST_0");
	case 4:   PRINT("ICONST_1");
	case 5:   PRINT("ICONST_2");
	case 6:   PRINT("ICONST_3");
	case 7:   PRINT("ICONST_4");
	case 8:   PRINT("ICONST_5");
		
	case 9:   PRINT("LCONST_0");
	case 10:  PRINT("LCONST_1");
		
	case 11:  PRINT("FCONST_0");
	case 12:  PRINT("FCONST_1");
	case 13:  PRINT("FCONST_2");
		
	case 14:  PRINT("DCONST_0");
	case 15:  PRINT("DCONST_1");
		
	case 16:  PRINT("BIPUSH");
	case 17:  PRINT("SIPUSH");
		
	case 18:  PRINT("LDC");
	case 19:  PRINT("LDC_W");
	case 20:  PRINT("LDC2_W");
		
	case 21:  PRINT("ILOAD");
	case 22:  PRINT("LLOAD");
	case 23:  PRINT("FLOAD");
	case 24:  PRINT("DLOAD");
	case 25:  PRINT("ALOAD");
		
	case 26:  PRINT("ILOAD_0");
	case 27:  PRINT("ILOAD_1");
	case 28:  PRINT("ILOAD_2");
	case 29:  PRINT("ILOAD_3");
		
	case 30:  PRINT("LLOAD_0");
	case 31:  PRINT("LLOAD_1");
	case 32:  PRINT("LLOAD_2");
	case 33:  PRINT("LLOAD_3");
		
	case 34:  PRINT("FLOAD_0");
	case 35:  PRINT("FLOAD_1");
	case 36:  PRINT("FLOAD_2");
	case 37:  PRINT("FLOAD_3");
		
	case 38:  PRINT("DLOAD_0");
	case 39:  PRINT("DLOAD_1");
	case 40:  PRINT("DLOAD_2");
	case 41:  PRINT("DLOAD_3");
		
	case 42:  PRINT("ALOAD_0");
	case 43:  PRINT("ALOAD_1");
	case 44:  PRINT("ALOAD_2");
	case 45:  PRINT("ALOAD_3");
		
	case 46:  PRINT("IALOAD");
	case 47:  PRINT("LALOAD");
	case 48:  PRINT("FALOAD");
	case 49:  PRINT("DALOAD");
	case 50:  PRINT("AALOAD");
	case 51:  PRINT("BALOAD");
	case 52:  PRINT("CALOAD");
	case 53:  PRINT("SALOAD");
		
	case 54:  PRINT("ISTORE");
	case 55:  PRINT("LSTORE");
	case 56:  PRINT("FSTORE");
	case 57:  PRINT("DSTORE");
	case 58:  PRINT("ASTORE");
		
	case 59:  PRINT("ISTORE_0");
	case 60:  PRINT("ISTORE_1");
	case 61:  PRINT("ISTORE_2");
	case 62:  PRINT("ISTORE_3");
		
	case 63:  PRINT("LSTORE_0");
	case 64:  PRINT("LSTORE_1");
	case 65:  PRINT("LSTORE_2");
	case 66:  PRINT("LSTORE_3");
		
	case 67:  PRINT("FSTORE_0");
	case 68:  PRINT("FSTORE_1");
	case 69:  PRINT("FSTORE_2");
	case 70:  PRINT("FSTORE_3");
		
	case 71:  PRINT("DSTORE_0");
	case 72:  PRINT("DSTORE_1");
	case 73:  PRINT("DSTORE_2");
	case 74:  PRINT("DSTORE_3");
		
	case 75:  PRINT("ASTORE_0");
	case 76:  PRINT("ASTORE_1");
	case 77:  PRINT("ASTORE_2");
	case 78:  PRINT("ASTORE_3");
		
	case 79:  PRINT("IASTORE");
	case 80:  PRINT("LASTORE");
	case 81:  PRINT("FASTORE");
	case 82:  PRINT("DASTORE");
	case 83:  PRINT("AASTORE");
	case 84:  PRINT("BASTORE");
	case 85:  PRINT("CASTORE");
	case 86:  PRINT("SASTORE");
		
	case 87:  PRINT("POP");
	case 88:  PRINT("POP_W");
		
	case 89:  PRINT("DUP");
	case 90:  PRINT("DUP_X1");
	case 91:  PRINT("DUP_X2");
	case 92:  PRINT("DUP2");
	case 93:  PRINT("DUP2_X1");
	case 94:  PRINT("DUP2_X2");
		
	case 95:  PRINT("SWAP");
		
	case 96:  PRINT("IADD");
	case 97:  PRINT("LADD");
	case 98:  PRINT("FADD");
	case 99:  PRINT("DADD");
		
	case 100: PRINT("ISUB");
	case 101: PRINT("LSUB");
	case 102: PRINT("FSUB");
	case 103: PRINT("DSUB");
		
	case 104: PRINT("IMUL");
	case 105: PRINT("LMUL");
	case 106: PRINT("FMUL");
	case 107: PRINT("DMUL");
		
	case 108: PRINT("IDIV");
	case 109: PRINT("LDIV");
	case 110: PRINT("FDIV");
	case 111: PRINT("DDIV");
		
	case 112: PRINT("IREM");
	case 113: PRINT("LREM");
	case 114: PRINT("FREM");
	case 115: PRINT("DREM");
		
	case 116: PRINT("INEG");
	case 117: PRINT("LNEG");
	case 118: PRINT("FNEG");
	case 119: PRINT("DNEG");
		
	case 120: PRINT("ISHL");
	case 121: PRINT("LSHL");
	case 122: PRINT("FSHL");
	case 123: PRINT("DSHL");
		
	case 124: PRINT("IUSHR");
	case 125: PRINT("LUSHR");
		
	case 126: PRINT("IAND");
	case 127: PRINT("LAND");
		
	case 128: PRINT("IOR");
	case 129: PRINT("LOR");
		
	case 130: PRINT("IXOR");
	case 131: PRINT("LXOR");
		
	case 132: PRINT("IINC");
		
	case 133: PRINT("I2L");
	case 134: PRINT("I2F");
	case 135: PRINT("I2D");
	case 136: PRINT("L2I");
	case 137: PRINT("L2F");
	case 138: PRINT("L2D");
	case 139: PRINT("F2I");
	case 140: PRINT("F2L");
	case 141: PRINT("F2D");
	case 142: PRINT("D2I");
	case 143: PRINT("D2L");
	case 144: PRINT("D2F");
	case 145: PRINT("I2B");
	case 146: PRINT("I2C");
	case 147: PRINT("I2S");
		
	case 148: PRINT("LCMP");
	case 149: PRINT("FCMPL");
	case 150: PRINT("FCMPG");
	case 151: PRINT("DCMPL");
	case 152: PRINT("DCMPG");
		
	case 153: PRINT("IFEQ");
	case 154: PRINT("IFNE");
	case 155: PRINT("IFLT");
	case 156: PRINT("IFGE");
	case 157: PRINT("IFGT");
	case 158: PRINT("IFLE");
		
	case 159: PRINT("IF_ICMPEQ");
	case 160: PRINT("IF_ICMPNE");
	case 161: PRINT("IF_ICMPLT");
	case 162: PRINT("IF_ICMPGE");
	case 163: PRINT("IF_ICMPGT");
	case 164: PRINT("IF_ICMPLE");
	case 165: PRINT("IF_ACMPEQ");
	case 166: PRINT("IF_ACMPNE");
		
	case 167: PRINT("GOTO");
		
	case 168: PRINT("JSR");
	case 169: PRINT("RET");
		
	case 170: PRINT("TABLESWITCH");
	case 171: PRINT("LOOKUPSWITCH");
		
	case 172: PRINT("IRETURN");
	case 173: PRINT("LRETURN");
	case 174: PRINT("FRETURN");
	case 175: PRINT("DRETURN");
	case 176: PRINT("ARETURN");
	case 177: PRINT("RETURN");
		
	case 178: PRINT("GETSTATIC");
	case 179: PRINT("PUTSTATIC");
		
	case 180: PRINT("GETFIELD");
	case 181: PRINT("PUTFIELD");
		
	case 182: PRINT("INVOKEVIRTUAL");
	case 183: PRINT("INVOKESPECIAL");
	case 184: PRINT("INVOKESTATIC");
	case 185: PRINT("INVOKEINTERFACE");
		
	case 187: PRINT("NEW");
		
	case 188: PRINT("NEWARRAY");
	case 189: PRINT("ANEWARRAY");
	case 190: PRINT("ARRAYLENGTH");
		
	case 191: PRINT("ATHROW");
		
	case 192: PRINT("CHECKCAST");
	case 193: PRINT("INSTANCEOF");
		
	case 194: PRINT("MONITORENTER");
	case 195: PRINT("MONITOREXIT");
		
	case 196: PRINT("WIDE");
		
	case 197: PRINT("MULTIANEWARRAY");
		
	case 198: PRINT("IFNULL");
	case 199: PRINT("IFNONNULL");
		
	case 200: PRINT("GOTO_W");
		
	case 201: PRINT("JSR_W");
		
	case 202: PRINT("BREAKPOINT");
		
	case 254: PRINT("IMPDEP1");
	case 255: PRINT("IMPDEP2");
		
	default:  PRINT("UNRECOGNIZED OPCODE");
	}
	
#undef PRINT
}
#endif
