/*
 * l4thread.h
 * Internal threading backend of Kaffe for DROPS
 * 
 * Copyright (c) 2004, 2005
 *	TU Dresden, Operating System Group.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 *
 * written by Alexander Boettcher <ab764283@os.inf.tu-dresden.de>
 */

#ifndef __l4_thread_h
#define __l4_thread_h

#include <l4/sys/types.h>
#include <l4/semaphore/semaphore.h>

typedef struct _leaflet{
 volatile unsigned int daemonCount;       // all daemon threads of the VM
 volatile unsigned int threadCount;       // all nondaemon threads of the VM
#ifdef REALTIME_EXTENSION
 volatile unsigned int noheapthreadCount; // all NoHeapRealtimeThreads
#endif
 int datakey;                             // key for the thread-local-data
 /* semaphore for jthread_create & jthread_exit */
 l4semaphore_t threadLock;
 /* Semaphore for jthread_spinon, jthread_spinoff.
  * Kaffe's AWT QT implementation uses malloc, which is
  * not threadsafe for backend libdiet_be_simple_mem.
  */
 l4semaphore_t systemLock;
// l4semaphore_t gcLock;
 volatile l4_threadid_t gc;               // Thread ID of the Garbage Collector
 void * threadCollector;                  // memory
 void (*onthreadexit)(void*);             // function to call, if a thread dies
 void (*runatexit)(void);                 // function to call, if only daemon-threads alive
 int startPrio;
#ifdef REALTIME_EXTENSION
 l4_threadid_t preempter;                 // preempter thread
 // handler for overrun period, wcet or missing deadline
 void (*misshandler)(jthread_t, unsigned);
 unsigned wcetover;
 unsigned deadover;
#endif
} __leaflet;

#endif
