/*
 * Copyright (c) 1999, 2000 The University of Utah.  All rights reserved.
 *
 * This file is licensed under the terms of the GNU Public License.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file.
 */

#ifndef __lock_impl_h
#define __lock_impl_h

/* OSKit pthreads provides the jmutex/jcondvar lock interface. */

typedef pthread_mutex_t		jmutex;
typedef pthread_cond_t		jcondvar;

void jmutex_initialise(jmutex *lock);
void jmutex_lock(jmutex *lock);
void jmutex_unlock(jmutex *lock);
void jmutex_destroy(jmutex *lock);

void jcondvar_initialise(jcondvar *cv);
jboolean jcondvar_wait(jcondvar *cv, jmutex *lock, jlong timeout);
void jcondvar_signal(jcondvar *cv, jmutex *lock);
void jcondvar_destroy(jcondvar *lock);

#endif /* __lock_impl_h */
