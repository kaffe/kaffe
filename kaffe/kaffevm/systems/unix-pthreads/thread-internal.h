/*
 * thread-impl.h - pthread based ThreadInterface implementation
 *
 * Copyright (c) 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

#ifndef __thread_internal_h
#define __thread_internal_h

#include <pthread.h>
#include <semaphore.h>

#include "java_lang_Thread.h"


/*
 * 'nativeThread' is our link between native and Java thread objects.
 * It also serves as a container for our pthread extensions (namely
 * enumeration, and inter-thread suspend)
 */
typedef struct _nativeThread {
  /* these are our links to the native pthread implementation */
  pthread_t             tid;
  pthread_attr_t        attr;

  /* this is our Java Thread object */
  Hjava_lang_Thread     *thread;

  /* convars and mutexes aren't useful in signal handlers, semaphores are */
  sem_t                 sem;

  /* the following fields hold our extensions */
  int                   active;         /* are we in our user thread function 'func'? */
  int                   suspendState;   /* are we suspended for a critSection?  */
  int                   blockState;     /* are we in a Lwait or Llock (can handle signals)? */

  void                  (*func)(void*);  /* this kicks off the user thread func */
  void                  *stackMin;
  void                  *stackCur;      /* just useful if blocked or suspended */
  void                  *stackMax;

  struct _nativeThread  *next;
} nativeThread;

typedef nativeThread*	jthread_t;

/* suspend states (these are exclusive) */
#define SS_PENDING_SUSPEND   0x01  /* suspend signal has been sent, but not handled */
#define SS_SUSPENDED         0x02  /* suspend signal has been handled */
#define SS_PENDING_RESUME    0x04  /* resume signal  has been sent */


/* blocking states (might be accumulative) */
#define BS_THREAD            0x01  /* blocked on tLock (thread system internal) */
#define BS_MUTEX             0x02  /* blocked on a external mutex lock */
#define BS_CV                0x04  /* blocked on a external convar wait */
#define BS_CV_TO             0x08  /* blocked on a external convar timeout wait */


extern pthread_key_t   ntKey;
extern nativeThread    *activeThreads;

extern void tDump (void);


/*
 * This should really be as fast as possible (and a VM-public compile time
 * interface)
 */
#define GET_CURRENT_THREAD(_stackadr) \
  (nativeThread*) pthread_getspecific( ntKey)

#define NATIVE_THREAD(_jthread) \
  ((nativeThread*) unhand(_jthread)->PrivateInfo)


/* debugging and log helpers */
#if defined (KAFFE_VMDEBUG)

extern char stat_act[];
extern char stat_susp[];
extern char stat_block[];

#define TMSG_SHORT(_msg,_nt)     \
   (_msg" %p [tid:%d, java:%p]\n", \
    _nt, _nt->tid, _nt->thread)

#define TMSG_LONG(_msg,_nt)      \
   (_msg" %p [tid:%d, java:%p], stack [%p..%p..%p], state: %c%c%c\n",         \
	_nt, _nt->tid, _nt->thread, _nt->stackMin, _nt->stackCur, _nt->stackMax,  \
	stat_act[_nt->active], stat_susp[_nt->suspendState], stat_block[_nt->blockState])

#define CHECK_CURRENT_THREAD(_nt)                                          \
  if ( ((uintp) &_nt < (uintp) _nt->stackMin) ||           \
       ((uintp) &_nt > (uintp) _nt->stackMax) ) {          \
    printf( "?? inconsistent current thread: %x [tid: %d, java: %x]\n",    \
	  	    _nt, _nt->tid, _nt->thread);                                   \
    tDump();                                                               \
  }

#endif /* KAFFE_VMDEBUG */

#endif /* __thread_impl_h */
