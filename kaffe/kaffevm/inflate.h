/*
 * inflate.h
 * JAR inflater.
 *
 * This is a heavily hacked version of Mark Adler's inflater taken from the
 * public domina UNZIP sources, version c15c, 28 March 1997.
 *
 * Copyright (c) 1996, 1997
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

#ifndef __inflate_h
#define __inflate_h

struct _huft;

typedef struct _inflateInfo {
	uint8*		slide;
	unsigned 	hufts;		/* track memory usage */
	struct _huft*	fixed_tl;
	struct _huft*	fixed_td;
	int		fixed_bl;
	int		fixed_bd;
	unsigned	wp;		/* current position in slide */
	uint32		bb;		/* bit buffer */
	unsigned	bk;		/* bits in bit buffer */
	uint8*		inbuf;
	int		insz;
	uint8*		outbuf;
	int		outsz;
} inflateInfo;

extern inflateInfo* inflate_new(void);
extern int inflate_free(inflateInfo*);
extern int inflate_oneshot(uint8*, int, uint8*, int);

#endif
