/*
 * inflate.c
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

#define	IDBG(s) /* XXX */

#include "config.h"
#include "config-std.h"
#include "config-mem.h"
#include "debug.h"
#include "gtypes.h"
#include "gc.h"
#include "inflate.h"

#define	WSIZE	0x8000

typedef struct _huft {
	uint8	e;		/* number of extra bits or operation */
	uint8	b;		/* number of bits in this code or subcode */
	union {
		uint16	n;	/* literal, length base, or distance base */
		struct _huft* t;/* pointer to next level of table */
	} v;
} huft;

static int inflate_codes(inflateInfo*, huft*, huft*, size_t, size_t);
static int inflate_stored(inflateInfo*);
static int inflate_fixed(inflateInfo*);
static int inflate_dynamic(inflateInfo*);
static int inflate_block(inflateInfo*, int*);
static int huft_free(huft*);
static int inflate(inflateInfo*);
static int huft_build(inflateInfo* pG, unsigned* b, unsigned n, unsigned s, uint16* d, uint16* e, huft** t, size_t* m);

/* Tables for deflate from PKZIP's appnote.txt. */
static unsigned border[] = {    /* Order of the bit length code lengths */
        16, 17, 18, 0, 8, 7, 9, 6, 10, 5, 11, 4, 12, 3, 13, 2, 14, 1, 15};

static uint16 cplens[] = {         /* Copy lengths for literal codes 257..285 */
        3, 4, 5, 6, 7, 8, 9, 10, 11, 13, 15, 17, 19, 23, 27, 31,
        35, 43, 51, 59, 67, 83, 99, 115, 131, 163, 195, 227, 258, 0, 0};
        /* note: see note #13 above about the 258 in this list. */

static uint16 cplext[] = {         /* Extra bits for literal codes 257..285 */
        0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 2,
        3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5, 0, 99, 99}; /* 99==invalid */

static uint16 cpdist[] = {         /* Copy offsets for distance codes 0..29 */
        1, 2, 3, 4, 5, 7, 9, 13, 17, 25, 33, 49, 65, 97, 129, 193,
        257, 385, 513, 769, 1025, 1537, 2049, 3073, 4097, 6145,
        8193, 12289, 16385, 24577};

static uint16 cpdext[] = {         /* Extra bits for distance codes */
        0, 0, 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6,
        7, 7, 8, 8, 9, 9, 10, 10, 11, 11,
        12, 12, 13, 13};

static uint16 mask_bits[] = {
    0x0000,
    0x0001, 0x0003, 0x0007, 0x000f, 0x001f, 0x003f, 0x007f, 0x00ff,
    0x01ff, 0x03ff, 0x07ff, 0x0fff, 0x1fff, 0x3fff, 0x7fff, 0xffff
};

#define	LBITS	9		/* bits in base literal/length lookup table */
#define	DBITS	6		/* bits in base distance lookup table */

#define NEEDBITS(G, N)						\
	{							\
		uint32 c;					\
		while (k < (N)) {				\
			if ((G)->insz <= 0) {			\
				return (1);			\
			}					\
			c = (G)->inbuf[0];			\
			(G)->inbuf++;				\
			b |= c << k;				\
			k += 8;					\
		}						\
	}

#define DUMPBITS(G, N)						\
	{							\
		b >>= (N);					\
		k -= (N);					\
	}

#define	FLUSH(G, N)						\
	{							\
		int c;						\
		int p;						\
		c = ((N) < (G)->outsz ? (N) : (G)->outsz);	\
		for (p = 0; c > 0; p++, c--) {			\
			(G)->outbuf[p] = (G)->slide[p];		\
		}						\
		(G)->outbuf += p;				\
		(G)->outsz -= p;				\
	}


static
int
inflate_codes(inflateInfo* pG, huft* tl, huft* td, size_t bl, size_t bd)
{
  register size_t e;  /* table entry flag/number of extra bits */
  size_t n, d;        /* length and index for copy */
  size_t w;           /* current window position */
  huft *t;       /* pointer to table entry */
  int ml, md;      /* masks for bl and bd bits */
  register uint32 b;       /* bit buffer */
  register size_t k;  /* number of bits in bit buffer */


  /* make local copies of globals */
  b = pG->bb;                       /* initialize bit buffer */
  k = pG->bk;
  w = pG->wp;                       /* initialize window position */


  /* inflate the coded data */
  ml = mask_bits[bl];           /* precompute masks for speed */
  md = mask_bits[bd];
  while (1)                     /* do until end of block */
  {
    NEEDBITS(pG, bl)
    if ((e = (t = tl + ((unsigned)b & ml))->e) > 16)
      do {
        if (e == 99)
          return 1;
        DUMPBITS(pG, t->b)
        e -= 16;
        NEEDBITS(pG, e)
      } while ((e = (t = t->v.t + ((unsigned)b & mask_bits[e]))->e) > 16);
    DUMPBITS(pG, t->b)
    if (e == 16)                /* then it's a literal */
    {
      pG->slide[w++] = (uint8)t->v.n;
      if (w == WSIZE)
      {
        FLUSH(pG, w);
        w = 0;
      }
    }
    else                        /* it's an EOB or a length */
    {
      /* exit if end of block */
      if (e == 15)
        break;

      /* get length of block to copy */
      NEEDBITS(pG, e)
      n = t->v.n + ((unsigned)b & mask_bits[e]);
      DUMPBITS(pG, e);

      /* decode distance of block to copy */
      NEEDBITS(pG, bd)
      if ((e = (t = td + ((unsigned)b & md))->e) > 16)
        do {
          if (e == 99)
            return 1;
          DUMPBITS(pG, t->b)
          e -= 16;
          NEEDBITS(pG, e)
        } while ((e = (t = t->v.t + ((unsigned)b & mask_bits[e]))->e) > 16);
      DUMPBITS(pG, t->b)
      NEEDBITS(pG, e)
      d = w - t->v.n - ((unsigned)b & mask_bits[e]);
      DUMPBITS(pG, e)

      /* do the copy */
      do {
          n -= (e = (e = WSIZE - ((d &= WSIZE-1) > w ? d : w)) > n ? n : e);
        if (w - d >= e)         /* (this test assumes unsigned comparison) */
        {
          memcpy(pG->slide + w, pG->slide + d, e);
          w += e;
          d += e;
        }
        else                    /* do it slowly to avoid memcpy() overlap */
          do {
            pG->slide[w++] = pG->slide[d++];
          } while (--e);
        if (w == WSIZE)
        {
          FLUSH(pG, w);
          w = 0;
        }
      } while (n);
    }
  }


  /* restore the globals from the locals */
  pG->wp = w;                       /* restore global window pointer */
  pG->bb = b;                       /* restore global bit buffer */
  pG->bk = k;


  /* done */
  return 0;
}

static
int
inflate_stored(inflateInfo* pG)
{
  size_t n;           /* number of bytes in block */
  size_t w;           /* current window position */
  register int32 b;       /* bit buffer */
  register int k;  /* number of bits in bit buffer */


  /* make local copies of globals */
  IDBG(dprintf("\nstored block"));
  b = pG->bb;                       /* initialize bit buffer */
  k = pG->bk;
  w = pG->wp;                       /* initialize window position */


  /* go to byte boundary */
  n = k & 7;
  DUMPBITS(pG, n);


  /* get the length and its complement */
  NEEDBITS(pG, 16)
  n = ((unsigned)b & 0xffff);
  DUMPBITS(pG, 16)
  NEEDBITS(pG, 16)
  if (n != (unsigned)((~b) & 0xffff))
    return 1;                   /* error in compressed data */
  DUMPBITS(pG, 16)


  /* read and output the compressed data */
  while (n--)
  {
    NEEDBITS(pG, 8)
    pG->slide[w++] = (uint8)b;
    if (w == WSIZE)
    {
      FLUSH(pG, w);
      w = 0;
    }
    DUMPBITS(pG, 8)
  }


  /* restore the globals from the locals */
  pG->wp = w;                       /* restore global window pointer */
  pG->bb = b;                       /* restore global bit buffer */
  pG->bk = k;
  return 0;
}


static
int
inflate_fixed(inflateInfo* pG)
{
  /* if first time, set up tables for fixed blocks */
  IDBG(dprintf("\nliteral block"));
  if (pG->fixed_tl == 0)
  {
    int i;                /* temporary variable */
    unsigned l[288];      /* length list for huft_build */

    /* literal table */
    for (i = 0; i < 144; i++)
      l[i] = 8;
    for (; i < 256; i++)
      l[i] = 9;
    for (; i < 280; i++)
      l[i] = 7;
    for (; i < 288; i++)          /* make a complete, but wrong code set */
      l[i] = 8;
    pG->fixed_bl = 7;
    if ((i = huft_build(pG, l, 288, 257, cplens, cplext,
                        &pG->fixed_tl, &pG->fixed_bl)) != 0)
    {
      pG->fixed_tl = NULL;
      return i;
    }

    /* distance table */
    for (i = 0; i < 30; i++)      /* make an incomplete code set */
      l[i] = 5;
    pG->fixed_bd = 5;
    if ((i = huft_build(pG, l, 30, 0, cpdist, cpdext,
                        &pG->fixed_td, &pG->fixed_bd)) > 1)
    {
      huft_free(pG->fixed_tl);
      pG->fixed_tl = NULL;
      return i;
    }
  }

  /* decompress until an end-of-block code */
  return inflate_codes(pG, pG->fixed_tl, pG->fixed_td,
                             pG->fixed_bl, pG->fixed_bd) != 0;
}

static
int
inflate_dynamic(inflateInfo* pG)
{
  int i;                /* temporary variables */
  unsigned j;
  unsigned l;           /* last length */
  unsigned m;           /* mask for bit lengths table */
  unsigned n;           /* number of lengths to get */
  huft *tl;      /* literal/length code table */
  huft *td;      /* distance code table */
  size_t bl;            /* lookup bits for tl */
  size_t bd;            /* lookup bits for td */
  unsigned nb;          /* number of bit length codes */
  unsigned nl;          /* number of literal/length codes */
  unsigned nd;          /* number of distance codes */
  unsigned ll[288+32]; /* literal/length and distance code lengths */
  register uint32 b;       /* bit buffer */
  register unsigned k;  /* number of bits in bit buffer */


  /* make local bit buffer */
  IDBG(dprintf("\ndynamic block"));
  b = pG->bb;
  k = pG->bk;


  /* read in table lengths */
  NEEDBITS(pG, 5)
  nl = 257 + ((unsigned)b & 0x1f);      /* number of literal/length codes */
  DUMPBITS(pG, 5)
  NEEDBITS(pG, 5)
  nd = 1 + ((unsigned)b & 0x1f);        /* number of distance codes */
  DUMPBITS(pG, 5)
  NEEDBITS(pG, 4)
  nb = 4 + ((unsigned)b & 0xf);         /* number of bit length codes */
  DUMPBITS(pG, 4)
  if (nl > 288 || nd > 32)
    return 1;                   /* bad lengths */


  /* read in bit-length-code lengths */
  for (j = 0; j < nb; j++)
  {
    NEEDBITS(pG, 3)
    ll[border[j]] = (unsigned)b & 7;
    DUMPBITS(pG, 3)
  }
  for (; j < 19; j++)
    ll[border[j]] = 0;


  /* build decoding table for trees--single level, 7 bit lookup */
  bl = 7;
  i = huft_build(pG, ll, 19, 19, NULL, NULL, &tl, &bl);
  if (bl == 0)                        /* no bit lengths */
    i = 1;
  if (i)
  {
    if (i == 1)
      huft_free(tl);
    return i;                   /* incomplete code set */
  }


  /* read in literal and distance code lengths */
  n = nl + nd;
  m = mask_bits[bl];
  i = l = 0;
  while ((unsigned)i < n)
  {
    NEEDBITS(pG, (unsigned)bl)
    j = (td = tl + ((unsigned)b & m))->b;
    DUMPBITS(pG, j)
    j = td->v.n;
    if (j < 16)                 /* length of code in bits (0..15) */
      ll[i++] = l = j;          /* save last length in l */
    else if (j == 16)           /* repeat last length 3 to 6 times */
    {
      NEEDBITS(pG, 2)
      j = 3 + ((unsigned)b & 3);
      DUMPBITS(pG, 2)
      if ((unsigned)i + j > n)
        return 1;
      while (j--)
        ll[i++] = l;
    }
    else if (j == 17)           /* 3 to 10 zero length codes */
    {
      NEEDBITS(pG, 3)
      j = 3 + ((unsigned)b & 7);
      DUMPBITS(pG, 3)
      if ((unsigned)i + j > n)
        return 1;
      while (j--)
        ll[i++] = 0;
      l = 0;
    }
    else                        /* j == 18: 11 to 138 zero length codes */
    {
      NEEDBITS(pG, 7)
      j = 11 + ((unsigned)b & 0x7f);
      DUMPBITS(pG, 7)
      if ((unsigned)i + j > n)
        return 1;
      while (j--)
        ll[i++] = 0;
      l = 0;
    }
  }


  /* free decoding table for trees */
  huft_free(tl);


  /* restore the global bit buffer */
  pG->bb = b;
  pG->bk = k;


  /* build the decoding tables for literal/length and distance codes */
  bl = LBITS;
  i = huft_build(pG, ll, nl, 257, cplens, cplext, &tl, &bl);
  if (bl == 0)                        /* no literals or lengths */
    i = 1;
  if (i)
  {
    if (i == 1) {
      huft_free(tl);
    }
    return i;                   /* incomplete code set */
  }
  bd = DBITS;
  i = huft_build(pG, ll + nl, nd, 0, cpdist, cpdext, &td, &bd);
  if (bd == 0 && nl > 257)    /* lengths but no distances */
  {
    huft_free(tl);
    return 1;
  }
  if (i == 1) {
    i = 0;
  }
  if (i)
  {
    huft_free(tl);
    return i;
  }


  /* decompress until an end-of-block code */
  if (inflate_codes(pG, tl, td, bl, bd))
    return 1;


  /* free the decoding tables, return */
  huft_free(tl);
  huft_free(td);
  return 0;
}



static
int
inflate_block(inflateInfo* pG, int* e)
{
  unsigned t;           /* block type */
  register uint32 b;       /* bit buffer */
  register unsigned k;  /* number of bits in bit buffer */


  /* make local bit buffer */
  b = pG->bb;
  k = pG->bk;


  /* read in last block bit */
  NEEDBITS(pG, 1)
  *e = (int)b & 1;
  DUMPBITS(pG, 1)


  /* read in block type */
  NEEDBITS(pG, 2)
  t = (unsigned)b & 3;
  DUMPBITS(pG, 2)


  /* restore the global bit buffer */
  pG->bb = b;
  pG->bk = k;


  /* inflate that block type */
  if (t == 2)
    return inflate_dynamic(pG);
  if (t == 0)
    return inflate_stored(pG);
  if (t == 1)
    return inflate_fixed(pG);


  /* bad block type */
  return 2;
}

/*
 * Create a new inflater.
 */
inflateInfo*
inflate_new(void)
{
	inflateInfo* info;

	info = gc_malloc(sizeof(inflateInfo), KGC_ALLOC_FIXED);
	if (!info) {
		return NULL;
	}
	info->fixed_tl = NULL;
	info->fixed_td = NULL;
	info->fixed_bl = 0;
	info->fixed_bd = 0;
	info->slide = gc_malloc(WSIZE, KGC_ALLOC_FIXED);
	if (!info->slide){
		gc_free(info);
		return NULL;
	}

	return (info);
}

/*
 * We pass in a buffer of deflated data and a place to stored the inflated
 * result.  This does not provide continuous operation and should only be
 * use in "one shot" more.
 */
int
inflate_oneshot(uint8* ibuf, int ilen, uint8* obuf, int olen)
{
	int r;                /* result code: 0 on success */
	inflateInfo* pG;

	pG = inflate_new();

	if (!pG) {
		return 1;
	}
	
	pG->inbuf = ibuf;
	pG->insz = ilen;
	pG->outbuf = obuf;
	pG->outsz = olen;

	r = inflate(pG);

	inflate_free(pG);

	return (r);
}

/*
 * Inflate the given data into the given buffer.
 */
static
int
inflate(inflateInfo* pG)
{
  int e;                /* last block flag */
  int r;                /* result code */
  unsigned h;           /* maximum huft's malloc'ed */

  /* initialize window, bit buffer */
  pG->wp = 0;
  pG->bk = 0;
  pG->bb = 0;

  /* decompress until the last block */
  h = 0;
  do {
    pG->hufts = 0;
    if ((r = inflate_block(pG, &e)) != 0) {
      return r;
    }
    if (pG->hufts > h) {
      h = pG->hufts;
    }
  } while (!e);


  /* flush out G.slide */
  FLUSH(pG, pG->wp);

  /* return success */
  IDBG(dprintf("\n%u bytes in Huffman tables (%d/entry)\n", h * sizeof(huft), sizeof(huft)));
  return 0;
}

int
inflate_free(inflateInfo* pG)
{
  if (pG != 0) {
    if (pG->fixed_tl != 0) {
      huft_free(pG->fixed_td);
      huft_free(pG->fixed_tl);
      pG->fixed_td = pG->fixed_tl = NULL;
    }
    gc_free(pG->slide);
    gc_free(pG);
  }

  return 0;
}


/* If BMAX needs to be larger than 16, then h and x[] should be uint32. */
#define BMAX 16         /* maximum bit length of any code (16 for explode) */
#define N_MAX 288       /* maximum number of codes in any set */

static
int
huft_build(inflateInfo* pG, unsigned* b, unsigned n, unsigned s, uint16* d, uint16* e, huft** t, size_t* m)
{
  unsigned a;                   /* counter for codes of length k */
  unsigned c[BMAX+1];           /* bit length count table */
  unsigned el;                  /* length of EOB code (value 256) */
  unsigned f;                   /* i repeats in table every f entries */
  size_t g;                     /* maximum code length */
  int h;                        /* table level */
  register unsigned i;          /* counter, current code */
  register unsigned j;          /* counter */
  register size_t k;            /* number of bits in current code */
  int lx[BMAX+1];               /* memory for l[-1..BMAX-1] */
  int *l = lx+1;                /* stack of bits per table */
  register unsigned *p;         /* pointer into c[], b[], or v[] */
  register huft *q;      /* points to current table */
  huft r;                /* table entry for structure assignment */
  huft *u[BMAX];         /* table stack */
  unsigned v[N_MAX];            /* values in order of bit length */
  register size_t w;            /* bits before this table == (l * h) */
  unsigned x[BMAX+1];           /* bit offsets, then code stack */
  unsigned *xp;                 /* pointer into x */
  int y;                        /* number of dummy codes added */
  size_t z;                     /* number of entries in current table */


  /* Generate counts for each bit length */
  el = n > 256 ? b[256] : BMAX; /* set length of EOB code, if any */
  memset(c, 0, sizeof(c));
  p = b;  i = n;
  do {
    c[*p]++; p++;               /* assume all entries <= BMAX */
  } while (--i);
  if (c[0] == n)                /* null input--all zero length codes */
  {
    *t = NULL;
    *m = 0;
    return 0;
  }


  /* Find minimum and maximum length, bound *m by those */
  for (j = 1; j <= BMAX; j++)
    if (c[j])
      break;
  k = j;                        /* minimum code length */
  if ((unsigned)*m < j)
    *m = j;
  for (i = BMAX; i; i--)
    if (c[i])
      break;
  g = i;                        /* maximum code length */
  if ((unsigned)*m > i)
    *m = i;


  /* Adjust last length count to fill out codes, if needed */
  for (y = 1 << j; j < i; j++, y <<= 1)
    if ((y -= c[j]) < 0)
      return 2;                 /* bad input: more codes than bits */
  if ((y -= c[i]) < 0)
    return 2;
  c[i] += y;


  /* Generate starting offsets into the value table for each length */
  x[1] = j = 0;
  p = c + 1;  xp = x + 2;
  while (--i) {                 /* note that i == g from above */
    *xp++ = (j += *p++);
  }


  /* Make a table of values in order of bit lengths */
  memset(v, 0, sizeof(v));
  p = b;  i = 0;
  do {
    if ((j = *p++) != 0)
      v[x[j]++] = i;
  } while (++i < n);
  n = x[g];                     /* set n to length of v */


  /* Generate the Huffman codes and for each, make the table entries */
  x[0] = i = 0;                 /* first Huffman code is zero */
  p = v;                        /* grab values in bit order */
  h = -1;                       /* no tables yet--level -1 */
  w = l[-1] = 0;                /* no bits decoded yet */
  u[0] = NULL;   /* just to keep compilers happy */
  q = NULL;      /* ditto */
  z = 0;                        /* ditto */

  /* go through the bit lengths (k already is bits in shortest code) */
  for (; k <= g; k++)
  {
    a = c[k];
    while (a--)
    {
      /* here i is the Huffman code of length k bits for value *p */
      /* make tables up to required level */
      while (k > w + l[h])
      {
        w += l[h++];            /* add bits already decoded */

        /* compute minimum size table less than or equal to *m bits */
        z = (z = g - w) > *m ? *m : z;        /* upper limit */
        if ((f = 1 << (j = k - w)) > a + 1)     /* try a k-w bit table */
        {                       /* too few codes for k-w bit table */
          f -= a + 1;           /* deduct codes from patterns left */
          xp = c + k;
          while (++j < z)       /* try smaller tables up to z bits */
          {
            if ((f <<= 1) <= *++xp)
              break;            /* enough codes to use up j bits */
            f -= *xp;           /* else deduct codes from patterns */
          }
        }
        if ((unsigned)w + j > el && (unsigned)w < el)
          j = el - w;           /* make EOB code end at table */
        z = 1 << j;             /* table entries for j-bit table */
        l[h] = j;               /* set table size in stack */

        /* allocate and link in new table */
        if ((q = (huft *)gc_malloc((z + 1)*sizeof(huft), KGC_ALLOC_FIXED)) ==
            0)
        {
          if (h)
            huft_free(u[0]);
          return 3;             /* not enough memory */
        }
        pG->hufts += z + 1;         /* track memory usage */
        *t = q + 1;             /* link to list for huft_free() */
        *(t = &(q->v.t)) = NULL;
        u[h] = ++q;             /* table starts after link */

        /* connect to last table, if there is one */
        if (h)
        {
          x[h] = i;             /* save pattern for backing up */
          r.b = (uint8)l[h-1];    /* bits to dump before this table */
          r.e = (uint8)(16 + j);  /* bits in this table */
          r.v.t = q;            /* pointer to this table */
          j = (i & ((1 << w) - 1)) >> (w - l[h-1]);
          u[h-1][j] = r;        /* connect to last table */
        }
      }

      /* set up table entry in r */
      r.b = (uint8)(k - w);
      if (p >= v + n)
        r.e = 99;               /* out of values--invalid code */
      else if (*p < s)
      {
        r.e = (uint8)(*p < 256 ? 16 : 15);  /* 256 is end-of-block code */
        r.v.n = (uint16)*p++;                /* simple code is just the value */
      }
      else
      {
        r.e = (uint8)e[*p - s];   /* non-simple--look up in lists */
        r.v.n = d[*p++ - s];
      }

      /* fill code-like entries with r */
      f = 1 << (k - w);
      for (j = i >> w; j < z; j += f)
        q[j] = r;

      /* backwards increment the k-bit code i */
      for (j = 1 << (k - 1); i & j; j >>= 1)
        i ^= j;
      i ^= j;

      /* backup over finished tables */
      while ((i & ((1 << w) - 1)) != x[h])
        w -= l[--h];            /* don't need to update q */
    }
  }


  /* return actual size of base table */
  *m = l[0];


  /* Return true (1) if we were given an incomplete table */
  return y != 0 && g != 1;
}



static
int
huft_free(huft* t)
{
  huft *p, *q;

  /* Go through linked list, freeing from the malloced (t[-1]) address. */
  p = t;
  while (p != 0)
  {
    q = (--p)->v.t;
    gc_free(p);
    p = q;
  }
  return 0;
}
