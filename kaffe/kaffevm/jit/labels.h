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

#define	Lnull		0x00		/* Unused label */

#define	Ltypemask	0x0F		/* Label type mask */
#define Lquad		0x01		/* Label is 64 bits long */
#define	Llong		0x02		/* Label is 32 bits long */
#define	Lframe		0x03		/* Label is the frame size */
#define Lnegframe	0x04		/* Label is the negative frame size */

#define	Larchdepend	(Lnegframe+1)	/* First architecture dependent label */

/* Modifications to "to"  */
#define Ltomask		0x1F0
#define	Lgeneral	0x010	/* Label references general code */
#define Lexternal	0x020	/* Label references external routine */
#define	Lcode		0x030	/* Label references bytecode offset */
#define Lconstant	0x040	/* Label references a constpool element */
#define	Linternal	0x050	/* Label references internal routine */
#define	Lepilogue	0x060	/* Label references internal epilogue */

/* Modifications to "from" */
#define Lfrommask	0x600
#define	Labsolute	0x200	/* Absolute value */
#define	Lrelative	0x400	/* Relative value to place of insertion */

#define Lrangecheck	0x1000	/* Check for overflow in the fixup */

#define Lnoprofile	0x2000	/* don't profile this called label */

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

struct codeinfo;
void KaffeJIT_setEpilogueLabel(uintp);
void KaffeJIT_linkLabels(struct codeinfo*, uintp);
label* KaffeJIT_newLabel(void);
void KaffeJIT_resetLabels(void);

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
label *KaffeJIT_getInternalLabel(label **lptr, uintp counter);

#endif
