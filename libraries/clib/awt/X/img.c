/**
 * img.c - 
 *
 * Copyright (c) 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */


#include <setjmp.h>

#include <stdio.h>
#include <stdlib.h>
#include "jpeglib.h"

#include "config.h"
#include "toolkit.h"



/************************************************************************************
 * own auxiliary funcs
 */

__inline__ Image*
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


__inline__ int
interpolate ( int ul, int ur, int ll, int lr, double dx, double dy )
{
  double u = ul + (double)(ur - ul) * dx;
  double l = ll + (double)(lr - ll) * dx;

  return (int) (u + (l - u) * dy  + 0.5);
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
  int            dx, dy, dxInc, dyInc, sx, sy, x, y, x0, y0;
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

	  if ( (c = getScaledPixel( X, src, sx, sy, sxDelta, syDelta)) != -1 )
  		XPutPixel( tgt->xImg, dx, dy, c);
	  else {
		XPutPixel( tgt->xMask, dx, dy, 0);
		XPutPixel( tgt->xImg, dx, dy, 0);
	  }
	}
  }
}

/************************************************************************************
 * JPEG input rotinues
 */

#define	CM_RED		0
#define	CM_GREEN	1
#define	CM_BLUE		2

struct my_error_mgr {
  struct jpeg_error_mgr pub;
  jmp_buf setjmp_buffer;
};

typedef struct my_error_mgr * my_error_ptr;

void
my_error_exit ( j_common_ptr cinfo)
{
  my_error_ptr myerr = (my_error_ptr) cinfo->err;
  (*cinfo->err->output_message)(cinfo);
  longjmp(myerr->setjmp_buffer, 1);
}

void
jscan_to_img( Image * img, JSAMPROW buf, struct jpeg_decompress_struct * cinfo)
{
  register col, pix, rgb, idx;
  register JSAMPARRAY colormap = cinfo->colormap;

  for ( col = 0; col < cinfo->output_width; col++) {
    idx = *buf++;
    rgb = (colormap[CM_RED][idx] << 16) | (colormap[CM_GREEN][idx] << 8) | colormap[CM_BLUE][idx];    
    pix = pixelValue( X, rgb);
    XPutPixel( img->xImg, col, cinfo->output_scanline-1, pix);
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
  register        row, col;
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
      rgb = clr[*idx];
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
  register        row, col;
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

  free( img);
}


/* native file-based image construction */

void*
Java_java_awt_Toolkit_imgCreateGifImage ( JNIEnv* env, jclass clazz, jstring gifpath)
{
  Image* img = 0;

  /* Ideally there'd be a routine here to generate the GIF image - but GIF
   * is a patented technology so we can't redistribute the code.
   */

  return img;
}


void*
Java_java_awt_Toolkit_imgCreateJpegImage ( JNIEnv* env, jclass clazz, 
										   jstring jpegPath, jint colors)
{
  Image *img;
  struct jpeg_decompress_struct cinfo;
  struct my_error_mgr jerr;
  FILE * infile;
  JSAMPARRAY buffer;
  int row_stride;
  char * filename = java2CString( env, X, jpegPath);

  if ((infile = fopen(filename, "rb")) == NULL)
    return 0;

  cinfo.err = jpeg_std_error(&jerr.pub);
  jerr.pub.error_exit = my_error_exit;

  if ( setjmp(jerr.setjmp_buffer)) {
	if ( img )
	  Java_java_awt_Toolkit_imgFreeImage( env, clazz, img);

    jpeg_destroy_decompress(&cinfo);
    fclose( infile);
    return 0;
  }

  jpeg_create_decompress(&cinfo);
  jpeg_stdio_src(&cinfo, infile);
  (void) jpeg_read_header(&cinfo, TRUE);

  if ( colors < 8 )
    colors = 8;
  else if ( colors > 256 )
    colors = 256;

  cinfo.desired_number_of_colors = colors;
  cinfo.quantize_colors = TRUE;
  cinfo.out_color_space = JCS_RGB;

  (void) jpeg_start_decompress(&cinfo);
  row_stride = cinfo.output_width * cinfo.output_components;

  /* it's time to create the target image */
  img = createImage( cinfo.output_width, cinfo.output_height);
  img->xImg = createXImage( X, img->width, img->height);

  buffer = (*cinfo.mem->alloc_sarray)
	((j_common_ptr) &cinfo, JPOOL_IMAGE, row_stride, 1);

  while (cinfo.output_scanline < cinfo.output_height) {
    (void) jpeg_read_scanlines(&cinfo, buffer, 1);
    jscan_to_img( img, buffer[0], &cinfo);
  }

  (void) jpeg_finish_decompress(&cinfo);
  jpeg_destroy_decompress(&cinfo);
  fclose(infile);

  return img;
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

/* field access */

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
