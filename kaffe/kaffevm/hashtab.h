/*
 * hashtab.h
 *
 * Internal hash table library.
 *
 * Copyright (c) 1998
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#ifndef __hashtab_h
#define __hashtab_h

/*
 * Intern hash table library.
 * 
 * This keeps a hash table consisting of pointers to whatever, where
 * whatever is some chunk of memory allocated from the GC. This is used
 * for intern'ing strings, etc.
 * 
 * The Hash table is not walked by the GC; however, you can optionally
 * specify that each pointer added to the hash table adds a GC reference
 * to the memory so it won't get collected (removing the pointer from
 * the hash table removes the GC reference).
 * 
 * You supply the hashing function and the equality tester.
 *
 * The hashfunc should return a 32 bit hash value.
 *
 * The compfunc_t should return 0 iff the two memory blocks are "equal".
 * It is assumed that compfunc(x, x) == 0. It is always called such
 * that the first argument is the pointer in question and the second
 * argument is the pointer already in the hash table.
 */

/* Flags to hashInit() */
#define HASH_ADD_REFS		0x0001	/* gc_add_ref() for each pointer */
#define HASH_SYNCHRONIZE	0x0002	/* make hash table thread-safe */
#define HASH_REENTRANT		0x0004	/* make hash table reenterable 
					   during alloc/free */

struct _hashtab;
typedef struct _hashtab	*hashtab_t;
typedef int		(*hashfunc_t)(const void *ptr1);
typedef int		(*compfunc_t)(const void *ptr1, const void *ptr2);
typedef void*		(*allocfunc_t)(size_t);
typedef void		(*freefunc_t)(const void *ptr);

extern hashtab_t	hashInit(hashfunc_t, compfunc_t, 
				 allocfunc_t, freefunc_t, int);
extern void*		hashAdd(hashtab_t, const void*);
extern void		hashRemove(hashtab_t, const void*);
extern void*		hashFind(hashtab_t, const void*);
extern void		hashDestroy(hashtab_t);

#endif	/* __hashtab_h */

