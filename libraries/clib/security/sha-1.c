
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

#include "config.h"
#include "config-std.h"
#include "config-mem.h"
#include "sha-1.h"

#if !defined(BIG_ENDIAN)
#define	BIG_ENDIAN	1
#endif
#if !defined(LITTLE_ENDIAN)
#define	LITTLE_ENDIAN	2
#endif
#if !defined(BYTE_ORDER)
#if defined(WORDS_BIGENDIAN)
#define	BYTE_ORDER	BIG_ENDIAN
#else
#define	BYTE_ORDER	LITTLE_ENDIAN
#endif
#endif

/* Implemented from SHA-1 document - The Secure Hash Algorithm
 */

#define INIT_DATA_h0 (unsigned long)0x67452301L
#define INIT_DATA_h1 (unsigned long)0xefcdab89L
#define INIT_DATA_h2 (unsigned long)0x98badcfeL
#define INIT_DATA_h3 (unsigned long)0x10325476L
#define INIT_DATA_h4 (unsigned long)0xc3d2e1f0L

#define K_00_19	0x5a827999L
#define K_20_39 0x6ed9eba1L
#define K_40_59 0x8f1bbcdcL
#define K_60_79 0xca62c1d6L

static void sha1_block(SHA1_CTX *c, register unsigned long *p);

void SHA1Init(SHA1_CTX *c)
	{
	c->h0=INIT_DATA_h0;
	c->h1=INIT_DATA_h1;
	c->h2=INIT_DATA_h2;
	c->h3=INIT_DATA_h3;
	c->h4=INIT_DATA_h4;
	c->Nl=0;
	c->Nh=0;
	c->num=0;
	}

void SHA1Update(SHA1_CTX *c, register const unsigned char *data, unsigned long len)
	{
	register ULONG *p;
	int ew,ec,sw,sc;
	ULONG l;

	if (len == 0) return;

	l=(c->Nl+(len<<3))&0xffffffff;
	if (l < c->Nl) /* overflow */
		c->Nh++;
	c->Nh+=(len>>29);
	c->Nl=l;

	if (c->num != 0)
		{
		p=c->data;
		sw=c->num>>2;
		sc=c->num&0x03;

		if ((c->num+len) >= SHA_CBLOCK)
			{
			l= p[sw];
			p_c2nl(data,l,sc);
			p[sw++]=l;
			for (; sw<SHA_LBLOCK; sw++)
				{
				c2nl(data,l);
				p[sw]=l;
				}
			len-=(SHA_CBLOCK-c->num);

			sha1_block(c,p);
			c->num=0;
			/* drop through and do the rest */
			}
		else
			{

			c->num+=(int)len;
			if ((sc+len) < 4) /* ugly, add char's to a word */
				{
				l= p[sw];
				p_c2nl_p(data,l,sc,len);
				p[sw]=l;
				}
			else
				{
				ew=(c->num>>2);
				ec=(c->num&0x03);
				l= p[sw];
				p_c2nl(data,l,sc);
				p[sw++]=l;
				for (; sw < ew; sw++)
					{ c2nl(data,l); p[sw]=l; }
				if (ec)
					{
					c2nl_p(data,l,ec);
					p[sw]=l;
					}
				}
			return;
			}
		}
	/* we now can process the input data in blocks of SHA_CBLOCK
	 * chars and save the leftovers to c->data. */
	p=c->data;
	while (len >= SHA_CBLOCK)
		{
#if defined(BYTE_ORDER) && (defined(BIG_ENDIAN) || defined(LITTLE_ENDIAN))
		memcpy(p,data,SHA_CBLOCK);
		data+=SHA_CBLOCK;
#if BYTE_ORDER == LITTLE_ENDIAN
		for (sw=(SHA_LBLOCK/4); sw; sw--)
			{
			Endian_Reverse32(p[0]);
			Endian_Reverse32(p[1]);
			Endian_Reverse32(p[2]);
			Endian_Reverse32(p[3]);
			p+=4;
			}
#endif
#else
		for (sw=(SHA_BLOCK/4); sw; sw--)
			{
			c2nl(data,l); *(p++)=l;
			c2nl(data,l); *(p++)=l;
			c2nl(data,l); *(p++)=l;
			c2nl(data,l); *(p++)=l;
			}
#endif
		p=c->data;
		sha1_block(c,p);
		len-=SHA_CBLOCK;
		}
	ec=(int)len;
	c->num=ec;
	ew=(ec>>2);
	ec&=0x03;

	for (sw=0; sw < ew; sw++)
		{ c2nl(data,l); p[sw]=l; }
	c2nl_p(data,l,ec);
	p[sw]=l;
	}

static void sha1_block(SHA1_CTX *c, register unsigned long *X)
	{
	register ULONG A,B,C,D,E,T;

	A=c->h0;
	B=c->h1;
	C=c->h2;
	D=c->h3;
	E=c->h4;

	BODY_00_15( 0,A,B,C,D,E,T);
	BODY_00_15( 1,T,A,B,C,D,E);
	BODY_00_15( 2,E,T,A,B,C,D);
	BODY_00_15( 3,D,E,T,A,B,C);
	BODY_00_15( 4,C,D,E,T,A,B);
	BODY_00_15( 5,B,C,D,E,T,A);
	BODY_00_15( 6,A,B,C,D,E,T);
	BODY_00_15( 7,T,A,B,C,D,E);
	BODY_00_15( 8,E,T,A,B,C,D);
	BODY_00_15( 9,D,E,T,A,B,C);
	BODY_00_15(10,C,D,E,T,A,B);
	BODY_00_15(11,B,C,D,E,T,A);
	BODY_00_15(12,A,B,C,D,E,T);
	BODY_00_15(13,T,A,B,C,D,E);
	BODY_00_15(14,E,T,A,B,C,D);
	BODY_00_15(15,D,E,T,A,B,C);
	BODY_16_19(16,C,D,E,T,A,B);
	BODY_16_19(17,B,C,D,E,T,A);
	BODY_16_19(18,A,B,C,D,E,T);
	BODY_16_19(19,T,A,B,C,D,E);

	BODY_20_39(20,E,T,A,B,C,D);
	BODY_20_39(21,D,E,T,A,B,C);
	BODY_20_39(22,C,D,E,T,A,B);
	BODY_20_39(23,B,C,D,E,T,A);
	BODY_20_39(24,A,B,C,D,E,T);
	BODY_20_39(25,T,A,B,C,D,E);
	BODY_20_39(26,E,T,A,B,C,D);
	BODY_20_39(27,D,E,T,A,B,C);
	BODY_20_39(28,C,D,E,T,A,B);
	BODY_20_39(29,B,C,D,E,T,A);
	BODY_20_39(30,A,B,C,D,E,T);
	BODY_20_39(31,T,A,B,C,D,E);
	BODY_20_39(32,E,T,A,B,C,D);
	BODY_20_39(33,D,E,T,A,B,C);
	BODY_20_39(34,C,D,E,T,A,B);
	BODY_20_39(35,B,C,D,E,T,A);
	BODY_20_39(36,A,B,C,D,E,T);
	BODY_20_39(37,T,A,B,C,D,E);
	BODY_20_39(38,E,T,A,B,C,D);
	BODY_20_39(39,D,E,T,A,B,C);

	BODY_40_59(40,C,D,E,T,A,B);
	BODY_40_59(41,B,C,D,E,T,A);
	BODY_40_59(42,A,B,C,D,E,T);
	BODY_40_59(43,T,A,B,C,D,E);
	BODY_40_59(44,E,T,A,B,C,D);
	BODY_40_59(45,D,E,T,A,B,C);
	BODY_40_59(46,C,D,E,T,A,B);
	BODY_40_59(47,B,C,D,E,T,A);
	BODY_40_59(48,A,B,C,D,E,T);
	BODY_40_59(49,T,A,B,C,D,E);
	BODY_40_59(50,E,T,A,B,C,D);
	BODY_40_59(51,D,E,T,A,B,C);
	BODY_40_59(52,C,D,E,T,A,B);
	BODY_40_59(53,B,C,D,E,T,A);
	BODY_40_59(54,A,B,C,D,E,T);
	BODY_40_59(55,T,A,B,C,D,E);
	BODY_40_59(56,E,T,A,B,C,D);
	BODY_40_59(57,D,E,T,A,B,C);
	BODY_40_59(58,C,D,E,T,A,B);
	BODY_40_59(59,B,C,D,E,T,A);

	BODY_60_79(60,A,B,C,D,E,T);
	BODY_60_79(61,T,A,B,C,D,E);
	BODY_60_79(62,E,T,A,B,C,D);
	BODY_60_79(63,D,E,T,A,B,C);
	BODY_60_79(64,C,D,E,T,A,B);
	BODY_60_79(65,B,C,D,E,T,A);
	BODY_60_79(66,A,B,C,D,E,T);
	BODY_60_79(67,T,A,B,C,D,E);
	BODY_60_79(68,E,T,A,B,C,D);
	BODY_60_79(69,D,E,T,A,B,C);
	BODY_60_79(70,C,D,E,T,A,B);
	BODY_60_79(71,B,C,D,E,T,A);
	BODY_60_79(72,A,B,C,D,E,T);
	BODY_60_79(73,T,A,B,C,D,E);
	BODY_60_79(74,E,T,A,B,C,D);
	BODY_60_79(75,D,E,T,A,B,C);
	BODY_60_79(76,C,D,E,T,A,B);
	BODY_60_79(77,B,C,D,E,T,A);
	BODY_60_79(78,A,B,C,D,E,T);
	BODY_60_79(79,T,A,B,C,D,E);

	c->h0=(c->h0+E)&0xffffffff; 
	c->h1=(c->h1+T)&0xffffffff;
	c->h2=(c->h2+A)&0xffffffff;
	c->h3=(c->h3+B)&0xffffffff;
	c->h4=(c->h4+C)&0xffffffff;
	}

void SHA1Final(unsigned char *md, SHA1_CTX *c)
	{
	register int i,j;
	register ULONG l;
	register ULONG *p;
	static unsigned char end[4]={0x80,0x00,0x00,0x00};
	unsigned char *cp=end;

	/* c->num should definitly have room for at least one more byte. */
	p=c->data;
	j=c->num;
	i=j>>2;
#ifdef PURIFY
	if ((j&0x03) == 0) p[i]=0;
#endif
	l=p[i];
	p_c2nl(cp,l,j&0x03);
	p[i]=l;
	i++;
	/* i is the next 'undefined word' */
	if (c->num >= SHA_LAST_BLOCK)
		{
		for (; i<SHA_LBLOCK; i++)
			p[i]=0;
		sha1_block(c,p);
		i=0;
		}
	for (; i<(SHA_LBLOCK-2); i++)
		p[i]=0;
	p[SHA_LBLOCK-2]=c->Nh;
	p[SHA_LBLOCK-1]=c->Nl;
	sha1_block(c,p);
	cp=md;
	l=c->h0; nl2c(l,cp);
	l=c->h1; nl2c(l,cp);
	l=c->h2; nl2c(l,cp);
	l=c->h3; nl2c(l,cp);
	l=c->h4; nl2c(l,cp);

	/* clear stuff, sha1_block may be leaving some stuff on the stack
	 * but I'm not worried :-) */
	c->num=0;
/*	memset((char *)&c,0,sizeof(c));*/
	}

#ifdef MAIN_TEST

char *test[]={
        "abc",
        "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq",
        NULL,
};
char *ret[]={
        "a9993e364706816aba3e25717850c26c9cd0d89d",
        "84983e441c3bd26ebaae4aa1f95129e5e54670f1",
};

#define SHA_DIGEST_LENGTH 20

unsigned char *SHA1(d, n, md)
unsigned char *d;
unsigned long n;
unsigned char *md;
{
        SHA1_CTX c;
        static unsigned char m[SHA_DIGEST_LENGTH];

        if (md == NULL) md=m;
        SHA1Init(&c);
        SHA1Update(&c,d,n);
        SHA1Final(md,&c);
        memset(&c,0,sizeof(c));
        return(md);
}

static char *pt(md)
unsigned char *md;
{
        int i;
        static char buf[80];

        for (i=0; i<SHA_DIGEST_LENGTH; i++)
                sprintf(&(buf[i*2]),"%02x",md[i]);
        return(buf);
}

int main(argc,argv)
int argc;
char *argv[];
{
        int i,err=0;
        unsigned char **P,**R;
        static unsigned char buf[1000];
        char *p,*r;
        SHA1_CTX c;
        unsigned char md[SHA_DIGEST_LENGTH];

        P=(unsigned char **)test;
        R=(unsigned char **)ret;
        i=1;
        while (*P != NULL)
                {
                p=pt(SHA1(*P,(unsigned long)strlen((char *)*P),NULL));
                if (strcmp(p,(char *)*R) != 0)
                        {
                        printf("error calculating SHA1 on '%s'\n",*P);
                        printf("got %s instead of %s\n",p,*R);
                        err++;
                        }
                else
                        printf("test %d ok\n",i);
                i++;
                R++;
                P++;
        }

        return(0);
}

#endif /* MAIN_TEST */
