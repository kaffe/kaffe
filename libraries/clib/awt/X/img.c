/**
 * img.c - 
 *
 * Copyright (c) 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */


#include "config.h"
#include "toolkit.h"


/* interfaces of image conversion functions */
Image* readGifFile ( FILE* );
Image* readGifData ( unsigned char*, long len );
Image* readJpegFile ( FILE* );
Image* readJpegData ( unsigned char*, long len );
Image* readPngFile ( FILE* );
Image* readPngData ( unsigned char*, long len );

/************************************************************************************
 * own auxiliary funcs
 */

Image*
createImage ( int width, int height )
{
  Image * img = calloc( 1, sizeof( Image));
  img->trans  = -1;
  img->width = width;
  img->height = height;

  return img;
}


XImage*
createXMaskImage ( Toolkit* X, int width, int height )
{
  XImage  *xim;
  int     bytes_per_line, nBytes;
  char    *data;
  Visual  *vis = DefaultVisual( X->dsp, DefaultScreen( X->dsp));

  bytes_per_line = (width + 7) / 8;
  nBytes = bytes_per_line * height;

  data = malloc( nBytes);
  memset( data, 0xff, nBytes);

  xim = XCreateImage( X->dsp, vis, 1, XYBitmap, 0,
					  data, width, height, 8, bytes_per_line );
  return xim;
}


XImage*
createXImage ( Toolkit* X, int width, int height )
{
  XImage *xim;
  int bitmap_pad;
  int bytes_per_line;
  int bytes_per_pix;
  char *data;
  Visual *vis  = DefaultVisual( X->dsp, DefaultScreen( X->dsp));
  int    depth = DefaultDepth(  X->dsp, DefaultScreen( X->dsp));
	

  if ( depth <= 8)	      bytes_per_pix = 1;
  else if ( depth <= 16)  bytes_per_pix = 2;
  else			          bytes_per_pix = 4;

  bytes_per_line = bytes_per_pix * width;
  bitmap_pad = bytes_per_pix * 8;

  data = calloc( width * height, bytes_per_pix);

  xim = XCreateImage( X->dsp, vis, depth, ZPixmap, 0,
					  data, width, height, bitmap_pad, bytes_per_line);

  return xim;
}

AlphaImage*
createAlphaImage ( Toolkit* X, int width, int height )
{
  int nBytes = width * height;
  AlphaImage    *img = malloc( sizeof( AlphaImage));

  img->width  = width;
  img->height = height;

  img->buf = malloc( nBytes);
  memset( img->buf, 0xff, nBytes);
  
  return img;
}

/*
 * For images with a full alpha channel, check if we really need an alpha byte for
 * each pel, or if a mask bitmap (alpha 0x00 / 0xff) will be sufficient
 */
int
needsFullAlpha ( Toolkit* X, Image *img, double threshold )
{
  int i, j, a;
  int n = 0, max;

  if ( !img->alpha ) return 0;

  max = (img->width * img->height) * threshold;

  for ( i=0; i<img->height; i++ ) {
	for ( j=0; j<img->width; j++ ) {
	  a = GetAlpha( img->alpha, j, i);
	  if ( (a != 0) && (a != 0xff) ) {
		if ( ++n > max )
		  return 1;
	  }
	}
  }

  return 0;
}

void
countAlphas ( Image *img, int* noAlpha, int* partAlpha, int* fullAlpha )
{
  int i, j, a;

  if ( !img->alpha ) return;

  for ( i=0; i<img->height; i++ ) {
	for ( j=0; j<img->width; j++ ) {
	  a = GetAlpha( img->alpha, j, i);
	  if ( a == 0 )
		(*noAlpha)++;
	  else if ( a == 0xff )
		(*fullAlpha)++;
	  else
		(*partAlpha)++;
	}
  }
}

/*
 * A full alpha image channel is way slower than using a mask bitmap (= 0 / 0xff alpha).
 * This function provides a simple alpha-to-mask translation
 */
void
reduceAlpha ( Toolkit* X, Image* img, int threshold )
{
  int i, j, a;

  if ( !img->alpha )
	return;

  img->xMask = createXMaskImage( X, img->width, img->height);

  for ( i=0; i<img->height; i++ ) {
	for ( j=0; j<img->width; j++ ) {
	  a = GetAlpha( img->alpha, j, i);
	  if ( a < threshold ) {
		XPutPixel( img->xImg, j, i, 0);
		XPutPixel( img->xMask, j, i, 0);
	  }
	}
  }

  free( img->alpha->buf);
  free( img->alpha);
  img->alpha = 0;
}


__inline__ int
interpolate ( int ul, int ur, int ll, int lr, double dx, double dy )
{
  double u = ul + (double)(ur - ul) * dx;
  double l = ll + (double)(lr - ll) * dx;

  return (int) (u + (l - u) * dy  + 0.5);
}

unsigned int
getScaledAlpha ( Toolkit* X, Image* img, int x, int y, double dx, double dy )
{
  int   ul, ur, ll, lr, a;
  int   xi = (dx) ? x+1 : x;
  int   yi = (dy) ? y+1 : y;

  if ( img->alpha ) {
	ul = GetAlpha( img->alpha, x, y);
	ur = GetAlpha( img->alpha, xi, y);
	ll = GetAlpha( img->alpha, x, yi);
	lr = GetAlpha( img->alpha, xi,yi);
	a = (unsigned int) interpolate( ul, ur, ll, lr, dx, dy);
	return a;
  }

  return 0xff;
}

long
getScaledPixel ( Toolkit* X, Image* img, int x, int y, double dx, double dy )
{
  unsigned long  ul, ur, ll, lr;
  int            ulR, urR, llR, lrR, ulG, urG, llG, lrG, ulB, urB, llB, lrB, r, g, b;
  int            xi = (dx) ? x+1 : x;
  int            yi = (dy) ? y+1 : y;

  if ( img->xMask ) {
	ul = XGetPixel( img->xMask, x, y);
	ur = XGetPixel( img->xMask, xi, y);
	ll = XGetPixel( img->xMask, x, yi);
	lr = XGetPixel( img->xMask, xi,yi);
	
	if ( !interpolate( ul, ur, ll, lr, dx, dy) )
	  return -1;
  }

  ul = XGetPixel( img->xImg, x, y);
  ur = XGetPixel( img->xImg, xi, y);
  ll = XGetPixel( img->xImg, x, yi);
  lr = XGetPixel( img->xImg, xi,yi);


  if ( (ul == ur) && (ll == ul) && (lr == ll) ) {
	rgbValues( X, ul, &r, &g, &b);
  }
  else {
	rgbValues( X, ul, &ulR, &ulG, &ulB);
	rgbValues( X, ur, &urR, &urG, &urB);
	rgbValues( X, ll, &llR, &llG, &llB);
	rgbValues( X, lr, &lrR, &lrG, &lrB);

	r = interpolate( ulR, urR, llR, lrR, dx, dy);
	g = interpolate( ulG, urG, llG, lrG, dx, dy);
	b = interpolate( ulB, urB, llB, lrB, dx, dy);
  }

  return pixelValue( X, (r << 16) | (g << 8) | b);
}



void
initScaledImage ( Toolkit* X, Image *tgt, Image *src,
				  int dx0, int dy0, int dx1, int dy1,
				  int sx0, int sy0, int sx1, int sy1 )
{
  double         xScale, yScale, sX, sY, sxDelta, syDelta;
  int            dx, dy, dxInc, dyInc, sx, sy;
  long           c;

  dxInc = (dx1 > dx0) ? 1 : -1;
  dyInc = (dy1 > dy0) ? 1 : -1;

  dx1 += dxInc;
  dy1 += dyInc;

  xScale = (double) (dx1 - dx0) / (double) (sx1 - sx0 +1);
  yScale = (double) (dy1 - dy0) / (double) (sy1 - sy0 +1);

  for ( dy=dy0; dy != dy1; dy += dyInc ) {
	sY = sy0 + (dy - dy0) / yScale;
	sy = (int) sY;
	syDelta = (sy < sy1) ? sY - sy : 0;

	for ( dx=dx0; dx != dx1; dx += dxInc ) {
	  sX = sx0 + (dx - dx0) / xScale;
	  sx = (int) sX;
	  sxDelta = (sx < sx1) ? sX - sx : 0;

	  if ( (c = getScaledPixel( X, src, sx, sy, sxDelta, syDelta)) != -1 ){
  		XPutPixel( tgt->xImg, dx, dy, c);
		if ( src->alpha )
		  PutAlpha( tgt->alpha, dx, dy, getScaledAlpha( X, src, sx, sy, sxDelta, syDelta));
	  }
	  else {
		XPutPixel( tgt->xMask, dx, dy, 0);
		XPutPixel( tgt->xImg, dx, dy, 0);
	  }
	}
  }
}


/************************************************************************************
 * exported native methods
 */


void*
Java_java_awt_Toolkit_imgCreateImage ( JNIEnv* env, jclass clazz, jint width, jint height )
{
  Image *img = createImage( width, height);
  img->xImg = createXImage( X, img->width, img->height);
  return img;
}


void*
Java_java_awt_Toolkit_imgCreateScreenImage ( JNIEnv* env, jclass clazz, jint width, jint height )
{
  Image  *img = createImage( width, height);
  int    depth = DefaultDepth(  X->dsp, DefaultScreen( X->dsp));

  img->pix  = XCreatePixmap( X->dsp, X->root, width, height, depth);

  return img;
}


/* generic (ImageProducer) based image construction */

void
Java_java_awt_Toolkit_imgSetIdxPels ( JNIEnv* env, jclass clazz, Image * img,
									  jint x, jint y, jint w, jint h,
									  jarray clrMap, jarray idxPels, jint trans,
									  jint off, jint scan)
{
  register int    row, col;
  unsigned long   pix;
  jint            rgb;
  jboolean        isCopy;
  jint            *clr = (*env)->GetIntArrayElements( env, clrMap, &isCopy);
  jbyte           *pel = (*env)->GetByteArrayElements( env, idxPels, &isCopy);
  jbyte           *idx = pel + off;
  int             maxCol = x + w;
  int             maxRow = y + h;

  if ( (trans >= 0) && !img->xMask )
	img->xMask = createXMaskImage( X, img->width, img->height);

  for ( row = y; row < maxRow; row++) {
    for ( col = x; col < maxCol; col++, idx++) {
      rgb = clr[(int)*idx];
      pix = pixelValue( X, rgb);
      if ( trans >= 0 ) {
		if ( *idx == trans ){
		  pix = 0;
		  XPutPixel( img->xMask, col, row, 0);
		}
      }
      XPutPixel( img->xImg, col, row, pix);
    }
    x = 0;
  }

  (*env)->ReleaseIntArrayElements( env, clrMap, clr, JNI_ABORT);
  (*env)->ReleaseByteArrayElements( env, idxPels, pel, JNI_ABORT);
}


void
Java_java_awt_Toolkit_imgSetRGBPels ( JNIEnv* env, jclass clazz, Image * img,
									  jint x, jint y, jint w, jint h,
									  jarray rgbPels, jint off, jint scan)
{
  register int    row, col;
  unsigned long   pix;
  jboolean        isCopy;
  jint            *rgbs = (*env)->GetIntArrayElements( env, rgbPels, &isCopy);
  jint            *rgb = rgbs + off;
  int             maxCol = x + w;
  int             maxRow = y + h;

  for ( row = y; row < maxRow; row++) {
    for ( col = x; col < maxCol; col++, rgb++) {
	  if ( *rgb & 0xff000000 ) {
		pix = pixelValue( X, *rgb);
		XPutPixel( img->xImg, col, row, pix);
	  }
	  else {
		if ( !img->xMask )
		  img->xMask = createXMaskImage( X, img->width, img->height);
		XPutPixel( img->xMask, col, row, 0);
		XPutPixel( img->xImg, col, row, 0);
	  }
    }
    x = 0;
  }

  (*env)->ReleaseIntArrayElements( env, rgbPels, rgbs, JNI_ABORT);
}


void
Java_java_awt_Toolkit_imgFreeImage( JNIEnv* env, jclass clazz, Image * img)
{
  if ( img->pix ){
	XFreePixmap( X->dsp, img->pix);
	img->pix = 0;
  }

  /*
   * note that XDestroyImage automatically frees any non-NULL data pointer
   * (since we explicitly allocated the data, we better free t explicitly, too)
   * malloc, free might be resolved
   */
  if ( img->xImg ){
	free( img->xImg->data);
	img->xImg->data = 0;

    XDestroyImage( img->xImg);
	img->xImg = 0;
  }

  if ( img->xMask ){
	free( img->xMask->data);
	img->xMask->data = 0;

    XDestroyImage( img->xMask);
	img->xMask = 0;
  }

  if ( img->alpha ) {
	free( img->alpha->buf);
	free( img->alpha);
	img->alpha = 0;
  }

  free( img);
}


void*
Java_java_awt_Toolkit_imgCreateScaledImage ( JNIEnv* env, jclass clazz,
											 Image* img, int width, int height )
{
  int depth;

  Image *scaledImg = createImage( width, height);

  if ( img->xImg ) {
	scaledImg->xImg = createXImage( X, width, height);
	if ( img->xMask )
	  scaledImg->xMask = createXMaskImage( X, width, height);

	initScaledImage ( X, scaledImg, img,
					  0, 0, width-1, height-1,
					  0, 0, img->width-1, img->height-1);
  }
  else if ( img->pix ) {
	depth = DefaultDepth(  X->dsp, DefaultScreen( X->dsp));
	scaledImg->pix  = XCreatePixmap( X->dsp, X->root, width, height, depth);
  }

  return scaledImg;
}


void
Java_java_awt_Toolkit_imgProduceImage ( JNIEnv* env, jclass clazz,
										jobject producer, Image* img )
{
  int       i, j;
  int       r, g, b;
  unsigned long pix;
  jboolean  isCopy;
  jclass    prodClazz  = (*env)->GetObjectClass( env, producer);
  jclass    modelClazz = (*env)->FindClass( env, "java/awt/image/ColorModel");

  jmethodID modelCtor = (*env)->GetStaticMethodID( env, modelClazz, "getRGBdefault",
												   "()Ljava/awt/image/ColorModel;");
  jmethodID setDim    = (*env)->GetMethodID( env, prodClazz, "setDimensions", "(II)V");
  jmethodID setCM     = (*env)->GetMethodID( env, prodClazz, "setColorModel",
											 "(Ljava/awt/image/ColorModel;)V");
  jmethodID setHints  = (*env)->GetMethodID( env, prodClazz, "setHints", "(I)V");
  jmethodID setPix    = (*env)->GetMethodID( env, prodClazz, "setPixels",
											 "(IIIILjava/awt/image/ColorModel;[III)V");
  jmethodID imgCompl  = (*env)->GetMethodID( env, prodClazz, "imageComplete", "(I)V");

  jobject   model     = (*env)->CallStaticObjectMethod( env, modelClazz, modelCtor);

  jarray    scanLine  = (*env)->NewIntArray( env, img->width);
  jint*     pels = (*env)->GetIntArrayElements( env, scanLine, &isCopy);

  (*env)->CallVoidMethod( env, producer, setDim, img->width, img->height);
  (*env)->CallVoidMethod( env, producer, setCM, model);
  (*env)->CallVoidMethod( env, producer, setHints, 6); /* TOPDOWNLEFTRIGHT | COMPLETESCANLINES */

  for ( j=0; j<img->height; j++ ){
	for ( i=0; i<img->width; i++ ) {
	  if ( (img->xMask == 0) || XGetPixel( img->xMask, i, j) ) {
		pix = XGetPixel( img->xImg, i, j);
		rgbValues( X, pix, &r, &g, &b);
		pels[i] = (0xff000000 | (r << 16) | (g << 8) | b);
	  }
	  else {
		pels[i] = 0;
	  }
	}

	if ( isCopy )
	  (*env)->ReleaseIntArrayElements( env, scanLine, pels, JNI_COMMIT);
	(*env)->CallVoidMethod( env, producer, setPix,
						0, j, img->width, 1, model, scanLine, 0, img->width);
  }

  (*env)->CallVoidMethod( env, producer, imgCompl, 3); /* 3 = STATICIMAGEDONE */

  if ( isCopy )
	(*env)->ReleaseIntArrayElements( env, scanLine, pels, JNI_ABORT);
}


/************************************************************************************
 * native format production dispatcher routines (GIF, jpeg, png, ..)
 */

#define SIG_LENGTH 4
#define SIG_GIF    1
#define SIG_JPEG   2
#define SIG_PNG    3

Image *unknownImage = 0;  /* fill in some default image here */

int imageFormat ( unsigned char* sig ) {
  if ( (sig[0] == 'G') && (sig[1] == 'I') && (sig[2] == 'F') )
	return SIG_GIF;

  if ( (sig[0] == 0xff) && (sig[1] == 0xd8) && (sig[2] == 0xff) && (sig[3] == 0xe0) )
	return SIG_JPEG;

  if ( (sig[0] == 0x89) && (sig[1] == 'P') && (sig[2] == 'N') && (sig[3] == 'G') )
	return SIG_PNG;

  return 0;
}

void*
Java_java_awt_Toolkit_imgCreateFromFile ( JNIEnv* env, jclass clazz, jstring fileName )
{
  Image *img = 0;
  FILE  *infile;
  char  *fn = java2CString( env, X, fileName);
  unsigned char  sig[SIG_LENGTH];

  if ((infile = fopen( fn, "rb")) != NULL) {
	if ( fread( sig, 1, sizeof(sig), infile) == sizeof(sig) ) {
	  rewind( infile);  /* some native converters can't skip the signature read */

	  switch ( imageFormat( sig) ) {
	  case SIG_GIF:
		img = readGifFile( infile);
		break;
	  case SIG_JPEG:
		img = readJpegFile( infile);
		break;
	  case SIG_PNG:
		img = readPngFile( infile);
		break;
	  default:
		img = unknownImage;
	  }
	}
	fclose( infile);
  }

  return img;
}

void*
Java_java_awt_Toolkit_imgCreateFromData ( JNIEnv* env, jclass clazz,
										  jarray jbuffer, jint off, jint len )
{
  Image *img = 0;
  jboolean isCopy;
  jint   length = (*env)->GetArrayLength( env, jbuffer);
  jbyte  *jb = (*env)->GetByteArrayElements( env, jbuffer, &isCopy);
  unsigned char *buf = (unsigned char*) jb + off;

  /* in case of a buffer overrun, we probably have a JPEG read error, anyway */
  if ( (off + len) <= length ) {
	switch ( imageFormat( buf) ) {
	case SIG_GIF:
	  img = readGifData( buf, len);
	  break;
	case SIG_JPEG:
	  img = readJpegData( buf, len);
	  break;
	case SIG_PNG:
	  img = readPngData( buf, len);
	  break;
	default:
	  img = unknownImage;
	}
  }

  (*env)->ReleaseByteArrayElements( env, jbuffer, jb, JNI_ABORT);
  return img;  
}


/************************************************************************************
 * field access
 */

jint
Java_java_awt_Toolkit_imgGetWidth ( JNIEnv* env, jclass clazz, Image* img)
{
  return img->width;
}

jint
Java_java_awt_Toolkit_imgGetHeight ( JNIEnv* env, jclass clazz, Image* img)
{
  return img->height;
}
