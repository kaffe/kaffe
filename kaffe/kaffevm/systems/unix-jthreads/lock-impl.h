/*
 * Copyright (c) 1996, 1997, 1998, 1999
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * This file is licensed under the terms of the GNU Public License.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file.
 */

#ifndef __lock_impl_h
#define __lock_impl_h

struct _jthread;

/*
 * a mutex has pointer to the holder and a queue of threads waiting
 * on the mutex.
 */
typedef struct _jmutex
{
        struct _jthread *holder;
        struct _KaffeNodeQueue *waiting;
} jmutex;

/*
 * a condition variable is simply expressed as a list of threads
 * waiting to be notified
 */
typedef struct _KaffeNodeQueue *jcondvar;


/* 
 * Locking API
 */
extern void jmutex_initialise(jmutex *lock);
extern void jmutex_lock(jmutex *lock);
extern void jmutex_unlock(jmutex *lock);
extern void jmutex_destroy(jmutex *lock);

extern void jcondvar_initialise(jcondvar *cv);
extern jboolean jcondvar_wait(jcondvar *cv, jmutex *lock, jlong timeout);
extern void jcondvar_signal(jcondvar *cv, jmutex *lock);
extern void jcondvar_broadcast(jcondvar *cv, jmutex *lock);
extern void jcondvar_destroy(jcondvar *lock);

#endif /* __lock_impl_h */
