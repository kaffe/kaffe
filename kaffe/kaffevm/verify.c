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

/* needed for pass 3 */
#include "bytecode.h"
#include "itypes.h"
#include "soft.h"

#include "verify.h"


/*
 * Returns whether the given class is "trusted" (i.e. does not require verification).
 */
static
bool
isTrustedClass(Hjava_lang_Class* class) {
        /* recall (from main.c): -verifyremote (default) ==> verifyMode = 2
	 *                       -verify                 ==> verifyMode = 3
	 *                       -noverify               ==> verifyMode = 0
	 */
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
	
	/* parse the type parameters */
	for (sig++; sig && *sig != ')' && *sig != '\0'; sig = parseFieldTypeDescriptor(sig)) {
		DBG(VERIFY2, dprintf("            parameter sig: %s\n", sig); );
	}
	
	if (sig == NULL || *sig == '\0') {
		DBG(VERIFY2, dprintf("            error: no ReturnDescriptor\n"); );
		return false;
	}
	sig++;
	
	DBG(VERIFY2, dprintf("            ReturnDescriptor: %s\n", sig); );	
	if (*sig == 'V') {
		sig++;
		return *sig == '\0';
	}
	
	if (parseFieldTypeDescriptor(sig) != NULL) return true;
	
	return false;
}



/*********************************************************************************
 * Pass 2 Verification
 *********************************************************************************/

static bool checkField(Field*, errorInfo*);
static bool checkConstructor(Method*, errorInfo*);
static bool checkMethodStaticConstraints(Method*, errorInfo*);
static bool checkAbstractMethod(Method*, errorInfo*);

/* perhaps this should go in classMethod.[ch]... */
static bool isMethodVoid(Method* method)
{
	char* sig = (char*)method->parsed_sig->signature->data;
	int i = strlen(sig);
	
	return (i > 2) && (sig[i-2] == ')' && sig[i-1] == 'V');
}


/*
 * For debugging purposes.  Prints out the value of the specified constant pool entry.
 */
#if !(defined(NDEBUG) || !defined(KAFFE_VMDEBUG))
const char* indent  = "                ";
const char* indent2 = "                        ";

static
uint32
printConstantPoolEntry(const Hjava_lang_Class* class, uint32 idx)
{
	const constants* pool = CLASS_CONSTANTS(class);
	
	switch (pool->tags[idx]) {
	case CONSTANT_Utf8:
		DBG(VERIFY2, dprintf("   UTF8: %s", CONST_UTF2CHAR(idx, pool)) );
		break;
			
			
	case CONSTANT_Long:
	case CONSTANT_Double:
		idx++;
	case CONSTANT_Integer:
	case CONSTANT_Float:
		DBG(VERIFY2, dprintf("   NUMERICAL"); );
		break;
			
			
	case CONSTANT_ResolvedString:
	case CONSTANT_ResolvedClass:
		DBG(VERIFY2, dprintf("   RESOLVED: %s",
				     ((Hjava_lang_Class*)pool->data[idx])->name->data); );
		break;
			
			
			
	case CONSTANT_Class:
		DBG(VERIFY2, dprintf("   UNRESOLVED CLASS: %s", CLASS_NAMED(idx, pool)); );
		break;
			
	case CONSTANT_String:
		DBG(VERIFY2, dprintf("   STRING: %s", CONST_STRING_NAMED(idx, pool)); );
		break;
			
			
			
	case CONSTANT_Fieldref:
		DBG(VERIFY2, dprintf("   FIELDREF: %s  --type--  %s",
				     FIELDREF_NAMED(idx, pool), FIELDREF_SIGD(idx, pool)); );
		break;
			
	case CONSTANT_Methodref:
		DBG(VERIFY2, dprintf("   METHODREF: %s  --type--  %s",
				     METHODREF_NAMED(idx, pool), METHODREF_SIGD(idx, pool)); );
		break;
			
			
	case CONSTANT_InterfaceMethodref:
		DBG(VERIFY2, dprintf("   INTERFACEMETHODREF: %s  --type--  %s",
				     INTERFACEMETHODREF_NAMED(idx, pool), INTERFACEMETHODREF_SIGD(idx, pool)); );
		break;
			
			
	case CONSTANT_NameAndType:
		DBG(VERIFY2, dprintf("   NAMEANDTYPE: %s  --and--  %s",
				     NAMEANDTYPE_NAMED(idx, pool), NAMEANDTYPE_SIGD(idx, pool)); );
		break;
			
	default:
		DBG(VERIFY2, dprintf("   *** UNRECOGNIZED CONSTANT POOL ENTRY in class %s *** ",
				     CLASS_CNAME(class)); );
	}
	
	return idx;
}

static
void
printConstantPool(const Hjava_lang_Class* class)
{
	uint32 idx;
	const constants *pool = CLASS_CONSTANTS(class);
	
	DBG(VERIFY2, dprintf("    CONSTANT POOL FOR %s\n", class->name->data); );
	
	for (idx = 1; idx < pool->size; idx++) {
		DBG(VERIFY2, dprintf("      %d", idx); );
		
		idx = printConstantPoolEntry(class, idx);
		
		DBG(VERIFY2, dprintf("\n"); );
	}
}
#endif /* !(defined(NDEBUG) || !defined(KAFFE_VMDEBUG)) */


/* Helper function for pool errors */
static inline
bool
poolError(Hjava_lang_Class* class, errorInfo *einfo)
{
	postExceptionMessage(einfo, JAVA_LANG(ClassFormatError), "malformed constant pool in class \"%s\"", CLASS_CNAME(class));
	return (false);
}

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
	
#ifdef UNENFORCED_INTERFACE_RESTRICTIONS
	/* this is commented out because Sun's Java runtime environment doesn't enforce the restrictions
	 * placed upon interfaces by the specification.
	 * GJC also doesn't follow this!
	 */
	if (CLASS_IS_INTERFACE(class)) {
	        /* JVML spec p. 96: if a class has ACC_INTERFACE flag set, it must also have
		 * ( ACC_ABSTRACT set and may have ACC_PUBLIC set.  it may not have any other flags
		 * (in the given table) set.
		 */

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
#endif /* UNENFORCED_INTERFACE_RESTRICTIONS */

	{
		Field *fld;
		int lpc;
		
		fld = CLASS_FIELDS(class);
		for( lpc = 0; lpc < CLASS_NFIELDS(class); lpc++ )
		{
			if( !checkField(&fld[lpc], einfo) )
				return(false);
		}
	}
	
        /* java/lang/Object does not have a superclass, is not final, and is not an interface,
	 * so we skip all of those checks
	 */
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
		        /* we separate this from the rest of the method checking because the only requirement
			 * of methods in an interface is that they be public, abstract, and nothing else.
			 */
			Method* method;
			int n;
		
			for (n = CLASS_NMETHODS(class), method = CLASS_METHODS(class);
			     n > 0;
			     --n, ++method) {
				
			        /* if it's <clinit> (init_name) then it doesn't have to be public.
				 * compilers often insert a <clinit> function to initialize an
				 * interfaces public static fields, if there are any.
				 */
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
				        /* something else is wrong...exception raised elsewhere */
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
			
			        /* check static constraints on each method */
			
				if (METHOD_IS_CONSTRUCTOR(curMethod) && checkConstructor(curMethod, einfo) == false) {
					return(false);
				} else if (checkMethodStaticConstraints(curMethod, einfo) == false) {
					return(false);
				}
			}
			
			
			/* cycle through superclass hiarchy and make sure final methods aren't overriden */
			for (superclass = class->superclass; superclass != NULL; superclass = superclass->superclass) {
				
			        /* cycle through methods in current super class */
				for (m = CLASS_NMETHODS(superclass), method = CLASS_METHODS(superclass);
				     m > 0;
				     --m, ++method) {
					
					if (METHOD_IS_FINAL(method) && !METHOD_IS_PRIVATE(method) &&
					    
					    /* the following exceptions come from testing against Sun's JVM behavior */
					    (strcmp(init_name->data, METHOD_NAMED(method)) &&
					     strcmp("this", METHOD_NAMED(method)))) {
						
					        /* make sure the method in question was not overriden in the current class */
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
	/* error message for step 3 */
		
	pool = CLASS_CONSTANTS(class);
	
        /* Constant pool loaded - check it's integrity. */
	for (idx = 1; idx < pool->size; idx++) {
		switch (pool->tags[idx]) {
		case CONSTANT_Fieldref:
		case CONSTANT_Methodref:
		case CONSTANT_InterfaceMethodref:
			tag = CONST_TAG(FIELDREF_CLASS(idx, pool), pool);
			if (tag != CONSTANT_Class && tag != CONSTANT_ResolvedClass) {
				return poolError(class, einfo);
			}
			if (CONST_TAG(FIELDREF_NAMEANDTYPE(idx, pool), pool) != CONSTANT_NameAndType) {
				return poolError(class, einfo);
			}
			break;
			
		case CONSTANT_NameAndType:
			if (CONST_TAG(NAMEANDTYPE_NAME(idx, pool), pool) != CONSTANT_Utf8) {
				return poolError(class, einfo);
			}
			if (CONST_TAG(NAMEANDTYPE_SIGNATURE(idx, pool), pool) != CONSTANT_Utf8) {
				return poolError(class, einfo);
			}
			break;
			
			
			/* the following tags are always legal, so no furthur checks need be performed */
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
		        /* undefined tag */
			return poolError(class, einfo);
			break;
		}
	}

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
	
	/* Constant pool loaded - check the integrity of the type references */
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
		        /* we'll never get here, because of pass 3 */
			postExceptionMessage(einfo, JAVA_LANG(InternalError),
					     "step 4 of pass 2 verification has screwed up while processing class \"%s\"",
					     CLASS_CNAME(class));
			return(false);
		}
	}
	
	return (true);
}

static
bool
checkField(Field *field, errorInfo *einfo)
{
	bool retval = true;
	char *reason;

	if( (reason = checkAccessFlags(CLASS_IS_INTERFACE(field->clazz) ?
				       ACC_TYPE_INTERFACE_FIELD :
				       ACC_TYPE_FIELD,
				       field->accflags)) != NULL )
	{
		postExceptionMessage(einfo,
				     JAVA_LANG(ClassFormatError),
				     "(class: %s, field: %s) %s",
				     CLASS_CNAME(field->clazz),
				     field->name->data,
				     reason);
		retval = false;
	}
	return( retval );
}


/*
 * makes sure a constructor's flags are valid.
 */
static
bool
checkConstructor(Method* method, errorInfo* einfo)
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
	        /* code length static constraints */
		
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

static inline
bool
abstractMethodError(Method* method, errorInfo* einfo, const char * msg)
{
	postExceptionMessage(einfo, 
			     JAVA_LANG(ClassFormatError),
			     "in method \"%s.%s\": abstract methods cannot be %s",
			     CLASS_CNAME(method->class),
			     METHOD_NAMED(method),
			     msg);
	return(false);
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
	
#ifdef ABSTRACT_METHOD_VS_ABSTRACT_CLASS	
	/* This is commented out because Sun's verifier doesn't care if an abstract method
	 * is in an abstract class.
	 */
	 /* ensure that only abstract classes may have abstract methods */
	if (!(CLASS_IS_INTERFACE(method->class) || CLASS_IS_ABSTRACT(method->class))) {
		postExceptionMessage(einfo, JAVA_LANG(ClassFormatError),
				     "in method \"%s.%s\": only abstract classes may have abstract methods",
				     CLASS_CNAME(method->class),
				     METHOD_NAMED(method));
		return(false);
	}
#endif /* ABSTRACT_METHOD_VS_ABSTRACT_CLASS */
	
	
	/* constructors cannot be abstract */
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
	
	
	/* ensure the abstract method has no code */
	if (METHOD_BYTECODE_LEN(method) > 0) {
		postExceptionMessage(einfo, JAVA_LANG(ClassFormatError),
				     "in method \"%s.%s\": abstract methods cannot have a Code attribute",
				     CLASS_CNAME(method->class),
				     METHOD_NAMED(method));
		return(false);
	}
	
	
	/* enforce access flag rules of the JVML spec. for abstract methods */
	if (METHOD_IS_PRIVATE(method))           { return abstractMethodError(method, einfo, "private");      }
	else if (METHOD_IS_FINAL(method))        { return abstractMethodError(method, einfo, "final");        }
	else if (METHOD_IS_NATIVE(method))       { return abstractMethodError(method, einfo, "native");       }
	else if (METHOD_IS_STATIC(method))       { return abstractMethodError(method, einfo, "static");       }
	else if (METHOD_IS_STRICT(method))       { return abstractMethodError(method, einfo, "strictfp");     }
	
	/* not enforced by Sun's verifier
	 *
	else if (METHOD_IS_SYNCHRONISED(method)) { return abstractMethodError(method, einfo, "synchronized"); }
	*/
	
	
	return(true);
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

/* lengths in bytes of all the instructions */
/* 16 rows of 16 */
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


/* these retrieve the word (16 bits) or double world (32 bits) of bytecode starting
 * at pc = _PC
 */
#define	WORD(_CODE, _PC)  ((int16)( \
			   (_CODE[(_PC)+0] << 8) | \
			   (_CODE[(_PC)+1])))
#define	DWORD(_CODE, _PC) ((int32)( \
			   (_CODE[(_PC)+0] << 24) | \
			   (_CODE[(_PC)+1] << 16) | \
			   (_CODE[(_PC)+2] << 8) | \
			   (_CODE[(_PC)+3])))


/*
 * types for type checking (pass 3b)
 */
static Type  verify_UNSTABLE;
static Type* TUNSTABLE = &verify_UNSTABLE;

static Type  verify_INT;
static Type* TINT = &verify_INT;
       
static Type  verify_FLOAT;
static Type* TFLOAT = &verify_FLOAT;
       
static Type  verify_LONG;
static Type* TLONG = &verify_LONG;
       
static Type  verify_DOUBLE;
static Type* TDOUBLE = &verify_DOUBLE;

/* used for the second space of LONGs and DOUBLEs
 * in local variables or on the operand stack
 */
static Type  _WIDE;
static Type* TWIDE = &_WIDE;

static inline
bool
isWide(const Type * tinfo)
{
	return (tinfo->data.class == TWIDE->data.class);
}

static Type  verify_NULL;
static Type* TNULL = &verify_NULL;
static inline
bool
isNull(const Type * tinfo)
{
	return (tinfo->data.class == TNULL->data.class);
}

static const char* OBJECT_SIG  = "Ljava/lang/Object;";
static Type  verify_OBJ;
static Type* TOBJ = &verify_OBJ;

static const char* OBJARR_SIG = "[Ljava/lang/Object;";
static Type  verify_OBJARR;
static Type* TOBJARR = &verify_OBJARR;


static const char* STRING_SIG = "Ljava/lang/String;";
static Type  verify_STRING;
static Type* TSTRING = &verify_STRING;


static const char* CHARARR_SIG = "[C";
static Type  verify_CHARARR;
static Type* TCHARARR = &verify_CHARARR;

static const char* BYTEARR_SIG = "[B";
static Type  verify_BYTEARR;
static Type* TBYTEARR = &verify_BYTEARR;

static const char* BOOLARR_SIG = "[Z";
static Type  verify_BOOLARR;
static Type* TBOOLARR = &verify_BOOLARR;

static const char* SHORTARR_SIG = "[S";
static Type  verify_SHORTARR;
static Type* TSHORTARR = &verify_SHORTARR;

static const char* INTARR_SIG = "[I";
static Type  verify_INTARR;
static Type* TINTARR = &verify_INTARR;

static const char* LONGARR_SIG = "[J";
static Type  verify_LONGARR;
static Type* TLONGARR = &verify_LONGARR;

static const char* FLOATARR_SIG = "[F";
static Type  verify_FLOATARR;
static Type* TFLOATARR = &verify_FLOATARR;

static const char* DOUBLEARR_SIG = "[D";
static Type  verify_DOUBLEARR;
static Type* TDOUBLEARR = &verify_DOUBLEARR;

/***********************************************************************************
 * Methods for Pass 3 Verification
 ***********************************************************************************/
#ifdef KAFFE_VMDEBUG
static void printInstruction(const int opcode);
static void printType(const Type*);
static void printBlock(const Method* method, const BlockInfo* binfo, const char* indent);
#endif

static BlockInfo*         createBlock(const Method* method);
static void               copyBlockData(const Method* method, BlockInfo* fromBlock, BlockInfo* toBlock);
static void               copyBlockState(const Method* method, BlockInfo* fromBlock, BlockInfo* toBlock);
static void               freeBlock(BlockInfo* binfo);

static BlockInfo*         inWhichBlock(uint32 pc, BlockInfo** blocks, uint32 numBlocks);


static SigStack*          pushSig(SigStack* sigs, const char* sig);
static void               freeSigStack(SigStack* sigs);

static bool               checkUninit(Hjava_lang_Class* this, Type* type);
static UninitializedType* pushUninit(UninitializedType* uninits, const Type* type);
static void               popUninit(const Method*, UninitializedType*, BlockInfo*);
static void               freeUninits(UninitializedType* uninits);


static bool               verifyMethod(errorInfo* einfo, Method* method);
static BlockInfo**        verifyMethod3a(errorInfo* einfo,
					 Method* method,
					 uint32* status,     /* array of status info for all opcodes */
					 uint32* numBlocks); /* number of basic blocks */
static bool               verifyMethod3b(errorInfo* einfo,
					 const Method* method,
					 const uint32* status,
					 BlockInfo** blocks,
					 const uint32 numBlocks,
					 SigStack** sigs,
					 UninitializedType** uninits);

static bool               merge(errorInfo* einfo, const Method* method, BlockInfo* fromBlock, BlockInfo* toBlock);
static bool               verifyBasicBlock(errorInfo*,
					   const Method*,
					   BlockInfo*,
					   SigStack**,
					   UninitializedType**);

static const char*        getNextArg(const char* sig, char* buf);
static bool               loadInitialArgs(const Method* method, errorInfo* einfo,
					  BlockInfo* block, SigStack** sigs, UninitializedType** uninits);

static bool               isReference(const Type* type);
static bool               isArray(const Type* type);
static bool               sameType(Type* t1, Type* t2);
static bool               sameRefType(Type* t1, Type* t2);
static void               resolveType(errorInfo* einfo, Hjava_lang_Class* this, Type *type);

static bool               mergeTypes(errorInfo*, Hjava_lang_Class* this,
				     Type* t1, Type* t2);
static Hjava_lang_Class*  getCommonSuperclass(Hjava_lang_Class* t1,
					      Hjava_lang_Class* t2);


static bool               typecheck(errorInfo*, Hjava_lang_Class* this, Type* t1, Type* t2);

static const char*        getReturnSig(const Method*);
static uint32             countSizeOfArgsInSignature(const char* sig);
static bool               checkMethodCall(errorInfo* einfo, const Method* method,
					  BlockInfo* binfo, uint32 pc,
					  SigStack** sigs, UninitializedType** uninits);


/*
 * Initialize Type structures needed for verification
 */
void
initVerifierPrimTypes(void)
{
	TUNSTABLE->tinfo = TINFO_SYSTEM;
	TUNSTABLE->data.class = (Hjava_lang_Class*)TUNSTABLE;
	
	TWIDE->tinfo = TINFO_SYSTEM;
	TWIDE->data.class = (Hjava_lang_Class*)TWIDE;
	
	
	TINT->tinfo = TINFO_PRIMITIVE;
	TINT->data.class = intClass;
	
	TLONG->tinfo = TINFO_PRIMITIVE;
	TLONG->data.class = longClass;
	
	TFLOAT->tinfo = TINFO_PRIMITIVE;
	TFLOAT->data.class = floatClass;
	
	TDOUBLE->tinfo = TINFO_PRIMITIVE;
	TDOUBLE->data.class = doubleClass;
	
	
	TNULL->tinfo = TINFO_CLASS;
	TNULL->data.class = (Hjava_lang_Class*)TNULL;
	
	TOBJ->tinfo = TINFO_SIG;
	TOBJ->data.sig = OBJECT_SIG;
	
	TOBJARR->tinfo = TINFO_SIG;	
	TOBJARR->data.sig = OBJARR_SIG;
	
	
	TSTRING->data.sig = STRING_SIG;
	TSTRING->tinfo = TINFO_SIG;
	
	
	TCHARARR->tinfo = TINFO_SIG;
	TCHARARR->data.sig = CHARARR_SIG;
	
	TBYTEARR->tinfo = TINFO_SIG;
	TBYTEARR->data.sig = BYTEARR_SIG;
	
	TBOOLARR->tinfo = TINFO_SIG;
	TBOOLARR->data.sig = BOOLARR_SIG;
	
	TSHORTARR->tinfo = TINFO_SIG;
	TSHORTARR->data.sig = SHORTARR_SIG;
	
	TINTARR->tinfo = TINFO_SIG;
	TINTARR->data.sig = INTARR_SIG;
	
	TLONGARR->tinfo = TINFO_SIG;
	TLONGARR->data.sig = LONGARR_SIG;
	
	TFLOATARR->tinfo = TINFO_SIG;
	TFLOATARR->data.sig = FLOATARR_SIG;
	
	TDOUBLEARR->tinfo = TINFO_SIG;	
	TDOUBLEARR->data.sig = DOUBLEARR_SIG;
}


/*
 * Verify pass 3:  Check the consistency of the bytecode.
 *
 * This is the k-razy step that does data-flow analysis to prove the safety of the code.
 */
bool
verify3(Hjava_lang_Class* class, errorInfo *einfo)
{
	uint32 n;
	bool success = true;
	Method* method;
	
	/* see if verification is turned on, and whether the class we're about to verify requires verification
	 *
	 * NOTE: we don't skip interfaces here because an interface may contain a <clinit> method with bytecode
	 */
	if (isTrustedClass(class)) {
		goto done;
	}
	
	
	/* make sure it's initialized...we had some problems because of this */
	einfo->type = 0;
	
	
	DBG(VERIFY3, dprintf("\nPass 3 Verifying Class \"%s\"\n", CLASS_CNAME(class)); );
	DBG(VERIFY3, {
		Hjava_lang_Class* tmp;
		for (tmp = class->superclass; tmp; tmp = tmp->superclass) {
			dprintf("                        |-> %s\n", CLASS_CNAME(tmp));
		}
	});
	
	for (n = CLASS_NMETHODS(class), method = CLASS_METHODS(class);
	     n > 0;
	     --n, ++method) {
		
		DBG(VERIFY3, dprintf("\n  -----------------------------------\n  considering method %s%s\n",
				     METHOD_NAMED(method), METHOD_SIGD(method)); );
		
		/* if it's abstract or native, no verification necessary */
		if (!(METHOD_IS_ABSTRACT(method) || METHOD_IS_NATIVE(method))) {
			DBG(VERIFY3, dprintf("  verifying method %s\n", METHOD_NAMED(method)); );
			
			if (!parseMethodTypeDescriptor(METHOD_SIGD(method))) {
				postExceptionMessage(einfo, JAVA_LANG(ClassFormatError),
						     "Method %s.%s has invalid signature, %s",
						     CLASS_CNAME(class), METHOD_NAMED(method), METHOD_SIGD(method));
				success = false;
				goto done;
			}
			else if (!verifyMethod(einfo, method)) {
				if (einfo->type == 0) {
					postExceptionMessage(einfo, JAVA_LANG(InternalError),
							     "failure to verify method %s.%s ... reason unspecified",
							     CLASS_CNAME(class), METHOD_NAMED(method));
				}
				success = false;
				goto done;
			}
		}
	}
	
	
 done:
	DBG(VERIFY3, dprintf("\nDone Pass 3 Verifying Class \"%s\"\n", CLASS_CNAME(class)); );
	return success;
}

	
/**************************************************************************************************
 * Memory Management Macros
 **************************************************************************************************/
/* to make sure we don't forget to unalloc anything...
 * should be called during ANY EXIT FROM verifyMethod
 */
static inline
void
cleanupInVerifyMethod(uint32* status, SigStack* sigs, UninitializedType* uninits, uint32* numBlocks, BlockInfo ** blocks)
{
	DBG(VERIFY3, dprintf("    cleaning up..."); );
	KFREE(status);
	if (blocks != NULL) {
		while (*numBlocks > 0) {
			freeBlock(blocks[--(*numBlocks)]);
		}
		KFREE(blocks);
	}
        freeSigStack(sigs);
        freeUninits(uninits);
        DBG(VERIFY3, dprintf(" done\n"); );
}

static inline
bool
failInVerifyMethod(errorInfo *einfo, Method* method, uint32* status, SigStack* sigs, UninitializedType* uninits, uint32* numBlocks, BlockInfo ** blocks)
{
        DBG(VERIFY3, dprintf("    Verify Method 3b: %s.%s%s: FAILED\n",
			     CLASS_CNAME(method->class), METHOD_NAMED(method), METHOD_SIGD(method)); );
	if (einfo->type == 0) {
		DBG(VERIFY3, dprintf("      DBG ERROR: should have raised an exception\n"); );
		postException(einfo, JAVA_LANG(VerifyError));
	}
        cleanupInVerifyMethod(status, sigs, uninits, numBlocks, blocks);
        return(false);
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
        /* to save some typing, etc. */
	int codelen  = METHOD_BYTECODE_LEN(method);
	
	uint32* status = NULL; /* the status of each instruction...changed, visited, etc.
				* used primarily to help find the basic blocks initially
				*/

	SigStack* sigs = NULL;
	
	UninitializedType* uninits = NULL;
	
	uint32      numBlocks = 0;
	BlockInfo** blocks    = NULL;
	
	/**************************************************************************************************
	 * Memory Allocation
	 **************************************************************************************************/
	DBG(VERIFY3, dprintf("        allocating memory for verification (codelen = %d)...\n", codelen); );
	
        status   = checkPtr((char*)gc_malloc(codelen * sizeof(uint32), GC_ALLOC_VERIFIER));
	
	/* find basic blocks and allocate memory for them */
	blocks = verifyMethod3a(einfo, method, status, &numBlocks);
	if (!blocks) {
		DBG(VERIFY3, dprintf("        some kinda error finding the basic blocks in pass 3a\n"); );
		
		/* propagate error */
		return failInVerifyMethod(einfo, method, status, sigs, uninits, &numBlocks, blocks);
	}
	
	DBG(VERIFY3, dprintf("        done allocating memory\n"); );
	/**************************************************************************************************
	 * Prepare for data-flow analysis
	 **************************************************************************************************/
	
	/* load initial arguments into local variable array */
	DBG(VERIFY3, dprintf("    about to load initial args...\n"); );
	if (!loadInitialArgs(method, einfo, blocks[0], &sigs, &uninits)) {
	        /* propagate error */
		return failInVerifyMethod(einfo, method, status, sigs, uninits, &numBlocks, blocks);
	}
	DBG(VERIFY3, {
	        /* print out the local arguments */
		int n;
		for(n = 0; n < method->localsz; n++) {
			dprintf("        local %d: ", n);
			printType(&blocks[0]->locals[n]);
			dprintf("\n");
		}
	} );
	
	
	if (!verifyMethod3b(einfo, method,
			    status, blocks, numBlocks,
			    &sigs, &uninits)) {
		return failInVerifyMethod(einfo, method, status, sigs, uninits, &numBlocks, blocks);
	}
	
	cleanupInVerifyMethod(status, sigs, uninits, &numBlocks, blocks);
	DBG(VERIFY3, dprintf("    Verify Method 3b: done\n"); );
	return(true);
}

/*
 * Helper function for error reporting in verifyMethod3a.
 */
static inline
BlockInfo **
verifyErrorInVerifyMethod3a(errorInfo* einfo,
			    Method* method,
			    const char * msg)
{
	if (einfo->type == 0) {
		postExceptionMessage(einfo, JAVA_LANG(VerifyError),
				     "in method \"%s.%s\": %s",
				     CLASS_CNAME(method->class), METHOD_NAMED(method), msg);
	}
	return NULL;
}

/*
 * Helper function for error reporting in BRANCH_IN_BOUNDS macro in verifyMethod3a.
 */
static inline
BlockInfo **
branchInBoundsErrorInVerifyMethod3a(errorInfo* einfo,
				    Method* method,
				    int codelen,
				    int n)
{
  DBG(VERIFY3, dprintf("ERROR: branch to (%d) out of bound (%d) \n", n, codelen); );
  return verifyErrorInVerifyMethod3a(einfo, method, "branch out of method code");
}

/*
 * Helper function for error reporting in CHECK_LOCAL_INDEX macro in verifyMethod3a.
 */
static inline
BlockInfo **
checkLocalIndexErrorInVerifyMethod3a(errorInfo* einfo,
				     Method* method,
				     uint32 pc,
				     unsigned char* code,
				     int n)
{
  DBG(VERIFY3,
      dprintf("ERROR:  pc = %d, instruction = ", pc);
      printInstruction(code[pc]);
      dprintf(", localsz = %d, localindex = %d\n", method->localsz, n);
      );
  return verifyErrorInVerifyMethod3a(einfo, method, "attempting to access a local variable beyond local array");
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
static
BlockInfo**
verifyMethod3a(errorInfo* einfo,
	       Method* method,
	       uint32* status,    /* array of status info for all opcodes */
	       uint32* numBlocks) /* number of basic blocks */
{

#define ENSURE_NON_WIDE \
	if (wide) { \
		return verifyErrorInVerifyMethod3a(einfo, method, "illegal instruction following wide instruction"); \
	}

#define CHECK_POOL_IDX(_IDX) \
	if (_IDX > pool->size) { \
		return verifyErrorInVerifyMethod3a(einfo, method, "attempt to access a constant pool index beyond constant pool range"); \
	}
	
#define GET_IDX(_IDX, _PC) \
	(_IDX) = (_PC) + 1; \
	(_IDX) = code[_IDX]; \
	CHECK_POOL_IDX(_IDX)
	
#define GET_WIDX(_IDX, _PC) \
	_IDX = (_PC) + 1; \
	_IDX = WORD(code, _IDX); \
	CHECK_POOL_IDX(_IDX)

#define BRANCH_IN_BOUNDS(_N, _INST) \
	if (_N < 0 || _N >= codelen) { \
	  return branchInBoundsErrorInVerifyMethod3a(einfo, method, codelen, _N); \
	}

        /* makes sure the index given for a local variable is within the correct index */
#define CHECK_LOCAL_INDEX(_N) \
	if ((_N) >= method->localsz) { \
	  return checkLocalIndexErrorInVerifyMethod3a(einfo, method, pc, code, _N); \
	}
	
	constants* pool     = CLASS_CONSTANTS(method->class);
	
	/* used for looking at method signatures... */
	const char* sig;
	
	int codelen         = METHOD_BYTECODE_LEN(method);
	unsigned char* code = METHOD_BYTECODE_CODE(method);
	
	uint32 pc = 0, newpc = 0, n = 0, idx = 0;
#define NEXTPC (pc + insnLen[code[pc]])
	int32 branchoffset = 0;
	int32 low, high;
	
	bool wide;
	bool inABlock; /* used when calculating the start/return address of each block */
	
	uint32 blockCount  = 0;
	BlockInfo** blocks = NULL;
	
	
	DBG(VERIFY3, dprintf("    Verifier Pass 3a: checking static constraints and finding basic blocks...\n"); );
	
	
	/* find the start of every instruction and basic block to determine legal branches
	 *
	 * also, this makes sure that only legal instructions follow the WIDE instruction
	 */
	status[0] |= START_BLOCK;
	wide = false;
	pc = 0;
	while(pc < codelen) {
		status[pc] |= IS_INSTRUCTION;
		
		DBG(VERIFY3, dprintf("        instruction: (%d) ", pc); printInstruction(code[pc]); dprintf("\n"); );
		
		if (codelen - pc < insnLen[code[pc]]) {
			return verifyErrorInVerifyMethod3a(einfo, method, "last operand in code array is cut off");
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
			
			
		case LDC1:
			GET_IDX(idx, pc);
			goto LDC_common;
		case LDC2:
			GET_WIDX(idx, pc);
		LDC_common:
			n = CONST_TAG(idx, pool);
			if (n != CONSTANT_Integer && n != CONSTANT_Float &&
			    n != CONSTANT_String && n != CONSTANT_ResolvedString) {
				return verifyErrorInVerifyMethod3a(einfo, method, "ldc* on constant pool entry other than int/float/string");
			}
			break;
			
		case LDC2W:
			GET_WIDX(idx, pc);
			n = CONST_TAG(idx, pool);
			if (n != CONSTANT_Double && n != CONSTANT_Long) {
				return verifyErrorInVerifyMethod3a(einfo, method, "ldc2_w on constant pool entry other than long or double");
			}
			break;
			
		case GETFIELD:  case PUTFIELD:
		case GETSTATIC: case PUTSTATIC:
			ENSURE_NON_WIDE;
			
			GET_WIDX(idx, pc);
			idx = CONST_TAG(idx, pool);
			if (idx != CONSTANT_Fieldref) {
				return verifyErrorInVerifyMethod3a(einfo, method, "[get/put][field/static] accesses something in the constant pool that is not a CONSTANT_Fieldref");
			}
			break;
			
		case INVOKEVIRTUAL:
		case INVOKESTATIC:
		case INVOKESPECIAL:
			ENSURE_NON_WIDE;
			
			GET_WIDX(idx, pc);
			n = CONST_TAG(idx, pool);
			if (n != CONSTANT_Methodref) {
				return verifyErrorInVerifyMethod3a(einfo, method, "invoke* accesses something in the constant pool that is not a CONSTANT_Methodref");
			}
			
			sig = METHODREF_SIGD(idx, pool);
			if (*sig == '<') {
				if (!strcmp(constructor_name->data, sig)) {
					if (code[pc] != INVOKESPECIAL) {
						return verifyErrorInVerifyMethod3a(einfo, method, "only invokespecial can be used to execute <init> methods");
					}
				} else {
					return verifyErrorInVerifyMethod3a(einfo, method, "no method with a name whose first character is '<' may be called by an invoke instruction");
				}
			}
			
			break;
			
			
			/* invokeinterface is a 5 byte instruction.  the first byte is the instruction.
			 * the next two are the index into the constant pool for the methodreference.
			 * the fourth is the number of parameters expected by the method, and the verifier
			 * must check that the actual method signature of the method to be invoked matches
			 * this number.  the 5th must be zero.  these are apparently present for historical
			 * reasons (yeah Sun :::smirk:::).
			 */
		case INVOKEINTERFACE:
			ENSURE_NON_WIDE;
			
			GET_WIDX(idx, pc);
			n = CONST_TAG(idx, pool);
			if (n != CONSTANT_InterfaceMethodref) {
				return verifyErrorInVerifyMethod3a(einfo, method, "invokeinterface accesses something in the constant pool that is not a CONSTANT_InterfaceMethodref");
			}
			
			sig = INTERFACEMETHODREF_SIGD(idx, pool);
			if (*sig == '<') {
				return verifyErrorInVerifyMethod3a(einfo, method, "invokeinterface cannot be used to invoke any instruction with a name starting with '<'");
			}
			
			if (code[pc + 3] == 0) {
				return verifyErrorInVerifyMethod3a(einfo, method, "fourth byte of invokeinterface is zero");
			} else if (code[pc + 4] != 0) {
				return verifyErrorInVerifyMethod3a(einfo, method, "fifth byte of invokeinterface is not zero");
			}
			
			break;
			
			
		case INSTANCEOF:
		case CHECKCAST:
			ENSURE_NON_WIDE;
			
			GET_WIDX(n, pc);
			n = CONST_TAG(n, pool);
			if (n != CONSTANT_Class && n != CONSTANT_ResolvedClass) {
				return verifyErrorInVerifyMethod3a(einfo, method, "instanceof/checkcast indexes a constant pool entry that is not type CONSTANT_Class or CONSTANT_ResolvedClass");
			}
			
			break;
			
			
		case MULTIANEWARRAY:
			ENSURE_NON_WIDE;
			
			GET_WIDX(idx, pc);
			n = CONST_TAG(idx, pool);
			if (n != CONSTANT_Class && n != CONSTANT_ResolvedClass) {
				return verifyErrorInVerifyMethod3a(einfo, method, "multinewarray indexes a constant pool entry that is not type CONSTANT_Class or CONSTANT_ResolvedClass");
			}
			
			/* number of dimensions must be <= num dimensions of array type being created */
			sig = CLASS_NAMED(idx, pool);
			newpc = code[pc + 3];
			if (newpc == 0) {
				return verifyErrorInVerifyMethod3a(einfo, method, "dimensions operand of multianewarray must be non-zero");
			}
			for(n = 0; *sig == '['; sig++, n++);
			if (n < newpc) {
				return verifyErrorInVerifyMethod3a(einfo, method, "dimensions operand of multianewarray is > the number of dimensions in array being created");
			}
			
			break;
			
			
		case NEW:
			ENSURE_NON_WIDE;
			
			GET_WIDX(idx, pc);
			n = CONST_TAG(idx, pool);
			if (n != CONSTANT_Class && n != CONSTANT_ResolvedClass) {
				return verifyErrorInVerifyMethod3a(einfo, method, "new indexes a constant pool entry that is not type CONSTANT_Class or CONSTANT_ResolvedClass");
			}
			
			/* cannot create arrays with NEW */
			sig = CLASS_NAMED(idx, pool);
			if (*sig == '[') {
				return verifyErrorInVerifyMethod3a(einfo, method, "new instruction used to create a new array");
			}
			break;
			
			
		case ANEWARRAY:
			ENSURE_NON_WIDE;
			
			GET_WIDX(idx, pc);
			n = CONST_TAG(idx, pool);
			if (n != CONSTANT_Class && n != CONSTANT_ResolvedClass) {
				return verifyErrorInVerifyMethod3a(einfo, method, "anewarray indexes a constant pool entry that is not type CONSTANT_Class or CONSTANT_ResolvedClass");
			}
			
			/* count the number of dimensions of the array being created...it must be <= 255 */
			sig = CLASS_NAMED(idx, pool);
			for (n = 0; *sig == '['; sig++, n++);
			if (n > 255) {
				return verifyErrorInVerifyMethod3a(einfo, method, "anewarray used to create an array of > 255 dimensions");
			}
			
			break;
			
		case NEWARRAY:
			ENSURE_NON_WIDE;
			
			n = code[pc + 1];
			if (n < 4 || n > 11) {
				return verifyErrorInVerifyMethod3a(einfo, method, "newarray operand must be in the range [4,11]");
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
			        /* the WIDE is considered the beginning of the instruction */
				status[pc] ^= IS_INSTRUCTION;
				status[pc] |= WIDE_MODDED;
				
				pc++;
				wide = false;
				
				n = WORD(code, pc);
			}
			else {
				n = code[pc + 1];
			}
			
			CHECK_LOCAL_INDEX(n);
			break;
			
		case LLOAD: case LSTORE:
		case DLOAD: case DSTORE:
			if (wide == true) {
			        /* the WIDE is considered the beginning of the instruction */
				status[pc] ^= IS_INSTRUCTION;
				status[pc] |= WIDE_MODDED;
				
				pc++;
				wide = false;
				
				n = WORD(code, pc);
			}
			else {
				GET_IDX(n, pc);
			}
			
			/* makes sure the index given for a local variable is within the correct index
			 *
			 * REM: longs and doubles take two consecutive local spots
			 */
			CHECK_LOCAL_INDEX(n + 1);
			break;
			
			
		case IINC:
			if (wide == true) {
			        /* the WIDE is considered the beginning of the instruction */
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
			
			/* the next instruction is a target for branching via RET */
			pc = NEXTPC;
			BRANCH_IN_BOUNDS(pc, "jsr/ret");
			status[pc] |= START_BLOCK;
			continue;
			
		case RET:
			status[pc] |= END_BLOCK;
			if (!wide) {
				GET_IDX(idx, pc);
			} else {
				GET_WIDX(idx, pc);
				
				status[pc] ^= IS_INSTRUCTION;
				status[pc] |= WIDE_MODDED;
				
				wide = false;
				pc += 2;
			}
			CHECK_LOCAL_INDEX(idx);
			pc = NEXTPC;
			continue;
			
			
		case LOOKUPSWITCH:
			ENSURE_NON_WIDE;
			status[pc] |= END_BLOCK;
			
			/* default branch...between 0 and 3 bytes of padding are added so that the
			 * default branch is at an address that is divisible by 4
			 */
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
			
			/* get number of key/target pairs */
			n += 4;
			low = DWORD(code, n);
			if (low < 0) {
				return verifyErrorInVerifyMethod3a(einfo, method, "lookupswitch with npairs < 0");
			}
			
			/* make sure all targets are in bounds */
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
			
			/* default branch...between 0 and 3 bytes of padding are added so that the
			 * default branch is at an address that is divisible by 4
			 */
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
			
			/* get the high and low values of the table */
			low  = DWORD(code, n + 4);
			high = DWORD(code, n + 8);
			if (high < low) {
				DBG(VERIFY3, dprintf("ERROR: low = %d, high = %d\n", low, high); );
				return verifyErrorInVerifyMethod3a(einfo, method, "tableswitch high val < low val");
			}
			n += 12;
			
			/* high and low are used as temps in this loop that checks
			 * the validity of all the branches in the table
			 */
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
			
			
			/* the rest of the ways to end a block */
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
				return verifyErrorInVerifyMethod3a(einfo, method, "illegal instruction following wide instruction");
			}
		}
		
		pc = NEXTPC;
	}
	
	
	DBG(VERIFY3, dprintf("    Verifier Pass 3a: second pass to locate illegal branches and count blocks...\n"); );
	
	/* newpc is going to stand for the PC of the previous instruction */
	for (newpc = 0, pc = 0; pc < codelen; pc++) {
		if (status[pc] & IS_INSTRUCTION) {
			if (status[pc] & START_BLOCK) {
				blockCount++;
				
				if (newpc < pc) {
				        /* make sure that the previous instruction is
					 * marked as the end of a block (it would only
					 * have been marked so if it were some kind of
					 * branch).
					 */
					status[newpc] |= END_BLOCK;
				}
			}
			
			newpc = pc;
		}
		else if (status[pc] & START_BLOCK) {
			return verifyErrorInVerifyMethod3a(einfo, method, "branch into middle of instruction");
		}
	}
	
	
	DBG(VERIFY3, dprintf("        perusing exception table\n"); );
	if (method->exception_table != 0) {
		jexceptionEntry *entry;
		for (n = 0; n < method->exception_table->length; n++) {
			entry = &(method->exception_table->entry[n]);
			
			pc = entry->handler_pc;
			if (pc >= codelen) {
				return verifyErrorInVerifyMethod3a(einfo, method, "exception handler is beyond bound of method code");
			}
			else if (!(status[pc] & IS_INSTRUCTION)) {
				return verifyErrorInVerifyMethod3a(einfo, method, "exception handler starts in the middle of an instruction");
			}
			
			status[pc] |= (EXCEPTION_HANDLER & START_BLOCK);
			
			
			/* verify properties about the clause
			 *
			 * if entry->catch_type == 0, it's a finally clause
			 */
			if (entry->catch_type != 0) {
				if (entry->catch_type == NULL) {
					entry->catch_type = getClass(entry->catch_idx, method->class, einfo);
				}
				if (entry->catch_type == NULL) {
					DBG(VERIFY3, dprintf("        ERROR: could not resolve catch type...\n"); );
					entry->catch_type = UNRESOLVABLE_CATCHTYPE;
					
					return verifyErrorInVerifyMethod3a(einfo, method, "unresolvable catch type");
				}
				if (!instanceof(javaLangThrowable, entry->catch_type)) {
					return verifyErrorInVerifyMethod3a(einfo, method, "Exception to be handled by exception handler is not a subclass of Java/Lang/Throwable");
				}
			}
		}
	}
	
	
	DBG(VERIFY3, dprintf("        done, %d blocks found.\n", blockCount); );
	
	
	DBG(VERIFY3, dprintf("    Verifier Pass 3a: third pass to allocate memory for basic blocks...\n"); );
	
	blocks = checkPtr((BlockInfo**)gc_malloc(blockCount * sizeof(BlockInfo*), GC_ALLOC_VERIFIER));
	
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
	
	
	DBG(VERIFY3, dprintf("    Verifier Pass 3a: done\n"); );
	
	*numBlocks = blockCount;
	return blocks;
	
	
#undef CHECK_LOCAL_INDEX	
#undef NEXTPC
#undef BRANCH_IN_BOUNDS
#undef GET_IDX
#undef GET_WIDX
#undef CHECK_POOL_IDX
#undef ENSURE_NON_WIDE
}


/*
 * Helper function for error reporting in verifyMethod3b
 */
static inline 
bool
verifyErrorInVerifyMethod3b(errorInfo* einfo, const Method* method, BlockInfo* curBlock, const char * msg)
{
        KFREE(curBlock);
        if (einfo->type == 0) {
        	postExceptionMessage(einfo, JAVA_LANG(VerifyError),
				     "in method \"%s.%s\": %s",
				     CLASS_CNAME(method->class), METHOD_NAMED(method), msg);
	}
	return(false);
}

/*
 * verifyMethod3b()
 *    The Data-flow Analyzer
 *
 * The data-flow algorithm is taken from the JVM 2 spec, which describes it more or less as follows:
 *
 *  0  data-flow analyzer is initialised
 *       - for the first instruction of the method, the local variables that represent parameters
 *         initially contain values of the types indicated by the method's type descriptor.
 *       - the operand stack is empty.
 *       - all local variables contain an illegal value.
 *       - for the other instructions, which have not been examined yet, no information is available
 *         regarding the operand stack or local variables.
 *       - the "changed" bit is only set for the first instruction.
 *
 *  1  select a VM instruction whose "changed" bit is set
 *
 *       - if no such instruction remains, the method has successfully been verified.
 *       - otherwise, turn off the "changed" bit of the selected instruction.
 *
 *  2  model the effect of the instruction on the operand stack and local variable array by:
 *
 *       - if the instruction uses values from the operand stack, ensure that there are a
 *         sufficient number of values on the stack and that the top values on the stack are
 *         of an appropriate type.
 *       - if the instruction uses a local variable, ensure that the specified local variable
 *         contains a value of the appropriate type.
 *       - if the instruction pushes values onto the operand stack, ensure that there is sufficient
 *         room on the operand stack for the new values.  add the indicated types to the type of the
 *         modeled operand stack.
 *       - if the instruction modifies a local variable, record that the local variable now contains
 *         a type.
 *
 *  3  determine the instructions that can follow the current instruction.  successor instructions
 *     can be one of the following:
 *
 *       - the next instruction, if the current instruction is not an unconditional control tranfer
 *         instruction (ie - goto, return, or athrow).  basically check to make sure you don't
 *         "fall off" the last instruction of the method.
 *       - the target of a conditional or unconditional branch or switch.
 *       - any exception handlers for this instruction.
 *
 *  4  merge the state of the operand stack and local variable array at the end of the execution of the
 *     current instruction into each of the successor instructions.
 *
 *     (see merge function below)
 *
 *  5  continue at step 1.
 */
static
bool
verifyMethod3b(errorInfo* einfo, const Method* method,
	       const uint32* status,
	       BlockInfo** blocks, const uint32 numBlocks,
	       SigStack** sigs,
	       UninitializedType** uninits)
{
	const uint32 codelen      = METHOD_BYTECODE_LEN(method);
	const unsigned char* code = METHOD_BYTECODE_CODE(method);
	
	uint32 curIndex;
	BlockInfo* curBlock;
	BlockInfo* nextBlock;	

	uint32 pc = 0, newpc = 0, n = 0;
	int32 high = 0, low = 0;  /* for the switching instructions */
	
	
	
	DBG(VERIFY3, dprintf("    Verifier Pass 3b: Data Flow Analysis and Type Checking...\n"); );
	DBG(VERIFY3, dprintf("        memory allocation...\n"); );
	curBlock = createBlock(method);
	
	
	DBG(VERIFY3, dprintf("        doing the dirty data flow analysis...\n"); );
	blocks[0]->status |= CHANGED;
	curIndex = 0;
	while(curIndex < numBlocks) {
		DBG(VERIFY3,
		    dprintf("      blockNum/first pc/changed/stksz = %d / %d / %d / %d\n",
			    curIndex,
			    blocks[curIndex]->startAddr,
			    blocks[curIndex]->status & CHANGED,
			    blocks[curIndex]->stacksz);
		    dprintf("          before:\n");
		    printBlock(method, blocks[curIndex], "                 ");
		    );
		
		if (!(blocks[curIndex]->status & CHANGED)) {
			DBG(VERIFY3, dprintf("        not changed...skipping\n"); );
			curIndex++;
			continue;
		}
		
		blocks[curIndex]->status ^= CHANGED; /* unset CHANGED bit */
		blocks[curIndex]->status |= VISITED; /* make sure we've visited it...important for merging */
		copyBlockData(method, blocks[curIndex], curBlock);
		
		if (curBlock->status & EXCEPTION_HANDLER && curBlock->stacksz > 0) {
			return verifyErrorInVerifyMethod3b(einfo, method, curBlock, "it's possible to reach an exception handler with a nonempty stack");
		}
		
		
		if (!verifyBasicBlock(einfo, method, curBlock, sigs, uninits)) {
			return verifyErrorInVerifyMethod3b(einfo, method, curBlock, "failure to verify basic block");
		}
		
		
		DBG(VERIFY3, dprintf("          after:\n"); printBlock(method, curBlock, "                 "); );
		
		
		/*
		 * merge this block's information into the next block
		 */
		pc = curBlock->lastAddr;
		if (code[pc] == WIDE && code[pc + insnLen[code[pc]]] == RET)
			pc += insnLen[code[pc]];
		switch(code[pc])
			{
			case GOTO:
				newpc = pc + 1;
				newpc = pc + WORD(code, newpc);
				nextBlock = inWhichBlock(newpc, blocks, numBlocks);
				
				if (!merge(einfo, method, curBlock, nextBlock)) {
					return verifyErrorInVerifyMethod3b(einfo, method, curBlock, "error merging operand stacks");
				}
				break;
				
			case GOTO_W:
				newpc = pc + 1;
				newpc = pc + DWORD(code, newpc);
				nextBlock = inWhichBlock(newpc, blocks, numBlocks);
				
				if (!merge(einfo, method, curBlock, nextBlock)) {
					return verifyErrorInVerifyMethod3b(einfo, method, curBlock, "error merging operand stacks");
				}
				break;
					
			case JSR:
				newpc = pc + 1;
				newpc = pc + WORD(code, newpc);
				goto JSR_common;
			case JSR_W:
				newpc = pc + 1;
				newpc = pc + DWORD(code, newpc);
			JSR_common:
				nextBlock = inWhichBlock(newpc, blocks, numBlocks);
				
				if (!merge(einfo, method, curBlock, nextBlock)) {
					return verifyErrorInVerifyMethod3b(einfo, method, curBlock, "jsr: error merging operand stacks");
				}
	
				/*
				 * args, we need to verify the RET block first ...
				 */
				for (;curIndex<numBlocks && blocks[curIndex]!=nextBlock; curIndex++);
				assert (curIndex < numBlocks);
				continue;
				
			case RET:
				if (status[pc] & WIDE_MODDED) {
					n = pc + 1;
					n = WORD(code, n);
				} else {
					n = code[pc + 1];
				}
				
				if (!IS_ADDRESS(&curBlock->locals[n])) {
					return verifyErrorInVerifyMethod3b(einfo, method, curBlock, "ret instruction does not refer to a variable with type returnAddress");
				}
				
				newpc = curBlock->locals[n].tinfo;
				
				/* each instance of return address can only be used once */
				curBlock->locals[n] = *TUNSTABLE;
				
				nextBlock = inWhichBlock(newpc, blocks, numBlocks);
				if (!merge(einfo, method, curBlock, nextBlock)) {
					return verifyErrorInVerifyMethod3b(einfo, method, curBlock, "error merging opstacks when returning from a subroutine");
				}

				/* 
				 * unmark this block as visited, so that the next
				 * entry is treated as a first time merge.
				 */
				blocks[curIndex]->status ^= VISITED;
				break;	
				
			case IF_ACMPEQ:  case IFNONNULL:
			case IF_ACMPNE:  case IFNULL:
			case IF_ICMPEQ:  case IFEQ:
			case IF_ICMPNE:	 case IFNE:
			case IF_ICMPGT:	 case IFGT:
			case IF_ICMPGE:	 case IFGE:
			case IF_ICMPLT:	 case IFLT:
			case IF_ICMPLE:	 case IFLE:
				newpc     = pc + 1;
				newpc     = pc + WORD(code, newpc);
				nextBlock = inWhichBlock(newpc, blocks, numBlocks);
				
				if (!merge(einfo, method, curBlock, nextBlock)) {
					return verifyErrorInVerifyMethod3b(einfo, method, curBlock, "error merging operand stacks");
				}
				
				/* if the condition is false, then the next block is the one that will be executed */
				curIndex++;
				if (curIndex >= numBlocks) {
					return verifyErrorInVerifyMethod3b(einfo, method, curBlock, "execution falls off the end of a basic block");
				}
				else if (!merge(einfo, method, curBlock, blocks[curIndex])) {
					return verifyErrorInVerifyMethod3b(einfo, method, curBlock, "error merging operand stacks");
				}
				break;
				
				
			case LOOKUPSWITCH:
			        /* default branch...between 0 and 3 bytes of padding are added so that the
				 * default branch is at an address that is divisible by 4
				 */
				n = (pc + 1) % 4;
				if (n) n = pc + 5 - n;
				else   n = pc + 1;
				newpc = pc + DWORD(code, n);
				nextBlock = inWhichBlock(newpc, blocks, numBlocks);
				if (!merge(einfo, method, curBlock, nextBlock)) {
					return verifyErrorInVerifyMethod3b(einfo, method, curBlock, "error merging into the default branch of a lookupswitch instruction");
				}
				
				/* get number of key/target pairs */
				n += 4;
				low = DWORD(code, n);
				
				/* branch into all targets */
				for (n += 4, high = n + 8*low; n < high; n += 8) {
					newpc = pc + DWORD(code, n+4);
					nextBlock = inWhichBlock(newpc, blocks, numBlocks);
					if (!merge(einfo, method, curBlock, nextBlock)) {
						return verifyErrorInVerifyMethod3b(einfo, method, curBlock, "error merging into a branch of a lookupswitch instruction");
					}
				}
				
				break;
				
			case TABLESWITCH:
			        /* default branch...between 0 and 3 bytes of padding are added so that the
				 * default branch is at an address that is divisible by 4
				 */
				n = (pc + 1) % 4;
				if (n) n = pc + 5 - n;
				else   n = pc + 1;
				newpc = pc + DWORD(code, n);
				
				/* get the high and low values of the table */
				low  = DWORD(code, n + 4);
				high = DWORD(code, n + 8);
				
				n += 12;
				
				/* high and low are used as temps in this loop that checks
				 * the validity of all the branches in the table
				 */
				for (high = n + 4*(high - low + 1); n < high; n += 4) {
					newpc = pc + DWORD(code, n);
					nextBlock = inWhichBlock(newpc, blocks, numBlocks);
					if (!merge(einfo, method, curBlock, nextBlock)) {
						return verifyErrorInVerifyMethod3b(einfo, method, curBlock, "error merging into a branch of a tableswitch instruction");
					}
				}
				break;
				
				
				/* the rest of the ways to end a block */
			case RETURN:
			case ARETURN:
			case IRETURN:
			case FRETURN:
			case LRETURN:
			case DRETURN:
			case ATHROW:
				curIndex++;
				continue;
				
			default:
				for (n = pc + 1; n < codelen; n++) {
					if (status[n] & IS_INSTRUCTION) break;
				}
				if (n == codelen) {
					return verifyErrorInVerifyMethod3b(einfo, method, curBlock, "execution falls off the end of a code block");
				}
				else if (!merge(einfo, method, curBlock, blocks[curIndex+1])) {
					return verifyErrorInVerifyMethod3b(einfo, method, curBlock, "error merging operand stacks");
				}
			}
		
		
		for (curIndex = 0; curIndex < numBlocks; curIndex++) {
			if (blocks[curIndex]->status & CHANGED)
				break;
		}
	}
	
	
	DBG(VERIFY3, dprintf("    Verifier Pass 3b: Complete\n"); );
	KFREE(curBlock);
	return(true);
}


/*
 * Helper function for error reporting in merge.
 */
static inline
bool
verifyErrorInMerge(errorInfo* einfo,
		   const Method* method,
		   const char * msg)
{
        if (einfo->type == 0) {
        	postExceptionMessage(einfo, JAVA_LANG(VerifyError),
				     "in method \"%s.%s\": %s",
				     CLASS_CNAME(method->class), METHOD_NAMED(method), msg);
	}
	return(false);
}


/*
 * merges two operand stacks.  just to repeat what the JVML 2 spec says about this:
 *   Merge the state of the operand stack and local variable array at the end of the
 *   execution of the current instruction into each of the successor instructions.  In
 *   the special case of control transfer to an exception handler, the operand stack is
 *   set to contain a single object of the exception type indicated by the exception
 *   handler information.
 *     - if this if the first time the successor instruction has been visited, record
 *       that the operand stack and local variable values calculated in steps 2 and 3
 *       are the state of the operand stack and local variable array prior to executing
 *       the successor instruction.  Set the "changed" bit for the successor instruction.
 *     - if the successor instruction has been seen before, merge the operand stack and
 *       local variable values calculated in steps 2 and 3 into the values already there.
 *       set the "changed" bit if there is any modification to the values.
 *
 *   to merge two operand stacks, the number of values on each stack must be identical.
 *   the types of values on the stacks must also be identical, except that differently
 *   typed reference values may appear at corresponding places on the two stacks.  in this
 *   case, the merged operand stack contains a reference to an instance of the first common
 *   superclass of the two types.  such a reference type always exists because the type Object
 *   is a superclass of all class and interface types.  if the operand stacks cannot be merged,
 *   verification of the method fails.
 *
 *   to merge two local variable array states, corresponding pairs of local variables are
 *   compared.  if the two types are not identical, then unless both contain reference values,
 *   the verification records that the local variable contains an unusable value.  if both of
 *   the pair of local variables contain reference values, the merged state contains a reference
 *   to an instance of the first common superclass of the two types.
 */
static
bool
merge(errorInfo* einfo,
      const Method* method,
      BlockInfo* fromBlock,
      BlockInfo* toBlock)
{
	uint32 n;
	
	
	/* Ensure that no uninitiazed object instances are in the local variable array
	 * or on the operand stack during a backwards branch
	 */
	if (toBlock->startAddr < fromBlock->startAddr) {
		for (n = 0; n < method->localsz; n++) {
			if (fromBlock->locals[n].tinfo & TINFO_UNINIT) {
				return verifyErrorInMerge(einfo, method, "uninitialized object reference in a local variable during a backwards branch");
			}
		}
		for (n = 0; n < fromBlock->stacksz; n++) {
			if (fromBlock->opstack[n].tinfo & TINFO_UNINIT) {
				return verifyErrorInMerge(einfo, method, "uninitialized object reference on operand stack during a backwards branch");
			}
		}
	}
	
	if (!(toBlock->status & VISITED)) {
		DBG(VERIFY3, dprintf("          visiting block starting at %d for the first time\n",
				     toBlock->startAddr); );
		
		copyBlockState(method, fromBlock, toBlock);
		toBlock->status |= CHANGED;
		return(true);
	}
	
	DBG(VERIFY3,
	    dprintf("%snot a first time merge\n", indent);
	    dprintf("%s  from block (%d - %d):\n", indent, fromBlock->startAddr, fromBlock->lastAddr);
	    printBlock(method, fromBlock, indent2);
	    dprintf("%s  to block (%d - %d):\n", indent, toBlock->startAddr, toBlock->lastAddr);
	    printBlock(method, toBlock, indent2);
	    dprintf("\n");
	    );
	
	
	if (fromBlock->stacksz != toBlock->stacksz) {
		postExceptionMessage(einfo, JAVA_LANG(VerifyError),
				     "in method %s.%s: merging two operand stacks of unequal size",
				     METHOD_NAMED(method), CLASS_CNAME(method->class));
		return(false);
	}
	
	
	/* merge the local variable arrays */
	for (n = 0; n < method->localsz; n++) {
		if (mergeTypes(einfo, method->class,
			       &fromBlock->locals[n], &toBlock->locals[n])) {
			toBlock->status |= CHANGED;
		}
	}
	
	/* merge the operand stacks */
	for (n = 0; n < fromBlock->stacksz; n++) {
	        /* if we get unstable here, not really a big deal until we try to use it.
		 * i mean, we could get an unstable value and then immediately pop it off the stack,
		 * for instance.
		 */
		if (mergeTypes(einfo, method->class,
			       &fromBlock->opstack[n], &toBlock->opstack[n])) {
			toBlock->status |= CHANGED;
		}
	}
	
	
	DBG(VERIFY3,
	    dprintf("%s  result block:\n", indent);
	    printBlock(method, toBlock, indent2);
	    );
	
	
	return(true);
}

/*
 * Helper function for error reporting in verifyBasicBlock.
 */
static inline
bool
verifyErrorInVerifyBasicBlock(errorInfo* einfo,
			      const Method* method,
			      Hjava_lang_Class* this,
			      const char * msg)
{
	if (einfo->type == 0) {
		postExceptionMessage(einfo, JAVA_LANG(VerifyError),
				     "in method \"%s.%s\": %s",
				     CLASS_CNAME(this), METHOD_NAMED(method), msg);
	}
	return(false);
}

/*
 * Helper function for error reporting in ENSURE_LOCAL_TYPE macro in verifyBasicBlock.
 */
static inline
bool
ensureLocalTypeErrorInVerifyBasicBlock(errorInfo* einfo,
				       const Method* method,
				       BlockInfo* block,
				       Hjava_lang_Class* this,
				       unsigned int n)
{
	if (block->locals[n].data.class == TUNSTABLE->data.class) {
		return verifyErrorInVerifyBasicBlock(einfo, method, this, "attempt to access an unstable local variable");
	} else {
		return verifyErrorInVerifyBasicBlock(einfo, method, this, "attempt to access a local variable not of the correct type");
	}
}

/*
 * Helper function for error reporting in ENSURE_OPSTACK_SIZE macro in verifyBasicBlock.
 */
static inline
bool
ensureOpstackSizeErrorInVerifyBasicBlock(errorInfo* einfo,
					 const Method* method,
					 BlockInfo* block,
					 Hjava_lang_Class* this)
{
	DBG(VERIFY3,
	    dprintf("                here's the stack: \n");
	    printBlock(method, block, "                    ");
	    );
	return verifyErrorInVerifyBasicBlock(einfo, method, this, "not enough items on stack for operation");
}

/*
 * Helper function for error reporting in CHECK_STACK_OVERFLOW macro in verifyBasicBlock.
 */
static inline
bool
checkStackOverflowErrorInVerifyBasicBlock(errorInfo* einfo,
					  const Method* method,
					  BlockInfo* block,
					  Hjava_lang_Class* this,
					  unsigned int n)
{
	DBG(VERIFY3,
	    dprintf("                block->stacksz: %d :: N = %d :: method->stacksz = %d\n",
		    block->stacksz,
		    n,
		    method->stacksz);
	    );
                DBG(VERIFY3,
		    dprintf("                here's the stack: \n");
		    printBlock(method, block, "                    ");
		    );
		return verifyErrorInVerifyBasicBlock(einfo, method, this, "stack overflow");
}

/*
 * Helper function for opstack access in verifyBasicBlock.
 *
 * @return nth item on the operand stack from the top.
 */
static inline
Type *
getOpstackItem(BlockInfo* block,
	       unsigned int n)
{
	return (&block->opstack[block->stacksz - n]);
}

/*
 * Helper function for opstack access in verifyBasicBlock.
 *
 * @return first item on the operand stack from the top.
 */
static inline
Type *
getOpstackTop(BlockInfo* block)
{
	return getOpstackItem(block, 1);
}

/*
 * Helper function for opstack access in verifyBasicBlock.
 *
 * @return second item on the operand stack from the top.
 */
static inline
Type *
getOpstackWTop(BlockInfo* block)
{
	return getOpstackItem(block, 2);
}

/*
 * Helper function for opstack access in verifyBasicBlock.
 */
static inline
void
opstackPushBlind(BlockInfo* block,
		 const Type* type)
{
	block->opstack[block->stacksz++] = *(type);
}

/*
 * Helper function for opstack access in verifyBasicBlock.
 * only use for LONGs and DOUBLEs.
 */
static inline
void
opstackWPushBlind(BlockInfo* block,
		  const Type* type)
{
	opstackPushBlind(block, type);
	opstackPushBlind(block, TWIDE);
}

/*
 * Helper function for opstack access in verifyBasicBlock.
 */
static inline
void
opstackPopBlind(BlockInfo* block)
{
	block->stacksz--;
	block->opstack[block->stacksz] = *TUNSTABLE;
}

/*
 * Helper function for opstack access in verifyBasicBlock.
 */
static inline
void
opstackWPopBlind(BlockInfo* block)
{
	opstackPopBlind(block);
	opstackPopBlind(block);
}

/*
 * Helper function for opstack access in verifyBasicBlock.
 * pop _N things off the stack off the stack.
 */
static inline
void
opstackPopNBlind(BlockInfo* block,
		 unsigned int n)
{
	unsigned int i;

	for (i = 0; i < n; ++i) {
		opstackPopBlind(block);
	}
}

/*
 * Helper function for error reporting in OPSTACK_PEEK_T_BLIND macro in verifyBasicBlock.
 */
static inline
bool
opstackPeekTBlindErrorInVerifyBasicBlock(errorInfo* einfo,
					 const Method* method,
					 BlockInfo* block,
					 Hjava_lang_Class* this,
					 const Type* type)
{
	DBG(VERIFY3,
	    dprintf("                OPSTACK_TOP: ");
	    printType(getOpstackTop(block));
	    dprintf(" vs. what's we wanted: ");
	    printType(type);
	    dprintf("\n");
	    );
	return verifyErrorInVerifyBasicBlock(einfo, method, this, "top of opstack does not have desired type");
}

/*
 * verifyBasicBlock()
 *   Simulates execution of a basic block by modifying its simulated operand stack and local variable array.
 */
static
bool
verifyBasicBlock(errorInfo* einfo,
		 const Method* method,
		 BlockInfo* block,
		 SigStack** sigs,
		 UninitializedType** uninits)
{
	/**************************************************************************************************
	 * VARIABLES
	 **************************************************************************************************/
	uint32            pc   = 0;
	unsigned char*    code = METHOD_BYTECODE_CODE(method);
	Hjava_lang_Class* this = method->class;
	
	bool wide = false;       /* was the previous opcode a WIDE instruction? */
	
	uint32 n = 0;            /* used as a general temporary variable, often as a temporary pc */
	
	Type* type = NULL;
	Type* arrayType = NULL;
	Hjava_lang_Class* class; /* for when we need a pointer to an actual class */
	
	/* for the rare occasions when we actually need a Type */
	Type  tt;
	Type* t = &tt;
	
	int tag;                 /* used for constant tag stuff */
	
	uint32     idx;          /* index into constant pool */
	constants* pool = CLASS_CONSTANTS(method->class);
	
	const char* sig;
	
	
	/**************************************************************************************************
	 * HANDY MACROS USED ONLY IN THIS METHOD
	 *    most of these belong to one of two categories:
	 *         - those dealing with locals variables
	 *         - those dealing with the operand stack
	 **************************************************************************************************/

#define GET_IDX \
	idx = code[pc + 1]
	
#define GET_WIDX \
	idx = pc + 1; idx = WORD(code, idx)
	
	
	/* checks whether the specified local variable is of the specified type. */
#define ENSURE_LOCAL_TYPE(_N, _TINFO) \
	if (!typecheck(einfo, this, (_TINFO), &block->locals[_N])) { \
		return ensureLocalTypeErrorInVerifyBasicBlock(einfo, method, block, this, _N); \
	} 
	
	/* only use with TLONG and TDOUBLE */
#define ENSURE_LOCAL_WTYPE(_N, _TINFO) \
	if (block->locals[_N].data.class != (_TINFO)->data.class) { \
		return verifyErrorInVerifyBasicBlock(einfo, method, this, "local variable not of correct type"); \
	} \
	else if (block->locals[_N + 1].data.class != TWIDE->data.class) { \
		return verifyErrorInVerifyBasicBlock(einfo, method, this, "accessing a long or double in a local where the following local has been corrupted"); \
	}

	
#define ENSURE_OPSTACK_SIZE(_N) \
	if (block->stacksz < (_N)) { \
		return ensureOpstackSizeErrorInVerifyBasicBlock(einfo, method, block, this); \
	}

#define CHECK_STACK_OVERFLOW(_N) \
	if (block->stacksz + _N > method->stacksz) { \
		return checkStackOverflowErrorInVerifyBasicBlock(einfo, method, block, this, _N); \
	}
	
#define OPSTACK_PUSH(_TINFO) \
	CHECK_STACK_OVERFLOW(1); \
	opstackPushBlind(block, _TINFO)

#define OPSTACK_WPUSH(_T) \
	CHECK_STACK_OVERFLOW(2); \
        opstackWPushBlind(block, _T)
	
	
	
	/* ensure that the top item on the stack is of type _T	*/
#define OPSTACK_PEEK_T_BLIND(_TINFO) \
	if (!typecheck(einfo, this, _TINFO, getOpstackTop(block))) { \
		return opstackPeekTBlindErrorInVerifyBasicBlock(einfo, method, block, this, _TINFO); \
	}
	
#define OPSTACK_PEEK_T(_TINFO) \
        ENSURE_OPSTACK_SIZE(1); \
	OPSTACK_PEEK_T_BLIND(_TINFO)
	
	/* ensure that the top item on the stack is of wide type _T
	 * this only works with doubles and longs
	 */
#define OPSTACK_WPEEK_T_BLIND(_TINFO) \
	if (getOpstackTop(block)->data.class != TWIDE->data.class) { \
		return verifyErrorInVerifyBasicBlock(einfo, method, this, "trying to pop a wide value off operand stack where there is none"); \
	} else if (getOpstackWTop(block)->data.class != (_TINFO)->data.class) { \
		return verifyErrorInVerifyBasicBlock(einfo, method, this, "mismatched stack types"); \
	}
	
#define OPSTACK_WPEEK_T(_TINFO) \
	ENSURE_OPSTACK_SIZE(2); \
	OPSTACK_WPEEK_T_BLIND(_TINFO)
	
#define OPSTACK_POP \
        ENSURE_OPSTACK_SIZE(1); \
	opstackPopBlind(block)

	/* pop a type off the stack and typecheck it */
#define OPSTACK_POP_T_BLIND(_TINFO) \
	OPSTACK_PEEK_T_BLIND(_TINFO); \
	opstackPopBlind(block)

#define OPSTACK_POP_T(_TINFO) \
	OPSTACK_PEEK_T(_TINFO); \
        opstackPopBlind(block)

#define OPSTACK_WPOP \
	ENSURE_OPSTACK_SIZE(2); \
	opstackWPopBlind(block)

	/* pop a wide type off the stack and typecheck it */
#define OPSTACK_WPOP_T_BLIND(_TINFO) \
	OPSTACK_WPEEK_T_BLIND(_TINFO); \
	opstackWPopBlind(block)

#define OPSTACK_WPOP_T(_TINFO) \
        OPSTACK_WPEEK_T(_TINFO); \
	opstackWPopBlind(block)
	
#define OPSTACK_POP_N(_N) \
        ENSURE_OPSTACK_SIZE(_N); \
	opstackPopNBlind(block, _N)
	
	
	
	/**************************************************************************************************
	 * BLOCK-LEVEL DATA FLOW ANALYASIS
	 *    this is actually pretty easy, since there are never any branches.  basically, it just
	 *    manipulates the working stack after every instruction as if it were actually running the
	 *    code so that, after verifying the block, the working block can be used to merge this block
	 *    with its successors.
	 **************************************************************************************************/
	DBG(VERIFY3,
	    dprintf("        about to verify the block...\n");
	    dprintf("        block->startAddr = %d, block->lastAddr = %d, first instruction = %d\n",
		    block->startAddr, block->lastAddr, code[block->startAddr]);
	    );
	
	pc = block->startAddr;
	while (pc <= block->lastAddr) {
		DBG(VERIFY3,
		    dprintf("            pc = %d, opcode = %d == ", pc, code[pc]);
		    printInstruction(code[pc]);
		    dprintf("\n");
		    );
		
		switch(code[pc]) {
			/**************************************************************
			 * INSTRUCTIONS FOR PUSHING CONSTANTS ONTO THE STACK
			 **************************************************************/
			/* pushes NULL onto the stack, which matches any object */
		case ACONST_NULL:
			OPSTACK_PUSH(TNULL);
			break;
			
			/* iconst_<n> pushes n onto the stack */
		case ICONST_0: case ICONST_1: case ICONST_2:
		case ICONST_3: case ICONST_4: case ICONST_5:
			
		case ICONST_M1: /* pushes -1 onto the stack */
		case BIPUSH:    /* sign extends an 8-bit int to 32-bits and pushes it onto stack */
		case SIPUSH:    /* sign extends a 16-bit int to 32-bits and pushes it onto stack */
			OPSTACK_PUSH(TINT);
			break;
			
		case FCONST_0:
		case FCONST_1:
		case FCONST_2:
			OPSTACK_PUSH(TFLOAT);
			break;
			
		case LCONST_0:
		case LCONST_1:
			OPSTACK_WPUSH(TLONG);
			break;
			
		case DCONST_0:
		case DCONST_1:
			OPSTACK_WPUSH(TDOUBLE);
			break;
			
			
		case LDC1:
			GET_IDX;
			goto LDC_common;
		case LDC2:
			GET_WIDX;
		LDC_common:
			tag = CONST_TAG(idx, pool);
			switch(tag) {
			case CONSTANT_Integer: OPSTACK_PUSH(TINT);    break;
			case CONSTANT_Float:   OPSTACK_PUSH(TFLOAT);  break;
			case CONSTANT_ResolvedString:
			case CONSTANT_String:
			        /* we do this because we might be loading a class before
				 * loading String
				 */
				OPSTACK_PUSH(TSTRING);
				break;
			}
			break;
			
		case LDC2W:
			GET_WIDX;
			tag = CONST_TAG(idx, pool);
			if (tag == CONSTANT_Long) {
				OPSTACK_WPUSH(TLONG);
			} else {
				OPSTACK_WPUSH(TDOUBLE);
			}
			break;
			
			
			/**************************************************************
			 * INSTRUCTIONS DEALING WITH THE LOCALS AND STACK
			 **************************************************************/
		case POP:
			OPSTACK_POP;
			break;
		case POP2:
			OPSTACK_WPOP;
			break;
			
			
#define GET_CONST_INDEX \
			if (wide == true) { GET_WIDX; } \
			else              { GET_IDX;  }
			
			
			/* aload_<n> takes the object reference in location <n> and pushes it onto the stack */
		case ALOAD_0: idx = 0; goto ALOAD_common;
		case ALOAD_1: idx = 1; goto ALOAD_common;
		case ALOAD_2: idx = 2; goto ALOAD_common;
		case ALOAD_3: idx = 3; goto ALOAD_common;
		case ALOAD:
			GET_CONST_INDEX;
		ALOAD_common:
			if (!isReference(&block->locals[idx])) {
				DBG(VERIFY3, dprintf("%sERROR: ", indent); printType(&block->locals[idx]); dprintf("\n"); );
				return verifyErrorInVerifyBasicBlock(einfo, method, this, "aload<_n> where local variable does not contain an object reference");
			}
			
			OPSTACK_PUSH(&block->locals[idx]);
			break;
			
			
			/* stores whatever's on the top of the stack in local <n> */
		case ASTORE_0: idx = 0; goto ASTORE_common;
		case ASTORE_1: idx = 1; goto ASTORE_common;
		case ASTORE_2: idx = 2; goto ASTORE_common;
		case ASTORE_3: idx = 3; goto ASTORE_common;
		case ASTORE:
			GET_CONST_INDEX;
		ASTORE_common:
			ENSURE_OPSTACK_SIZE(1);
			type = getOpstackTop(block);
			
			if (!IS_ADDRESS(type) && !isReference(type)) {
				return verifyErrorInVerifyBasicBlock(einfo, method, this, "astore: top of stack is not a return address or reference type");
			}
			
			block->locals[idx] = *type;
			opstackPopBlind(block);
			break;
			
			
			
			/* iload_<n> takes the variable in location <n> and pushes it onto the stack */
		case ILOAD_0: idx = 0; goto ILOAD_common;
		case ILOAD_1: idx = 1; goto ILOAD_common;
		case ILOAD_2: idx = 2; goto ILOAD_common;
		case ILOAD_3: idx = 3; goto ILOAD_common;
		case ILOAD:
			GET_CONST_INDEX;
		ILOAD_common:
			ENSURE_LOCAL_TYPE(idx, TINT);
			OPSTACK_PUSH(TINT);
			break;
			
			
		case ISTORE_0: idx =0; goto ISTORE_common;
		case ISTORE_1: idx =1; goto ISTORE_common;
		case ISTORE_2: idx =2; goto ISTORE_common;
		case ISTORE_3: idx =3; goto ISTORE_common;
		case ISTORE:
			GET_CONST_INDEX;
		ISTORE_common:
			OPSTACK_POP_T(TINT);
			block->locals[idx] = *TINT;
			break;
			
			
			/* fload_<n> takes the variable at location <n> and pushes it onto the stack */
		case FLOAD_0: idx =0; goto FLOAD_common;
		case FLOAD_1: idx =1; goto FLOAD_common;
		case FLOAD_2: idx =2; goto FLOAD_common;
		case FLOAD_3: idx = 3; goto FLOAD_common;
		case FLOAD:
			GET_CONST_INDEX;
		FLOAD_common:
			ENSURE_LOCAL_TYPE(idx, TFLOAT);
			OPSTACK_PUSH(TFLOAT);
			break;
			
			
			/* stores a float from top of stack into local <n> */
		case FSTORE_0: idx = 0; goto FSTORE_common;
		case FSTORE_1: idx = 1; goto FSTORE_common;
		case FSTORE_2: idx = 2; goto FSTORE_common;
		case FSTORE_3: idx = 3; goto FSTORE_common;
		case FSTORE:
			GET_CONST_INDEX;
		FSTORE_common:
			OPSTACK_POP_T(TFLOAT);
			block->locals[idx] = *TFLOAT;
			break;
			
			
			/* lload_<n> takes the variable at location <n> and pushes it onto the stack */
		case LLOAD_0: idx = 0; goto LLOAD_common;
		case LLOAD_1: idx = 1; goto LLOAD_common;
		case LLOAD_2: idx = 2; goto LLOAD_common;
		case LLOAD_3: idx = 3; goto LLOAD_common;
		case LLOAD:
			GET_CONST_INDEX;
		LLOAD_common:
			ENSURE_LOCAL_WTYPE(idx, TLONG);
			OPSTACK_WPUSH(TLONG);
			break;
			
			
			/* lstore_<n> stores a long from top of stack into local <n> */
		case LSTORE_0: idx = 0; goto LSTORE_common;
		case LSTORE_1: idx = 1; goto LSTORE_common;
		case LSTORE_2: idx = 2; goto LSTORE_common;
		case LSTORE_3: idx = 3; goto LSTORE_common;
		case LSTORE:
			GET_CONST_INDEX;
		LSTORE_common:
			OPSTACK_WPOP_T(TLONG);
			block->locals[idx] = *TLONG;
			block->locals[idx + 1] = *TWIDE;
			break;
			
			
			/* dload_<n> takes the double at local <n> and pushes it onto the stack */
		case DLOAD_0: idx = 0; goto DLOAD_common;
		case DLOAD_1: idx = 1; goto DLOAD_common;
		case DLOAD_2: idx = 2; goto DLOAD_common;
		case DLOAD_3: idx = 3; goto DLOAD_common;
		case DLOAD:
			GET_CONST_INDEX;
		DLOAD_common:
			ENSURE_LOCAL_WTYPE(idx, TDOUBLE);
			OPSTACK_WPUSH(TDOUBLE);
			break;
			
			
			/* dstore stores a double from the top of stack into a local variable */
		case DSTORE_0: idx = 0; goto DSTORE_common;
		case DSTORE_1: idx = 1; goto DSTORE_common;
		case DSTORE_2: idx = 2; goto DSTORE_common;
		case DSTORE_3: idx = 3; goto DSTORE_common;
		case DSTORE:
			GET_CONST_INDEX;
		DSTORE_common:
			OPSTACK_WPOP_T(TDOUBLE);
			block->locals[idx] = *TDOUBLE;
			block->locals[idx + 1] = *TWIDE;
			break;
			
			
#undef GET_CONST_INDEX
			/**************************************************************
			 * ARRAY INSTRUCTIONS!
			 **************************************************************/
			/* i put ANEWARRAY code by NEW instead of in the array instructions
			 * section because of similarities with NEW
			
			 * for creating a primitive array
			 */
		case NEWARRAY:
		        OPSTACK_POP_T(TINT);   /* array size */
			
			switch(code[pc + 1]) {
			case TYPE_Boolean: OPSTACK_PUSH(TBOOLARR);   break;
			case TYPE_Char:    OPSTACK_PUSH(TCHARARR);   break;
			case TYPE_Float:   OPSTACK_PUSH(TFLOATARR);  break;
			case TYPE_Double:  OPSTACK_PUSH(TDOUBLEARR); break;
			case TYPE_Byte:    OPSTACK_PUSH(TBYTEARR);   break;
			case TYPE_Short:   OPSTACK_PUSH(TSHORTARR);  break;
			case TYPE_Int:     OPSTACK_PUSH(TINTARR);    break;
			case TYPE_Long:    OPSTACK_PUSH(TLONGARR);   break;
			default: return verifyErrorInVerifyBasicBlock(einfo, method, this, "newarray of unknown type");
			}
			break;
			
		case ARRAYLENGTH:
			ENSURE_OPSTACK_SIZE(1);
			
			type = getOpstackTop(block);
			if (!isArray(type)) {
				DBG(VERIFY3, dprintf("%stype = ", indent); printType(type); dprintf("\n"); );
				return verifyErrorInVerifyBasicBlock(einfo, method, this, "arraylength: top of operand stack is not an array");
			}
			
			*type = *TINT;
			break;
			
			
#define ARRAY_LOAD(_T, _ARRT) \
                                OPSTACK_POP_T(TINT); \
                                OPSTACK_POP_T(_ARRT); \
				OPSTACK_PUSH(_T);

#define ARRAY_WLOAD(_T, _ARRT) \
                                OPSTACK_POP_T(TINT); \
                                OPSTACK_POP_T(_ARRT); \
				OPSTACK_WPUSH(_T);
			
			
		case AALOAD:
			ENSURE_OPSTACK_SIZE(2);
			
			if (getOpstackTop(block)->data.class != TINT->data.class) {
				return verifyErrorInVerifyBasicBlock(einfo, method, this, "aaload: item on top of stack is not an integer");
			}
			opstackPopBlind(block);
			
			type = getOpstackTop(block);
			if (!isArray(type)) {
				DBG(VERIFY3, dprintf("%serror: type = ", indent); printType(type); dprintf("\n"); );
				return verifyErrorInVerifyBasicBlock(einfo, method, this, "aaload: top of operand stack is not an array");
			}
			
			if (type->tinfo & TINFO_NAME || type->tinfo & TINFO_SIG) {
				type->tinfo = TINFO_SIG;
				(type->data.sig)++;
			}
			else if (type->data.class != TNULL->data.class) {
				type->tinfo = TINFO_SIG;
				type->data.sig = CLASS_CNAME(type->data.class) + 1;
			}
			DBG(VERIFY3, dprintf("%sarray type: ", indent); printType(type); dprintf("\n"); );
			break;
			
		case IALOAD: ARRAY_LOAD(TINT,   TINTARR);   break;
		case FALOAD: ARRAY_LOAD(TFLOAT, TFLOATARR); break;
		case CALOAD: ARRAY_LOAD(TINT,   TCHARARR);  break;
		case SALOAD: ARRAY_LOAD(TINT,   TSHORTARR); break;
			
		case LALOAD: ARRAY_WLOAD(TLONG,   TLONGARR);   break;
		case DALOAD: ARRAY_WLOAD(TDOUBLE, TDOUBLEARR); break;
#undef ARRAY_LOAD
#undef ARRAY_WLOAD

		case BALOAD:
			/* BALOAD can be used for bytes or booleans .... */
			OPSTACK_POP_T(TINT);

			if (!typecheck (einfo, this, TBYTEARR, getOpstackTop(block)) &&
			    !typecheck (einfo, this, TBOOLARR, getOpstackTop(block))) {
                                DBG(VERIFY3,
                                    dprintf("                OPSTACK_TOP: ");
                                    printType(getOpstackTop(block));
                                    dprintf(" vs. what's we wanted: TBYTEARR or TBOOLARR"); )
                                return verifyErrorInVerifyBasicBlock(einfo, method, this, "top of opstack does not have desired type");
			}

			opstackPopBlind(block);
			OPSTACK_PUSH(TINT);
			break;


		case AASTORE:
		        /* the runtime value of the type on the top of the stack must be
			 * assignment compatible with the type of the array
			 */
			ENSURE_OPSTACK_SIZE(3);
			
			if (getOpstackItem(block, 2)->data.class != TINT->data.class) {
				return verifyErrorInVerifyBasicBlock(einfo, method, this, "aastore: array index is not an integer");
			}
			
			type      = getOpstackItem(block, 1);
			arrayType = getOpstackItem(block, 3);
			
			DBG(VERIFY3,
			    dprintf("%sarrayType: ", indent); printType(arrayType);
			    dprintf(" vs. type: "); printType(type);
			    dprintf("\n");
			    );
			
			if (!isArray(arrayType)) {
				DBG(VERIFY3, dprintf("%serror: type = ", indent); printType(type); dprintf("\n"); );
				return verifyErrorInVerifyBasicBlock(einfo, method, this, "aastore: top of operand stack is not an array");
			}
			
			if (arrayType->tinfo & TINFO_NAME || arrayType->tinfo & TINFO_SIG) {
				arrayType->tinfo = TINFO_SIG;
				(arrayType->data.sig)++;
			}
			else {
				if (arrayType->data.class == TOBJARR->data.class) {
					*arrayType = *TOBJ;
				} else if (arrayType->data.class != TNULL->data.class) {
					arrayType->tinfo = TINFO_SIG;
					arrayType->data.sig = CLASS_CNAME(arrayType->data.class) + 1;
				}
			}
			
			if (!typecheck(einfo, this, arrayType, type)) {
				return verifyErrorInVerifyBasicBlock(einfo, method, this, "attempting to store incompatible type in array");
			}
			
			opstackPopNBlind(block, 3);
			break;

#define ARRAY_STORE(_T, _ARRT) \
				OPSTACK_POP_T(_T); \
				OPSTACK_POP_T(TINT); \
				OPSTACK_POP_T(_ARRT);
			
#define ARRAY_WSTORE(_T, _ARRT) \
				OPSTACK_WPOP_T(_T); \
				OPSTACK_POP_T(TINT); \
				OPSTACK_POP_T(_ARRT);
			
			
			
			
		case IASTORE: ARRAY_STORE(TINT,   TINTARR);   break;
		case FASTORE: ARRAY_STORE(TFLOAT, TFLOATARR); break;
		case CASTORE: ARRAY_STORE(TINT,   TCHARARR);  break;
		case SASTORE: ARRAY_STORE(TINT,   TSHORTARR); break;
			
		case LASTORE: ARRAY_WSTORE(TLONG,   TLONGARR);   break;
		case DASTORE: ARRAY_WSTORE(TDOUBLE, TDOUBLEARR); break;
#undef ARRAY_STORE
#undef ARRAY_WSTORE

		case BASTORE: 
			/* BASTORE can store either bytes or booleans .... */
			OPSTACK_POP_T(TINT);
			OPSTACK_POP_T(TINT);

			if ( !typecheck(einfo, this, TBYTEARR, getOpstackTop(block)) &&
			     !typecheck(einfo, this, TBOOLARR, getOpstackTop(block))) {
				DBG(VERIFY3,
				    dprintf("                OPSTACK_TOP: ");
				    printType(getOpstackTop(block));
				    dprintf(" vs. what's we wanted: TBYTEARR or TBOOLARR"); )
				return verifyErrorInVerifyBasicBlock(einfo, method, this, "top of opstack does not have desired type");
			}
			opstackPopBlind(block);
			break;			
			
			
			/**************************************************************
			 * ARITHMETIC INSTRUCTIONS
			 **************************************************************/
		case IAND: case IOR:  case IXOR:
		case IADD: case ISUB: case IMUL: case IDIV: case IREM:
		case ISHL: case ISHR: case IUSHR:
			OPSTACK_POP_T(TINT);
			break;
		case INEG:
			OPSTACK_PEEK_T(TINT);
			break;
			
			
		case LAND: case LOR:  case LXOR:
		case LADD: case LSUB: case LMUL: case LDIV: case LREM:
			OPSTACK_WPOP_T(TLONG);
			break;
		case LNEG:
			OPSTACK_WPEEK_T(TLONG);
			break;
			
		case LSHL: case LSHR: case LUSHR:
			OPSTACK_POP_T(TINT);
			OPSTACK_WPEEK_T(TLONG);
			break;
			
			
		case FADD: case FSUB: case FMUL: case FDIV: case FREM:
			OPSTACK_POP_T(TFLOAT);
			break;
		case FNEG:
			OPSTACK_PEEK_T(TFLOAT);
			break;
			
			
		case DADD: case DSUB: case DDIV: case DMUL: case DREM:
			OPSTACK_WPOP_T(TDOUBLE);
			break;
		case DNEG:
			OPSTACK_WPEEK_T(TDOUBLE);
			break;
			
			
		case LCMP:
			OPSTACK_WPOP_T(TLONG);
			OPSTACK_WPOP_T(TLONG);
			opstackPushBlind(block, TINT);
			break;
			
		case FCMPG:
		case FCMPL:
			OPSTACK_POP_T(TFLOAT);
			OPSTACK_POP_T(TFLOAT);
			opstackPushBlind(block, TINT);
			break;
				
		case DCMPG:
		case DCMPL:
			OPSTACK_WPOP_T(TDOUBLE);
			OPSTACK_WPOP_T(TDOUBLE);
			opstackPushBlind(block, TINT);
			break;
			
			
		case IINC:
			if (wide == true) { GET_WIDX; }
			else              { GET_IDX; }
			
			ENSURE_LOCAL_TYPE(idx, TINT);
			
			pc += insnLen[code[pc]];
			if (wide == true) {
				pc += 2;
				wide = false;
			}
			continue;
			
			
			/**************************************************************
			 * PRIMITIVE CONVERSION STUFF
			 **************************************************************/
		case INT2BYTE:
		case INT2CHAR:
		case INT2SHORT:
			OPSTACK_PEEK_T(TINT);
			break;
			
		case I2F:
			OPSTACK_POP_T(TINT);
			opstackPushBlind(block, TFLOAT);
			break;
		case I2L:
			OPSTACK_POP_T(TINT);
			CHECK_STACK_OVERFLOW(2);
			opstackWPushBlind(block, TLONG);
			break;
		case I2D:
			OPSTACK_POP_T(TINT);
			CHECK_STACK_OVERFLOW(2);
			opstackWPushBlind(block, TDOUBLE);
			break;
			
		case F2I:
			OPSTACK_POP_T(TFLOAT);
			opstackPushBlind(block, TINT);
			break;
		case F2L:
			OPSTACK_POP_T(TFLOAT);
			OPSTACK_WPUSH(TLONG);
			break;
		case F2D:
			OPSTACK_POP_T(TFLOAT);
			OPSTACK_WPUSH(TDOUBLE);
			break;
			
		case L2I:
			OPSTACK_WPOP_T(TLONG);
			opstackPushBlind(block, TINT);
			break;
		case L2F:
			OPSTACK_WPOP_T(TLONG);
			opstackPushBlind(block, TFLOAT);
			break;
		case L2D:
			OPSTACK_WPOP_T(TLONG);
			opstackWPushBlind(block, TDOUBLE);
			break;
			
		case D2I:
			OPSTACK_WPOP_T(TDOUBLE);
			opstackPushBlind(block, TINT);
			break;
		case D2F:
			OPSTACK_WPOP_T(TDOUBLE);
			opstackPushBlind(block, TFLOAT);
			break;
		case D2L:
			OPSTACK_WPOP_T(TDOUBLE);
			opstackWPushBlind(block, TLONG);
			break;
			
			
			
			/**************************************************************
			 * OBJECT CREATION/TYPE CHECKING
			 **************************************************************/
		case INSTANCEOF:
			ENSURE_OPSTACK_SIZE(1);
			if (!isReference(getOpstackItem(block, 1))) {
				return verifyErrorInVerifyBasicBlock(einfo, method, this, "instanceof: top of stack is not a reference type");
			}
			*getOpstackTop(block) = *TINT;
			break;
			
		case CHECKCAST:
			ENSURE_OPSTACK_SIZE(1);
			opstackPopBlind(block);
			goto NEW_COMMON;
			
		case MULTIANEWARRAY:
			n = code[pc + 3];
			ENSURE_OPSTACK_SIZE(n);
			while (n > 0) {
				if (getOpstackTop(block)->data.class != TINT->data.class) {
					return verifyErrorInVerifyBasicBlock(einfo, method, this, "multinewarray: first <n> things on opstack must be integers");
				}
				opstackPopBlind(block);
				n--;
			}
			goto NEW_COMMON;
			
		NEW_COMMON:
			GET_WIDX;
			
			CHECK_STACK_OVERFLOW(1);
			block->stacksz++;
			type = getOpstackTop(block);
			
			if (pool->tags[idx] == CONSTANT_ResolvedClass) {
				type->tinfo = TINFO_CLASS;
				type->data.class = CLASS_CLASS(idx, pool);
			} else {
				const char* namestr;
				
				namestr = CLASS_NAMED(idx, pool);
				
				if (*namestr == '[') {
					type->tinfo = TINFO_SIG;
					type->data.sig = namestr;
				} else {
					type->tinfo = TINFO_NAME;
					type->data.sig = namestr;
				}
			}
			
			DBG(VERIFY3,
			    dprintf("%s", indent);
			    printType(getOpstackTop(block));
			    dprintf("\n"); );
			break;
			
		case NEW:
			GET_WIDX;
			
			CHECK_STACK_OVERFLOW(1);
			block->stacksz++;
			type = getOpstackTop(block);
			if (pool->tags[idx] == CONSTANT_ResolvedClass) {
				type->tinfo = TINFO_CLASS;
				type->data.class = CLASS_CLASS(idx, pool);
			} else {
				const char* namestr = CLASS_NAMED(idx, pool);
				
				if (*namestr == '[') {
					return verifyErrorInVerifyBasicBlock(einfo, method, this, "new: used to create an array");
				}
				
				type->tinfo = TINFO_NAME;				
				type->data.name = namestr;
			}
			
			*uninits = pushUninit(*uninits, type);
			type->tinfo = TINFO_UNINIT;
			type->data.uninit  = *uninits;
			
			DBG(VERIFY3,
			    dprintf("%s", indent);
			    printType(getOpstackTop(block));
			    dprintf("\n"); );
			break;
			
			
		case ANEWARRAY:
			GET_WIDX;
			OPSTACK_PEEK_T(TINT);
			
			type = getOpstackTop(block);
			if (pool->tags[idx] == CONSTANT_ResolvedClass) {
				class = CLASS_CLASS(idx, pool);
				type->tinfo = TINFO_CLASS;
				type->data.class  = lookupArray(class, einfo);
				
				if (type->data.class == NULL) {
					return verifyErrorInVerifyBasicBlock(einfo, method, this, "anewarray: error creating array type");
				}
			} else {
				char* namestr;
				
				sig = CLASS_NAMED(idx, pool);
				if (*sig == '[') {
					namestr = checkPtr(gc_malloc(sizeof(char) * (strlen(sig) + 2), GC_ALLOC_VERIFIER));
					*sigs = pushSig(*sigs, namestr);
					sprintf(namestr, "[%s", sig);
				} else {
					namestr = checkPtr(gc_malloc(sizeof(char) * (strlen(sig) + 4), GC_ALLOC_VERIFIER));
					*sigs = pushSig(*sigs, namestr);
					sprintf(namestr, "[L%s;", sig);
				}
				
				type->tinfo = TINFO_SIG;
				type->data.sig  = namestr;
			}
			
			DBG(VERIFY3,
			    dprintf("%s", indent);
			    printType(getOpstackTop(block));
			    dprintf("\n"); );
			break;
			
			
		case GETFIELD:
			ENSURE_OPSTACK_SIZE(1);
			if (!checkUninit(this, getOpstackTop(block))) {
				return verifyErrorInVerifyBasicBlock(einfo, method, this, "getfield: uninitialized type on top of operand stack");
			}
			
			GET_WIDX;
			n = FIELDREF_CLASS(idx, pool);
			
			if (pool->tags[n] == CONSTANT_ResolvedClass) {
				t->tinfo = TINFO_CLASS;
				t->data.class = CLASS_CLASS(n, pool);
			} else {
				t->tinfo = TINFO_NAME;
				t->data.name = CLASS_NAMED(n, pool);
			}
			
			OPSTACK_POP_T_BLIND(t);
			goto GET_COMMON;
			
		case GETSTATIC:
			GET_WIDX;
			CHECK_STACK_OVERFLOW(1);
		GET_COMMON:
			sig = FIELDREF_SIGD(idx, pool);
			
			DBG(VERIFY3, dprintf("%sfield type: %s\n", indent, sig); );
			
			/* TODO: we should just have a function that returns a type based on a signature */
			switch (*sig) {
			case 'I': case 'Z': case 'S': case 'B': case 'C':
				opstackPushBlind(block, TINT);
				break;
				
			case 'F': opstackPushBlind(block, TFLOAT); break;
			case 'J': OPSTACK_WPUSH(TLONG); break;
			case 'D': OPSTACK_WPUSH(TDOUBLE); break;
				
			case '[':
			case 'L':
				CHECK_STACK_OVERFLOW(1);
				block->stacksz++;
				type = getOpstackTop(block);
				type->tinfo = TINFO_SIG;
				type->data.name = sig;
				break;
				
			default:
				DBG(VERIFY3, dprintf("%sweird type signature: %s", indent, sig); );
				return verifyErrorInVerifyBasicBlock(einfo, method, this, "get{field/static}: unrecognized type signature");
				break;
			}
			break;
			
			
		case PUTFIELD:
			if (isWide(getOpstackTop(block))) n = 3;
			else                      n = 2;
			ENSURE_OPSTACK_SIZE(n);
			
			if (!checkUninit(this, getOpstackTop(block))) {
				return verifyErrorInVerifyBasicBlock(einfo, method, this, "putfield: uninitialized type on top of operand stack");
			}
			
			GET_WIDX;
			sig = FIELDREF_SIGD(idx, pool);
			DBG(VERIFY3, dprintf("                field name::type: %s::%s\n",
					     FIELDREF_NAMED(idx, pool), sig); );
			
			switch (*sig) {
			case 'I': case 'Z': case 'S': case 'B': case 'C':
				OPSTACK_POP_T_BLIND(TINT);
				break;
				
			case 'F': OPSTACK_POP_T_BLIND(TFLOAT);   break;
			case 'J': OPSTACK_WPOP_T_BLIND(TLONG);   break;
			case 'D': OPSTACK_WPOP_T_BLIND(TDOUBLE); break;
				
			case '[':
			case 'L':
				t->tinfo = TINFO_SIG;
				t->data.sig = sig;
				OPSTACK_POP_T_BLIND(t);
				break;
				
			default:
				DBG(VERIFY3, dprintf("%sweird type signature: %s", indent, sig); );
				return verifyErrorInVerifyBasicBlock(einfo, method, this, "put{field/static}: unrecognized type signature");
				break;
			}
			
			
			n = FIELDREF_CLASS(idx, pool);
			if (pool->tags[n] == CONSTANT_ResolvedClass) {
				t->tinfo = TINFO_CLASS;
				t->data.class = CLASS_CLASS(n, pool);
			} else {
				t->tinfo = TINFO_NAME;
				t->data.name = CLASS_NAMED(n, pool);
			}
			
			OPSTACK_POP_T_BLIND(t);
			break;
			
			
		case PUTSTATIC:
			if (getOpstackTop(block) == TWIDE) n = 2;
			else                      n = 1;
			ENSURE_OPSTACK_SIZE(n);
			
			GET_WIDX;
			sig = FIELDREF_SIGD(idx, pool);
			
			DBG(VERIFY3, dprintf("                field name::type: %s::%s\n",
					     FIELDREF_NAMED(idx, pool), sig); );
			
			switch (*sig) {
			case 'I': case 'Z': case 'S': case 'B': case 'C':
				OPSTACK_POP_T_BLIND(TINT);
				break;
				
			case 'F': OPSTACK_POP_T_BLIND(TFLOAT);   break;
			case 'J': OPSTACK_WPOP_T_BLIND(TLONG);   break;
			case 'D': OPSTACK_WPOP_T_BLIND(TDOUBLE); break;
				
			case '[':
			case 'L':
				t->tinfo = TINFO_SIG;
				t->data.sig = sig;
				OPSTACK_POP_T_BLIND(t);
				break;
				
			default:
				DBG(VERIFY3, dprintf("%sweird type signature: %s", indent, sig); );
				return verifyErrorInVerifyBasicBlock(einfo, method, this, "put{field/static}: unrecognized type signature");
				break;
			}
			break;
			
			
			/**************************************************************
			 * BRANCHING INSTRUCTIONS...END OF BASIC BLOCKS
			 **************************************************************/
		case GOTO:
		case GOTO_W:
			break;
			
		case JSR_W:
		case JSR:
			CHECK_STACK_OVERFLOW(1);
			block->stacksz++;
			type = getOpstackTop(block);
			type->tinfo = TINFO_ADDR;
			type->data.addr = pc + insnLen[code[pc]];
			break;

		case RET:
		        /* type checking done during merging stuff... */
			break;
			
		case IF_ACMPEQ:
		case IF_ACMPNE:
			ENSURE_OPSTACK_SIZE(2);
			if (!isReference(getOpstackTop(block)) ||
			    !isReference(getOpstackWTop(block))) {
				return verifyErrorInVerifyBasicBlock(einfo, method, this, "if_acmp* when item on top of stack is not a reference type");
			}
			opstackPopBlind(block);
			opstackPopBlind(block);
			break;
			
		case IF_ICMPEQ:
		case IF_ICMPNE:
		case IF_ICMPGT:
		case IF_ICMPGE:
		case IF_ICMPLT:
		case IF_ICMPLE:
			OPSTACK_POP_T(TINT);
		case IFEQ:
		case IFNE:
		case IFGT:
		case IFGE:
		case IFLT:
		case IFLE:
			OPSTACK_POP_T(TINT);
			break;
			
		case IFNONNULL:
		case IFNULL:
			ENSURE_OPSTACK_SIZE(1);
			if (!isReference(getOpstackItem(block, 1))) {
				return verifyErrorInVerifyBasicBlock(einfo, method, this, "if[non]null: thing on top of stack is not a reference");
			}
			opstackPopBlind(block);
			break;
			
		case LOOKUPSWITCH:
		case TABLESWITCH:
			OPSTACK_POP_T(TINT);
			return(true);
			
			
			/**************************************************************
			 * METHOD CALLING/RETURNING
			 **************************************************************/
		case INVOKEVIRTUAL:
		case INVOKESPECIAL:
		case INVOKEINTERFACE:
			
		case INVOKESTATIC:
			if (!checkMethodCall(einfo, method, block, pc, sigs, uninits)) {
				DBG(VERIFY3,
				    dprintf("\n                some problem with a method call...here's the block:\n");
				    printBlock(method, block, "                "); );
				
				/* propagate error */
				return verifyErrorInVerifyBasicBlock(einfo, method, this, "invoke* error");
			}
			break;
			
			
		case IRETURN:
			OPSTACK_PEEK_T(TINT);
			sig = getReturnSig(method);
			if (strlen(sig) != 1 || (*sig != 'I' && *sig != 'Z' && *sig != 'S' && *sig != 'B' && *sig != 'C')) {
				return verifyErrorInVerifyBasicBlock(einfo, method, this, "ireturn: method doesn't return an integer");
			}
			break;
		case FRETURN:
			OPSTACK_PEEK_T(TFLOAT);
			sig = getReturnSig(method);
			if (strcmp(sig, "F")) {
				return verifyErrorInVerifyBasicBlock(einfo, method, this, "freturn: method doesn't return an float");
			}
			break;
		case LRETURN:
			OPSTACK_WPEEK_T(TLONG);
			sig = getReturnSig(method);
			if (strcmp(sig, "J")) {
				return verifyErrorInVerifyBasicBlock(einfo, method, this, "lreturn: method doesn't return a long");
			}
			break;
		case DRETURN:
			OPSTACK_WPEEK_T(TDOUBLE);
			sig = getReturnSig(method);
			if (strcmp(sig, "D")) {
				return verifyErrorInVerifyBasicBlock(einfo, method, this, "dreturn: method doesn't return a double");
			}
			break;
		case RETURN:
			sig = getReturnSig(method);
			if (strcmp(sig, "V")) {
				return verifyErrorInVerifyBasicBlock(einfo, method, this, "return: must return something in a non-void function");
			}
			break;
		case ARETURN:
			ENSURE_OPSTACK_SIZE(1);
			t->tinfo = TINFO_SIG;
			t->data.sig  = getReturnSig(method);
			if (!typecheck(einfo, this, t, getOpstackTop(block))) {
				return verifyErrorInVerifyBasicBlock(einfo, method, this, "areturn: top of stack is not type compatible with method return type");
			}
			break;
			
		case ATHROW:
			ENSURE_OPSTACK_SIZE(1);
			if (!javaLangThrowable) {
			        /* TODO: this is here for now, but perhaps we should have a TTHROWABLE that initialized as
				 *       a signature, like we do for String and Object
				 */
				loadStaticClass(&javaLangThrowable, "java/lang/Throwable");
			}
			t->tinfo = TINFO_CLASS;
			t->data.class = javaLangThrowable;
			if (!typecheck(einfo, this, t, getOpstackTop(block))) {
				DBG(VERIFY3, dprintf("%sATHROW error: ", indent); printType(getOpstackTop(block)); dprintf ("\n"); );
				return verifyErrorInVerifyBasicBlock(einfo, method, this, "athrow: object on top of stack is not a subclass of throwable");
			}
			
			for (n = 0; n < method->localsz; n++) {
				if (block->locals[n].tinfo & TINFO_UNINIT) {
					return verifyErrorInVerifyBasicBlock(einfo, method, this, "athrow: uninitialized class instance in a local variable");
				}
			}
			break;
			
			
			/**************************************************************
			 * MISC
			 **************************************************************/
		case NOP:
			break;
			
			
		case BREAKPOINT:
		        /* for internal use only: cannot appear in a class file */
			return verifyErrorInVerifyBasicBlock(einfo, method, this, "breakpoint instruction cannot appear in classfile");
			break;
			
			
		case MONITORENTER:
		case MONITOREXIT:
			ENSURE_OPSTACK_SIZE(1);
			if(!isReference(getOpstackTop(block))) {
				return verifyErrorInVerifyBasicBlock(einfo, method, this, "monitor*: top of stack is not an object reference");
			}
			opstackPopBlind(block);
			break;
			
			
		case DUP:
			ENSURE_OPSTACK_SIZE(1);
			if (isWide(getOpstackTop(block))) {
				return verifyErrorInVerifyBasicBlock(einfo, method, this, "dup: on a long or double");
			}
			
			OPSTACK_PUSH(getOpstackTop(block));
			break;
			
		case DUP_X1:
			ENSURE_OPSTACK_SIZE(2);
			if (isWide(getOpstackTop(block)) || isWide(getOpstackWTop(block))) {
				return verifyErrorInVerifyBasicBlock(einfo, method, this, "dup_x1: splits up a double or long");
			}
			
			OPSTACK_PUSH(getOpstackTop(block));
			
			*getOpstackItem(block, 2) = *getOpstackItem(block, 3);
			*getOpstackItem(block, 3) = *getOpstackItem(block, 1);
			break;
			
		case DUP_X2:
			ENSURE_OPSTACK_SIZE(3);
			if (isWide(getOpstackTop(block))) {
				return verifyErrorInVerifyBasicBlock(einfo, method, this, "cannot dup_x2 when top item on operand stack is a two byte item");
			}
			
			OPSTACK_PUSH(getOpstackTop(block));
			
			*getOpstackItem(block, 2) = *getOpstackItem(block, 3);
			*getOpstackItem(block, 3) = *getOpstackItem(block, 4);
			*getOpstackItem(block, 4) = *getOpstackItem(block, 1);
			break;
			
		case DUP2:
			ENSURE_OPSTACK_SIZE(2);
			
			OPSTACK_PUSH(getOpstackWTop(block));
			OPSTACK_PUSH(getOpstackWTop(block));
			break;
			
		case DUP2_X1:
			ENSURE_OPSTACK_SIZE(2);
			if (isWide(getOpstackItem(block, 2))) {
				return verifyErrorInVerifyBasicBlock(einfo, method, this, "dup_x1 requires top 2 bytes on operand stack to be single bytes items");
			}
			CHECK_STACK_OVERFLOW(2);
			
			opstackPushBlind(block, getOpstackItem(block, 2));
			opstackPushBlind(block, getOpstackItem(block, 2));
			
			*getOpstackItem(block, 3) = *getOpstackItem(block, 5);
			*getOpstackItem(block, 4) = *getOpstackItem(block, 1);
			*getOpstackItem(block, 5) = *getOpstackItem(block, 2);
			break;
			
		case DUP2_X2:
			ENSURE_OPSTACK_SIZE(4);
			if (isWide(getOpstackItem(block, 2)) || isWide(getOpstackItem(block, 4))) {
				return verifyErrorInVerifyBasicBlock(einfo, method, this, "dup2_x2 where either 2nd or 4th byte is 2nd half of a 2 byte item");
			}
			CHECK_STACK_OVERFLOW(2);
			
			opstackPushBlind(block, getOpstackItem(block, 2));
			opstackPushBlind(block, getOpstackItem(block, 2));
			
			*getOpstackItem(block, 3) = *getOpstackItem(block, 5);
			*getOpstackItem(block, 4) = *getOpstackItem(block, 6);
			*getOpstackItem(block, 5) = *getOpstackItem(block, 1);
			*getOpstackItem(block, 6) = *getOpstackItem(block, 2);
			break;
			
			
		case SWAP:
			ENSURE_OPSTACK_SIZE(2);
			if (isWide(getOpstackTop(block)) || isWide(getOpstackWTop(block))) {
				return verifyErrorInVerifyBasicBlock(einfo, method, this, "cannot swap 2 bytes of a long or double");
			}
			
			*type         = *getOpstackTop(block);
			*getOpstackTop(block)  = *getOpstackWTop(block);
			*getOpstackWTop(block) = *type;
			break;
			
			
		case WIDE:
			wide = true;
			pc += insnLen[code[pc]];
			continue;
			
		default:
		        /* should never get here because of preprocessing in defineBasicBlocks() */
			return verifyErrorInVerifyBasicBlock(einfo, method, this, "unknown opcode encountered");
		}
		
		
		pc += insnLen[code[pc]];
		if (wide == true) {
			wide = false;
			pc++;
		}
	}
		
	
	/* SUCCESS! */
	return(true);


	
	/* take care of the namespace */
#undef OPSTACK_POP_N
#undef OPSTACK_POP_N_BLIND

#undef OPSTACK_WPOP_T
#undef OPSTACK_WPOP_T_BLIND
#undef OPSTACK_WPOP

#undef OPSTACK_POP_T
#undef OPSTACK_POP_T_BLIND
#undef OPSTACK_POP

#undef OPSTACK_WPEEK_T
#undef OPSTACK_WPEEK_T_BLIND
#undef OPSTACK_PEEK_T
#undef OPSTACK_PEEK_T_BLIND

#undef OPSTACK_WPUSH
#undef OPSTACK_WPUSH_BLIND
#undef OPSTACK_PUSH
#undef OPSTACK_PUSH_BLIND

#undef CHECK_STACK_OVERFLOW
#undef ENSURE_OPSTACK_SIZE

#undef ENSURE_LOCAL_WTYPE
#undef ENSURE_LOCAL_TYPE

#undef GET_WIDX
#undef GET_IDX
}


/* 
 * parses the next argument from sig into buf, returning pointer beyond arg.
 */
static
const char*
getNextArg(const char* sig, char* buf)
{
	const char* afterSig;
	
	if (*sig == ')') {
		buf[0] = ')';
		buf[1] = '\0';
		return sig;
	}
	/* parseFieldTypeDescriptor doesn't deal with void signatures */
	else if (*sig == 'V') {
		buf[0] = 'V';
		buf[1] = '\0';
		sig++;
		return sig;
	}
	
	for (afterSig = parseFieldTypeDescriptor(sig);
	     sig < afterSig;
	     sig++, buf++) {
		*buf = *sig;
	}
	
	*buf = '\0';
	
	return afterSig;
}


/*
 * countSizeOfArgsInSignature()
 *    Longs & Double count for 2, all else counts for one.
 */
static
uint32
countSizeOfArgsInSignature(const char* sig)
{
	uint32 count = 0;
	
	for (sig++; *sig != ')'; sig = parseFieldTypeDescriptor(sig)) {
		if (*sig == 'J' || *sig == 'D')
			count += 2;
		else
			count++;
	}
	
	return count;
}

/*
 * Helper function for error reporting in checkMethodCall.
 */
static inline
bool
verifyErrorInCheckMethodCall(errorInfo* einfo,
			     const Method* method,
			     char* argbuf,
			     uint32 pc,
			     const uint32 idx,
			     const constants* pool,
			     const char* methSig,
			     const char* msg)
{
	gc_free(argbuf);
	DBG(VERIFY3,
	    dprintf("                error with method invocation, pc = %d, method = %s%s\n",
		    pc,
		    METHODREF_NAMED(idx, pool),
		    methSig);
	    );
	if (einfo->type == 0) {
		postExceptionMessage(einfo, JAVA_LANG(VerifyError),
				     "in method \"%s.%s\": %s",
				     CLASS_CNAME(method->class),
				     METHOD_NAMED(method),
				     msg);
	}
	return(false);
}

/* 
 * checkMethodCall()
 *    verify an invoke instruction.  this includes making sure that the types
 *    on the operand stack are type compatible with those expected by the method
 *    being called.
 *
 *    note: we don't check to make sure that the class being referenced by the
 *          method call actually has the method, or that we have permission to
 *          access it, as those checks are deferred until pass 4.
 *
 * returns whether the method's arguments type check correctly.
 * it also pushes the return type onto binfo's operand stack.
 */
static
bool
checkMethodCall(errorInfo* einfo, const Method* method,
		BlockInfo* binfo, uint32 pc,
		SigStack** sigs, UninitializedType** uninits)
{
#define TYPE_ERROR return verifyErrorInCheckMethodCall(einfo, method, argbuf, pc, idx, pool, methSig, "parameters fail type checking in method invocation")
	
	const unsigned char* code        = METHOD_BYTECODE_CODE(method);
	const uint32 opcode              = code[pc];
	
	const constants* pool            = CLASS_CONSTANTS(method->class);
	const uint32 idx                 = WORD(code, pc + 1);
				   				 
	const uint32 classIdx            = METHODREF_CLASS(idx, pool);
	Type  mrc;
	Type* methodRefClass             = &mrc;
	Type* t                          = &mrc; /* for shorthand :> */
	Type* receiver                   = NULL;
	
	const char* methSig              = METHODREF_SIGD(idx, pool);
	const char* sig                  = methSig;
	uint32 nargs                     = countSizeOfArgsInSignature(sig);
	
	uint32 paramIndex                = 0;
	char* argbuf                     = checkPtr(gc_malloc(strlen(sig) * sizeof(char), GC_ALLOC_VERIFIER));
	
	
	DBG(VERIFY3, dprintf("%scalling method %s%s\n", indent, METHODREF_NAMED(idx, pool), sig); );
	
	
	if (nargs > binfo->stacksz) {
		return verifyErrorInCheckMethodCall(einfo, method, argbuf, pc, idx, pool, methSig, "not enough stuff on opstack for method invocation");
	}
	
	
	/* make sure that the receiver is type compatible with the class being invoked */
	if (opcode != INVOKESTATIC) {
		if (nargs == binfo->stacksz) {
			return verifyErrorInCheckMethodCall(einfo, method, argbuf, pc, idx, pool, methSig, "not enough stuff on opstack for method invocation");
		}
		
		
		receiver = &binfo->opstack[binfo->stacksz - (nargs + 1)];
		if (!(receiver->tinfo & TINFO_UNINIT) && !isReference(receiver)) {
			return verifyErrorInCheckMethodCall(einfo, method, argbuf, pc, idx, pool, methSig, "invoking a method on something that is not a reference");
		}
		
		if (pool->tags[classIdx] == CONSTANT_Class) {
			methodRefClass->tinfo = TINFO_NAME;
			methodRefClass->data.name = UNRESOLVED_CLASS_NAMED(classIdx, pool);
		} else {
			methodRefClass->tinfo = TINFO_CLASS;
			methodRefClass->data.class = CLASS_CLASS(classIdx, pool);
		}
		
		
		if (!strcmp(METHODREF_NAMED(idx,pool), constructor_name->data)) {
			if (receiver->tinfo & TINFO_UNINIT) {
				UninitializedType* uninit = receiver->data.uninit;
				
				if (receiver->tinfo == TINFO_UNINIT_SUPER) {
					Type t;
					t.tinfo = TINFO_CLASS;
					t.data.class = uninit->type.data.class->superclass;
					
					if (!sameType(methodRefClass, &uninit->type) &&
					    uninit->type.data.class != TOBJ->data.class &&
					    !sameType(methodRefClass, &t)) {
						return verifyErrorInCheckMethodCall(einfo, method, argbuf, pc, idx, pool, methSig, "incompatible receiving type for superclass constructor call");
					}
				} else if (!sameType(methodRefClass, &uninit->type)) {
					DBG(VERIFY3,
					    dprintf("%smethodRefClass: ", indent); printType(methodRefClass);
					    dprintf("\n%sreceiver: ", indent); printType(&uninit->type); dprintf("\n"); );
					return verifyErrorInCheckMethodCall(einfo, method, argbuf, pc, idx, pool, methSig, "incompatible receiving type for constructor call");
				}
				
				/* fix front of list, if necessary */
				if (uninit == *uninits) {
					*uninits = (*uninits)->next;
					if (*uninits) {
						(*uninits)->prev = NULL;
					}
					uninit->next = NULL;
				}
				
				popUninit(method, uninit, binfo);
			}
			else if (!sameType(methodRefClass, receiver)) {
				return verifyErrorInCheckMethodCall(einfo, method, argbuf, pc, idx, pool, methSig, "incompatible receiving type for constructor call");
			}
		}
		else if (!typecheck(einfo, method->class, methodRefClass, receiver)) {
			if (receiver->tinfo & TINFO_UNINIT) {
				return verifyErrorInCheckMethodCall(einfo, method, argbuf, pc, idx, pool, methSig, "invoking a method on an uninitialized object reference");
			}
			
			DBG(VERIFY3,
			    dprintf("%srequired receiver type: ", indent);
			    printType(methodRefClass);
			    dprintf("\n%sactual   receiver type: ", indent);
			    printType(receiver);
			    dprintf("\n");
			    );
			return verifyErrorInCheckMethodCall(einfo, method, argbuf, pc, idx, pool, methSig, "expected method receiver does not typecheck with object on operand stack");
		}
	}
	
	
	/* here we use paramIndex to represent which parameter we're currently considering.
	 * remember, when we call a method, the first parameter is deepest in the stack,
	 * so when we traverse the parameter list in the method signature we have to look
	 * from the bottom up.
	 */
	paramIndex = binfo->stacksz - nargs;
	for (sig = getNextArg(sig + 1, argbuf); *argbuf != ')'; sig = getNextArg(sig, argbuf)) {
		
		if (paramIndex >= binfo->stacksz) {
			KFREE(argbuf);
			return verifyErrorInCheckMethodCall(einfo, method, argbuf, pc, idx, pool, methSig, "error: not enough parameters on stack for method invocation");
		}
		
		
		switch (*argbuf) {
		case '[':
		case 'L':
			t->tinfo = TINFO_SIG;
			t->data.sig = argbuf;
			
			if (!typecheck(einfo, method->class, t, &binfo->opstack[paramIndex])) {
				TYPE_ERROR;
			}
			
			binfo->opstack[paramIndex] = *TUNSTABLE;
			paramIndex++;
			break;
			
		case 'Z': case 'S': case 'B': case 'C':
		case 'I':
			if (binfo->opstack[paramIndex].data.class != TINT->data.class) {
				TYPE_ERROR;
			}
			
			binfo->opstack[paramIndex] = *TUNSTABLE;
			paramIndex++;
			break;
			
		case 'F':
			if (binfo->opstack[paramIndex].data.class != TFLOAT->data.class) {
				TYPE_ERROR;
			}
			
			binfo->opstack[paramIndex] = *TUNSTABLE;
			paramIndex++;
			break;
			
		case 'J':
			if (binfo->opstack[paramIndex].data.class != TLONG->data.class ||
			    !isWide(&binfo->opstack[paramIndex + 1])) {
				TYPE_ERROR;
			}
			
			binfo->opstack[paramIndex]    = *TUNSTABLE;
			binfo->opstack[paramIndex+ 1] = *TUNSTABLE;
			paramIndex += 2;
			break;
			
		case 'D':
			if (binfo->opstack[paramIndex].data.class != TDOUBLE->data.class ||
			    !isWide(&binfo->opstack[paramIndex + 1])) {
				TYPE_ERROR;
			}
			
			binfo->opstack[paramIndex]     = *TUNSTABLE;
			binfo->opstack[paramIndex + 1] = *TUNSTABLE;
			paramIndex += 2;
			break;
			
		default:
			TYPE_ERROR;
		}
	}
	binfo->stacksz -= nargs;
	
	
	if (opcode != INVOKESTATIC) {
	        /* pop object reference off the stack */
		binfo->stacksz--;
		binfo->opstack[binfo->stacksz] = *TUNSTABLE;
	}
	
	
	/**************************************************************
	 * Process Return Type
	 **************************************************************/
	sig++;
	sig = getNextArg(sig, argbuf);
	
	if (*argbuf == 'J' || *argbuf == 'D') {
		if (method->stacksz < binfo->stacksz + 2) {
			return verifyErrorInCheckMethodCall(einfo, method, argbuf, pc, idx, pool, methSig, "not enough room on operand stack for method call's return value");
		}
	}
	else if (*argbuf != 'V') {
		if (method->stacksz < binfo->stacksz + 1) {
			return verifyErrorInCheckMethodCall(einfo, method, argbuf, pc, idx, pool, methSig, "not enough room on operand stack for method call's return value");
		}
	}
	
	switch (*argbuf) {
	case 'Z': case 'S': case 'B': case 'C':
	case 'I':
		binfo->opstack[binfo->stacksz++] = *TINT;
		break;
		
	case 'F':
		binfo->opstack[binfo->stacksz++] = *TFLOAT;
		break;
		
	case 'J':
		binfo->opstack[binfo->stacksz]     = *TLONG;
		binfo->opstack[binfo->stacksz + 1] = *TWIDE;
		binfo->stacksz += 2;
		break;
		
	case 'D':
		binfo->opstack[binfo->stacksz]     = *TDOUBLE;
		binfo->opstack[binfo->stacksz + 1] = *TWIDE;
		binfo->stacksz += 2;
		break;
		
	case 'V':
		break;
		
	case '[':
	case 'L':
		*sigs = pushSig(*sigs, argbuf);
		
		binfo->opstack[binfo->stacksz].data.class = (Hjava_lang_Class*)argbuf;
		binfo->opstack[binfo->stacksz].tinfo = TINFO_SIG;
		binfo->stacksz++;
		
		/* no freeing of the argbuf here... */
		return(true);
		
	default:
	        /* shouldn't get here because of parsing during pass 2... */
		DBG(VERIFY3, dprintf("                unrecognized return type signature: %s\n", argbuf); );
		KFREE(argbuf);
		postExceptionMessage(einfo, JAVA_LANG(InternalError),
				     "unrecognized return type signature");
		return(false);
	}
	
	KFREE(argbuf);
	return(true);
#undef TYPE_ERROR
#undef VERIFY_ERROR
}




/*
 * pushes the initial method arguments into local variable array
 */
static
bool
loadInitialArgs(const Method* method, errorInfo* einfo,
		BlockInfo* block,
		SigStack** sigs, UninitializedType** uninits)
{
#define VERIFY_ERROR(_MSG) \
	postExceptionMessage(einfo, JAVA_LANG(VerifyError), \
			     "method %s.%s: %s", \
			     CLASS_CNAME(method->class), METHOD_NAMED(method), _MSG); \
	KFREE(argbuf); \
	return(false)

#define LOCAL_OVERFLOW_ERROR \
	VERIFY_ERROR("method arguments cannot fit into local variables")
	
	
	uint32 paramCount = 0;
	
	/* the +1 skips the initial '(' */
	const char* sig = METHOD_SIGD(method) + 1;
	char* argbuf    = checkPtr(gc_malloc((strlen(sig)+1) * sizeof(char), GC_ALLOC_VERIFIER));
	char* newsig    = NULL;
	
	Type* locals = block->locals;
	
	DBG(VERIFY3, dprintf("        sig: %s\n", sig); );
	
	/* must have at least 1 local variable for the object reference	*/
	if (!METHOD_IS_STATIC(method)) {
		if (method->localsz <= 0) {
			VERIFY_ERROR("number of locals in non-static method must be > 0");
		}
		
		/* the first local variable in every method is the class to which it belongs */
		locals[0].tinfo = TINFO_CLASS;
		locals[0].data.class = method->class;
		paramCount++;
		if (!strcmp(METHOD_NAMED(method), constructor_name->data)) {
		        /* the local reference in a constructor is uninitialized */
			*uninits = pushUninit(*uninits, &locals[0]);
			locals[0].tinfo = TINFO_UNINIT_SUPER;
			locals[0].data.uninit = *uninits;
		}
	}
	
	for (sig = getNextArg(sig, argbuf); *argbuf != ')'; sig = getNextArg(sig, argbuf)) {
		if (paramCount > method->localsz) {
			LOCAL_OVERFLOW_ERROR;
		}
		
		switch (*argbuf) {
		case 'Z': case 'S': case 'B': case 'C':
		case 'I': locals[paramCount++] = *TINT; break;
		case 'F': locals[paramCount++] = *TFLOAT; break;
			
		case 'J':
			if (paramCount + 1 > method->localsz) {
				LOCAL_OVERFLOW_ERROR;
			}
			locals[paramCount] = *TLONG;
			locals[paramCount+1] = *TWIDE;
			paramCount += 2;
			break;
			
		case 'D':
			if (paramCount + 1 > method->localsz) {
				LOCAL_OVERFLOW_ERROR;
			}
			locals[paramCount] = *TDOUBLE;
			locals[paramCount+1] = *TWIDE;
			paramCount += 2;
			break;
			
		case '[':
		case 'L':
			newsig = checkPtr(gc_malloc((strlen(argbuf) + 1) * sizeof(char), GC_ALLOC_VERIFIER));
			*sigs = pushSig(*sigs, newsig);
			sprintf(newsig, "%s", argbuf);
			locals[paramCount].tinfo = TINFO_SIG;
			locals[paramCount].data.sig = newsig;
			paramCount++;
			break;
			
		default:
			DBG(VERIFY3,
			    dprintf("ERROR, loadInitialArgs(): argument to method has bad signature.\n");
			    dprintf("        it starts with an unrecognized character: %c\n", *argbuf);
			    dprintf("        the rest of argbuf: %s\n", argbuf);
			    );
			
			VERIFY_ERROR("unrecognized first character in parameter type descriptor");
			break;
		}
	}
	
	
	/* success! */
	KFREE(argbuf);
	return(true);

#undef LOCAL_OVERFLOW_ERROR
#undef VERIFY_ERROR
}


/*
 * getReturnSig()
 */
static
const char*
getReturnSig(const Method* method)
{
	const char* sig = METHOD_SIGD(method);
	
	/* skip the type parameters */
	for (sig++; *sig != ')'; sig = parseFieldTypeDescriptor(sig));
	sig++;
	
	return sig;
}


/*
 * resolveType()
 *     Ensures that the type is a pointer to an instance of Hjava_lang_Class.
 */
static
void
resolveType(errorInfo* einfo, Hjava_lang_Class* this, Type *type)
{
	const char* sig;
	char* tmp = NULL;

	if (type->tinfo & TINFO_NAME) {
		sig = type->data.sig;
		
		if (*sig != '[') {
			tmp = checkPtr(gc_malloc((strlen(sig) + 3) * sizeof(char), GC_ALLOC_VERIFIER));
			sprintf(tmp, "L%s;", sig);
			sig = tmp;
		}
		
		type->tinfo = TINFO_CLASS;
		type->data.class = getClassFromSignature(sig, this->loader, einfo);
		
		if (tmp) {
			KFREE(tmp);
		}
	}
	else if (type->tinfo & TINFO_SIG) {
		type->tinfo = TINFO_CLASS;
		type->data.class = getClassFromSignature(type->data.sig, this->loader, einfo);
	}
}


/*
 * mergeTypes()
 *     merges two types, t1 and t2, into t2.  this result could
 *     be a common superclass, a common class that both types implement, or,
 *     in the event that the types are not compatible, TUNSTABLE.
 *
 * returns whether an actual merger was made (i.e. they weren't the same type)
 *
 * note: the precedence of merged types goes (from highest to lowest):
 *     actual pointer to Hjava_lang_Class*
 *     TINFO_SIG
 *     TINFO_NAME
 *
 * TODO: right now the priority is to be a common superclass, as stated in
 *       the JVML2 specs.  a better verification technique might check this first,
 *       and then check interfaces that both classes implement.  of course, depending
 *       on the complexity of the inheritance hirearchy, this could take a lot of time.
 *       
 *       the ideal solution is to remember *all* possible highest resolution types,
 *       which, of course, would require allocating more memory on the fly, etc., so,
 *       at least for now, we're not really even considering it.
 */
static
bool
mergeTypes(errorInfo* einfo, Hjava_lang_Class* this,
	   Type* t1, Type* t2)
{
	if (IS_ADDRESS(t1) || IS_ADDRESS(t2)) {
	        /* if one of the types is TADDR, the other one must also be TADDR */
		if (t1->tinfo != t2->tinfo) {
			return false;
		}
		
		/* TODO: should this be an error if they don't agree? */
		t2->tinfo = t1->tinfo;
		return true;
	}
	else if (t2->data.class == TUNSTABLE->data.class || sameType(t1, t2)) {
		return false;
	}
	else if (t1->tinfo & TINFO_UNINIT || t2->tinfo & TINFO_UNINIT ||
		 !isReference(t1) || !isReference(t2)) {
		
		*t2 = *TUNSTABLE;
		return true;
	}
	/* references only from here on out */
	else if (t1->data.class == TOBJ->data.class) {
		*t2 = *t1;
		return true;
	}
	
	
	/* not equivalent, must resolve them */
	resolveType(einfo, this, t1);
	if (t1->data.class == NULL) {
		return false;
	}

	resolveType(einfo, this, t2);
	if (t2->data.class == NULL) {
		return false;
	}
	
	if (CLASS_IS_INTERFACE(t1->data.class) &&
	    instanceof_interface(t1->data.class, t2->data.class)) {
	
		/* t1 is an interface and t2 implements it,
		 * so the interface is the merged type.
		 */

		*t2 = *t1;
		
		return true;
	
	} else if (CLASS_IS_INTERFACE(t2->data.class) &&
		   instanceof_interface(t2->data.class, t1->data.class)) {
		
		/* same as above, but we don't need to merge, since
		 * t2 already is the merged type
		 */

		return false;
	} else {
		/*
		 * neither of the types is an interface, so we have to
		 * check for common superclasses. Only merge iff t2 is
		 * not the common superclass.
		 */
		Hjava_lang_Class *tmp = t2->data.class;
		
		t2->data.class = getCommonSuperclass(t1->data.class, t2->data.class);
		
		return tmp != t2->data.class;
	} 
}


/*
 * returns the first (highest) common superclass of classes A and B.
 *
 * precondition: neither type is an array type
 *               nor is either a primitive type
 */
static
Hjava_lang_Class*
getCommonSuperclass(Hjava_lang_Class* t1, Hjava_lang_Class* t2)
{
	Hjava_lang_Class* A;
	Hjava_lang_Class* B;
	
	for (A = t1; A != NULL; A = A->superclass) {
		for (B = t2; B != NULL; B = B->superclass) {
			if (A == B) return A;
		}
	}
	
	/* error of some kind...at the very least, we shoulda gotten to Object
	 * when traversing the class hirearchy
	 */
	return TUNSTABLE->data.class;
}


/*
 * isReference()
 *    returns whether the type is a reference type
 */
static
bool
isReference(const Type* type)
{
	return (type->tinfo & TINFO_NAME ||
		type->tinfo & TINFO_SIG ||
		type->tinfo & TINFO_CLASS ||
		type->tinfo & TINFO_UNINIT);
}

/*
 * isArray()
 *     returns whether the Type is an array Type
 */
static
bool
isArray(const Type* type)
{
	if (!isReference(type)) {
		return false;
	}
	else if (type->tinfo & TINFO_NAME || type->tinfo & TINFO_SIG) {
		return (*(type->data.sig) == '[');
	}
	else if (type->tinfo != TINFO_CLASS) {
		return false;
	}
	else {
		return (*(CLASS_CNAME(type->data.class)) == '[');
	}
}


/*
 * sameType()
 *     returns whether two Types are effectively equivalent.
 */
static
bool
sameType(Type* t1, Type* t2)
{
	switch (t1->tinfo) {
	case TINFO_SYSTEM:
		return (t2->tinfo == TINFO_SYSTEM &&
			t1->data.class == t2->data.class);
		
	case TINFO_ADDR:
		return (t2->tinfo == TINFO_ADDR &&
			t1->data.addr == t2->data.addr);
		
	case TINFO_PRIMITIVE:
		return (t2->tinfo == TINFO_PRIMITIVE &&
			t1->data.class == t2->data.class);
		
	case TINFO_UNINIT:
	case TINFO_UNINIT_SUPER:
		return (t2->tinfo & TINFO_UNINIT &&
			(t1->data.uninit == t2->data.uninit ||
			 sameRefType(&(t1->data.uninit->type),
				     &(t2->data.uninit->type))));
		
	default:
		DBG(VERIFY3, dprintf("%ssameType(): unrecognized tinfo (%d)\n", indent, t1->tinfo); );
		return false;
		
	case TINFO_SIG:
	case TINFO_NAME:
	case TINFO_CLASS:
		return ((t2->tinfo == TINFO_SIG ||
			 t2->tinfo == TINFO_NAME || 
			 t2->tinfo == TINFO_CLASS) &&
			sameRefType(t1,t2));
	}
}

/*
 * sameRefType()
 *     returns whether two Types are effectively equivalent.
 *
 *     pre: t1 and t2 are both reference types
 */
static
bool
sameRefType(Type* t1, Type* t2)
{
	const char* sig1 = NULL;
	const char* sig2 = NULL;
	uint32 len1, len2;
	
	if (isNull(t1) || isNull(t2)) {
		return true;
	}
	
	if (t1->tinfo & TINFO_NAME) {
		sig1 = t1->data.name;
		
		if (t2->tinfo & TINFO_NAME) {
			return (!strcmp(sig1, t2->data.name));
		}
		else if (t2->tinfo & TINFO_SIG) {
			sig2 = t2->data.sig;
			
			len1 = strlen(sig1);
			len2 = strlen(sig2);
			
			sig2++;
			if ((len1 + 2 != len2) || strncmp(sig1, sig2, len1))
				return false;
		}
		else {
			if (strcmp(sig1, CLASS_CNAME(t2->data.class)))
				return false;
		}
		
		*t1 = *t2;
		return true;
	}
	else if (t1->tinfo & TINFO_SIG) {
		sig1 = t1->data.sig;
		
		if (t2->tinfo & TINFO_SIG) {
			return (!strcmp(sig1, t2->data.sig));
		}
		else if (t2->tinfo & TINFO_NAME) {
			sig2 = t2->data.name;
			
			len1 = strlen(sig1);
			len2 = strlen(sig2);
			sig1++;
			
			if ((len1 != len2 + 2) || strncmp(sig1, sig2, len2))
				return false;
			
			*t2 = *t1;
			return true;
		}
		else {
			sig2 = CLASS_CNAME(t2->data.class);
			
			len1 = strlen(sig1);
			len2 = strlen(sig2);
			sig1++;
			
			if ((len1 != len2 + 2) || strncmp(sig1, sig2, len2))
				return false;
			
			*t1 = *t2;
			return true;
		}
	}
	else {
		sig1 = CLASS_CNAME(t1->data.class);
		
		if (t2->tinfo & TINFO_SIG) {
			sig2 = t2->data.sig;
			
			len1 = strlen(sig1);
			len2 = strlen(sig2);
			sig2++;
			if ((len1 + 2 != len2) || strncmp(sig1, sig2, len1))
				return false;
			
			*t2 = *t1;
			return true;
		}
		else if (t2->tinfo & TINFO_NAME) {
			sig2 = t2->data.name;
			
			if (strcmp(sig1, sig2))
				return false;
			
			*t2 = *t1;
			return true;
		}
		else {
		        /* we should never get here */
			sig2 = CLASS_CNAME(t2->data.class);
			return (!strcmp(sig1, sig2));
		}
	}
}


/*
 * returns whether t2 can be a t1
 */
static
bool
typecheck(errorInfo* einfo, Hjava_lang_Class* this, Type* t1, Type* t2)
{
	DBG(VERIFY3, dprintf("%stypechecking ", indent); printType(t1); dprintf("  vs.  "); printType(t2); dprintf("\n"); );
	
	if (sameType(t1, t2)) {
		return true;
	}
	else if (t1->tinfo & TINFO_UNINIT || t2->tinfo & TINFO_UNINIT) {
		return false;
	}
	else if (!isReference(t1) || !isReference(t2)) {
		return false;
	}
	else if (sameType(t1, TOBJ)) {
		return true;
	}

	resolveType(einfo, this, t1);
	if (t1->data.class == NULL) {
		return false;
	}

	resolveType(einfo, this, t2);
	if (t2->data.class == NULL) {
		return false;
	}

	return instanceof(t1->data.class, t2->data.class);
}

/*
 * allocate memory for a block info and fill in with default values
 */
BlockInfo*
createBlock(const Method* method)
{
	int i;
	
	BlockInfo* binfo = checkPtr((BlockInfo*)gc_malloc(sizeof(BlockInfo), GC_ALLOC_VERIFIER));
	
	binfo->startAddr   = 0;
	binfo->status      = IS_INSTRUCTION | START_BLOCK;  /* not VISITED or CHANGED */
	
	/* allocate memory for locals */
	if (method->localsz > 0) {
		binfo->locals = checkPtr(gc_malloc(method->localsz * sizeof(Type), GC_ALLOC_VERIFIER));
		
		for (i = 0; i < method->localsz; i++) {
			binfo->locals[i] = *TUNSTABLE;
		}
	} else {
		binfo->locals = NULL;
	}
	
	
	/* allocate memory for operand stack */
	binfo->stacksz = 0;
	if (method->stacksz > 0) {
		binfo->opstack = checkPtr(gc_malloc(method->stacksz * sizeof(Type), GC_ALLOC_VERIFIER));
		
		for (i = 0; i < method->stacksz; i++) {
			binfo->opstack[i] = *TUNSTABLE;
		}
	} else {
		binfo->opstack = NULL;
	}
	
	return binfo;
}

/*
 * frees the memory of a basic block
 */
void
freeBlock(BlockInfo* binfo)
{
	if (binfo == NULL) return;
	
	if (binfo->locals != NULL)
		KFREE(binfo->locals);
	if (binfo->opstack != NULL)
		KFREE(binfo->opstack);
	
	KFREE(binfo);
}

/*
 * copies information from one stack of basic blocks to another
 */
void
copyBlockData(const Method* method, BlockInfo* fromBlock, BlockInfo* toBlock)
{
	toBlock->startAddr = fromBlock->startAddr;
	toBlock->lastAddr  = fromBlock->lastAddr;
	
	copyBlockState(method, fromBlock, toBlock);
}

/*
 * copies the local variables, operand stack, status, and context
 * from one block to another.
 */
void
copyBlockState(const Method* method, BlockInfo* fromBlock, BlockInfo* toBlock)
{
	uint32 n;
	
	toBlock->status  = fromBlock->status;
	
	for (n = 0; n < method->localsz; n++) {
		toBlock->locals[n] = fromBlock->locals[n];
	}
	
	toBlock->stacksz = fromBlock->stacksz;
	for (n = 0; n < method->stacksz; n++) {
		toBlock->opstack[n] = fromBlock->opstack[n];
	}
}

/*
 * returns which block the given pc is in
 */
static
BlockInfo*
inWhichBlock(uint32 pc, BlockInfo** blocks, uint32 numBlocks)
{
	uint32 i;
	for (i = 0; i < numBlocks; i++) {
		if (pc < blocks[i]->startAddr) continue;
		if (pc <= blocks[i]->lastAddr) return blocks[i];
	}
	
	/* shouldn't ever get here unless the specified PC is messed up */
	DBG(VERIFY3, dprintf("inWhichBlock(...): pc = %d out of range...weird.\n", pc); );
	
	return NULL;
}



/*
 * pushSig()
 *     Pushes a new signature on the Stack
 */
static
SigStack*
pushSig(SigStack* sigs, const char* sig)
{
	SigStack* new_sig = checkPtr(gc_malloc(sizeof(SigStack), GC_ALLOC_VERIFIER));
	new_sig->sig = sig;
	new_sig->next = sigs;
	return new_sig;
}


/*
 * freeSigStack()
 *     Frees the memory consumed by a stack of names and signatures.
 */
static
void
freeSigStack(SigStack* sigs)
{
	SigStack* tmp;
	while(sigs != NULL) {
		tmp = sigs->next;
		KFREE(sigs);
		sigs = tmp;
	}
}


/*
 * checkUninit()
 *     To be called when dealing with (get/put)field access.  Makes sure that get/putfield and
 *     invoke* instructions have access to the instance fields of the object in question.
 */
static
bool
checkUninit(Hjava_lang_Class* this, Type* type)
{
	if (type->tinfo & TINFO_UNINIT) {
		if (type->tinfo & TINFO_UNINIT_SUPER) {
			UninitializedType* uninit = type->data.uninit;
			Type t;
			t.tinfo = TINFO_CLASS;
			t.data.class = this;
			
			if (!sameType(&uninit->type, &t)) {
				return false;
			}
		}
		else {
			return false;
		}
	}
	
	return true;
}

/*
 * pushUninit()
 *    Adds an unitialized type to the list of uninitialized types.
 *
 *    uninits is the front of the list to be added onto.
 */
static
UninitializedType*
pushUninit(UninitializedType* uninits, const Type* type)
{
	UninitializedType* uninit = checkPtr(gc_malloc(sizeof(UninitializedType), GC_ALLOC_VERIFIER));
	uninit->type = *type;
	uninit->prev = NULL;
	
	if (!uninits) {
		uninit->next = NULL;
		return uninit;
	}
	
	uninit->prev = NULL;
	uninit->next = uninits;
	uninits->prev = uninit;
	return uninit;
}

/*
 * popUninit()
 *     Pops an uninitialized type off of the operand stack
 */
static
void
popUninit(const Method* method, UninitializedType* uninit, BlockInfo* binfo)
{
	uint32 n;
	
	for (n = 0; n < method->localsz; n++) {
		if (binfo->locals[n].tinfo & TINFO_UNINIT &&
		    ((UninitializedType*)binfo->locals[n].data.class) == uninit) {
			binfo->locals[n] = uninit->type;
		}
	}
	
	for (n = 0; n < binfo->stacksz; n++) {
		if (binfo->opstack[n].tinfo & TINFO_UNINIT &&
		    ((UninitializedType*)binfo->opstack[n].data.class) == uninit) {
			binfo->opstack[n] = uninit->type;
		}
	}
	
	if (uninit->prev) {
		uninit->prev->next = uninit->next;
	}
	if (uninit->next) {
		uninit->next->prev = uninit->prev;
	}
	
	KFREE(uninit);
}

/*
 * freeUninits
 *    frees a list of unitialized types
 */
static
void
freeUninits(UninitializedType* uninits)
{
	UninitializedType* tmp;
	while (uninits) {
		tmp = uninits->next;
		KFREE(uninits);
		uninits = tmp;
	}
}



/* for debugging */
#if !(defined(NDEBUG) || !defined(KAFFE_VMDEBUG))

/*
 * printInstruction()
 *     prints out a string representation of the instruction.
 *
 *     TODO: print out extra information with the instruction.
 */
static
void
printInstruction(const int opcode)
{
#define PRINT(_OP) dprintf("%s", _OP); return;
	
	switch(opcode) {
	case 0: PRINT("NOP");
		
	case 1: PRINT("ACONST-null");
		
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

static
void
printType(const Type* t)
{
	const Hjava_lang_Class* type = t->data.class;
	
	dprintf("(%d)", t->tinfo);
	switch(t->tinfo) {
	case TINFO_SYSTEM:
		if (type == TUNSTABLE->data.class) {
			dprintf("TUNSTABLE");
		}
		else if (isWide(t)) {
			dprintf("TWIDE");
		}
		else {
			dprintf("UNKNOWN SYSTEM TYPE");
		}
		break;
		
	case TINFO_ADDR:
		dprintf("TADDR: %d", t->data.addr);
		break;
		
	case TINFO_PRIMITIVE:
		if (type == TINT->data.class) {
			dprintf("TINT");
		}
		else if (type == TLONG->data.class) {
			dprintf("TLONG");
		}
		else if (type == TFLOAT->data.class) {
			dprintf("TFLOAT");
		}
		else if (type == TDOUBLE->data.class) {
			dprintf("TDOUBLE");
		}
		else {
			dprintf("UKNOWN PRIMITIVE TYPE");
		}
		break;
		
	case TINFO_SIG:
		dprintf("%s", t->data.sig);
		break;
		
	case TINFO_NAME:
		dprintf("%s", t->data.name);
		break;
		
	case TINFO_CLASS:
		if (type == NULL) {
			dprintf("NULL");
		}
		else if (isNull(t)) {
			dprintf("TNULL");
		}
		
		else if (type == TCHARARR->data.class) {
			dprintf("TCHARARR");
		}
		else if (type == TBOOLARR->data.class) {
			dprintf("TBOOLARR");
		}
		else if (type == TBYTEARR->data.class) {
			dprintf("TBYTEARR");
		}
		else if (type == TSHORTARR->data.class) {
			dprintf("TSHORTARR");
		}
		else if (type == TINTARR->data.class) {
			dprintf("TINTARR");
		}
		else if (type == TLONGARR->data.class) {
			dprintf("TLONGARR");
		}
		else if (type == TFLOATARR->data.class) {
			dprintf("TFLOATARR");
		}
		else if (type == TDOUBLEARR->data.class) {
			dprintf("TDOUBLEARR");
		}
		else if (type == TOBJARR->data.class) {
			dprintf("TOBJARR");
		}
		else {
			if (type->name == NULL || CLASS_CNAME(type) == NULL) {
				dprintf("<NULL NAME>");
			} else {
				dprintf("%s", CLASS_CNAME(type));
			}
		}
		break;
		
	case TINFO_UNINIT:
	case TINFO_UNINIT_SUPER:
		printType(&(t->data.uninit->type));
		break;
		
	default:
		dprintf("UNRECOGNIZED TINFO");
		break;
	}
}


/*
 * printBlock()
 *    For debugging.  Prints out a basic block.
 */
static
void
printBlock(const Method* method, const BlockInfo* binfo, const char* indent)
{
	uint32 n;
	
	dprintf("%slocals:\n", indent);
	for (n = 0; n < method->localsz; n++) {
		dprintf("%s    %d: ", indent, n);
		printType(&binfo->locals[n]);
		dprintf("\n");
	}
	dprintf("%sopstack (%d):\n", indent, binfo->stacksz);
	for (n = 0; n < method->stacksz; n++) {
		dprintf("%s    %d: ", indent, n);
		printType(&binfo->opstack[n]);
		dprintf("\n");
	}
}


#endif /* !(defined(NDEBUG) || !defined(KAFFE_VMDEBUG)) */
