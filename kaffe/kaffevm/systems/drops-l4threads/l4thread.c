/*
 * l4thread.c
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

#include <l4/sys/rt_sched.h> /* realtime extension */
#include <l4/semaphore/semaphore.h>
#include <l4/log/l4log.h>
#include <l4/util/macros.h> /* l4util_idfmt, l4util_idstr */
#include <l4/util/rdtsc.h> /* l4_rdtsc */

#include "debug.h"
#include "jthread.h"
#include "l4thread.h"
#include "gc.h"

#ifdef REALTIME_EXTENSION
#include <l4/dm_generic/types.h>
#include <l4/dm_mem/dm_mem.h>
#include <l4/dm_phys/dm_phys.h>
#include <l4/sys/rt_sched.h>
#include <l4/util/kip.h>
#include "realtimethread.h"
#endif

__leaflet leaflet = { 0, 0,
                      #ifdef REALTIME_EXTENSION
                      0, /* noheapthreadcount */
                      #endif	
                      -1, 
                      L4SEMAPHORE_INITIALIZER(1),
                      L4SEMAPHORE_INITIALIZER(1),
                      L4_INVALID_ID, 
                      0, 0, 0, 0x05
#ifdef REALTIME_EXTENSION
                     ,L4_INVALID_ID
                     ,NULL //handler to call if overrun situation
                     ,0    //cost(wcet) overruns
                     ,0    //deadline overruns
#endif
                    };


#ifdef REALTIME_EXTENSION
static l4_uint32_t  area;
#define L4KAFFE_PERIODIC 0x2U

#endif

volatile jthread_t            live        = 0;
static volatile unsigned long sleepcount  = 0;

void _l4threads_suspendall(void);
void _l4threads_resumepoint(void);
void _l4threads_suspendall_c(void) NONRETURNING;

void _l4threads_suspendall_c(void)
{
  l4_msgdope_t result;
  if (sleepcount == leaflet.threadCount)
  {
    {
	  l4_ipc_send(leaflet.gc,
              L4_IPC_SHORT_MSG,
              0, //l4_umword_t  	  snd_dword0,
              0, //l4_umword_t  	  snd_dword1,
	      L4_IPC_NEVER,
              &result);
    } while (L4_IPC_ERROR(result) == L4_IPC_SECANCELED ||
             L4_IPC_ERROR(result) == L4_IPC_SEABORTED);
  }
  while (1) l4thread_sleep_forever();
} 

asm("_l4threads_suspendall:       \n\t"
     "pushl $0x0                  \n\t" /*space for return address*/
     "pushfl                      \n\t" /*eflags 32 bit*/
     "pushal                      \n\t" /*eax,ecx,edx,ebx,esp,ebp,esi,edi*/
     "incl sleepcount             \n\t"
     "jmp _l4threads_suspendall_c \n\t"
     "_l4threads_resumepoint:     \n\t" // resume here, with ex_regs
     "popal                       \n\t"
     "popfl                       \n\t"
     "retl");


/*
 * prevent all other threads from running
 */
void 
jthread_suspendall(void)
{
 l4_umword_t   old_eflags;
 l4_umword_t   dummy;
 l4_msgdope_t  result;
 l4_threadid_t src;
 l4_threadid_t preempter  = L4_INVALID_ID;
 l4_threadid_t pager      = L4_INVALID_ID;
 jthread_t     current    = live;
 l4thread_t    myself     = l4thread_myself();

 leaflet.gc = l4_myself();

// LOG("suspendall");
 /*
  * ex-regs all threads and save their state
  */
 while (current)
  {
    /**
     * NoHeapRealtimeThreads will not be stopped
     */
    if (current->daemon==0 &&
        #ifdef REALTIME_EXTENSION
        current->threadtype != THREADTYPE_NOHEAPREAL &&
        #endif
        !l4thread_equal(current->l4thread,myself))
    { 
      l4_thread_ex_regs(l4thread_l4_id(current->l4thread),
                        (unsigned long)_l4threads_suspendall,
                        (l4_umword_t)-1,
                        &preempter,
                        &pager,
                        &old_eflags,&current->eip,&current->esp);
    }
    current = current -> next;
  }

 /*
  * Waiting for the notification IPC of the last stopped thread
  */
 {
   l4_ipc_wait(&src, L4_IPC_SHORT_MSG, &dummy, &dummy,
                           L4_IPC_NEVER,&result);
 }while(!l4_tasknum_equal(leaflet.gc, src) ||
         L4_IPC_ERROR(result) == L4_IPC_RECANCELED ||
         L4_IPC_ERROR(result) == L4_IPC_REABORTED);

// LOG("suspendall-complete");
}

/*
 * restore threads
 */
void
jthread_unsuspendall(void)
{
 volatile unsigned long int * ret_addr;
 l4_umword_t   old_eflags,dummy;
 l4_threadid_t preempter = L4_INVALID_ID;
 l4_threadid_t pager     = L4_INVALID_ID;
 l4thread_t    myself    = l4thread_myself();
 jthread_t     current   = live;

 while (current)
 {
   /**
    * NoHeapRealtimeThreads are never stopped and therefore they are not resumed
    */
   if (current->daemon==0 &&
       #ifdef REALTIME_EXTENSION
       current->threadtype != THREADTYPE_NOHEAPREAL &&
       #endif
       !l4thread_equal(current->l4thread,myself))
     {
       current->esp -= 0x04; 
       /* stack address for the return address*/
       ret_addr      = (unsigned long int *)current->esp;
       /* replace place holder with the eip*/
       *ret_addr     = current->eip;
       l4_thread_ex_regs(l4thread_l4_id(current->l4thread),
                         (unsigned long)_l4threads_resumepoint,
                         current->esp-0x24,
                         &preempter, &pager,
                         &old_eflags, &dummy, &dummy);
  }
  current = current -> next;
 }

 sleepcount = 0;

}

bool jthread_detach_current_thread (void)
{
 jthread_t current= jthread_current();

 printf("detach ....\n");
 KGC_rmRef((Collector *)leaflet.threadCollector, current);
 return true;
}

int jthread_extract_stack(jthread_t jtid, void **from, unsigned *len)
{
  l4_umword_t esp,eip,eflags;
  l4_addr_t low,high;
  l4_threadid_t pager     = L4_INVALID_ID;
  l4_threadid_t preempter = L4_INVALID_ID;

  if (l4thread_get_stack(jtid->l4thread,&low,&high)){
    assert(!"l4thread_get_stack() problem!");
  } else {
    l4_thread_ex_regs_flags(l4thread_l4_id(jtid->l4thread),
                            (l4_umword_t)-1, (l4_umword_t)-1,
                            &preempter, &pager, &eflags, &eip, &esp,
                            L4_THREAD_EX_REGS_NO_CANCEL);
    *from = (void *)esp;
    *len  = high - esp;
  }
  
  return (1);
}

/*
 * free a thread context
 */
void    
jthread_destroy(jthread_t tid UNUSED)
{
//  leaflet->memCollector.deallocator(tid);
}

/*
 * iterate over all live threads
 */
void
jthread_walkLiveThreads (void(*func)(jthread_t,void*), void *priv)
{
  jthread_t next    = live;

  while (next)
  { 
    #ifdef REALTIME_EXTENSION
    if (next->threadtype != THREADTYPE_NOHEAPREAL)
    #endif
      func(next,priv);

    next = next->next;
  }
}

void
jthread_walkLiveThreads_r(void (*func)(jthread_t, void*), void *priv)
{
  jthread_walkLiveThreads(func, priv);
}

#ifdef REALTIME_EXTENSION
/*
 * set a function to be run when deadline, period or wcet is missed/overruned
 */
void jthread_misshandler(void (*f)(jthread_t, unsigned), unsigned costoverrun,
                         unsigned deadlineoverrun)
{
  leaflet.misshandler  = f;
  leaflet.wcetover     = costoverrun;
  leaflet.deadover     = deadlineoverrun;
}

/* 
 * Handling of preemption IPC and notification of the Kaffe VM
 */
static void jthread_preemption(l4_threadid_t thread, unsigned type)
{
  jthread_t data = (jthread_t)l4thread_data_get(l4thread_id(thread), leaflet.datakey);

  if (leaflet.misshandler != NULL && data != NULL)
    leaflet.misshandler(data, type);
  else
    LOG("drop one preemption IPC"); 

}
 
/**
 * preempter thread for each Java thread
 */
static void NONRETURNING jthread_preempter(void * obj)
{
  l4_rt_preemption_t dw;
  l4_threadid_t src;
  l4_msgdope_t result;
  int error;
  l4_threadid_t self = l4_myself();

  l4thread_started(obj);

  while (1)
  {
      /* wait for request */
      error = l4_ipc_wait(&src,
                          L4_IPC_SHORT_MSG,
                          &dw.lh.low,
                          &dw.lh.high,
                          L4_IPC_NEVER,
                          &result);

      if (error == 0 && leaflet.misshandler != NULL) {
        if (l4_tasknum_equal(self, src)) {
          switch(dw.p.type){
            case L4_RT_PREEMPT_TIMESLICE:
//              LOG("TimeSlice "l4util_idfmt"/%d", l4util_idstr(src), dw.p.id);

              jthread_preemption(src, leaflet.wcetover);

              break;
            case L4_RT_PREEMPT_DEADLINE:
//              LOG("DeadLine "l4util_idfmt, l4util_idstr(src));

              jthread_preemption(src, leaflet.deadover);

              break;
            default:
              LOG("unknown preemption");
          }
        }
      }
  }
}
#endif

/*
 * Initialize the threading system.
 *
 */
jthread_t 
jthread_init(int pre UNUSED,
	int maxpr,  int minpr UNUSED,
        Collector *threadCollector,
	void (*_onthreadexit)(void*),
	void (*_onstop)(void) UNUSED,
	void (*_ondeadlock)(void) UNUSED)
{
        volatile jthread_t athread;

        #ifdef REALTIME_EXTENSION
        l4_threadid_t pager      = L4_INVALID_ID;
        l4_threadid_t preempter;
        l4_umword_t   dummy;

        /* start the preempter thread */
        leaflet.preempter = l4thread_l4_id(
          l4thread_create_long(L4THREAD_INVALID_ID,
	                       jthread_preempter,
		               0,
			       L4THREAD_INVALID_SP,
			       L4THREAD_DEFAULT_SIZE, //threadStackSize,
			       leaflet.startPrio + maxpr,
			       0,
			       L4THREAD_CREATE_SYNC));  

        #endif

        leaflet.threadCollector   = threadCollector;
        leaflet.onthreadexit      = _onthreadexit; /* function to call, if a thread dies*/
        leaflet.daemonCount       = 0; /* count of all daemon threads */
        leaflet.threadCount       = 1; /* count of all threads */
        #ifdef REALTIME_EXTENSION
        leaflet.noheapthreadCount = 0; /* count of all NoHeapRealtime threads */
        #endif

        athread = KGC_malloc(threadCollector, sizeof (*athread), KGC_ALLOC_THREADCTX);
        KGC_addRef(threadCollector, athread);

        athread->l4thread     = l4thread_myself();
        athread->prev         = NULL;
        athread->next         = NULL;
        athread->daemon       = 0; // no daemon thread
	athread->interrupting = 0;
        #ifdef REALTIME_EXTENSION
        athread->status       = 0;
        #endif

	assert(live == NULL);

        live = athread;

        /* allocate data key */
        leaflet.datakey = l4thread_data_allocate_key();
        if (leaflet.datakey < 0) assert(!"l4thread_data_allocate_key failed");

        if (l4thread_data_set_current(leaflet.datakey,athread) < 0) assert(!"l4thread_data_set_current failed");

        #ifdef REALTIME_EXTENSION
        /* set the preempter for the current thread */
        preempter = leaflet.preempter;

        l4_thread_ex_regs(l4_myself(), (l4_umword_t)-1, (l4_umword_t)-1,
                          &preempter, &pager,
                          &dummy,  &dummy,  &dummy);
        #endif

	return athread;
}

/**
 * All new Threads run this function on start.
 * It prevents the VM to call jthread_current() and other functions,
 * before jthread_create() has queued the new thread. 
 */
static void start_kaffe_l4thread(void *data UNUSED){

  jthread_t current;

  l4semaphore_down(&leaflet.threadLock);
  // prevent the VM to call jthread_current(), when creating a new thread
  l4semaphore_up(&leaflet.threadLock);

  /* register thread for handling ArithmeticException
     and Null Object accesses */
  drops_registerForExceptions();

  current = jthread_current();
  /* call start function provided by the JVM */
  current->func(current->data.jlThread);
}

/*
 * Create an jthread context to go with the initial thread in the system.
 * (The initial thread is created in jthread_init, above).
 */
jthread_t
jthread_createfirst(size_t mainThreadStackSize UNUSED,
                    unsigned int prio,
                    void* jlThread)
{
  volatile jthread_t current = jthread_current();
  current->data.jlThread = jlThread;
  //set priority
  assert(l4thread_set_prio(current->l4thread, (signed)prio + leaflet.startPrio)==0);

  return current;
}

/*
 * set a function to be run when all non-daemon threads have exited
 */
void
jthread_atexit(void (*f)(void))
{
  leaflet.runatexit = f;
}

/*
 * create a new thread
 */
jthread_t
jthread_create(unsigned int pri, void (*func)(void *), int isDaemon,
	       void *jlThread, size_t threadStackSize UNUSED)
{
  volatile jthread_t next;
  volatile jthread_t athread = KGC_malloc((Collector *)leaflet.threadCollector,
                                           sizeof (*athread),
                                           KGC_ALLOC_THREADCTX);

  KGC_addRef((Collector *)leaflet.threadCollector, athread);

  athread->next          = 0;
  athread->data.jlThread = jlThread;
  athread->daemon        = isDaemon;
  athread->func          = func;
  athread->interrupting	 = 0;
  #ifdef REALTIME_EXTENSION
  athread->status        = 0;
  athread->threadtype    = THREADTYPE_NORMAL; // normal thread
  #endif

  l4semaphore_down(&leaflet.threadLock);

  athread->l4thread      = l4thread_create_long(L4THREAD_INVALID_ID,
				start_kaffe_l4thread,
				0,
				L4THREAD_INVALID_SP,
				L4THREAD_DEFAULT_SIZE, //threadStackSize,
				leaflet.startPrio+(signed)pri,
				0,
				L4THREAD_CREATE_ASYNC);  

  if (isDaemon) leaflet.daemonCount ++;
  else leaflet.threadCount ++;

  next    = live;
  while (next->next) next = next->next;
  athread->prev = next;
  next->next = athread;

  if (l4thread_data_set(athread->l4thread,leaflet.datakey,athread) < 0)
    assert(!"l4thread_data_set_current failed");

  l4semaphore_up(&leaflet.threadLock);

//  LOG("start thread "l4util_idfmt" %p", l4util_idstr(l4thread_l4_id(athread->l4thread)),&athread->data.sem);
  return athread;
}

#ifdef REALTIME_EXTENSION
/*
 * create a new realtime thread 
 */
jthread_t
jthread_createRT(unsigned int pri, void (*func)(void *), int isDaemon,
	         void *jlThread, size_t threadStackSize UNUSED, int threadtype)
{
  l4_threadid_t pager      = L4_INVALID_ID;
  l4_threadid_t preempter;
  l4_umword_t   dummy;

  volatile jthread_t next;
  volatile jthread_t athread = KGC_malloc((Collector *)leaflet.threadCollector,
                                          sizeof (*athread),
                                          KGC_ALLOC_THREADCTX);

  KGC_addRef((Collector *)leaflet.threadCollector, athread);

  athread->next          = 0;
  athread->data.jlThread = jlThread;
  athread->daemon        = isDaemon;
  athread->func          = func;
  athread->threadtype    = threadtype;
  athread->status        = 0;  		
  athread->interrupting  = 0;

  l4semaphore_down(&leaflet.threadLock);

  athread->l4thread      = l4thread_create_long(L4THREAD_INVALID_ID,
				start_kaffe_l4thread,
				0,
				L4THREAD_INVALID_SP,
				L4THREAD_DEFAULT_SIZE, //threadStackSize,
				leaflet.startPrio+(signed)pri,
				0,
				L4THREAD_CREATE_ASYNC);  

  assert(!(athread->threadtype == THREADTYPE_NOHEAPREAL && isDaemon));
  
  if (athread->threadtype == THREADTYPE_NOHEAPREAL)
    leaflet.noheapthreadCount ++;
  else
   if (isDaemon) leaflet.daemonCount ++;
   else leaflet.threadCount ++;


  next    = live;
  while (next->next) next = next->next;
  athread->prev = next;
  next->next = athread;

  if (l4thread_data_set(athread->l4thread,leaflet.datakey,athread) < 0) assert(!"l4thread_data_set_current failed");

  /* set the preempter for the current thread */
  preempter = leaflet.preempter;

  l4_thread_ex_regs_flags(l4thread_l4_id(athread->l4thread),
                          (l4_umword_t)-1, (l4_umword_t)-1,
                          &preempter, &pager,
                          &dummy, &dummy, &dummy,
                          L4_THREAD_EX_REGS_NO_CANCEL);

  l4semaphore_up(&leaflet.threadLock);

  return athread;
}
#endif

/*
 * Java thread exit.
 */
void
jthread_exit(void)
{
  int last = 0;
  jthread_t current = jthread_current();

  if (leaflet.onthreadexit) (*leaflet.onthreadexit)(current->data.jlThread);

  l4semaphore_down(&leaflet.threadLock);

  #ifdef REALTIME_EXTENSION
  if (current->threadtype == THREADTYPE_NOHEAPREAL)
    leaflet.noheapthreadCount --;
  else
  #endif
    if (current->daemon) leaflet.daemonCount --;
    else leaflet.threadCount --;

  if (leaflet.threadCount == 0
  #ifdef REALTIME_EXTENSION
      && leaflet.noheapthreadCount == 0
  #endif
      && !current->daemon) //only daemon threads left, shut down the VM
    last = 1;

  if (current->prev == 0){
    assert(live == current);
    if (current->next == 0) live = 0; //May or can it ever happen ?
    else {
      live = current->next;
      live->prev = 0;
    }
  }else{
    current->prev->next = current->next;
    if (current->next){
      current->next->prev = current->prev;
    }
  }
  l4semaphore_up(&leaflet.threadLock);

  #ifdef REALTIME_EXTENSION
  if (current->status & L4KAFFE_PERIODIC) 
    jthread_stopPeriodic();
  #endif

  if (last){ //only daemon threads left, shut down the VM
    leaflet.runatexit();
    exit(0);
  } 

//  LOG("start thread "l4util_idfmt, l4util_idstr(l4thread_l4_id(current->l4thread)));
  l4thread_sleep_forever();
//  l4thread_exit(); 
  while (1) assert(!"This better not return");
}

#ifdef REALTIME_EXTENSION
void * jthread_reservearea(l4_size_t size)
{ 
  l4_addr_t addr = 0;
  int err;

  err = l4rm_area_reserve(size,
                          L4RM_LOG2_ALIGNED,
                          &addr,
                          &area
                         );

  assert( err == 0);

  return (void *)addr;
}

void * jthread_createarea(l4_size_t size)
{
  l4dm_dataspace_t ds = L4DM_INVALID_DATASPACE_INITIALIZER;
  int err;
  void *addr1           = 0;


  err = l4dm_mem_open(L4DM_DEFAULT_DSM,
                      size,
                      0, //alignment
                      0, //flags
                      "test1",
                      &ds);

  assert( err == 0);

  err = l4rm_area_attach (&ds,
                          area,
                          size,
                          0,
                          L4DM_RW + L4RM_MAP,
                          &addr1);

  assert( err == 0);

  return addr1;
}
#endif

/* 
 * Check whether a thread is alive.
 */
int
jthread_alive(jthread_t tid UNUSED)
{
  printf("jthread_alive todo\n");
  return 0;
}

/*
 * Stop a thread
 */
void
jthread_stop(jthread_t jtid UNUSED)
{
  printf("jthread_stop todo\n");
}
/*      
 * sleep for time milliseconds
 */     
void
jthread_sleep(jlong jtime UNUSED)
{
  printf("jthread_sleep todo\n");
}

/*
 * Print info about a given jthread to stderr
 */
void 
jthread_dumpthreadinfo(jthread_t tid UNUSED)
{
  printf("jthread_dumpthreadinfo todo\n");
}

int
jthread_frames(jthread_t thrd UNUSED)
{
 printf("jthread_frames\n");
 return 0;
}

void jthread_set_blocking (int fd UNUSED, int blocking UNUSED){
  printf("todo -jthread_set_blocking \n");

}

#ifdef REALTIME_EXTENSION
jint jthread_startPeriodic(jlong start_ms,
                          jint  start_ns,
                          jlong period_ms,
                          jint  period_ns,
                          jlong wcet_ms,
                          jint  wcet_ns)
{ 
  // basis us - transform ms and ns values
  int               timeslice;  // =   wcet_ms * 1000;// +   wcet_ns / 1000;
  l4_kernel_clock_t task_period;// = period_ms * 1000;// + period_ns / 1000;
  l4_uint64_t       current;
  l4_threadid_t     dest        = l4_myself();
  int               prio        = l4thread_get_prio(l4thread_myself());
  jthread_t         thread      = jthread_current();
  l4_kernel_info_t  * kip;

  // basis us - transform ms and ns values
  timeslice   =   wcet_ms * 1000 +   wcet_ns / 1000;
  task_period = period_ms * 1000 + period_ns / 1000;

  thread->status = thread->status | L4KAFFE_PERIODIC;

  LOG("s: %lld %ld p: %lld %ld wcet: %lld %ld", (long long)start_ms, (long)start_ns,
                                                (long long)period_ms, (long)period_ns,
                                                (long long)wcet_ms, (long)wcet_ns);
  if (!l4_rt_add_timeslice(dest, prio, timeslice))
    {
      l4_rt_set_period(dest, task_period);

      kip         = l4util_kip_map();
      assert(kip != NULL);

      current     = kip->clock + start_ms * 1000 + start_ns / 1000;
      
      if (l4_rt_begin_strictly_periodic(dest, current))
        return 1; //error

      if (!l4_rt_next_period())
        return 0; //ok
    }

  return 1; //error
}

void jthread_stopPeriodic()
{
  jthread_t         thread      = jthread_current();

  l4_threadid_t     dest        = l4_myself();
  l4_rt_end_periodic(dest);

  thread->status = thread->status & ~L4KAFFE_PERIODIC;
}

void jthread_nextPeriod()
{
  assert(l4_rt_next_period() == 0);
}
#endif //REALTIME_EXTENSION
