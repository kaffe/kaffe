/*
 * methodcalls.h
 * Implementation of method calls
 *
 * Copyright (c) 2004
 *      The kaffe.org's developers. See ChangeLog for details.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#ifndef __method_calls_h__
#define __method_calls_h__

#include "classMethod.h"
#include "support.h"

/*
 * Build a trampoline if necessary, return the address of the native code
 * to either the trampoline or the translated or native code.
 *
 * Sets *where to the address of the native code.
 *
 * Return the address of the native code or 0 on failure
 */

void *
engine_buildTrampoline (Method *meth, void **where, errorInfo *einfo);


void
engine_callMethod (callMethodInfo *call);

/*
 * Fix up trampoline.
 */
nativecode*
soft_fixup_trampoline(FIXUP_TRAMPOLINE_DECL);

/*
 * extra args the engine wants to pass when using callMethodA / callMethodV
 */
#define engine_reservedArgs(M) 0

#endif /* __method_dispatch_h__ */
