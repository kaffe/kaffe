/*
 * methodCache.h
 *
 * A faster way to find a translated method given a pc.
 *
 * Copyright (c) 1998, 1999
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 *
 * Written by Godmar Back <gback@cs.utah.edu>
 */

#ifndef __kaffevm_methodcache_h
#define __kaffevm_methodcache_h

extern bool makeMethodActive(Method* meth);
extern void makeMethodInactive(Method* meth);
extern void dumpActiveMethods(jobject printstream, jobject loader);

#endif /* __kaffevm_methodcache_h */
