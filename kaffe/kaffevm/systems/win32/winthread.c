/*
 * winthread.c
 *
 * Copyright (c) 1999
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

#define	SDBG(s)

#include "config.h"
#include "config-std.h"
#include "config-mem.h"
#include "gtypes.h"
#include "thread.h"
#include "locks.h"
#include "gc.h"
#include "support.h"
#if defined(HAVE_SYS_TIMEB_H)
#include <sys/timeb.h>
#endif

extern void walkMemory(void*);
extern void markObject(void*);
extern void walkConservative(void*, int);

#include "winthread.h"
#define NOUNIXPROTOTYPES
#include "jsyscall.h"

static Hjava_lang_Thread* TcurrentJava(void);

jboolean runFinalizerOnExit;
jint nonDemonThreads;
static Hjava_lang_Thread* activeThreads;

/* Pure guess work !!! */
#define	STACK_TOP(S)	((((uintp)(S)) + 0x1000 - 1) & -0x1000)

jthread_t
jthread_init(void)
{
	nativeThread* ptr;
	uintp stkp;
	char* mem;

	ptr = (nativeThread*)gc_malloc_fixed(sizeof(nativeThread));

	ptr->task = GetCurrentThreadId();
	ptr->hand = GetCurrentThread();
	ptr->sem = CreateSemaphore(NULL, 0, 1, NULL);

	/* Calculate the stack start and end */
	__asm
	{
		mov stkp, esp
	}

	// printf("Current stkp = %x\n", stkp);
	stkp = STACK_TOP(stkp);
	ptr->stkend = stkp;
	ptr->stkbase = ptr->stkend - threadStackSize;

	/* Touch the base of stack to get it allocated */
	mem = alloca(threadStackSize);
	mem[threadStackSize - 1] = 0;

	// printf("stack: %x - %x\n", ptr->stkbase, ptr->stkend);

	ptr->next = activeThreads;
	// activeThreads = tid;

	nonDemonThreads = 1;

	return (ptr);
}

static
DWORD
WINAPI
startNewThread(void* arg)
{
	Hjava_lang_Thread* tid;
	nativeThread* ntid;
	uintp stkp;

	tid = TcurrentJava();
	ntid = NATIVE_THREAD(tid);

	/* Calculate the stack start and end */
	__asm
	{
		mov stkp, esp
	}

	stkp = STACK_TOP(stkp);
	ntid->stkend = stkp;
	ntid->stkbase = ntid->stkend - threadStackSize;

	ntid->func();

	return (0);
}

void    
jthread_setpriority(jthread_t tid, jint prio)
{
	switch (prio) {
	case 1:
		prio = THREAD_PRIORITY_IDLE;
		break;
	case 2:
		prio = THREAD_PRIORITY_LOWEST;
		break;
	case 3:
	case 4:
		prio = THREAD_PRIORITY_BELOW_NORMAL;
		break;
	case 5:
	default:
		prio = THREAD_PRIORITY_NORMAL;
		break;
	case 6:
	case 7:
	case 8:
		prio = THREAD_PRIORITY_ABOVE_NORMAL;
		break;
	case 9:
	case 10:
		prio = THREAD_PRIORITY_HIGHEST;
		break;
	case 11:
		prio = THREAD_PRIORITY_TIME_CRITICAL;
		break;
	}

	SetThreadPriority(tid->hand, prio);
}            

jthread_t
jthread_create(unsigned char pri, void (*func)(void*), int daemon, void* cookie, size_t stacksz)
{
	nativeThread* ptr;

	ptr = (nativeThread*)gc_malloc_fixed(sizeof(nativeThread));
	// unhand(tid)->PrivateInfo = (struct Hkaffe_util_Ptr*)ptr;

	ptr->func = (void(*)(void))func;
	ptr->hand = CreateThread(NULL, stacksz,
		startNewThread, NULL, CREATE_SUSPENDED, &ptr->task);
	ptr->sem = CreateSemaphore(NULL, 0, 1, NULL);
	jthread_setpriority(ptr, pri);

	// ptr->next = activeThreads;
	// activeThreads = tid;

	if (daemon == 0) {
		nonDemonThreads++;
	}

	ResumeThread(ptr->hand);
}

/*
 * Terminate current thread.
 */
void
jthread_exit(void)
{
	Hjava_lang_Thread** ptr;
	nativeThread* ntid;
	Hjava_lang_Thread* tid;

	tid = getCurrentThread();

	if (unhand(tid)->daemon == 0) {
		nonDemonThreads--;
		if (nonDemonThreads == 0) {
			exit(1);
		}
	}

	ntid = NATIVE_THREAD(tid);

	for (ptr = &activeThreads; *ptr != 0; ptr = &NATIVE_THREAD(*ptr)->next) {
		if (tid == *ptr) {
			*ptr = ntid->next;
			if (ntid->task == GetCurrentThreadId()) {
				/* wait for the finalizer to notify us to clean up
				 * (we can't do it right here, because we are going to kill our
				 * stack, which causes havoc to the gc/finalizer)
				 */
				SuspendThread(ntid->hand);

				/* Ok, suicide is painless.. */
				CloseHandle(ntid->sem);
				CloseHandle(ntid->hand);

				gc_free_fixed(ntid);
				unhand(tid)->PrivateInfo = 0;

				ExitThread(0);
				/* This should never return - but if it does ... */
				exit(1);
			}
			else {
				/* shouldn't happen anymore, not yet clear how to clean up if it does */
				CloseHandle((HANDLE)unhand(tid)->sem);

				TerminateThread(ntid->hand, 0);
			}
			return;
		}
	}
}

/*
 * Yield processor
 */
void
jthread_yield(void)
{
	Sleep(0);
}

void
jthread_sleep(jlong time)
{
	abort();	/* Deprecated */
}

void
jthread_stop(jthread_t jtid)
{
	abort();	/* Deprecated */
}

void
jthread_interrupt(jthread_t jtid)
{
	abort();	/* Deprecated */
}

void
jthread_frames(jthread_t jtid)
{
	abort();	/* Deprecated */
}

void
jthread_alive(jthread_t jtid)
{
	abort();	/* Deprecated */
}

void
jthread_suspendall(void)
{
	/* Does nothing */
}

void
jthread_unsuspendall(void)
{
	/* Does nothing */
}

/*
 * Thread is being finalized - free any held resource.
 */
static
void    
Tfinalize(Hjava_lang_Thread* tid)
{
	nativeThread* ntid = NATIVE_THREAD(tid);
	/* wake up, it's time to die */
	ResumeThread( ntid->hand);
}       

static          
Hjava_lang_Thread*      
TcurrentJava(void)      
{
	Hjava_lang_Thread* ptr;
	DWORD ct;

	ct = GetCurrentThreadId();
	for (ptr = activeThreads; ptr != 0; ptr = NATIVE_THREAD(ptr)->next) {
		if (NATIVE_THREAD(ptr)->task == ct) {
			return (ptr);
		}
	}
	return (0);
}

static          
void*
TcurrentNative(void)      
{
	return ((void*)GetCurrentThreadId());
}

void
jthread_walkLiveThreads(void (*func)(void*))
{               
	Hjava_lang_Thread* tid;

	// printf("Walking threads\n");
	for (tid = activeThreads; tid != NULL; tid = NATIVE_THREAD(tid)->next) {
		(*func)((void*)tid);
	}
}

void
jthread_extract_stack(jthread_t jtid, void** from, unsigned* len)
{
	*from = jtid->stkbase;
	*len = jtid->stkend - jtid->stakbase;
}
              
/*
 * Get a semaphore (with timeout).
 */
jboolean
Lsemget(void* sem, jlong timeout)
{
	DWORD tm;

SDBG(	kprintf("Lsemget: %x\n", sem);	)
	if (timeout == 0) {
		tm = INFINITE;
	}
	else {
		tm = (DWORD)timeout;
	}
	if (WaitForSingleObject((HANDLE)sem, tm) != WAIT_TIMEOUT) {
		return (true);
	}
	else {
		return (false);
	}
}

/*
 * Put a semaphore.
 */
void
Lsemput(void* sem)
{
SDBG(	kprintf("Lsemput: %x\n", sem);	)
	ReleaseSemaphore((HANDLE)sem, 1, NULL);
}
