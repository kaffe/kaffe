/*
 * java.lang.String.c
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * Copyright (c) 2005
 *      Kaffe.org contributors.  See ChangeLog for details. 
 *      All rights reserved.
 8
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#include "config.h"
#include "config-std.h"

#ifdef HAVE_STRING_H
#include <string.h>
#endif

#include "stringSupport.h"
#include "java_lang_String.h"

Hjava_lang_String*
java_lang_String_intern0(Hjava_lang_String* str)
{
	Hjava_lang_String *ret = stringInternString(str);

	if (!ret) {
		errorInfo info;
		postOutOfMemory(&info);
		throwError(&info);
	}
	return ret;
}


jint
java_lang_String_indexOf(Hjava_lang_String* str, Hjava_lang_String* pat, jint offset)
{
  jchar *a;
  jchar *p;

  int   n;
  int   m;
  int   m2;
  jint  i, k;
  unsigned char  bs[256];
  int *ibs;

  if (pat == 0) {
    SignalError("java.lang.NullPointerException", "");
  }

  if ( !str ) return -1;

  a = &(unhand_array(unhand(str)->value)->body[unhand(str)->offset]);
  n = unhand(str)->count;

  p = &(unhand_array(unhand(pat)->value)->body[unhand(pat)->offset]);
  m = unhand(pat)->count;
  m2 = m * 2;

  if ( m > n ) return -1;

  if ( offset < 0 ) offset = 0;

  if ( (m < 3) || (n < 128) || (m > 256) ) {
	/*
	 * If the pattern is too small, iterating the searched array would be
	 * cheaper than to set up the badmatch table, or the pattern length exceeds
	 * what we can store in the badmatch table, we revert to plain old brute force
	 */
	k = n - m+1;
	for ( i=offset; i<k; i++ ) {
	  if ( memcmp( &a[i], p, (size_t)m2) == 0 )
		return i;		
	}
  }
  else {
	/*
	 * Otherwise we use the Quick search variant of Boyer-Moore for 
	 * O(n/m) lookup. Note that the (char) cast in the badmatch table access
	 * is the justification for turning this into a native method
	 */

	/* set up the badmatch table */
	k = (m << 24) | (m<<16) | (m<<8) | m;
	for ( i=0, ibs = (int*)bs; i<64; i++ )
	  ibs[i] = k; /* the default jump: m */
	for ( i=0; i<m; i++ )
	  bs[ (unsigned char)p[i] ] = m-i;

	k= n - m + 1;
	for ( i=offset; i < k; ) {
	  if ( memcmp( &a[i], p, (size_t)m2) == 0 )
		return i;
	  i += bs[ (unsigned char)a[i+m] ];
	}
  }

  return -1;
}
