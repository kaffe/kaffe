/*
 * verify.h
 *
 * only publically accessible (i.e. required outside of the verifier)
 * are declared here.
 *
 * copyright 2003, Rob Gonzalez (rob@kaffe.org)
 */

#ifndef __verify_h
#define __verify_h

#include "classMethod.h"
#include "errors.h"
#include "gtypes.h"

extern bool isTrustedClass(Hjava_lang_Class* class);
extern const char* parseBaseTypeDescriptor(const char* sig);
extern const char* parseObjectTypeDescriptor(const char* sig);
extern const char* parseArrayTypeDescriptor(const char* sig);
extern const char* parseFieldTypeDescriptor(const char* sig);
extern bool parseMethodTypeDescriptor(const char* sig);

/**********************************************************
 * Pass 2
 **********************************************************/
extern bool verify2(Hjava_lang_Class* class, errorInfo *einfo);

/**********************************************************
 * Pass 3
 **********************************************************/

extern bool verify3(Hjava_lang_Class* class, errorInfo *einfo);

#endif /* __verify_h */
