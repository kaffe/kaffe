
/*
 * Copyright (c) 1995-1999 Eric Young
 * All rights reserved.
 * 
 * Subject to the following obligations and disclaimer of warranty, use and
 * redistribution of this software, in source or object code forms, with or
 * without modifications are expressly permitted by Eric Young; provided,
 * however, that:
 * 1. Any and all reproductions of the source or object code must include the
 *    copyright notice above and the following disclaimer of warranties; and
 * 2. No rights are granted, in any manner or form, to use Eric Young
 *    trademarks, including the mark "Eric Young" on advertising,
 *    endorsements, or otherwise except as such appears in the above
 *    copyright notice or in the software.
 * 
 * THIS SOFTWARE IS BEING PROVIDED BY ERIC YOUNG "AS IS", AND TO THE
 * MAXIMUM EXTENT PERMITTED BY LAW, ERIC YOUNG MAKES NO REPRESENTATIONS
 * OR WARRANTIES, EXPRESS OR IMPLIED, REGARDING THIS SOFTWARE,
 * INCLUDING WITHOUT LIMITATION, ANY AND ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, OR NON-INFRINGEMENT.
 * ERIC YOUNG DOES NOT WARRANT, GUARANTEE, OR MAKE ANY REPRESENTATIONS
 * REGARDING THE USE OF, OR THE RESULTS OF THE USE OF THIS SOFTWARE
 * IN TERMS OF ITS CORRECTNESS, ACCURACY, RELIABILITY OR OTHERWISE.
 * IN NO EVENT SHALL ERIC YOUNG BE LIABLE FOR ANY DAMAGES RESULTING
 * FROM OR ARISING OUT OF ANY USE OF THIS SOFTWARE, INCLUDING WITHOUT
 * LIMITATION, ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY,
 * PUNITIVE, OR CONSEQUENTIAL DAMAGES, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES, LOSS OF USE, DATA OR PROFITS, HOWEVER CAUSED
 * AND UNDER ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ERIC YOUNG IS
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _SHA_1_H_
#define _SHA_1_H_

#define SHA_CBLOCK	64
#define SHA_LBLOCK	16
#define SHA_BLOCK	16
#define SHA_LAST_BLOCK  56
#define SHA_LENGTH_BLOCK 8
#define SHA_DIGEST_LENGTH 20

typedef struct SHAstate_st
	{
	unsigned long h0,h1,h2,h3,h4;
	unsigned long Nl,Nh;
	unsigned long data[SHA_LBLOCK];
	int num;
	} SHA1_CTX;

void SHA1Init(SHA1_CTX *c);
void SHA1Update(SHA1_CTX *c, const unsigned char *data, unsigned long len);
void SHA1Final(unsigned char *md, SHA1_CTX *c);

/* one or the other needs to be defined */
#define SHA_1 /* FIPE 180-1 */

#define ULONG	unsigned long
#define UCHAR	unsigned char
#define UINT	unsigned int

#undef c2nl
#define c2nl(c,l)	(l =(((unsigned long)(*((c)++)))<<24), \
			 l|=(((unsigned long)(*((c)++)))<<16), \
			 l|=(((unsigned long)(*((c)++)))<< 8), \
			 l|=(((unsigned long)(*((c)++)))    ))

#undef p_c2nl
#define p_c2nl(c,l,n)	{ \
			switch (n) { \
			case 0: l =((unsigned long)(*((c)++)))<<24; \
			case 1: l|=((unsigned long)(*((c)++)))<<16; \
			case 2: l|=((unsigned long)(*((c)++)))<< 8; \
			case 3: l|=((unsigned long)(*((c)++))); \
			default: \
				break; \
				} \
			}

#undef c2nl_p
/* NOTE the pointer is not incremented at the end of this */
#define c2nl_p(c,l,n)	{ \
			l=0; \
			(c)+=n; \
			switch (n) { \
			case 3: l =((unsigned long)(*(--(c))))<< 8; \
			case 2: l|=((unsigned long)(*(--(c))))<<16; \
			case 1: l|=((unsigned long)(*(--(c))))<<24; \
			default: \
				break; \
				} \
			}

#undef p_c2nl_p
#define p_c2nl_p(c,l,sc,len) { \
			switch (sc) \
				{ \
			case 0: l =((unsigned long)(*((c)++)))<<24; \
				if (--len == 0) break; \
			case 1: l|=((unsigned long)(*((c)++)))<<16; \
				if (--len == 0) break; \
			case 2: l|=((unsigned long)(*((c)++)))<< 8; \
			default: \
				break; \
				} \
			}

#undef nl2c
#define nl2c(l,c)	(*((c)++)=(unsigned char)(((l)>>24)&0xff), \
			 *((c)++)=(unsigned char)(((l)>>16)&0xff), \
			 *((c)++)=(unsigned char)(((l)>> 8)&0xff), \
			 *((c)++)=(unsigned char)(((l)    )&0xff))

/* I have taken some of this code from my MD5 implementation */

#undef ROTATE
#if defined(WIN32) && !defined(__GNUC__)
#define ROTATE(a,n)     _lrotl(a,n)
#else
#define ROTATE(a,n)     (((a)<<(n))|(((a)&0xffffffff)>>(32-(n))))
#endif

/* A nice byte order reversal from Wei Dai <weidai@eskimo.com> */
#if defined(WIN32)
/* 5 instructions with rotate instruction, else 9 */
#define Endian_Reverse32(a) \
	{ \
	unsigned long __l=(a); \
	(a)=((ROTATE(__l,8)&0x00FF00FF)|(ROTATE(__l,24)&0xFF00FF00)); \
	}
#else
/* 6 instructions with rotate instruction, else 8 */
#define Endian_Reverse32(a) \
	{ \
	unsigned long __l=(a); \
	l=(((__l&0xFF00FF00)>>8L)|((__l&0x00FF00FF)<<8L)); \
	(a)=ROTATE(__l,16L); \
	}
#endif

/* As  pointed out by Wei Dai <weidai@eskimo.com>, F() below can be
 * simplified to the code in F_00_19.  Wei attributes these optimisations
 * to Peter Gutmann's SHS code, and he attributes it to Rich Schroeppel.
 * #define F(x,y,z) (((x) & (y))  |  ((~(x)) & (z)))
 * I've just become aware of another tweak to be made, again from Wei Dai,
 * in F_40_59, (x&a)|(y&a) -> (x|y)&a
 */
#define	F_00_19(b,c,d)	((((c) ^ (d)) & (b)) ^ (d)) 
#define	F_20_39(b,c,d)	((b) ^ (c) ^ (d))
#define F_40_59(b,c,d)	(((b) & (c)) | (((b)|(c)) & (d))) 
#define	F_60_79(b,c,d)	F_20_39(b,c,d)

#ifdef SHA_0
#undef Xupdate
#define Xupdate(a,i) \
	X[(i)&0x0f]=(a)=\
		(X[(i)&0x0f]^X[((i)+2)&0x0f]^X[((i)+8)&0x0f]^X[((i)+13)&0x0f]);
#endif
#ifdef SHA_1
#undef Xupdate
#define Xupdate(a,i) \
	(a)=(X[(i)&0x0f]^X[((i)+2)&0x0f]^X[((i)+8)&0x0f]^X[((i)+13)&0x0f]); \
	X[(i)&0x0f]=(a)=ROTATE((a),1);
#endif

#define BODY_00_15(i,a,b,c,d,e,f) \
	(f)=X[i]+(e)+K_00_19+ROTATE((a),5)+F_00_19((b),(c),(d)); \
	(b)=ROTATE((b),30);

#define BODY_16_19(i,a,b,c,d,e,f) \
	Xupdate(f,i); \
	(f)+=(e)+K_00_19+ROTATE((a),5)+F_00_19((b),(c),(d)); \
	(b)=ROTATE((b),30);

#define BODY_20_39(i,a,b,c,d,e,f) \
	Xupdate(f,i); \
	(f)+=(e)+K_20_39+ROTATE((a),5)+F_20_39((b),(c),(d)); \
	(b)=ROTATE((b),30);

#define BODY_40_59(i,a,b,c,d,e,f) \
	Xupdate(f,i); \
	(f)+=(e)+K_40_59+ROTATE((a),5)+F_40_59((b),(c),(d)); \
	(b)=ROTATE((b),30);

#define BODY_60_79(i,a,b,c,d,e,f) \
	Xupdate(f,i); \
	(f)=X[(i)&0x0f]+(e)+K_60_79+ROTATE((a),5)+F_60_79((b),(c),(d)); \
	(b)=ROTATE((b),30);

#endif

