/*
 * Copyright (c) 1991-1994 by Xerox Corporation.  All rights reserved.
 * Copyright (c) 1999 by Hewlett-Packard Company.  All rights reserved.
 *
 * THIS MATERIAL IS PROVIDED AS IS, WITH ABSOLUTELY NO WARRANTY EXPRESSED
 * OR IMPLIED.  ANY USE IS AT YOUR OWN RISK.
 *
 * Permission is hereby granted to use or copy this program
 * for any purpose,  provided the above notices are retained on all copies.
 * Permission to modify the code and to distribute modified code is granted,
 * provided the above notices are retained, and a notice that the code was
 * modified is included with the above copyright notice.
 *
 */
/*
 * Some parts are
 *   Copyright(c) 2004
 *      The Kaffe.org's developers. See ChangeLog for details
 */

#include <stddef.h>
#include <stdlib.h>

#include <gc/gc.h>
#include <gc/gc_mark.h>

#include "gc-kaffe.h"

static int GC_kaffe_proc;
static int GC_kaffe_kind;
static void ** GC_kaffe_freelist;

/* This function is not exported by Boehm GC. However it is necessary
 * to defer the finalization of objects and push it to the finalizerMan
 * thread. */
extern void GC_notify_or_invoke_finalizers(void);

/* We need a mechanism to release the lock and invoke finalizers.	*/
/* We don't really have an opportunity to do this on a rarely executed	*/
/* path on which the lock is not held.  Thus we check at a 		*/
/* rarely executed point at which it is safe to release the lock.	*/
/* We do this even where we could just call GC_INVOKE_FINALIZERS,	*/
/* since it's probably cheaper and certainly more uniform.		*/
/* FIXME - Consider doing the same elsewhere?				*/
static void maybe_finalize()
{
   static GC_word last_finalized_no = 0;

   if (GC_gc_no == last_finalized_no) return;
   GC_notify_or_invoke_finalizers();
   last_finalized_no = GC_gc_no;
}

/* Allocate an object, clear it */
void * GC_kaffe_malloc(size_t lb)
{
  register void * op;

  maybe_finalize();
  op = (void *) GC_generic_malloc((GC_word)lb, GC_kaffe_kind);
  if (0 == op) {
    return(GC_oom_fn(lb));
  }
  return(op);
}

void GC_kaffe_init(GC_mark_proc proc)
{
  GC_kaffe_freelist = GC_new_free_list();
  GC_kaffe_proc = GC_new_proc(proc);
  GC_kaffe_kind = GC_new_kind(GC_kaffe_freelist,
			      GC_MAKE_PROC(GC_kaffe_proc, 0),
			      0, 1);
}
