/*
 * lock-impl.h
 * Mapping of L4Env specific semaphore packet to Kaffe's locking interface
 * 
 * Copyright (c) 2004, 2005
 *	TU Dresden, Operating System Group.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 *
 * written by Alexander Boettcher <ab764283@os.inf.tu-dresden.de>
 */

#ifndef __l4_lock_impl_h
 #define __l4_lock_impl_h

 // no jmutex/jcondvar implementation necessary
 #define THREAD_SYSTEM_HAS_KSEM

 #include <l4/semaphore/semaphore.h>

 typedef int jmutex;
 typedef int jcondvar;
 typedef struct Ksem{
   l4semaphore_t l4sem;
   jboolean failed; 
 } Ksem;

 static inline void ksem_init(struct Ksem* sem)
 {
   sem->l4sem  = L4SEMAPHORE_INIT(0);
   sem->failed = false;
 }

 static inline void ksem_put(struct Ksem* sem)
 {
   l4semaphore_up(&sem->l4sem);
 }

 static inline jboolean ksem_get(struct Ksem* sem, jlong timeout)
 {
   jboolean ret = true;

   if (timeout == 0){
     l4semaphore_down(&sem->l4sem);
   } else {
     ret = !(l4semaphore_down_timed(&sem->l4sem,(unsigned)timeout));
   }

   if (sem->failed == true) {
     ret = sem->failed = false;
   }
   
   return ret;
 }

 static inline void ksem_destroy(struct Ksem* sem)
 {
   sem->l4sem  = L4SEMAPHORE_INIT(0);
   sem->failed = false;
 }

#endif /* _drops_lock_impl_h */
