/* reference.c
 * Maintain the table of Java references. 
 * Implement special finalizer for reclaiming or clearing the references.
 *
 * Copyright (c) 2005
 *      The Kaffe.org's developers. See ChangeLog for details.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

#include "debug.h"
#include "gtypes.h"
#include "hashtab.h"
#include "baseClasses.h"
#include "reference.h"
#include "locks.h"
#include "gc.h"
#include "classMethod.h"
#include "native.h"
#include "thread.h"
#include "utf8const.h"
#include "kaffe/jmalloc.h"

typedef struct _referenceLinkList {
  jobject reference;
  kgc_reference_type weight;
  struct _referenceLinkList *next;
} referenceLinkList;

typedef struct _referenceLinkListHead {
  jobject obj;
  referenceLinkList *references;
} referenceLinkListHead;

static iStaticLock referencesLock;
static hashtab_t referencesHashTable;
static uint32 referentOffset = ~((uint32)0);

static int
objectHash(const void *p)
{
  const referenceLinkListHead *head = (const referenceLinkListHead *)p;
  void *obj = head->obj;

#if SIZEOF_INT == SIZEOF_VOID_P && SIZEOF_INT == 4
  return (int)(obj) & ((1UL << 31) - 1);
#elif SIZEOF_INT == SIZEOF_VOID_P && SIZEOF_INT == 8
  return (int)(obj) & ((1ULL << 63) - 1);
#elif SIZEOF_INT == 4 && SIZEOF_VOID_P == 8
  return (int)( ((uintp)obj >> 32) ^ ((uintp)obj & 0xffffffff));
#else
#error "Don't know what to do to build a hash function for objects."
#endif
}

static int
objectComp(const void *p1, const void *p2)
{
  const referenceLinkListHead *head1 = (const referenceLinkListHead *)p1;
  const referenceLinkListHead *head2 = (const referenceLinkListHead *)p2;
  return !(head1->obj == head2->obj);
}

void KaffeVM_referenceInit(void)
{
  referencesHashTable = hashInit(objectHash, objectComp, NULL, NULL);
  initStaticLock(&referencesLock);
}

void KaffeVM_registerObjectReference(jobject reference, jobject obj, kgc_reference_type reftype)
{
  referenceLinkList *ll;
  referenceLinkListHead *head, *temp;

  ll = KMALLOC(sizeof(referenceLinkList));
  ll->reference = reference;
  ll->weight = reftype;

  head = KMALLOC(sizeof(referenceLinkListHead));
  head->references = ll;
  head->obj = obj;
  
  lockStaticMutex(&referencesLock);
  temp = (referenceLinkListHead *)hashAdd(referencesHashTable, head);
  /* Check whether the hash entry was already filled by someone. */
  if (temp != 0 && temp != head)
    {
      KFREE(head);
      ll->next = temp->references;
      temp->references = ll;
    }
  unlockStaticMutex(&referencesLock);

  KaffeVM_setFinalizer(reference, KGC_OBJECT_REFERENCE_FINALIZER);
  KaffeVM_setFinalizer(obj, KGC_REFERENCE_FINALIZER);
 
  if (referentOffset == ~((uint32)0))
    {
      Utf8Const *referent_name = utf8ConstNew("referent", -1);
      Field *referent_field;
      errorInfo einfo;

      referent_field = lookupClassField(javaLangRefReference, referent_name, false, &einfo);
      utf8ConstRelease(referent_name);
      if (referent_field == NULL)
	{
	  dumpErrorInfo(&einfo);
	  DBG(REFERENCE,
	    dprintf("Internal error: The java/lang/ref/Reference class does not have any "
		    "'referent' field.\n"
		    "Aborting.\n");
	  );
	  KAFFEVM_ABORT();
	}

      referentOffset = FIELD_BOFFSET(referent_field);
    }

  KGC_addWeakRef(main_collector, obj,
		 (void **)((char *)reference + referentOffset));
}

bool KaffeVM_isReferenced(jobject ob)
{
  const referenceLinkListHead *ref;
  referenceLinkListHead search_ref;

  lockStaticMutex(&referencesLock);
  search_ref.obj = ob;
  ref = (referenceLinkListHead *)hashFind(referencesHashTable, &search_ref);
  unlockStaticMutex(&referencesLock);

  return ref != NULL;
}

static void
defaultObjectFinalizer(jobject ob)
{   
  Hjava_lang_Class* objclass;
  Hjava_lang_Object* obj = (Hjava_lang_Object*)ob;
  Method* final;
  
  objclass = OBJECT_CLASS(obj);

  final = objclass->finalizer;
  
  if (!final)
    {
      return;
    }

  KaffeVM_safeCallMethodA(final, METHOD_NATIVECODE(final), obj, NULL, NULL, 0);
  THREAD_DATA()->exceptObj = NULL;
}

static void
referenceObjectFinalizer(jobject ob)
{
  referenceLinkListHead *head;
  referenceLinkList *ll;
  referenceLinkListHead search_ref;

  lockStaticMutex(&referencesLock);
  search_ref.obj = ob;
  head = (referenceLinkListHead *)hashFind(referencesHashTable, &search_ref);
  hashRemove(referencesHashTable, head);
  unlockStaticMutex(&referencesLock);
  
  assert(head != NULL);
  assert(head->obj != NULL);
  ll = head->references;
  while (ll != NULL)
    {
      referenceLinkList *temp = ll->next;
      errorInfo einfo;
      Hjava_lang_Class *ref_clazz = OBJECT_CLASS((Hjava_lang_Object*)ll->reference);
      Method *mid = lookupClassMethod(ref_clazz, "enqueue", "()Z", &einfo);

      if (mid != NULL && !METHOD_IS_STATIC(mid))
	{
	  KaffeVM_safeCallMethodA(mid, METHOD_NATIVECODE(mid), ll->reference, NULL, NULL, false);
	  /* Clear any exception thrown */
	  THREAD_DATA()->exceptObj = NULL;
	}
      else
	{
	  DBG(REFERENCE,
	    dprintf("Internal error: a reference (%p) without the enqueue method "
		    "has been registered.", ll->reference);
	    dprintf("Aborting.\n");
	  );
	  KAFFEVM_ABORT();
	}

      DBG(REFERENCE, dprintf("Reference %p java-enqueued and C-dequeud\n", ll->reference); );
      KFREE(ll);
      ll = temp;
    }
  KFREE(head);
  defaultObjectFinalizer(ob);
}

static void
referenceFinalizer(jobject ref)
{
  void *referent;
  referenceLinkList **ll;
  referenceLinkListHead *head;
  referenceLinkListHead search_ref;

  DBG(REFERENCE, 
      dprintf("referenceFinalizer: finalizing reference %p (%s)\n", ref, 
	      CLASS_CNAME(OBJECT_CLASS((Hjava_lang_Object *)ref))); 
      );

  assert(referentOffset != ~((uint32)0));
  referent = *(void **)((char *)ref + referentOffset);
  if (referent == NULL)
    {
      DBG(REFERENCE,
	  dprintf("reference is NULL. The object has already been finalized\n");
	  );
      defaultObjectFinalizer(ref);
      return;
    }
  
  lockStaticMutex(&referencesLock);
  search_ref.obj = referent;
  head = (referenceLinkListHead *)hashFind(referencesHashTable, &search_ref);
  /* The object has already been finalized though the reference is still here. */
  if (head == NULL)
    {
      unlockStaticMutex(&referencesLock);
      DBG(REFERENCE,
	  dprintf("The reference has not been found in the hash table.\n");
	  );
      defaultObjectFinalizer(ref);
      return;
    }

  ll = &head->references;
  while (*ll != NULL)
    {
      if ((*ll)->reference == ref)
	{
	  void *temp = *ll;

	  *ll = (*ll)->next;
	  KFREE(temp);
	  break;
	}
      ll = &(*ll)->next;
    }
  unlockStaticMutex(&referencesLock);

  KGC_rmWeakRef(main_collector, referent,
		(void **)((char *)ref + referentOffset));

  defaultObjectFinalizer(ref);
}

void KaffeVM_setFinalizer(jobject obj, kgc_finalizer_type fintype)
{
  Hjava_lang_Object *kobj = (Hjava_lang_Object *)obj;

  switch (fintype)
    {
    case KGC_OBJECT_REFERENCE_FINALIZER:
      kobj->finalizer_call = referenceFinalizer;
      break;
    case KGC_DEFAULT_FINALIZER:
      kobj->finalizer_call = defaultObjectFinalizer;
      break;
    case KGC_REFERENCE_FINALIZER:
      kobj->finalizer_call = referenceObjectFinalizer;
      break;
    default:
      DBG(REFERENCE,
        dprintf("Internal error: invalid finalizer type %d for object %p.\n", fintype, obj);
        dprintf("Aborting.\n");
      );
      KAFFEVM_ABORT();
    }
}
