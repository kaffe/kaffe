/*
 * verify-debug.h
 *
 * Copyright 2004
 *   Kaffe.org contributors. See ChangeLog for details. All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 *
 */

#ifndef _VERIFY_ERRORS_H
#define _VERIFY_ERRORS_H

/*
 * General verification error method.
 * Returns false to allow statements such as:
 *   return verifyError(...);
 */
static inline
bool
verifyError(Verifier* v, const char* msg) {
	if (v->einfo->type == 0) {
		postExceptionMessage(v->einfo, JAVA_LANG(VerifyError),
				     "in method \"%s.%s\": %s",
				     CLASS_CNAME(v->method->class), METHOD_NAMED(v->method), msg);
	}
	return false;
}

#endif
