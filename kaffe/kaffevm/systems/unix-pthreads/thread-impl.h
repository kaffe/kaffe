/*
 * thread-impl.h - this is the part of the ThreadInterface implementation
 * we want to expose (by means of injection into the abstraction threads.h)
 *
 * Use it only for two purposes:
 *  (1) redefine generic interface function macros of threads.h in order
 *      to turn the runtime interface (completely or partly) into a
 *      compile-time interface
 *  (2) VERY RESTRICTIVE - define macros which can be used in clients to
 *      check for a certain subsystem (but JUST if there are strong reasons
 *      against using a clean interface function instead)
 *
 * Copyright (c) 1999
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

#ifndef __thread_impl_h
#define __thread_impl_h

#define UNIX_PTHREADS 1

#include "thread-internal.h"

#endif /* __thread_impl_h */
