/* 
 * XXX add FSF copyright information here. XXX
 *
 * This file copies the exception handling related part from libgcc2.c
 * Specifically, the part that's compiled when -DL_eh is defined.
 * 
 * NB: This file is temporary, hopefully.  I'm lobbying for integrating
 * the "__frame_state_for_func" hook into libgcc2.c
 *
 * It was created from GCC's libgcc2.c by executing this command:
 *
 *   sed '/#ifdef L_eh/,/#endif \/\* L_eh \*\//p; d' \
 *	 < "$1" | sed '1d;$d' 
 *
 * I have added this header to it:
 */ 

/* Begin of kaffe-specific configuration header */
#include "config.h"
#if defined(HAVE_GCJ_SUPPORT) && defined(JIT3) && defined(TRANSLATOR)

#include "gtypes.h"
#include "debug.h"
#include "md.h"
#include "gansidecl.h"

/* End of kaffe-specific configuration header */
/*----------------------------------------------------------------------*/

/* Shared exception handling support routines.  */

extern void __default_terminate (void) __attribute__ ((__noreturn__));

void
__default_terminate ()
{
  abort ();
}

void (*__terminate_func)() = __default_terminate;

void
__terminate ()
{
  (*__terminate_func)();
}

void *
__throw_type_match (void *catch_type, void *throw_type, void *obj)
{
#if 0
 printf ("__throw_type_match (): catch_type = %s, throw_type = %s\n",
	 catch_type, throw_type);
#endif
 if (strcmp ((const char *)catch_type, (const char *)throw_type) == 0)
   return obj;
 return 0;
}

void
__empty ()
{
}


/* Include definitions of EH context and table layout */

#include "eh-common.h"
#ifndef inhibit_libc
#include <stdio.h>
#endif

/* Allocate and return a new EH context structure. */

extern void __throw ();

static void *
new_eh_context ()
{
  struct eh_full_context {
    struct eh_context c;
    void *top_elt[2];
  } *ehfc = (struct eh_full_context *) malloc (sizeof *ehfc);

  if (! ehfc)
    __terminate ();

  memset (ehfc, 0, sizeof *ehfc);

  ehfc->c.dynamic_handler_chain = (void **) ehfc->top_elt;

  /* This should optimize out entirely.  This should always be true,
     but just in case it ever isn't, don't allow bogus code to be
     generated.  */

  if ((void*)(&ehfc->c) != (void*)ehfc)
    __terminate ();

  return &ehfc->c;
}

#if __GTHREADS
static __gthread_key_t eh_context_key;

/* Destructor for struct eh_context. */
static void
eh_context_free (void *ptr)
{
  __gthread_key_dtor (eh_context_key, ptr);
  if (ptr)
    free (ptr);
}
#endif

/* Pointer to function to return EH context. */

static struct eh_context *eh_context_initialize ();
static struct eh_context *eh_context_static ();
#if __GTHREADS
static struct eh_context *eh_context_specific ();
#endif

static struct eh_context *(*get_eh_context) () = &eh_context_initialize;

/* Routine to get EH context.
   This one will simply call the function pointer. */

void *
__get_eh_context ()
{
  return (void *) (*get_eh_context) ();
}

/* Get and set the language specific info pointer. */

void **
__get_eh_info ()
{
  struct eh_context *eh = (*get_eh_context) ();
  return &eh->info;
}

#ifdef DWARF2_UNWIND_INFO
static int dwarf_reg_size_table_initialized = 0;
static char dwarf_reg_size_table[FIRST_PSEUDO_REGISTER];

static void
init_reg_size_table ()
{
  __builtin_init_dwarf_reg_size_table (dwarf_reg_size_table);
  dwarf_reg_size_table_initialized = 1;
}
#endif

#if __GTHREADS
static void
eh_threads_initialize ()
{
  /* Try to create the key.  If it fails, revert to static method,
     otherwise start using thread specific EH contexts. */
  if (__gthread_key_create (&eh_context_key, &eh_context_free) == 0)
    get_eh_context = &eh_context_specific;
  else
    get_eh_context = &eh_context_static;
}
#endif /* no __GTHREADS */

/* Initialize EH context.
   This will be called only once, since we change GET_EH_CONTEXT
   pointer to another routine. */

static struct eh_context *
eh_context_initialize ()
{
#if __GTHREADS

  static __gthread_once_t once = __GTHREAD_ONCE_INIT;
  /* Make sure that get_eh_context does not point to us anymore.
     Some systems have dummy thread routines in their libc that
     return a success (Solaris 2.6 for example). */
  if (__gthread_once (&once, eh_threads_initialize) != 0
      || get_eh_context == &eh_context_initialize)
    {
      /* Use static version of EH context. */
      get_eh_context = &eh_context_static;
    }
#ifdef DWARF2_UNWIND_INFO
  {
    static __gthread_once_t once_regsizes = __GTHREAD_ONCE_INIT;
    if (__gthread_once (&once_regsizes, init_reg_size_table) != 0
	|| ! dwarf_reg_size_table_initialized)
      init_reg_size_table ();
  }
#endif

#else /* no __GTHREADS */

  /* Use static version of EH context. */
  get_eh_context = &eh_context_static;

#ifdef DWARF2_UNWIND_INFO
  init_reg_size_table ();
#endif

#endif /* no __GTHREADS */

  return (*get_eh_context) ();
}

/* Return a static EH context. */

static struct eh_context *
eh_context_static ()
{
  static struct eh_context eh;
  static int initialized;
  static void *top_elt[2];

  if (! initialized)
    {
      initialized = 1;
      memset (&eh, 0, sizeof eh);
      eh.dynamic_handler_chain = top_elt;
    }
  return &eh;
}

#if __GTHREADS
/* Return a thread specific EH context. */

static struct eh_context *
eh_context_specific ()
{
  struct eh_context *eh;
  eh = (struct eh_context *) __gthread_getspecific (eh_context_key);
  if (! eh)
    {
      eh = new_eh_context ();
      if (__gthread_setspecific (eh_context_key, (void *) eh) != 0)
	__terminate ();
    }

  return eh;
}
#endif __GTHREADS

/* Support routines for setjmp/longjmp exception handling.  */

/* Calls to __sjthrow are generated by the compiler when an exception
   is raised when using the setjmp/longjmp exception handling codegen
   method.  */

#ifdef DONT_USE_BUILTIN_SETJMP
extern void longjmp (void *, int);
#endif

/* Routine to get the head of the current thread's dynamic handler chain
   use for exception handling. */

void ***
__get_dynamic_handler_chain ()
{
  struct eh_context *eh = (*get_eh_context) ();
  return &eh->dynamic_handler_chain;
}

/* This is used to throw an exception when the setjmp/longjmp codegen
   method is used for exception handling.

   We call __terminate if there are no handlers left.  Otherwise we run the
   cleanup actions off the dynamic cleanup stack, and pop the top of the
   dynamic handler chain, and use longjmp to transfer back to the associated
   handler.  */

extern void __sjthrow (void) __attribute__ ((__noreturn__));

void
__sjthrow ()
{
  struct eh_context *eh = (*get_eh_context) ();
  void ***dhc = &eh->dynamic_handler_chain;
  void *jmpbuf;
  void (*func)(void *, int);
  void *arg;
  void ***cleanup;

  /* The cleanup chain is one word into the buffer.  Get the cleanup
     chain.  */
  cleanup = (void***)&(*dhc)[1];

  /* If there are any cleanups in the chain, run them now.  */
  if (cleanup[0])
    {
      double store[200];
      void **buf = (void**)store;
      buf[1] = 0;
      buf[0] = (*dhc);

      /* try { */
#ifdef DONT_USE_BUILTIN_SETJMP
      if (! setjmp (&buf[2]))
#else
      if (! __builtin_setjmp (&buf[2]))
#endif
	{
	  *dhc = buf;
	  while (cleanup[0])
	    {
	      func = (void(*)(void*, int))cleanup[0][1];
	      arg = (void*)cleanup[0][2];

	      /* Update this before running the cleanup.  */
	      cleanup[0] = (void **)cleanup[0][0];

	      (*func)(arg, 2);
	    }
	  *dhc = buf[0];
	}
      /* catch (...) */
      else
	{
	  __terminate ();
	}
    }
  
  /* We must call terminate if we try and rethrow an exception, when
     there is no exception currently active and when there are no
     handlers left.  */
  if (! eh->info || (*dhc)[0] == 0)
    __terminate ();
    
  /* Find the jmpbuf associated with the top element of the dynamic
     handler chain.  The jumpbuf starts two words into the buffer.  */
  jmpbuf = &(*dhc)[2];

  /* Then we pop the top element off the dynamic handler chain.  */
  *dhc = (void**)(*dhc)[0];

  /* And then we jump to the handler.  */

#ifdef DONT_USE_BUILTIN_SETJMP
  longjmp (jmpbuf, 1);
#else
  __builtin_longjmp (jmpbuf, 1);
#endif
}

/* Run cleanups on the dynamic cleanup stack for the current dynamic
   handler, then pop the handler off the dynamic handler stack, and
   then throw.  This is used to skip the first handler, and transfer
   control to the next handler in the dynamic handler stack.  */

extern void __sjpopnthrow (void) __attribute__ ((__noreturn__));

void
__sjpopnthrow ()
{
  struct eh_context *eh = (*get_eh_context) ();
  void ***dhc = &eh->dynamic_handler_chain;
  void (*func)(void *, int);
  void *arg;
  void ***cleanup;

  /* The cleanup chain is one word into the buffer.  Get the cleanup
     chain.  */
  cleanup = (void***)&(*dhc)[1];

  /* If there are any cleanups in the chain, run them now.  */
  if (cleanup[0])
    {
      double store[200];
      void **buf = (void**)store;
      buf[1] = 0;
      buf[0] = (*dhc);

      /* try { */
#ifdef DONT_USE_BUILTIN_SETJMP
      if (! setjmp (&buf[2]))
#else
      if (! __builtin_setjmp (&buf[2]))
#endif
	{
	  *dhc = buf;
	  while (cleanup[0])
	    {
	      func = (void(*)(void*, int))cleanup[0][1];
	      arg = (void*)cleanup[0][2];

	      /* Update this before running the cleanup.  */
	      cleanup[0] = (void **)cleanup[0][0];

	      (*func)(arg, 2);
	    }
	  *dhc = buf[0];
	}
      /* catch (...) */
      else
	{
	  __terminate ();
	}
    }

  /* Then we pop the top element off the dynamic handler chain.  */
  *dhc = (void**)(*dhc)[0];

  __sjthrow ();
}

/* Support code for all exception region-based exception handling.  */

int
__eh_rtime_match (void *rtime)
{
  void *info;
  __eh_matcher matcher;
  void *ret;

  info = *(__get_eh_info ());
  matcher = ((__eh_info *)info)->match_function;
  if (! matcher)
    {
#ifndef inhibit_libc
      dprintf("Internal Compiler Bug: No runtime type matcher.");
#endif
      return 0;
    }
  ret = (*matcher) (info, rtime, (void *)0);
  return (ret != NULL);
}

/* This value identifies the place from which an exception is being
   thrown.  */

#ifdef EH_TABLE_LOOKUP

EH_TABLE_LOOKUP

#else

#ifdef DWARF2_UNWIND_INFO

/* Return the table version of an exception descriptor */

short 
__get_eh_table_version (exception_descriptor *table) 
{
  return table->lang.version;
}

/* Return the originating table language of an exception descriptor */

short 
__get_eh_table_language (exception_descriptor *table)
{
  return table->lang.language;
}

/* This routine takes a PC and a pointer to the exception region TABLE for
   its translation unit, and returns the address of the exception handler
   associated with the closest exception table handler entry associated
   with that PC, or 0 if there are no table entries the PC fits in.

   In the advent of a tie, we have to give the last entry, as it represents
   an inner block.  */

static void *
old_find_exception_handler (void *pc, old_exception_table *table)
{
  if (table)
    {
      int pos;
      int best = -1;

      /* We can't do a binary search because the table isn't guaranteed
         to be sorted from function to function.  */
      for (pos = 0; table[pos].start_region != (void *) -1; ++pos)
        {
          if (table[pos].start_region <= pc && table[pos].end_region > pc)
            {
              /* This can apply.  Make sure it is at least as small as
                 the previous best.  */
              if (best == -1 || (table[pos].end_region <= table[best].end_region
                        && table[pos].start_region >= table[best].start_region))
                best = pos;
            }
          /* But it is sorted by starting PC within a function.  */
          else if (best >= 0 && table[pos].start_region > pc)
            break;
        }
      if (best != -1)
        return table[best].exception_handler;
    }

  return (void *) 0;
}

/* find_exception_handler finds the correct handler, if there is one, to
   handle an exception.
   returns a pointer to the handler which controlled should be transferred
   to, or NULL if there is nothing left.
   Parameters:
   PC - pc where the exception originates. If this is a rethrow, 
        then this starts out as a pointer to the exception table
	entry we wish to rethrow out of.
   TABLE - exception table for the current module.
   EH_INFO - eh info pointer for this exception.
   RETHROW - 1 if this is a rethrow. (see incoming value of PC).
   CLEANUP - returned flag indicating whether this is a cleanup handler.
*/
static void *
find_exception_handler (void *pc, exception_descriptor *table, 
                        __eh_info *eh_info, int rethrow, int *cleanup)
{

  void *retval = NULL;
  *cleanup = 1;
  if (table)
    {
      int pos = 0;
      /* The new model assumed the table is sorted inner-most out so the
         first region we find which matches is the correct one */

      exception_table *tab = &(table->table[0]);

      /* Subtract 1 from the PC to avoid hitting the next region */
      if (rethrow) 
        {
          /* pc is actually the region table entry to rethrow out of */
          pos = ((exception_table *) pc) - tab;
          pc = ((exception_table *) pc)->end_region - 1;

          /* The label is always on the LAST handler entry for a region, 
             so we know the next entry is a different region, even if the
             addresses are the same. Make sure its not end of table tho. */
          if (tab[pos].start_region != (void *) -1)
            pos++;
        }
      else
        pc--;
      
      /* We can't do a binary search because the table is in inner-most
         to outermost address ranges within functions */
      for ( ; tab[pos].start_region != (void *) -1; pos++)
        { 
          if (tab[pos].start_region <= pc && tab[pos].end_region > pc)
            {
              if (tab[pos].match_info)
                {
                  __eh_matcher matcher = eh_info->match_function;
                  /* match info but no matcher is NOT a match */
                  if (matcher) 
                    {
                      void *ret = (*matcher)((void *) eh_info, 
                                             tab[pos].match_info, table);
                      if (ret) 
                        {
                          if (retval == NULL)
                            retval = tab[pos].exception_handler;
                          *cleanup = 0;
                          break;
                        }
                    }
                }
              else
                {
                  if (retval == NULL)
                    retval = tab[pos].exception_handler;
                }
            }
        }
    }
  return retval;
}
#endif /* DWARF2_UNWIND_INFO */
#endif /* EH_TABLE_LOOKUP */

#ifdef DWARF2_UNWIND_INFO
/* Support code for exception handling using static unwind information.  */

#include "frame.h"

/* This type is used in get_reg and put_reg to deal with ABIs where a void*
   is smaller than a word, such as the Irix 6 n32 ABI.  We cast twice to
   avoid a warning about casting between int and pointer of different
   sizes.  */

typedef int ptr_type __attribute__ ((mode (pointer)));

#ifdef INCOMING_REGNO
/* Is the saved value for register REG in frame UDATA stored in a register
   window in the previous frame?  */

/* ??? The Sparc INCOMING_REGNO references TARGET_FLAT.  This allows us
   to use the macro here.  One wonders, though, that perhaps TARGET_FLAT
   compiled functions won't work with the frame-unwind stuff here.  
   Perhaps the entireity of in_reg_window should be conditional on having
   seen a DW_CFA_GNU_window_save?  */
#define target_flags 0

static int
in_reg_window (int reg, frame_state *udata)
{
  if (udata->saved[reg] == REG_SAVED_REG)
    return INCOMING_REGNO (reg) == reg;
  if (udata->saved[reg] != REG_SAVED_OFFSET)
    return 0;

#ifdef STACK_GROWS_DOWNWARD
  return udata->reg_or_offset[reg] > 0;
#else
  return udata->reg_or_offset[reg] < 0;
#endif
}
#else
static inline int in_reg_window (int reg, frame_state *udata) { return 0; }
#endif /* INCOMING_REGNO */

/* Get the address of register REG as saved in UDATA, where SUB_UDATA is a
   frame called by UDATA or 0.  */

static word_type *
get_reg_addr (unsigned reg, frame_state *udata, frame_state *sub_udata)
{
  while (udata->saved[reg] == REG_SAVED_REG)
    {
      reg = udata->reg_or_offset[reg];
      if (in_reg_window (reg, udata))
	{
          udata = sub_udata;
	  sub_udata = NULL;
	}
    }
  if (udata->saved[reg] == REG_SAVED_OFFSET)
    return (word_type *)(udata->cfa + udata->reg_or_offset[reg]);
  else
    abort ();
}

/* Get the value of register REG as saved in UDATA, where SUB_UDATA is a
   frame called by UDATA or 0.  */

static inline void *
get_reg (unsigned reg, frame_state *udata, frame_state *sub_udata)
{
  return (void *)(ptr_type) *get_reg_addr (reg, udata, sub_udata);
}

/* Overwrite the saved value for register REG in frame UDATA with VAL.  */

static inline void
put_reg (unsigned reg, void *val, frame_state *udata)
{
  *get_reg_addr (reg, udata, NULL) = (word_type)(ptr_type) val;
}

/* Copy the saved value for register REG from frame UDATA to frame
   TARGET_UDATA.  Unlike the previous two functions, this can handle
   registers that are not one word large.  */

static void
copy_reg (unsigned reg, frame_state *udata, frame_state *target_udata)
{
  word_type *preg = get_reg_addr (reg, udata, NULL);
  word_type *ptreg = get_reg_addr (reg, target_udata, NULL);

  memcpy (ptreg, preg, dwarf_reg_size_table [reg]);
}

/* Retrieve the return address for frame UDATA.  */

static inline void *
get_return_addr (frame_state *udata, frame_state *sub_udata)
{
  return __builtin_extract_return_addr
    (get_reg (udata->retaddr_column, udata, sub_udata));
}

/* Overwrite the return address for frame UDATA with VAL.  */

static inline void
put_return_addr (void *val, frame_state *udata)
{
  val = __builtin_frob_return_addr (val);
  put_reg (udata->retaddr_column, val, udata);
}

static frame_state *
/* ARGSUSED */
default_frame_state_for(void *pc, frame_state *udata, frame_state *caller_udata)
{
  return __frame_state_for(pc, caller_udata);
}

frame_state * (*__frame_state_for_func)(void *, frame_state *, frame_state *) =
	default_frame_state_for;

/* Given the current frame UDATA and its return address PC, return the
   information about the calling frame in CALLER_UDATA.  */

static void *
next_stack_level (void *pc, frame_state *udata, frame_state *caller_udata)
{
  caller_udata = (*__frame_state_for_func) (pc, udata, caller_udata);

  if (! caller_udata)
    return 0;

  /* Now go back to our caller's stack frame.  If our caller's CFA register
     was saved in our stack frame, restore it; otherwise, assume the CFA
     register is SP and restore it to our CFA value.  */
  if (udata->saved[caller_udata->cfa_reg])
    caller_udata->cfa = get_reg (caller_udata->cfa_reg, udata, 0);
  else
    caller_udata->cfa = udata->cfa;
  caller_udata->cfa += caller_udata->cfa_offset;

  return caller_udata;
}

/* Hook to call before __terminate if only cleanup handlers remain. */
void 
__unwinding_cleanup ()
{
}

/* throw_helper performs some of the common grunt work for a throw. This
   routine is called by throw and rethrows. This is pretty much split 
   out from the old __throw routine. An addition has been added which allows
   for a dummy call to a routine __unwinding_cleanup() when there are nothing
   but cleanups remaining. This allows a debugger to examine the state
   at which the throw was executed, before any cleanups, rather than
   at the terminate point after the stack has been unwound.

   EH is the current eh_context structure.
   PC is the address of the call to __throw.
   MY_UDATA is the unwind information for __throw.
   OFFSET_P is where we return the SP adjustment offset.  */

static void *
throw_helper (eh, pc, my_udata, offset_p)
     struct eh_context *eh;
     void *pc;
     frame_state *my_udata;
     long *offset_p;
{
  frame_state ustruct2, *udata = &ustruct2;
  frame_state ustruct;
  frame_state *sub_udata = &ustruct;
  void *saved_pc = pc;
  void *handler;
  void *handler_p;
  void *pc_p;
  frame_state saved_ustruct;
  int new_eh_model;
  int cleanup = 0;
  int only_cleanup = 0;
  int rethrow = 0;
  int saved_state = 0;
  long args_size;
  __eh_info *eh_info = (__eh_info *)eh->info;

  /* Do we find a handler based on a re-throw PC? */
  if (eh->table_index != (void *) 0)
    rethrow = 1;

  memcpy (udata, my_udata, sizeof (*udata));

  handler = (void *) 0;
  for (;;)
    { 
      frame_state *p = udata;
      udata = next_stack_level (pc, udata, sub_udata);
      sub_udata = p;

      /* If we couldn't find the next frame, we lose.  */
      if (! udata)
	break;

      if (udata->eh_ptr == NULL)
        new_eh_model = 0;
      else
        new_eh_model = (((exception_descriptor *)(udata->eh_ptr))->
                                          runtime_id_field == NEW_EH_RUNTIME);

      if (rethrow) 
        {
          rethrow = 0;
          handler = find_exception_handler (eh->table_index, udata->eh_ptr, 
                                          eh_info, 1, &cleanup);
          eh->table_index = (void *)0;
        }
      else
        if (new_eh_model)
          handler = find_exception_handler (pc, udata->eh_ptr, eh_info, 
                                            0, &cleanup);
        else
          handler = old_find_exception_handler (pc, udata->eh_ptr);

      /* If we found one, we can stop searching, if its not a cleanup. 
         for cleanups, we save the state, and keep looking. This allows
         us to call a debug hook if there are nothing but cleanups left. */
      if (handler)
	{
	  if (cleanup)
	    {
	      if (!saved_state)
		{
		  saved_ustruct = *udata;
		  handler_p = handler;
		  pc_p = pc;
		  saved_state = 1;
		  only_cleanup = 1;
		}
	    }
	  else
	    {
	      only_cleanup = 0;
	      break;
	    }
	}

      /* Otherwise, we continue searching.  We subtract 1 from PC to avoid
	 hitting the beginning of the next region.  */
      pc = get_return_addr (udata, sub_udata) - 1;
    }

  if (saved_state) 
    {
      udata = &saved_ustruct;
      handler = handler_p;
      pc = pc_p;
      if (only_cleanup)
        __unwinding_cleanup ();
    }

  /* If we haven't found a handler by now, this is an unhandled
     exception.  */
  if (! handler) 
    __terminate();

  eh->handler_label = handler;

  args_size = udata->args_size;

  if (pc == saved_pc)
    /* We found a handler in the throw context, no need to unwind.  */
    udata = my_udata;
  else
    {
      int i;

      /* Unwind all the frames between this one and the handler by copying
	 their saved register values into our register save slots.  */

      /* Remember the PC where we found the handler.  */
      void *handler_pc = pc;

      /* Start from the throw context again.  */
      pc = saved_pc;
      memcpy (udata, my_udata, sizeof (*udata));

      while (pc != handler_pc)
	{
	  frame_state *p = udata;
	  udata = next_stack_level (pc, udata, sub_udata);
	  sub_udata = p;

	  for (i = 0; i < FIRST_PSEUDO_REGISTER; ++i)
	    if (i != udata->retaddr_column && udata->saved[i])
	      {
		/* If you modify the saved value of the return address
		   register on the SPARC, you modify the return address for
		   your caller's frame.  Don't do that here, as it will
		   confuse get_return_addr.  */
		if (in_reg_window (i, udata)
		    && udata->saved[udata->retaddr_column] == REG_SAVED_REG
		    && udata->reg_or_offset[udata->retaddr_column] == i)
		  continue;
		copy_reg (i, udata, my_udata);
	      }

	  pc = get_return_addr (udata, sub_udata) - 1;
	}

      /* But we do need to update the saved return address register from
	 the last frame we unwind, or the handler frame will have the wrong
	 return address.  */
      if (udata->saved[udata->retaddr_column] == REG_SAVED_REG)
	{
	  i = udata->reg_or_offset[udata->retaddr_column];
	  if (in_reg_window (i, udata))
	    copy_reg (i, udata, my_udata);
	}
    }
  /* udata now refers to the frame called by the handler frame.  */

  /* We adjust SP by the difference between __throw's CFA and the CFA for
     the frame called by the handler frame, because those CFAs correspond
     to the SP values at the two call sites.  We need to further adjust by
     the args_size of the handler frame itself to get the handler frame's
     SP from before the args were pushed for that call.  */
#ifdef STACK_GROWS_DOWNWARD
  *offset_p = udata->cfa - my_udata->cfa + args_size;
#else
  *offset_p = my_udata->cfa - udata->cfa - args_size;
#endif
		       
  return handler;
}


/* We first search for an exception handler, and if we don't find
   it, we call __terminate on the current stack frame so that we may
   use the debugger to walk the stack and understand why no handler
   was found.

   If we find one, then we unwind the frames down to the one that
   has the handler and transfer control into the handler.  */

/*extern void __throw(void) __attribute__ ((__noreturn__));*/

void
__throw ()
{
  struct eh_context *eh = (*get_eh_context) ();
  void *pc, *handler;
  long offset;

  /* XXX maybe make my_ustruct static so we don't have to look it up for
     each throw.  */
  frame_state my_ustruct, *my_udata = &my_ustruct;

  /* This is required for C++ semantics.  We must call terminate if we
     try and rethrow an exception, when there is no exception currently
     active.  */
  if (! eh->info)
    __terminate ();
    
  /* Start at our stack frame.  */
label:
  my_udata = __frame_state_for (&&label, my_udata);
  if (! my_udata)
    __terminate ();

  /* We need to get the value from the CFA register. */
  my_udata->cfa = __builtin_dwarf_cfa ();

  /* Do any necessary initialization to access arbitrary stack frames.
     On the SPARC, this means flushing the register windows.  */
  __builtin_unwind_init ();

  /* Now reset pc to the right throw point.  */
  pc = __builtin_extract_return_addr (__builtin_return_address (0)) - 1;

  handler = throw_helper (eh, pc, my_udata, &offset);

  /* Now go!  */

  __builtin_eh_return ((void *)eh, offset, handler);

  /* Epilogue:  restore the handler frame's register values and return
     to the stub.  */
}

/*extern void __rethrow(void *) __attribute__ ((__noreturn__));*/

void
__rethrow (index)
     void *index;
{
  struct eh_context *eh = (*get_eh_context) ();
  void *pc, *handler;
  long offset;

  /* XXX maybe make my_ustruct static so we don't have to look it up for
     each throw.  */
  frame_state my_ustruct, *my_udata = &my_ustruct;

  /* This is required for C++ semantics.  We must call terminate if we
     try and rethrow an exception, when there is no exception currently
     active.  */
  if (! eh->info)
    __terminate ();

  /* This is the table index we want to rethrow from. The value of
     the END_REGION label is used for the PC of the throw, and the
     search begins with the next table entry. */
  eh->table_index = index;
    
  /* Start at our stack frame.  */
label:
  my_udata = __frame_state_for (&&label, my_udata);
  if (! my_udata)
    __terminate ();

  /* We need to get the value from the CFA register. */
  my_udata->cfa = __builtin_dwarf_cfa ();

  /* Do any necessary initialization to access arbitrary stack frames.
     On the SPARC, this means flushing the register windows.  */
  __builtin_unwind_init ();

  /* Now reset pc to the right throw point.  */
  pc = __builtin_extract_return_addr (__builtin_return_address (0)) - 1;

  handler = throw_helper (eh, pc, my_udata, &offset);

  /* Now go!  */

  __builtin_eh_return ((void *)eh, offset, handler);

  /* Epilogue:  restore the handler frame's register values and return
     to the stub.  */
}
#endif /* DWARF2_UNWIND_INFO */

/*----------------------------------------------------------------------*/
#endif /* defined(HAVE_GCJ_SUPPORT) */
