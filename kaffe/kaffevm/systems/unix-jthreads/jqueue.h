/*
 * jqueue.h - queue pool manager 
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 *
 * Written by Guilhem Lavaux <guilhem@kaffe.org> (C) 2003
 */
#ifndef _jqueue_h
#define _jqueue_h

#if defined(HAVE_CONFIG_H)
#include "debug.h"
#include "config.h"
#include "config-std.h"
#include "config-mem.h"
#else
#include "config-jthreads.h"
#endif

typedef void *(*KaffeAllocator)(size_t s);
typedef void (*KaffeDeallocator)(void *ptr);
typedef void *(*KaffeReallocator)(void *ptr, size_t s);

typedef struct _KaffeNodeQueue {
  void *element;
  struct _KaffeNodeQueue *next;
} KaffeNodeQueue;

typedef struct {
  KaffeNodeQueue **pools;
  KaffeNodeQueue **free_nodes;
  int num_free_nodes;
  int num_nodes_in_pool;
  int num_pools;

  KaffeAllocator allocator;
  KaffeDeallocator deallocator;
  KaffeReallocator reallocator;
} KaffePool;

void KaffeSetDefaultAllocator(KaffeAllocator allocator,
			      KaffeDeallocator deallocator,
			      KaffeReallocator reallocator);
KaffePool *KaffeCreatePool(void);
void KaffeDestroyPool(KaffePool *pool);
KaffeNodeQueue *KaffePoolNewNode(KaffePool *pool);
void KaffePoolReleaseNode(KaffePool *pool, KaffeNodeQueue *node);
void KaffePoolReleaseList(KaffePool *pool, KaffeNodeQueue *node);

#endif
