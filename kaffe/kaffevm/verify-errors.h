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
				    uint32 n)
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
				     uint32 n)
{
  DBG(VERIFY3,
      dprintf("ERROR:  pc = %d, instruction = ", pc);
      printInstruction(code[pc]);
      dprintf(", localsz = %d, localindex = %d\n", method->localsz, n);
      );
  return verifyErrorInVerifyMethod3a(einfo, method, "attempting to access a local variable beyond local array");
}

#endif
