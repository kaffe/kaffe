/* labels.h
 * Manage the labelling system.  These are used to provide the necessary
 * interlinking of branches and subroutine calls.
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#ifndef __label_h
#define __label_h

#define	Lnull		0x00		/** Label is unused */

#define	Ltypemask	0x0F		/** Label type mask */
#define	Lquad		0x01		/** Label is 64 bits long */
#define	Llong		0x02		/** Label is 32 bits long */

/* The following are handled by the architecture. */
#define	Lframe		0x03		/** Label is the frame size */
#define	Lnegframe	0x04		/** Label is the negative frame size */

#define	Larchdepend	(Lnegframe+1)	/** First architecture dependent label */

/* Modifications to "to"  */
#define	Ltomask		0x01F0		/** mask for values regarding 'to' */
#define	Lgeneral	0x0010		/** Label references general code */
#define	Lexternal	0x0020		/** Label references external routine */
#define	Lcode		0x0030		/** Label references bytecode offset */
#define	Lconstant	0x0040		/** Label references a constpool element */
#define	Linternal	0x0050		/** Label references internal routine */
#define	Lepilogue	0x0060		/** Label references internal epilogue */

/* Modifications to "at" */
#define	Latmask		0x2000		/** mask for values regarding 'at' */
#define	Lconstantpool	0x2000		/** Label is in contant pool */

/* Modifications to "from" */
#define	Lfrommask	0x0E00		/** mask for values regarding 'from' */
#define	Labsolute	0x0200		/** Absolute value */
#define	Lrelative	0x0400		/** Relative value to place of insertion */
#define	Lfuncrelative	0x0800		/** Relative value to start of function */

#define	Lrangecheck	0x1000		/** Check for overflow in the fixup */

#define	Lnoprofile	0x4000		/** don't profile this called label */

/**
 * The label object tracks the source, destination, and insertion point for
 * unbound references made by the generated code.
 *
 * next - Link to the next label in the global list.
 * at   - The code offset or constpool structure to fixup when addresses have
 *        been finalized.
 * to   - The reference to bind in the future.
 * from - The offset to use when computing a Lfuncrelative or Lrelative
 *        reference.
 * type - Holder for the above L* flags.
 * name - "Symbolic" label name.
 *
 * For example, a constpool reference for the PowerPC would look something
 * like:
 *
 *   l->to = newConstant(CPint, 0xdeadbeef);
 *   ... Generic code above, PowerPC specific code below ...
 *   l->type = Lconstant | Lrelative | Llong16noalign;
 *   l->at = CODEPC;
 *   l->from = r31FunctionStartOffset;
 *   // Load the word at r31 + 0 (to be fixed up later) into "w".
 *   LOUT = ppc_op_lwz(w, PPC_R31, 0);
 *
 * Where:
 *
 *   The "to" value points to a constpool object, whose "at" value will
 *   eventually be initialized to the absolute address of the constant value.
 *
 *   The "type" value indicates that this label points _to_ a constpool object,
 *   relative _from_ the method's base address plus some offset, and should
 *   be fixed up by the architecture dependent handler "Llong16noalign".
 *
 *   The "at" value holds an offset into the method pointing to the instruction
 *   to be fixed up when linkLabels() is called.
 *
 *   r31FunctionStartOffset is an integer holding the offset from the start
 *   of the method's instructions to the point where the PC was saved in
 *   register 31.
 *
 *   And finally, the "from" value is set to r31FunctionStartOffset and an
 *   instruction is emitted with the, to be overwritten value, zero.
 *
 * Then, when linkLabels(codebase) is called:
 *
 *   establishConstants() will have run, so the constpool's "at" value is
 *   pointing to the constant in memory (which precedes the method code).
 *
 *   The target address is then computed as:
 *     const->at - (codebase + r31FunctionStartOffset)
 *   And inserted into the lwz instruction by the Llong16noalign handler.
 */
typedef struct _label_ {
	struct _label_*	next;
	uintp		at;
	uintp		to;
	uintp		from;
	int		type;
#if defined(KAFFE_VMDEBUG)
	char name[8];
#endif
} label;

#define	ALLOCLABELNR	1024

/**
 * Labels are allocated and tracked as part of a chunk as represented by this
 * structure.
 *
 * next - Link to the next chunk in the list.
 * data - The labels contained in the chunk.
 */
typedef struct _labelchunk {
	struct _labelchunk *next;
	label data[ALLOCLABELNR];
} labelchunk;

/**
 * Set the address of all epilogue labels used in this method.
 *
 * Note:  There can be more than one epilogue label because there can be more
 * than one "return" in a method.
 *
 * @param to - The address of the epilogue instructions for this method.
 *
 * XXX Should be KaffeJIT3_setEpilogueLabels().
 */
void KaffeJIT3_setEpilogueLabel(uintp to);

/**
 * Finds and returns the last epilogue label created for this method.  This is
 * used to avoid generating noop branches on a normal return path.  For
 * example, a getter method on the x86 will generate the following code at the
 * end of the method:
 *
 *   0x22:  jmpl 0x27    # Jump to epilogue
 *   0x27:  popl %ebx    # Begin epilogue
 *   ...
 *
 * While this may not slow us down much, if at all, its a waste of five bytes,
 * so it would be nice to avoid such silliness.  Therefore, you grab the last
 * epilogue label and check to see if it corresponds to the previous
 * instruction.  If it is, then you backup CODEPC by however many instructions
 * and erase the label type so that it doesn't overwrite the epilogue code.
 * Otherwise, the method probably ended with a "throw" and doesn't have a
 * regular return path.
 *
 * returns - The last epilogue label created for this method or NULL if no
 *   epilogue labels were created.
 */
label *KaffeJIT3_getLastEpilogueLabel(void);

/**
 * Link all the active labels into the code.  This involves computing the
 * label addresses and fixing up the generated code with the final values
 * of the labels.  The code rewriting can be done by the library for simple
 * types like 32 or 64 bit numbers, but other types must be handled by the
 * architecture dependent code.
 *
 * @param codebase - The final destination for the method code.  Note: The constant
 *   pool, if there is one, will immediately precede this address.
 */
void KaffeJIT3_linkLabels(uintp codebase);

/**
 * @return A new label object that is linked into the global list.
 */
label* KaffeJIT3_newLabel(void);

/**
 * Reset the global list of labels for a new method.
 */
void KaffeJIT3_resetLabels(void);

/**
 * Iterate through the method internal labels that refer to the given native
 * "pc" value.
 *
 * @param lptr The iteration variable, initialize *lptr to NULL to start at the
 *   beginning of the list of labels.  The value of *lptr will then be changed
 *   after subsequent calls until the end of the list is reached.
 * @param pc The native PC value to search the "to" values for.
 * @return A label matching the given "pc" or NULL if there are no more
 *   internal labels found.
 */
label *KaffeJIT3_getInternalLabel(label **lptr, uintp counter);

#if defined(KAFFE_VMDEBUG)
/*
 * returns - The symbolic name of a label or the native pc offset if the
 *   referenced code has already been generated.  Useful for printing out
 *   the target of branches.
 */
char *KaffeJIT3_getLabelName(label *l);
#endif

#endif
