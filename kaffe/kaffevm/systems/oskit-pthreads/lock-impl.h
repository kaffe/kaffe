/*
 * Copyright (c) 1999 The University of Utah.  All rights reserved.
 *
 * This file is licensed under the terms of the GNU Public License.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file.
 */

#ifndef __lock_impl_h
#define __lock_impl_h

#define	SETUP_POSIX_LOCKS(L) \
	(L)->mux = thread_malloc(sizeof(jmutex)); \
        (L)->cv = thread_malloc(sizeof(jcondvar)); \
        jmutex_initialise((L)->mux); \
        jcondvar_initialise((L)->cv);

#define SEMGET          _SemGet
#define SEMPUT          _SemPut
#define LOCK(L)         jmutex_lock((L)->mux)
#define UNLOCK(L)       jmutex_unlock((L)->mux)
#define SIGNAL(L)       jcondvar_signal((L)->cv, (L)->mux)
#define WAIT(L,T)       (!(jcondvar_wait((L)->cv, (L)->mux, (T))))

#endif /* __lock_impl_h */
