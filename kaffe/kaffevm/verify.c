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
 */

#define	DBG(s)

#include "config.h"
#include "config-std.h"
#include "constants.h"
#include "classMethod.h"
#include "baseClasses.h"
#include "exception.h"
#include "errors.h"
#include "jni.h"

/*
 * Verify pass 2:  Check the internal consistency of the class file
 *  but do not check the bytecode.
 */
void
verify2(Hjava_lang_Class* class)
{
	constants* info;
	bool error;
	int i;
	int tag;

DBG(	printf("Verifing class %s\n", (char*)class->name->data);	)

	error = false;

	/* 1: Check final directives are obeyed */
		/* NOT YET */

	/* 2: Check class has a superclass or it java.lang.Object */
	if (class->superclass == 0 && strcmp((char*)class->name->data, OBJECTCLASS) != 0) {
		error = true;
	}

	/* 3: Check class constant pool is consistent */
	info = CLASS_CONSTANTS(class);
	/* Constant pool loaded - check it's integrity. */
	for (i = 1; i < info->size; i++) {
		switch (info->tags[i]) {
		case CONSTANT_Utf8:
		case CONSTANT_Integer:
		case CONSTANT_Float:
		case CONSTANT_ResolvedString:
		case CONSTANT_ResolvedClass:
			/* Always legal */
			break;

		case CONSTANT_Long:
		case CONSTANT_Double:
			/* Always legal */
			i++;
			break;

		case CONSTANT_Class:
		case CONSTANT_String: /* Same as Class */
			/* These have already been re-written to avoid the
			 * extra indirection added by Java.  This is to fit
			 * with the precompiled format.  Here we will not
			 * get an error.
			 */
			break;

		case CONSTANT_Fieldref:
		case CONSTANT_Methodref: /* Same as Fieldref */
		case CONSTANT_InterfaceMethodref: /* Same as Fieldref */
			tag = CONST_TAG(FIELDREF_CLASS(i, info), info);
			if (tag != CONSTANT_Class && tag != CONSTANT_ResolvedClass) {
				error = true;
			}
			if (CONST_TAG(FIELDREF_NAMEANDTYPE(i, info), info) != CONSTANT_NameAndType) {
				error = true;
			}
			break;

		case CONSTANT_NameAndType:
			if (CONST_TAG(NAMEANDTYPE_NAME(i, info), info) != CONSTANT_Utf8) {
				error = true;
			}
			if (CONST_TAG(NAMEANDTYPE_SIGNATURE(i, info), info) != CONSTANT_Utf8) {
				error = true;
			}
			break;

		default:
			error = true;
			break;
		}
	}

	/* 4: Check field and method references have valid names and types */
		/* NOT YET */

	/* If we found an inconsistency then throw an exception */
	if (error == true) {
		throwException(ClassFormatError);
	}
}

/*
 * Verify pass 3:  Check the consistency of the bytecode.
 */
void
verify3(Hjava_lang_Class* class)
{
	if ((class->loader == 0 && (Kaffe_JavaVMArgs[0].verifyMode & 1) == 0) ||
	    (class->loader != 0 && (Kaffe_JavaVMArgs[0].verifyMode & 2) == 0)) {
		return;
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
}
