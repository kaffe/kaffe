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
 * Helper function for error reporting in verifyMethod3a.
 */
static inline
void
verifyErrorInVerifyMethod3a(errorInfo* einfo,
			    const Verifier* v,
			    const char * msg)
{
	if (einfo->type == 0) {
		postExceptionMessage(einfo, JAVA_LANG(VerifyError),
				     "in method \"%s.%s\": %s",
				     CLASS_CNAME(v->method->class), METHOD_NAMED(v->method), msg);
	}
}

/*
 * Helper function for error reporting in BRANCH_IN_BOUNDS macro in verifyMethod3a.
 */
static inline
void
branchInBoundsErrorInVerifyMethod3a(errorInfo* einfo,
				    const Verifier* v,
				    uint32 codelen,
				    uint32 n)
{
  DBG(VERIFY3, dprintf("ERROR: branch to (%d) out of bound (%d) \n", n, codelen); );
  verifyErrorInVerifyMethod3a(einfo, v, "branch out of method code");
}

/*
 * Helper function for error reporting in CHECK_LOCAL_INDEX macro in verifyMethod3a.
 */
static inline
void
checkLocalIndexErrorInVerifyMethod3a(errorInfo* einfo,
				     const Verifier* v,
				     uint32 pc,
				     unsigned char* code,
				     uint32 n)
{
  DBG(VERIFY3,
      dprintf("ERROR:  pc = %d, instruction = ", pc);
      printInstruction(code[pc]);
      dprintf(", localsz = %d, localindex = %d\n", v->method->localsz, n);
      );
  verifyErrorInVerifyMethod3a(einfo, v, "attempting to access a local variable beyond local array");
}

#endif
