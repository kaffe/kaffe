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


#include "boehm/include/gc.h"
#include "boehm/include/private/gc_priv.h"
#include "boehm/include/gc_mark.h"
#include "gc-kaffe.h"

static int GC_kaffe_proc;
static int GC_kaffe_kind;
static ptr_t * GC_kaffe_freelist;

extern ptr_t GC_clear_stack();

/* We need a mechanism to release the lock and invoke finalizers.	*/
/* We don't really have an opportunity to do this on a rarely executed	*/
/* path on which the lock is not held.  Thus we check at a 		*/
/* rarely executed point at which it is safe to release the lock.	*/
/* We do this even where we could just call GC_INVOKE_FINALIZERS,	*/
/* since it's probably cheaper and certainly more uniform.		*/
/* FIXME - Consider doing the same elsewhere?				*/
static void maybe_finalize()
{
   static int last_finalized_no = 0;

   if (GC_gc_no == last_finalized_no) return;
   if (!GC_is_initialized) return;
   UNLOCK();
   GC_INVOKE_FINALIZERS();
   last_finalized_no = GC_gc_no;
   LOCK();
}

#define GENERAL_MALLOC(lb,k) \
    (GC_PTR)GC_clear_stack(GC_generic_malloc_inner((word)lb, k))

/* Allocate an object, clear it */
void * GC_kaffe_malloc(size_t lb)
{
register ptr_t op;
register ptr_t * opp;
register word lw;
DCL_LOCK_STATE;

    if( EXPECT(SMALL_OBJ(lb), 1) ) {
#       ifdef MERGE_SIZES
	  lw = GC_size_map[lb];
#	else
	  lw = ALIGNED_WORDS(lb);
#       endif
	opp = &(GC_kaffe_freelist[lw]);
	LOCK();
	op = *opp;
        if(EXPECT(op == 0, 0)) {
	    maybe_finalize();
            op = (ptr_t)GENERAL_MALLOC((word)lb, GC_kaffe_kind);
	    if (0 == op) {
		UNLOCK();
		return(GC_oom_fn(lb));
	    }
#	    ifdef MERGE_SIZES
		lw = GC_size_map[lb];	/* May have been uninitialized.	*/
#	    endif
        } else {
            *opp = obj_link(op);
            GC_words_allocd += lw;
        }
	UNLOCK();
    } else {
	LOCK();
	maybe_finalize();
	op = (ptr_t)GENERAL_MALLOC((word)lb, GC_kaffe_kind);
	if (0 == op) {
	    UNLOCK();
	    return(GC_oom_fn(lb));
	}
	UNLOCK();
    }
    return((GC_PTR) op);
}

void GC_kaffe_init(GC_mark_proc proc)
{
  GC_kaffe_freelist = (ptr_t *)GC_new_free_list();
  GC_kaffe_proc = GC_new_proc(proc);
  GC_kaffe_kind = GC_new_kind(GC_kaffe_freelist,
			      GC_MAKE_PROC(GC_kaffe_proc, 0),
			      FALSE, TRUE);
}
