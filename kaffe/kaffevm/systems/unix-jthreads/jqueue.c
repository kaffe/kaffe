/*
 * jqueue.c - queue pool manager
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 *
 * Written by Guilhem Lavaux <guilhem@kaffe.org> (C) 2003
 */
#include "jqueue.h"

#define DEFAULT_NUMBER_OF_NODES_IN_POOL 1024

static KaffeAllocator gs_default_allocator;
static KaffeDeallocator gs_default_deallocator;

void KaffeSetDefaultAllocator(KaffeAllocator allocator,
			      KaffeDeallocator deallocator)
{
  gs_default_allocator = allocator;
  gs_default_deallocator = deallocator;
}

KaffePool *KaffeCreatePool()
{
  KaffePool *pool;
  int i;

  assert(gs_default_allocator != NULL);
  assert(gs_default_deallocator != NULL);

  pool = (KaffePool *)gs_default_allocator(sizeof(KaffePool));
  
  pool->num_nodes_in_pool = DEFAULT_NUMBER_OF_NODES_IN_POOL;
  pool->num_free_nodes = pool->num_nodes_in_pool;
  pool->pool = (KaffeNodeQueue *)gs_default_allocator(sizeof(KaffeNodeQueue)*pool->num_nodes_in_pool);
  pool->free_nodes = (KaffeNodeQueue **)gs_default_allocator(sizeof(KaffeNodeQueue *)*pool->num_nodes_in_pool);

  for (i=0;i<pool->num_nodes_in_pool;i++) {
    pool->free_nodes[i] = &pool->pool[i];
  }

  pool->allocator = gs_default_allocator;
  pool->deallocator = gs_default_deallocator;

  return pool;
}

void KaffeDestroyPool(KaffePool *pool)
{
  pool->deallocator(pool->pool);
  pool->deallocator(pool->free_nodes);
  pool->deallocator(pool);
}

KaffeNodeQueue *KaffePoolNewNode(KaffePool *pool)
{
  int chosen_node;

  assert(pool != NULL);
  assert(pool->num_free_nodes != 0);

  pool->num_free_nodes--;
  chosen_node = pool->num_free_nodes;
  pool->free_nodes[chosen_node]->next = NULL;
  return pool->free_nodes[chosen_node];
}

void KaffePoolReleaseNode(KaffePool *pool, KaffeNodeQueue *node)
{
  int node_id;

  assert(pool != NULL);
  node_id = (int)(node-pool->pool)/sizeof(KaffeNodeQueue);
  assert(node_id >= 0 && node_id < pool->num_nodes_in_pool);
  assert(pool->num_free_nodes < pool->num_nodes_in_pool);

  pool->free_nodes[pool->num_free_nodes] = node;
  pool->num_free_nodes++;
}

void KaffePoolReleaseList(KaffePool *pool, KaffeNodeQueue *node)
{
  KaffeNodeQueue *tmp;

  while (node != NULL) {
    tmp = node->next;
    KaffePoolReleaseNode(pool, node);
    node = tmp;
  }
}
