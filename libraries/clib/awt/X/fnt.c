/**
 * fnt.c - 
 *
 * Copyright (c) 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */


#include "toolkit.h"

/*******************************************************************************
 * Font support
 */

/*
 * this is our ultima ratio in case everything else fails, at least this
 * one should be on every X fontlist
 */
const char* backupFont = "fixed";

/*
 * these are (hopefully) the most usual XFLD font weight names, ordered
 * in our preferred lookup sequence (from PLAIN to BOLD)
 */
const char* weight[] = { "medium", "normal", "regular", "thin", "light",
                   "black", "demibold", "heavy", "extrabold", "extrablack", "bold" };
#define NWEIGHT 11

/*
 * these are (more confident) the XFLD slant names, ordered in our
 * preferred lookup sequence (from PLAIN to ITALIC)
 */
const char* slant[] = { "r", "i", "o" };
#define NSLANT 3

/*
 * these are finally the size offsets we want to try
 */
int dsize[] = { 0, 10, -10, 20, -20, 30, -30, 40, -40 };
#define NDSIZE 9

/*
 * Our font lookup strategy is like this: every mapping that can be known
 * a priori goes into java.awt.Defaults, everything that might be specified
 * at runtime (sizes, slant, weight) has to be handled here. In detail:
 * 
 * (1) foundry, family, encoding and everything EXCEPT of weight,slant,size
 *     are specified in java.awt.Defaults FontSpec strings, which are used
 *     as simple C format specifications
 *     (e.g."-adobe-helvetica-%s-%s-*-*-*-%d-*-*-*-*-*-*")
 * (2) since both BOLD and ITALIC map to more than one XFLD attribute value
 *     (e.g. ITALIC-> "o", "i"), and many fonts are not available in all
 *     sizes, we have to make up a sequence of alternate specs to search
 * (3) the search first tries to vary sizes, then weights, then slants
 * (4) in case all of this fails, it directly tries the font spec
 * (5) if this fails, too, it backs up to what is supposed to be a safe X
 *     standard font (fix size)
 *
 * Once again - we don't try to deduce XFLD family names from the Java names,
 * that's in java.awt.Defaults, and Defaults is meant to be the thing which
 * adapts the AWT to your X installation/preferences. Don't lament, modify it!
 */

#ifdef KAFFE_I18N
static XOC create_xoc(Toolkit *tk, const char *font_name)
{
  XOC xoc;
  int i;
  char **missing_list;
  int missing_count;
  char *def_string;

  xoc = XCreateOC(tk->xom,
		XNBaseFontName, font_name,
		NULL);
  if (xoc) {
    XGetOCValues(xoc,
		XNMissingCharSet, &missing_count,
		XNRequiredCharSet, &missing_list,
		XNDefaultString, &def_string,
		NULL);
    if( missing_count!=0 ){
      DBG( AWT, fprintf(stderr, "missing list exists at %s.\n" ,font_name) );
      for (i = 0; i < missing_count; i++) {
        DBG( AWT, fprintf(stderr, "missing list[%d]: %s\n", i, missing_list[i]) );
      }
    }
  } else {
    DBG( AWT, fprintf(stderr, "XCreateOC error at %s.\n" ,font_name) );
  }
  return xoc;		    
}
#endif

jobject
Java_java_awt_Toolkit_fntInitFont ( JNIEnv* env, jclass clazz UNUSED, jstring jSpec,
                                    jint style, jint size )
{
  int  i, j, k, i0, i1, j0, j1, di, dj;
  char buf[160];
  char *spec = java2CString( env, X, jSpec);
#ifdef KAFFE_I18N
  XOC xoc = NULL;
#else  
  XFontStruct* fs = 0;
#endif  

	size *= 10;  /* convert into X pointsizes */

  if ( style & 0x1 ) { /* we have a Font.BOLD request */
    i0 = NWEIGHT - 1; i1 = -1; di = -1;
  }
  else {
    i0 = 0; i1 = NWEIGHT; di = 1;
  }

  if ( style & 0x2 ) { /* we have a Font.ITALIC request */
    j0 = NSLANT - 1; j1 = -1; dj = -1;
  }
  else {
    j0 = 0; j1 = NSLANT; dj = 1;
  }

#ifdef KAFFE_I18N
  for ( j=j0; (xoc == NULL) && (j != j1); j += dj ) {
    for ( i=i0; (xoc == NULL) && (i != i1); i += di ) {
      for ( k=0;  (xoc == NULL) && (k < NDSIZE); k++ ) {
#else	      
  for ( j=j0; !fs && (j != j1); j += dj ) {
    for ( i=i0; !fs && (i != i1); i += di ) {
      for ( k=0;  !fs && (k < NDSIZE); k++ ) {
#endif	      
        sprintf( buf, spec, weight[i], slant[j], size + dsize[k]);
        DBG( AWT_FNT, printf("look up font: %s\n", buf));
#ifdef KAFFE_I18N
        xoc = create_xoc( X, buf);
#else	
        fs = XLoadQueryFont( X->dsp, buf);
#endif	
      }
    }
  }

#ifdef KAFFE_I18N
  if ( ! xoc ){
    xoc = create_xoc( X, spec);
  }
  if ( ! xoc ){
        if ( !(xoc = create_xoc( X, backupFont)) ) {
	  fprintf( stderr, "font panic, no default font!\n");
	}
  }

  if (xoc != NULL)
    return JCL_NewRawDataObject (env, xoc);
  return NULL;
#else  
  if ( ! fs ){
    /* now we are getting desperate, try the spec directly (without vars) */
    fs = XLoadQueryFont( X->dsp, spec);
  }

  if ( ! fs ){
	DBG( AWT, printf("cannot load font: %s (backup to %s)\n", buf, backupFont));
	if ( !(fs = XLoadQueryFont( X->dsp, backupFont)) ) {
	  fprintf( stderr, "font panic, no default font!\n");
	}
  }

  if (fs != NULL)
    return JCL_NewRawDataObject (env, fs);
  return NULL;
#endif  
}

#ifdef KAFFE_I18N
void
Java_java_awt_Toolkit_fntFreeFont ( JNIEnv* env, jclass clazz UNUSED, jobject nativeXoc )
{
  XDestroyOC(UNVEIL_XOC(nativeXoc));
}
#else
void
Java_java_awt_Toolkit_fntFreeFont ( JNIEnv* env, jclass clazz, jobject fs )
{
  XFreeFont( X->dsp, UNVEIL_FS(fs));
}
#endif

/*******************************************************************************
 * FontMetrics support
 */

#ifdef KAFFE_I18N
KAFFE_FONT_FUNC_DECL( jobject, Java_java_awt_Toolkit_fntInitFontMetrics )
{
  return xoc;
}

KAFFE_FONT_FUNC_DECL( void, Java_java_awt_Toolkit_fntFreeFontMetrics )
{
}

KAFFE_FONT_FUNC_DECL( jint, Java_java_awt_Toolkit_fntGetAscent )
{
  return -XExtentsOfFontSet(UNVEIL_XOC(xoc))->max_logical_extent.y;	
}

KAFFE_FONT_FUNC_DECL( jint, Java_java_awt_Toolkit_fntGetDescent )
{
  XFontSetExtents *xfse=XExtentsOfFontSet(UNVEIL_XOC(xoc));
  return xfse->max_logical_extent.height-(-xfse->max_logical_extent.y);
}

KAFFE_FONT_FUNC_DECL( jint, Java_java_awt_Toolkit_fntGetFixedWidth )
{
  XFontSetExtents *xfse=XExtentsOfFontSet(UNVEIL_XOC(xoc));
  return ( xfse->max_logical_extent.width == xfse->max_ink_extent.width ) ?
	  xfse->max_logical_extent.width : 0;
}

KAFFE_FONT_FUNC_DECL( jint, Java_java_awt_Toolkit_fntGetHeight )
{
  return XExtentsOfFontSet(UNVEIL_XOC(xoc))->max_logical_extent.height;
}

KAFFE_FONT_FUNC_DECL( jint, Java_java_awt_Toolkit_fntGetLeading )
{
  return 0;  /* no leading (interline spacing) for X fonts */
}

KAFFE_FONT_FUNC_DECL( jint, Java_java_awt_Toolkit_fntGetMaxAdvance )
{
  return XExtentsOfFontSet(UNVEIL_XOC(xoc))->max_logical_extent.width;
}

KAFFE_FONT_FUNC_DECL( jint, Java_java_awt_Toolkit_fntGetMaxAscent )
{
  XFontSetExtents *xfse=XExtentsOfFontSet(UNVEIL_XOC(xoc));
  return -xfse->max_logical_extent.y;
}

KAFFE_FONT_FUNC_DECL( jint, Java_java_awt_Toolkit_fntGetMaxDescent )
{
  XFontSetExtents *xfse=XExtentsOfFontSet(UNVEIL_XOC(xoc));
  return xfse->max_logical_extent.height-(-xfse->max_logical_extent.y);
}

KAFFE_FONT_FUNC_DECL( jboolean, Java_java_awt_Toolkit_fntIsWideFont )
{
  return 2; /* We assume that wide font is double-wide. */
}

#else

KAFFE_FONT_FUNC_DECL( jobject, Java_java_awt_Toolkit_fntInitFontMetrics )
{
  return fs;
}

KAFFE_FONT_FUNC_DECL( void, Java_java_awt_Toolkit_fntFreeFontMetrics 
{
}

KAFFE_FONT_FUNC_DECL( jint, Java_java_awt_Toolkit_fntGetAscent )
{
  return UNVEIL_FS(fs)->ascent;
}

KAFFE_FONT_FUNC_DECL( jint, Java_java_awt_Toolkit_fntGetDescent )
{
  return UNVEIL_FS(fs)->descent;
}

KAFFE_FONT_FUNC_DECL( jint, Java_java_awt_Toolkit_fntGetFixedWidth )
{
  XFontStruct *xfs = UNVEIL_FS(fs);

  return (xfs->min_bounds.width == xfs->max_bounds.width) ? xfs->max_bounds.width : 0;
}

KAFFE_FONT_FUNC_DECL( jint, Java_java_awt_Toolkit_fntGetHeight )
{
  XFontStruct *xfs = UNVEIL_FS(fs);

  return xfs->ascent + xfs->descent +1;
}

KAFFE_FONT_FUNC_DECL( jint, Java_java_awt_Toolkit_fntGetLeading )
{
  return 0;  /* no leading (interline spacing) for X fonts */
}

KAFFE_FONT_FUNC_DECL( jint, Java_java_awt_Toolkit_fntGetMaxAdvance )
{
  return UNVEIl_FS(fs)->max_bounds.width;
}

KAFFE_FONT_FUNC_DECL( jint, Java_java_awt_Toolkit_fntGetMaxAscent )
{
  return UNVEIL_FS(fs)->max_bounds.ascent;
}

KAFFE_FONT_FUNC_DECL( jint, Java_java_awt_Toolkit_fntGetMaxDescent )
{
  return UNVEIL_FS(fs)->max_bounds.descent;
}

KAFFE_FONT_FUNC_DECL( jboolean, Java_java_awt_Toolkit_fntIsWideFont )
{
  XFontStruct *xfs = UNVEIL_FS(fs);

  return (xfs->min_byte1 | xfs->max_byte1);
}

#endif

KAFFE_FONT_FUNC_DECL( jobject, Java_java_awt_Toolkit_fntGetWidths )
{
  jintArray widths;
  jint      *jw;
  jboolean isCopy;
#ifdef KAFFE_I18N
  register  int i;
  XRectangle ink_array, logical_array;
  XRectangle overall_ink, overall_logical;
  wchar_t wch;
  int num;
#else  
  XFontStruct *xfs = UNVEIL_FS(fs);
  register  int i, j;
  const int n = 256;
#endif  

  widths = (*env)->NewIntArray( env, 256);
  jw = (*env)->GetIntArrayElements( env, widths, &isCopy);

#ifdef KAFFE_I18N
  for( i = 0; i < 256; i++ ) {
    wch = i;
    if (!XwcTextPerCharExtents(
	 UNVEIL_XOC(xoc) ,&wch ,1 ,&ink_array, &logical_array, 1,
         &num,&overall_ink, &overall_logical )){
      DBG( AWT, fprintf(stderr, __FILE__ "(%d)"
        "):" "XwcTextPerCharExtents Error\n",__LINE__ ));
    }
    jw[i] = overall_logical.width;
  }
#else  
  if ( xfs->max_char_or_byte2 < n ) n = xfs->max_char_or_byte2;

  if ( xfs->min_bounds.width == xfs->max_bounds.width ) {
	for ( i=xfs->min_char_or_byte2, j=0; i < n; i++, j++ )
	  jw[i] = xfs->max_bounds.width;
  }
  else {
	for ( i=xfs->min_char_or_byte2, j=0; i < n; i++, j++ )
	  jw[i] = xfs->per_char[j].width;
  }
#endif
  (*env)->ReleaseIntArrayElements( env, widths, jw, 0);

  return widths;
}


KAFFE_FONT_FUNC_DECL( jint, Java_java_awt_Toolkit_fntBytesWidth, jbyteArray jBytes, jint off, jint len )
{
  jboolean  isCopy;
  jbyte    *jb = (*env)->GetByteArrayElements( env, jBytes, &isCopy);
  int       n = (*env)->GetArrayLength( env, jBytes);
#ifdef KAFFE_I18N
  wchar_t   *wch;
  XRectangle *ink_array, *logical_array;
  XRectangle overall_ink, overall_logical;
  int num;
#else
  int       w;
#endif

  if ( off+len > n ) len = n - off;

#ifdef KAFFE_I18N
  wch = jbyte2wchar( jb + off, len );
  ink_array = (XRectangle *)malloc(sizeof(XRectangle)*len);
  logical_array = (XRectangle *)malloc(sizeof(XRectangle)*len);
  if(!XwcTextPerCharExtents(UNVEIL_XOC(xoc) ,wch ,len ,ink_array,logical_array, len,
      &num,&overall_ink ,&overall_logical)){
    DBG( AWT, fprintf(stderr, __FILE__ "(%d)"
         "):" "XwcTextPerCharExtents Error\n",__LINE__ ));
  }
  free( (void *)logical_array );
  free( (void *)ink_array );
  free( (void *)wch );
#else  
  w = XTextWidth( UNVEIL_FS(fs), jb+off, len);
#endif
  
  (*env)->ReleaseByteArrayElements( env, jBytes, jb, JNI_ABORT);
  
#ifdef KAFFE_I18N
  return overall_logical.width;
#else  
  return w;
#endif  
}

KAFFE_FONT_FUNC_DECL( jint, Java_java_awt_Toolkit_fntCharWidth, jchar jChar )
{
#ifdef KAFFE_I18N
  wchar_t wch;
  XRectangle ink_array, logical_array;
  XRectangle overall_ink, overall_logical;
  int num;

  wch = jChar;
  if(!XwcTextPerCharExtents(UNVEIL_XOC(xoc) ,&wch ,1 ,&ink_array, &logical_array,
	1, &num, &overall_ink, &overall_logical)){
    DBG( AWT, fprintf(stderr, __FILE__ "(%d)"
         "):" "XwcTextPerCharExtents Error\n",__LINE__ ));			
  }
  return overall_logical.width;     
	
#else /* !KAFFE_I18N */
	
#ifndef WORDS_BIGENDIAN
  jChar = (jChar << 8) | (jChar >> 8);
#endif

  return XTextWidth16( UNVEIL_FS(fs), (XChar2b*)&jChar, 1);
#endif  
}

KAFFE_FONT_FUNC_DECL( jint, Java_java_awt_Toolkit_fntCharsWidth, jcharArray jChars, jint off, jint len )
{
  jboolean  isCopy;
  jchar    *jc = (*env)->GetCharArrayElements( env, jChars, &isCopy);
  int      n = (*env)->GetArrayLength( env, jChars);
#ifdef KAFFE_I18N
  wchar_t  *wch;
  XRectangle *ink_array, *logical_array;
  XRectangle overall_ink, overall_logical;
  int num;
#else  
  XChar2b  *b;
  int      w;
#endif

  if ( off+len > n ) len = n - off;

#ifdef KAFFE_I18N
  wch = jchar2wchar( jc + off, len );
  ink_array = (XRectangle *)malloc(sizeof(XRectangle)*len);
  logical_array = (XRectangle *)malloc(sizeof(XRectangle)*len);
  if(!XwcTextPerCharExtents(UNVEIL_XOC(xoc) ,wch ,len ,ink_array,logical_array, len,
	&num,&overall_ink ,&overall_logical)){
    DBG( AWT, fprintf(stderr, __FILE__ "(%d)"
         "):" "XwcTextPerCharExtents Error\n",__LINE__ ));			    }
#else /* !KAFFE_I18N */
  
#ifndef WORDS_BIGENDIAN
  n = sizeof(XChar2b)*len;
  b = (XChar2b*) getBuffer( X, n);
  swab( (jc+off), b, n);
#else
  b = (XChar2b*) (jc + off);
#endif

#endif
 
#ifdef KAFFE_I18N
  free( (void *)logical_array );
  free( (void *)ink_array );
  free( (void *)wch );
#else  
  w = XTextWidth16( UNVEIL_FS(fs), b, len);
#endif
  
  (*env)->ReleaseCharArrayElements( env, jChars, jc, JNI_ABORT);
#ifdef KAFFE_I18N
  return overall_logical.width;
#else  
  return w;
#endif  
}

KAFFE_FONT_FUNC_DECL( jint, Java_java_awt_Toolkit_fntStringWidth, jstring jStr )
{
  jboolean isCopy;
  const jchar    *jc = (*env)->GetStringChars( env, jStr, &isCopy);
  int      len = (*env)->GetStringLength( env, jStr);
#ifdef KAFFE_I18N
  wchar_t  *wch;
  XRectangle *ink_array, *logical_array;
  XRectangle overall_ink, overall_logical;
  int num=len;

  wch = jchar2wchar( jc, len );
  ink_array = (XRectangle *)malloc(sizeof(XRectangle)*len);
  logical_array = (XRectangle *)malloc(sizeof(XRectangle)*len);
  if(!XwcTextPerCharExtents(UNVEIL_XOC(xoc) ,wch ,len ,ink_array,logical_array, len,
	&num,&overall_ink ,&overall_logical)){
    DBG( AWT, fprintf(stderr, __FILE__ "(%d)"
         "):" "XwcTextPerCharExtents Error\n",__LINE__ ));
  }
  free( (void *)logical_array );
  free( (void *)ink_array );
  free( (void *)wch );
#else /* !KAFFE_I18N */  
  int      w;
#ifndef WORDS_BIGENDIAN
  int      n;
#endif
  XChar2b  *b;

#ifndef WORDS_BIGENDIAN
  n = sizeof(XChar2b)*len;
  b = (XChar2b*) getBuffer( X, n);
  swab( jc, b, n);
#else
  b = (XChar2b*) jc;
#endif

  w = XTextWidth16( UNVEIL_FS(fs), b, len);
#endif /* !KAFFE_I18N */  

  (*env)->ReleaseStringChars( env, jStr, jc);
#ifdef KAFFE_I18N
  return overall_logical.width;  
#else  
  return w;
#endif  
}
