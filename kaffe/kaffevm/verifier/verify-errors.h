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


/*
 * Helper function for error reporting in BRANCH_IN_BOUNDS macro in verifyMethod3a.
 */
static inline
bool
branchInBoundsErrorInVerifyMethod3a(Verifier* v,
				    uint32 codelen,
				    uint32 n)
{
	DBG(VERIFY3, dprintf("ERROR: branch to (%d) out of bound (%d) \n", n, codelen); );
	return verifyError(v, "branch out of method code");
}

/*
 * Helper function for error reporting in CHECK_LOCAL_INDEX macro in verifyMethod3a.
 */
static inline
bool
checkLocalIndexErrorInVerifyMethod3a(Verifier* v,
				     uint32 pc,
				     unsigned char* code,
				     uint32 n)
{
	DBG(VERIFY3,
	    dprintf("ERROR:  pc = %d, instruction = ", pc);
	    printInstruction(code[pc]);
	    dprintf(", localsz = %d, localindex = %d\n", v->method->localsz, n);
	    );
	return verifyError(v, "attempting to access a local variable beyond local array");
}

#endif
