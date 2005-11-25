/*
 * signal.c
 * Exception handling - NullPointer, divison by zero
 * 
 * Copyright (c) 2004, 2005
 *	TU Dresden, Operating System Group.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 *
 * written by Alexander Boettcher <ab764283@os.inf.tu-dresden.de>
 */

#include "config.h"
#include "debug.h"
#include "config-std.h"
#include "config-signal.h"
#include "config-mem.h"
#include "config-setjmp.h"
#include "jthread.h"
#include "md.h"

#if defined(INTERPRETER)
#define	DEFINEFRAME()		/* Does nothing */
#define	EXCEPTIONPROTO		int sig
#define	EXCEPTIONFRAME(f, c)	/* Does nothing */
#define	EXCEPTIONFRAMEPTR	0
#elif defined(TRANSLATOR)
#define	DEFINEFRAME()		exceptionFrame frame
#define	EXCEPTIONFRAMEPTR	&frame
#endif /* TRANSLATOR */

#define SIG_T   void*

#include <l4/util/idt.h>
#include <l4/log/l4log.h>
#include <l4/l4rm/l4rm.h>

static struct {
 exchandler_t floatingHandler;
 exchandler_t nullHandler;
 exchandler_t stackOverflowHandler;
} leafleterr;

static struct {
      l4util_idt_header_t header;
      l4util_idt_desc_t   desc[0x20];
      char                padding[L4_PAGESIZE-sizeof(l4util_idt_header_t)
                                             -sizeof(l4util_idt_desc_t)*0x20];
} __attribute__((packed,aligned(L4_PAGESIZE))) idt;

static void nullException(void);
static void floatingException(void);

/*
 * Each Java thread calls this function to register itself for the exceptions.
 */
void drops_registerForExceptions(void){

 l4util_idt_load (&idt.header);

}

/*
 * Setup the internal exceptions.
 */
void
jthread_initexceptions(exchandler_t _nullHandler,
		       exchandler_t _floatingHandler,
		       exchandler_t _stackOverflowHandler)
{

  leafleterr.floatingHandler      = _floatingHandler;
  leafleterr.nullHandler          = _nullHandler;
  leafleterr.stackOverflowHandler = _stackOverflowHandler;

  l4util_idt_init (&idt.header, 0x20);
  l4util_idt_entry(&idt.header,  0, floatingException);  // divide error
  l4util_idt_entry(&idt.header, 14, nullException);      // page fault
  l4util_idt_entry(&idt.header, 16, floatingException);  // floating point error

  // register IDT in LIDT for this thread
  drops_registerForExceptions();

  // enable unresolvable page fault handling in l4rm
  l4rm_enable_pagefault_exceptions();                     
}

/*
 * Null exception handler
 * kernel sets the thread to this function
 */
static void
nullException()
{
	DEFINEFRAME();

	EXCEPTIONFRAME(frame, ctx);

	leafleterr.nullHandler(EXCEPTIONFRAMEPTR);
}

/*
 * Division by zero, 
 *  kernel sets the thread to this function
 */
static void
floatingException(void)
{
	DEFINEFRAME();

	EXCEPTIONFRAME(frame, ctx);

	leafleterr.floatingHandler(EXCEPTIONFRAMEPTR);
}

