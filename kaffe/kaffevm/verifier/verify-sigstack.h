/*
 * verify-sigstack.h
 *
 * Copyright 2004
 *   Kaffe.org contributors. See ChangeLog for details. All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 *
 * Interface to code for handing of signature stack in the verifier.
 */

#if !defined(VERIFY_SIGSTACK_H)
#define VERIFY_SIGSTACK_H

/*
 * the sig stack is the stack of signatures that we have allocated memory for which
 * must be freed after verification.
 */
/* TODO: use the builtin hash table data structure instead 
 * so we can avoid repeats
 */
typedef struct SigStack
{
	const char* sig;
	struct SigStack* next;
} SigStack;

extern SigStack*          pushSig(SigStack* sigs, const char* sig);
extern void               freeSigStack(SigStack* sigs);

#endif /* !defined(VERIFY_SIGSTACK_H) */
