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
static KaffeReallocator gs_default_reallocator;

void KaffeSetDefaultAllocator(KaffeAllocator allocator,
			      KaffeDeallocator deallocator,
			      KaffeReallocator reallocator)
{
  gs_default_allocator = allocator;
  gs_default_deallocator = deallocator;
  gs_default_reallocator = reallocator;
}

KaffePool *KaffeCreatePool()
{
  KaffePool *pool;
  int i;

  assert(gs_default_allocator != NULL);
  assert(gs_default_deallocator != NULL);
  assert(gs_default_reallocator != NULL);

  pool = (KaffePool *)gs_default_allocator(sizeof(KaffePool));
  assert(pool != NULL);
  
  pool->num_nodes_in_pool = DEFAULT_NUMBER_OF_NODES_IN_POOL;
  pool->num_free_nodes = pool->num_nodes_in_pool;
  pool->pools = (KaffeNodeQueue **)gs_default_allocator(sizeof(KaffeNodeQueue));
  pool->pools[0] = 
    (KaffeNodeQueue *)gs_default_allocator(
       sizeof(KaffeNodeQueue)*pool->num_nodes_in_pool);
  pool->free_nodes =
    (KaffeNodeQueue **)gs_default_allocator(
       sizeof(KaffeNodeQueue *)*pool->num_nodes_in_pool);

  for (i=0;i<pool->num_nodes_in_pool;i++) {
    pool->free_nodes[i] = &pool->pools[0][i];
  }

  pool->num_pools = 1;
  pool->allocator = gs_default_allocator;
  pool->deallocator = gs_default_deallocator;
  pool->reallocator = gs_default_reallocator;

  return pool;
}

void KaffeDestroyPool(KaffePool *pool)
{
  int i;
  
  assert(pool != NULL);

  pool->deallocator(pool->pools);
  for (i=0;i<pool->num_pools;i++)
    pool->deallocator(pool->pools[i]);
  pool->deallocator(pool->free_nodes);
  pool->deallocator(pool);
}

KaffeNodeQueue *KaffePoolNewNode(KaffePool *pool)
{
  int chosen_node;

  assert(pool != NULL);
  if (pool->num_free_nodes == 0)
  {
    int old_free = pool->num_free_nodes;
    int i, j;

    /* We need to increment the number of internal pool nodes.
     * The array of free nodes is reallocated, a new pool 
     * array node is allocated (to preserve the old nodes).
     */
    pool->num_free_nodes += DEFAULT_NUMBER_OF_NODES_IN_POOL;
    pool->num_nodes_in_pool += DEFAULT_NUMBER_OF_NODES_IN_POOL;
    
    pool->free_nodes =
      pool->reallocator(pool->free_nodes,
			pool->num_free_nodes*sizeof(KaffeNodeQueue *));
    /* No more memory to handle threads, abort */
    /* TODO: Be friendly */
    assert(pool->free_nodes != NULL);

    pool->num_pools++;
    pool->pools = (KaffeNodeQueue **)
      pool->reallocator(pool->pools,
			pool->num_pools*sizeof(KaffeNodeQueue *));
    /* No more memory to handle threads, abort */
    assert(pool->pools != NULL);
    
    pool->pools[pool->num_pools-1] = (KaffeNodeQueue *)
      pool->allocator(sizeof(KaffeNodeQueue)*DEFAULT_NUMBER_OF_NODES_IN_POOL);

    for (j=0,i=old_free;i<pool->num_free_nodes;j++,i++)
      pool->free_nodes[i] = &pool->pools[pool->num_pools-1][j];
  }
  assert(pool->num_free_nodes != 0);

  pool->num_free_nodes--;
  chosen_node = pool->num_free_nodes;
  pool->free_nodes[chosen_node]->next = NULL;
  return pool->free_nodes[chosen_node];
}

void KaffePoolReleaseNode(KaffePool *pool, KaffeNodeQueue *node)
{
  assert(pool != NULL);
  /* 
   * This check cannot be done anymore as there different pools
   * now.
   *
   * int node_id = (int)(node-pool->pool)/sizeof(KaffeNodeQueue);
   * assert(node_id >= 0 && node_id < pool->num_nodes_in_pool);
   */
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
