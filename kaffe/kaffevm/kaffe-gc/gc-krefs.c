/* gc-refs.c
 * Maintain set of references in the garbage collector root set.
 * This is independent of the actual collection mechanism
 *
 * Copyright (c) 1996, 1997, 1998, 1999
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * Copyright (c) 2004
 *      The Kaffe.org's developers. See ChangeLog for details.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#include "config.h"
#include "debug.h"
#include "config-std.h"
#include "config-mem.h"
#include "gtypes.h"
#include "gc.h"
#include "locks.h"
#include "thread.h"
#include "jthread.h"
#include "errors.h"
#include "md.h"
#include "gc-incremental.h"
#include "gc-refs.h"
#include "java_lang_Thread.h"
#include "locks.h"

/*
 * Walk the thread's internal context.
 * This is invoked by the garbage collector thread, which is not
 * stopped.
 *      
 * We will iterate through all threads, including the garbage collector
 * and those threads that haven't been started yet.
 */     
static void    
TwalkThread(Collector* collector, jthread_t jtid)
{       
  void *from;
  unsigned len;  
        
  /* Don't walk the gc thread's stack.  It was not stopped and
   * we hence don't have valid sp information.  In addition, there's
   * absolutely no reason why we should walk it at all.
   */
  if (jtid == KTHREAD(current)()) {
    DBG(JTHREAD,
	dprintf("%p NOT walking jtid %p\n", KTHREAD(current)(), jtid);
	); 
      return;
  }
        
  /* Ask threading system what the interesting stack range is;
   * If the thread is too young, the threading system will return
   * 0 from extract_stack.  In that case, we don't have walk anything.
   */
  if (KTHREAD(extract_stack)(jtid, &from, &len)) {
    DBG(JTHREAD|DBG_GCWALK,
	dprintf("walking stack of `%s' thread\n", nameThread(KTHREAD(get_data)(jtid)->jlThread));
	);
    /* and walk it if needed */
    KaffeGC_WalkConservative(collector, from, len);
  }
}

static Collector *running_collector;	

/* XXX will be fixed on thread interface gets fixed.
 * The best way to fix this is not to use walkThreads at all.
 *
 * That very much depends on what best means.  If we don't explicitly
 * walk threads while pushing roots, we are stuck calling
 * soft_instanceof in walkObject, which is a big source of overhead.
 */
static void
liveThreadWalker(jthread_t tid, void *private)
{
  Collector *c = (Collector *)private;
  threadData *thread_data = KTHREAD(get_data)(tid);

  /* if the gc is invoked while a new thread is being
   * initialized, we should not make any assumptions
   * about what is stored in its thread_data 
   */
  if (THREAD_DATA_INITIALIZED(thread_data))
    {
      Hjava_lang_VMThread *thread = (Hjava_lang_VMThread *)thread_data->jlThread;
      jnirefs *table;

      KGC_markObject(c, NULL, unhand(thread)->thread);
      KGC_markObject(c, NULL, thread);

      for(table = thread_data->jnireferences; table != NULL; table = table->prev)
	{
	  int i;

	  for (i = 0; i < table->frameSize; i++)
	    if (table->objects[i] != NULL)
	      KGC_markObject(c, NULL, table->objects[i]);
	}
  
      if (thread_data->exceptObj != NULL)
        {
          KGC_markObject(c, NULL, thread_data->exceptObj);
        }
    }

  TwalkThread(c, tid);
}

/*
 * Walk the set of registered root references.  
 * This is invoked at the beginning of each GC cycle. 
 */
void
KaffeGC_walkRefs(Collector* collector)
{
DBG(GCWALK,
    dprintf("Walking gc roots...\n");
    );

 KaffeGC_markAllRefs(collector);

DBG(GCWALK,
    dprintf("Walking live threads...\n");
    );

 running_collector = collector;
 /* Walk the thread objects as the threading system has them
  * registered.  Terminating a thread will remove it from the
  * threading system, and then we won't walk it here anymore
  */
 KTHREAD(walkLiveThreads_r)(liveThreadWalker, collector);
 DBG(GCWALK,
     dprintf("Following references now...\n");
     );
}
