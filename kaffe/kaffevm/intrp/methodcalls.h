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

void *
engine_buildTrampoline (Method *meth, void **where, errorInfo *einfo);

void engine_callMethod (callMethodInfo *call);

/*
 * extra args the engine wants to pass when using callMethodA / callMethodV
 */
#define engine_reservedArgs(M) 2

#endif /* __method_dispatch_h__ */
