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
#include "threads.h"
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

jbool runFinalizerOnExit;
jint nonDemonThreads;
static Hjava_lang_Thread* activeThreads;

/* Pure guess work !!! */
#define	STACK_TOP(S)	((((uintp)(S)) + 0x1000 - 1) & -0x1000)

static
void
TcreateFirst(Hjava_lang_Thread* tid)
{
	nativeThread* ptr;
	uintp stkp;
	char* mem;

	ptr = (nativeThread*)gc_malloc_fixed(sizeof(nativeThread));
	unhand(tid)->PrivateInfo = (struct Hkaffe_util_Ptr*)ptr;

	ptr->task = GetCurrentThreadId();
	ptr->hand = GetCurrentThread();
	unhand(tid)->sem = (void*)CreateSemaphore(NULL, 0, 1, NULL);

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
	activeThreads = tid;

	nonDemonThreads = 1;
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

static  
void    
Tprio(Hjava_lang_Thread* tid, jint prio)
{
	if (unhand(tid)->PrivateInfo == 0) {
		return;
	}
	switch (prio) {
	case 1:
		prio = THREAD_PRIORITY_IDLE;
		break;
	case 2:
		prio = THREAD_PRIORITY_LOWEST;
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

	SetThreadPriority(NATIVE_THREAD(tid)->hand, prio);
}            

static
void
Tcreate(Hjava_lang_Thread* tid, void* func)
{
	nativeThread* ptr;

	ptr = (nativeThread*)gc_malloc_fixed(sizeof(nativeThread));
	unhand(tid)->PrivateInfo = (struct Hkaffe_util_Ptr*)ptr;

	ptr->func = (void(*)(void))func;
	ptr->hand = CreateThread(NULL, threadStackSize,
		startNewThread, NULL, CREATE_SUSPENDED, &ptr->task);
	Tprio(tid, unhand(tid)->priority);

	unhand(tid)->sem = (void*)CreateSemaphore(NULL, 0, 1, NULL);

	ptr->next = activeThreads;
	activeThreads = tid;

	if (unhand(tid)->daemon == 0) {
		nonDemonThreads++;
	}

	ResumeThread(ptr->hand);
}

/*
 * Terminate current thread.
 */
static
void
Texit(Hjava_lang_Thread* tid)
{
	Hjava_lang_Thread** ptr;
	nativeThread* ntid;

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
				SuspendThread( ntid->hand);

				/* Ok, suicide is painless.. */
				CloseHandle((HANDLE)unhand(tid)->sem);
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
static
void
Tyield(void)
{
	Sleep(0);
}

static
void
Tstop(Hjava_lang_Thread* tid, HObject* exc)
{
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

static          
void
TwalkThreads(void)
{               
	Hjava_lang_Thread* tid;

	// printf("Walking threads\n");
	for (tid = activeThreads; tid != NULL; tid = NATIVE_THREAD(tid)->next) {
		walkMemory((void*)tid);
	}
}

/*      
 * Walk the thread's internal context.
 */
static
void
TwalkThread(Hjava_lang_Thread* tid)
{
	nativeThread* ntid;

	// printf("walking thread %x\n", tid);
	ntid = NATIVE_THREAD(tid);
	if (ntid == 0) {
		return;
	}

	markObject(unhand(tid)->jnireferences);
	markObject(unhand(tid)->exceptObj);

	// printf("stack %x - %x\n", ntid->stkbase, ntid->stkend);
	walkConservative((void*)ntid->stkbase, ntid->stkend - ntid->stkbase);
	// printf("done\n");
}

static
void
TnextFrame(stackFrame* fm)
{
	machineStackFrame* nfm;
	uintp fp;
	nativeThread* ct = NATIVE_THREAD(TcurrentJava());

	if (fm->return_frame == 0) {
		nfm = (machineStackFrame*)STACK_CURRENT_FRAME();
	}
	else {
		nfm = (machineStackFrame*)fm->return_frame;
	}

	fp = (uintp)STACK_FRAME(nfm);
	// printf("fp = %x (%x-%x)\n", fp, ct->stkbase, ct->stkend);
	if (fp > ct->stkbase && fp < ct->stkend) {
		fm->return_pc = STACK_PC(nfm);
		fm->return_frame = (void*)fp;
	}
	else {
		fm->return_pc = 0;
		fm->return_frame = 0;
	}
}

static
void
TstackInfo(threadStackInfo* info)
{
	nativeThread* ct = NATIVE_THREAD(TcurrentJava());

	info->low = (void*)ct->stkbase;
	info->high = (void*)ct->stkend;
	info->curr = info->low;
}
              
/*
 * Get a semaphore (with timeout).
 */
static
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
static
void
Lsemput(void* sem)
{
SDBG(	kprintf("Lsemput: %x\n", sem);	)
	ReleaseSemaphore((HANDLE)sem, 1, NULL);
}

static
void
Tinit(void)
{
}

void
notSupported()
{
	kprintf("Operation not supported\n");
}

void*
Mpagealloc(int size)
{
	void* ptr;

	size += gc_pgsize;

#undef malloc

	ptr = malloc(size);
	if (ptr == 0) {
		return (0);
	}
	ptr = (void*)((((uintp)ptr) + gc_pgsize - 1) & -(int)gc_pgsize);

	return (ptr);
}

static
int
Kinput(void)
{
	char ch;
	(*Kaffe_FileSystemCallInterface._read)(0, &ch ,1);
	return (ch);
}

static
void
Koutput(int chr)
{
	char ch = (char)chr;
	(*Kaffe_FileSystemCallInterface._write)(1, &ch ,1);
}

static
void
Kerror(int chr)
{
	char ch = (char)chr;
	(*Kaffe_FileSystemCallInterface._write)(2, &ch ,1);
}

static
jlong
Ktime(void)
{	
	jlong tme;
	struct _timeb tm;
	_ftime(&tm);
	tme = (((jlong)tm.time * (jlong)1000) + (jlong)tm.millitm);
	return (tme);
}

/*
 * Define the thread interface.
 */

ThreadInterface Kaffe_ThreadInterface = {

	Tinit,

	TcreateFirst,
	Tcreate,
	Tyield,
	Tprio, 
	Texit,
	Tstop,
	Tfinalize,

	TcurrentJava,
	TcurrentNative,

	TwalkThreads,
	TwalkThread,

	TnextFrame,

	TstackInfo,

};

/*
 * Define the lock interface.
 */

LockInterface Kaffe_LockInterface = {

	0,
	0,
	0,
	0,

	Lsemget,
	Lsemput,

};

/*
 * Define the system call interface.
 */

KernelCallInterface Kaffe_KernelCallInterface = {

	0,
	0,
	0,
	Mpagealloc,
	0,
	Kinput,
	Koutput,
	Kerror,
	Ktime,

};
