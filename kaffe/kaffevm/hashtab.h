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
 * for intern'ing strings, etc.  Note that this hashtable implementation
 * operates on pointers only, it can't be used for storing arbitrary
 * (key, value) pairs.
 * 
 * The Hash table is not walked by the GC.
 * You can supply a function to allocate memory for the hashtable.
 * If you do not, kaffe's default KCALLOC/KFREE will be used.
 *
 * You are responsible for providing appropriate synchronization.
 * You are allowed to remove entries while more memory is being allocated
 * when the table is being resized.
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

struct _hashtab;
typedef struct _hashtab	*hashtab_t;
typedef int		(*hashfunc_t)(const void *ptr1);
typedef int		(*compfunc_t)(const void *ptr1, const void *ptr2);
typedef void*		(*allocfunc_t)(size_t);
typedef void		(*freefunc_t)(const void *ptr);

extern hashtab_t	hashInit(hashfunc_t, compfunc_t, 
				 allocfunc_t, freefunc_t);
extern const void*	hashAdd(hashtab_t, const void*);
extern void		hashRemove(hashtab_t, const void*);
extern const void*	hashFind(hashtab_t, const void*);
extern void		hashDestroy(hashtab_t);

#endif	/* __hashtab_h */

