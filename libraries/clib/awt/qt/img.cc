/*
 * img.c - 
 *
 * Copyright (c) 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#include <stdio.h>

#include <qimage.h>

#include "toolkit.h"


/* interfaces of image conversion functions */
//Image* readGifFile ( int fd , char *fn);
//Image* readGifData ( unsigned char*, long len );
//Image* readJpegFile ( int fd );
//Image* readJpegData ( unsigned char*, long len );
//Image* readPngFile ( int fd );
//Image* readPngData ( unsigned char*, long len );

/************************************************************************************
 * own auxiliary funcs
 */

Image*
createImage ( int width, int height )
{
  Image * img = (Image*)AWT_CALLOC( 1, sizeof( Image));

  img->trans  = -1;     /* default to no alpha */

  img->width = width;   /* we need to (temp) store them for subsequent X image creation */
  img->height = height;

  return img;
}

#if 0
static int
createShmXImage ( Toolkit* X, Image* img, int depth, int isMask )
{
  return 0;
}


static void
destroyShmXImage ( Toolkit* X, Image* img, int isMask )
{
}
#endif


void
createXImage ( Toolkit* X, Image* img )
{
  int bitmap_pad;
  int bytes_per_line;
  int bytes_per_pix;
  unsigned int nPix;
  char *data;
#if 0
  Visual *vis  = DefaultVisual( X->dsp, DefaultScreen( X->dsp));
  int    depth = DefaultDepth(  X->dsp, DefaultScreen( X->dsp));
	
  if ( depth <= 8)	      bytes_per_pix = 1;
  else if ( depth <= 16)  bytes_per_pix = 2;
  else			          bytes_per_pix = 4;

  bytes_per_line = bytes_per_pix * img->width;
  bitmap_pad = bytes_per_pix * 8;
  nPix = img->width * img->height;

  if ( (X->shm == USE_SHM) && (nPix > X->shmThreshold) && (img->alpha == 0) ) {
	if ( createShmXImage( X, img, depth, False) ){
	  DBG( AWT_IMG, printf("alloc Shm: %p %p %p (%dx%d) \n", img, img->qImg, img->shmiImg,
					  img->width, img->height));
	  return;
	}
  }

  data = AWT_CALLOC( nPix, bytes_per_pix);
  img->xImg = XCreateImage( X->dsp, vis, depth, ZPixmap, 0,
							data, img->width, img->height, bitmap_pad, bytes_per_line);
#endif
  img->qImg = new QImage();
  DBG( AWT_IMG, printf( "alloc: %p %p (%dx%d)\n", img, img->qImg, img->width, img->height));
}

void
createXMaskImage ( Toolkit* X, Image* img )
{
  int     bytes_per_line;
  unsigned int nBytes, nPix;
  char    *data;
  //Visual  *vis = DefaultVisual( X->dsp, DefaultScreen( X->dsp));

  bytes_per_line = (img->width + 7) / 8;
  nPix   = img->width * img->height;
  nBytes = bytes_per_line * img->height;

#if 0
  if ( (X->shm == USE_SHM) && (nPix > X->shmThreshold) ) {
	if ( createShmXImage( X, img, 1, True) ){
	  DBG( AWT_IMG, printf( "alloc Shm mask: %p %p %p (%dx%d) \n", img, img->xMask, img->shmiMask,
					  img->width, img->height));
	  return;
	}
  }
#endif
  data = (char*)AWT_MALLOC( nBytes);
  memset( data, 0xff, nBytes);

  img->qImg_AlphaMask = new QImage();
//  img->xMask = XCreateImage( X->dsp, vis, 1, XYBitmap, 0,
//							 data, img->width, img->height, 8, bytes_per_line );
  DBG( AWT_IMG, printf( "alloc mask: %p %p (%dx%d)\n", img, img->qImg_AlphaMask, img->width, img->height));
}


void
createAlphaImage ( Toolkit* X, Image *img )
{
  int nBytes = img->width * img->height;

  img->alpha = (AlphaImage*)AWT_MALLOC( sizeof( AlphaImage));

  img->alpha->width  = img->width;
  img->alpha->height = img->height;
  img->alpha->buf = (unsigned char*)AWT_MALLOC( nBytes);
  memset( img->alpha->buf, 0xff, nBytes);
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

/*
 * A full alpha image channel is way slower than using a mask bitmap (= 0 / 0xff alpha).
 * This function provides a simple alpha-to-mask translation
 */
/* also used in imgpng */
void
reduceAlpha ( Toolkit* X, Image* img, int threshold )
{
  int i, j, a;

  if ( !img->alpha )
	return;

  //createXMaskImage( X, img);
  img->qImg->setAlphaBuffer(TRUE);
  *(img->qImg_AlphaMask) = img->qImg->createAlphaMask();

  for ( i=0; i<img->height; i++ ) {
	for ( j=0; j<img->width; j++ ) {
	  a = GetAlpha( img->alpha, j, i);
	  if ( a < threshold ) {
		//XPutPixel( img->xImg, j, i, 0);
		//XPutPixel( img->xMask, j, i, 0);
		
 		DBG( AWT_IMG, printf("reduce alpha! %d %d",img->qImg->width(),img->qImg->height()) );
		img->qImg->setPixel(j,i,0);
	  }
	}
  }

  AWT_FREE( img->alpha->buf);
  AWT_FREE( img->alpha);
  img->alpha = 0;
}

#if 0
static inline int
interpolate ( int ul, int ur, int ll, int lr, double dx, double dy )
{
  double u = ul + (double)(ur - ul) * dx;
  double l = ll + (double)(lr - ll) * dx;

  return (int) (u + (l - u) * dy  + 0.5);
}

static unsigned int
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

static long
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
//	rgbValues( X, ul, &r, &g, &b);
  }
  else {
//	rgbValues( X, ul, &ulR, &ulG, &ulB);
//	rgbValues( X, ur, &urR, &urG, &urB);
//	rgbValues( X, ll, &llR, &llG, &llB);
//	rgbValues( X, lr, &lrR, &lrG, &lrB);

	r = interpolate( ulR, urR, llR, lrR, dx, dy);
	g = interpolate( ulG, urG, llG, lrG, dx, dy);
	b = interpolate( ulB, urB, llB, lrB, dx, dy);
  }

//  return pixelValue( X, (r << 16) | (g << 8) | b);
  return 0;
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
		if                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                              ( src->alpha )
		  PutAlpha( tgt->alpha, dx, dy, getScaledAlpha( X, src, sx, sy, sxDelta, syDelta));
	  }
	  else {
		XPutPixel( tgt->xMask, dx, dy, 0);
		XPutPixel( tgt->xImg, dx, dy, 0);
	  }
	}
  }
}
#endif

/************************************************************************************
 * exported native methods
 */


void*
Java_java_awt_Toolkit_imgCreateImage ( JNIEnv* env, jclass clazz, jint width, jint height )
{
  printf("imgCreateImage w=%d h=%d\n",(int)width,(int)height);
  Image *img = createImage( width, height);
  createXImage( X, img);
  return img;
}


void*
Java_java_awt_Toolkit_imgCreateScreenImage ( JNIEnv* env, jclass clazz, jint width, jint height )
{
  printf("imgCreateScreenImage w=%d h=%d\n",(int)width,(int)height);
  Image  *img = createImage( width, height);
  //int    depth = DefaultDepth(  X->dsp, DefaultScreen( X->dsp));

  //img->pix  = XCreatePixmap( X->dsp, X->root, width, height, depth);
  img->qpm = new QPixmap(width,height);
  return img;
}


/* generic (ImageProducer) based image construction */

void
Java_java_awt_Toolkit_imgSetIdxPels ( JNIEnv* env, jclass clazz, Image * img,
		jint x, jint y, jint w, jint h,
		jintArray clrMap, jbyteArray idxPels, jint trans,
		jint off, jint scan)
{
  printf("imgSetIdxPels\n");
  register int    row, col;
  unsigned long   pix;
  jint            rgb;
  jboolean        isCopy;
  jint            *clr = env->GetIntArrayElements( clrMap, &isCopy);
  jbyte           *pel = env->GetByteArrayElements( idxPels, &isCopy);
  unsigned char   *idx = (unsigned char*)(pel + off);
  int             maxCol = x + w;
  int             maxRow = y + h;
  unsigned char   curPel;

//  if ( (trans >= 0) && !img->xMask )
//	createXMaskImage( X, img);
  if ( (trans >= 0) && !img->qImg_AlphaMask)
	img->qImg_AlphaMask = new QImage();

  for ( row = y; row < maxRow; row++) {
    for ( col = x; col < maxCol; col++) {
      curPel = idx[col + row * scan];
      rgb = clr[curPel];
//      pix = pixelValue( X, rgb);
      if ( trans >= 0 ) {
		if ( curPel == trans ){
		  pix = 0;
		  //XPutPixel( img->xMask, col, row, 0);
		  img->qImg_AlphaMask->setPixel(col,row,0);
		}
      }
      //XPutPixel( img->xImg, col, row, pix);
      img->qImg->setPixel(col,row,rgb);
    }
  }

  env->ReleaseIntArrayElements( clrMap, clr, JNI_ABORT);
  env->ReleaseByteArrayElements( idxPels, pel, JNI_ABORT);
}


void
Java_java_awt_Toolkit_imgSetRGBPels ( JNIEnv* env, jclass clazz, Image * img,
									  jint x, jint y, jint w, jint h,
									  jintArray rgbPels, jint off, jint scan)
{
  printf("imgSetRGBPels\n");
  register int    row, col;
  unsigned long   pix = 0;
  jboolean        isCopy;
  jint            *rgbs = env->GetIntArrayElements( rgbPels, &isCopy);
  jint            *rgb = rgbs + off;
  int             maxCol = x + w;
  int             maxRow = y + h;
  jint		  val;

  for ( row = y; row < maxRow; row++) {
    for ( col = x; col < maxCol; col++) {
	  val = rgb[col + row * scan];
	  if ( (val & 0xff000000) == 0xff000000 ) {
//		pix = pixelValue( X, val);
		//XPutPixel( img->xImg, col, row, pix);
		img->qImg->setPixel(col,row,val);
	  }
	  else {
		/*
		 * No way to tell for now if this will be a reduced (on/off) or a
		 * full alpha channel. We have to be prepared for the "worst", and reduce
		 * later
		 */
		if ( !img->alpha )
		  createAlphaImage( X, img);
		PutAlpha( img->alpha, col, row, (val >> 24));
		
  		//XPutPixel( img->xImg, col, row, pix);
		img->qImg->setPixel(col,row,val);
		/*
		if ( !img->xMask )
		  createXMaskImage( X, img);
		XPutPixel( img->xMask, col, row, 0);
		XPutPixel( img->xImg, col, row, 0);
		*/
		
		if( !img->qImg_AlphaMask)
		  img->qImg_AlphaMask = new QImage(img->width,img->height,img->qImg->depth());
		img->qImg_AlphaMask->setPixel(col,row,0);
	  }
    }
  }

  env->ReleaseIntArrayElements( rgbPels, rgbs, JNI_ABORT);
}

void
Java_java_awt_Toolkit_imgComplete( JNIEnv* env, jclass clazz, Image * img, jint status )
{
  /*
   * Check for alpha channel reduction. Note that full alpha images aren't created
   * with Shm (by policy), so you might loose the Shm speed factor. This method is just
   * called for external (generic) production, since our own prod facilities usually
   * know better if and how to do alpha support
   */
  printf("imgComplete\n");
  if ( img->alpha &&  !needsFullAlpha( X, img, 0.0) )
	reduceAlpha( X, img, 128);
}

void
Java_java_awt_Toolkit_imgFreeImage( JNIEnv* env, jclass clazz, Image * img)
{
  Image *next, *first = img;

  /* we have to be aware of image rings (GIF movies), iterate */
  do {
#if 0
	if ( img->pix ){
	  XFreePixmap( X->dsp, img->pix);
	  img->pix = 0;
	}

	/*
	 * note that XDestroyImage automatically frees any non-NULL data pointer
	 * (since we explicitly allocated the data, we better free it explicitly, too)
	 * malloc, free might be resolved
	 */

	if ( img->xImg ){
	  if ( img->shmiImg ) {
		DBG( AWT_IMG, printf( "free Shm: %p %p %p (%dx%d)\n", img, img->xImg, img->shmiImg,
						img->width, img->height));
		destroyShmXImage( X, img, False);
	  }
	  else {
		DBG( AWT_IMG, printf( "free: %p %p (%dx%d)\n", img, img->xImg, img->width, img->height));
		AWT_FREE( img->xImg->data);
		img->xImg->data = 0;
		XDestroyImage( img->xImg);
	  }
	  img->xImg = 0;
	}

	if ( img->xMask ){
	  if ( img->shmiMask ) {
		DBG( AWT_IMG, printf( "free Shm mask: %p %p %p (%dx%d)\n", img, img->xMask, img->shmiMask,
						img->width, img->height));
		destroyShmXImage( X, img, True);
	  }
	  else {
		DBG( AWT_IMG, printf( "free mask: %p %p (%dx%d)\n", img, img->xMask, img->width, img->height));
		AWT_FREE( img->xMask->data);
		img->xMask->data = 0;
		XDestroyImage( img->xMask);
	  }
	  img->xMask = 0;
	}
#endif
	if ( img->alpha ) {
	  AWT_FREE( img->alpha->buf);
	  AWT_FREE( img->alpha);
	  img->alpha = 0;
	}

	next = img->next;
	AWT_FREE( img);
	img = next;
  } while ( (img != 0) && (img != first) );
}


void*
Java_java_awt_Toolkit_imgCreateScaledImage ( JNIEnv* env, jclass clazz,
											 Image* img, int width, int height )
{
//  int depth;

  printf("imgCreateScaledImage img=%p w=%d h=%d\n",img, (int)width, (int)height);
  Image *scaledImg = createImage( width, height);

  *(scaledImg->qImg) = img->qImg->smoothScale(width,height);
  reconvertImage(scaledImg);

/*  
  if ( img->xImg ) {
	createXImage( X, scaledImg);
	if ( img->xMask )
	  createXMaskImage( X, scaledImg);

	initScaledImage ( X, scaledImg, img,
					  0, 0, width-1, height-1,
					  0, 0, img->width-1, img->height-1);
  }
  else if ( img->pix ) {
	depth = DefaultDepth(  X->dsp, DefaultScreen( X->dsp));
	scaledImg->pix  = XCreatePixmap( X->dsp, X->root, width, height, depth);
  }
*/
  return scaledImg;
}


void
Java_java_awt_Toolkit_imgProduceImage ( JNIEnv* env, jclass clazz, jobject producer, Image* img )
{
  printf("imgProduceImage\n");
  int            i, j;
  int            r, g, b;
  unsigned long  pix;
  jclass         prodClazz, modelClazz;
  jmethodID      modelCtor, setDim, setCM, setHints, setPix, imgCompl;
  jobject        model;
  jintArray      pelArray;
  jint           *pels;
  jboolean       isCopy;

  prodClazz  = env->GetObjectClass( producer);
  modelClazz = env->FindClass( "kaffe/awt/JavaColorModel");

  modelCtor = env->GetStaticMethodID( modelClazz, "getSingleton", "()Lkaffe/awt/JavaColorModel;");
  setDim    = env->GetMethodID( prodClazz, "setDimensions", "(II)V");
  setCM     = env->GetMethodID( prodClazz, "setColorModel", "(Ljava/awt/image/ColorModel;)V");
  setHints  = env->GetMethodID( prodClazz, "setHints", "(I)V");
  setPix    = env->GetMethodID( prodClazz, "setPixels", "(IIIILjava/awt/image/ColorModel;[III)V");
  imgCompl  = env->GetMethodID( prodClazz, "imageComplete", "(I)V");

  model     = env->CallStaticObjectMethod( modelClazz, modelCtor);

  /* for JDK compat, the pixel buffer has to be large enough to hold the *complete* image */
  pelArray  = env->NewIntArray( img->width * img->height);
  pels = env->GetIntArrayElements( pelArray, &isCopy);

  env->CallVoidMethod( producer, setDim, img->width, img->height);
  env->CallVoidMethod( producer, setCM, model);
  env->CallVoidMethod( producer, setHints, 6); /* TOPDOWNLEFTRIGHT | COMPLETESCANLINES */
#if 0
  if ( img->pix && !img->xImg ) {
	img->xImg = XGetImage( X->dsp, img->pix, 0, 0, img->width, img->height, 0xffffffff, ZPixmap);
  }
#endif
  if ( img->qImg ) {
	for ( j=0; j<img->height; j++ ) {
	  for ( i=0; i<img->width; i++ ) {
		if ( (img->qImg_AlphaMask == 0) || img->qImg_AlphaMask->pixel( i, j) ) {
		  pix = img->qImg->pixel(i,j);
//		  pix = XGetPixel( img->xImg, i, j);
//		  rgbValues( X, pix, &r, &g, &b);
		  pels[j*img->width+i] = (0xff000000 | (r << 16) | (g << 8) | b);
		}
		else {
		  pels[j*img->width+i] = 0;
		}
	  }
	}
  }
#if 0
  if ( img->pix && img->xImg ) {
	XDestroyImage( img->xImg);
	img->xImg = 0;
  }
#endif
  if ( isCopy ) {
    env->ReleaseIntArrayElements( pelArray, pels, JNI_COMMIT);
  }

  env->CallVoidMethod( producer, setPix, 0, 0, img->width, img->height, model, pelArray, 0, img->width);
  env->CallVoidMethod( producer, imgCompl, 3); /* 3 = STATICIMAGEDONE */
}



/************************************************************************************
 * native format production dispatcher routines (GIF, jpeg, png, ..)
 */

#define SIG_LENGTH 4
#define SIG_GIF    1
#define SIG_JPEG   2
#define SIG_PNG    3

Image *unknownImage = 0;  /* fill in some default image here */

static int imageFormat ( unsigned char* sig ) {
  if ( (sig[0] == 'G') && (sig[1] == 'I') && (sig[2] == 'F') )
	return SIG_GIF;

  if ( (sig[0] == 0xff) && (sig[1] == 0xd8) && (sig[2] == 0xff) && (sig[3] == 0xe0) )
	return SIG_JPEG;

  if ( (sig[0] == 0x89) && (sig[1] == 'P') && (sig[2] == 'N') && (sig[3] == 'G') )
	return SIG_PNG;

  return 0;
}


bool reconvertImage(Image *img)
{
    bool success = FALSE;

    if ( !img->qImg) return FALSE;
/*
    if ( alloc_context ) {
	QColor::destroyAllocContext( alloc_context );
	alloc_context = 0;
    }
    if ( useColorContext ) {
	alloc_context = QColor::enterAllocContext();
	// Clear the image to hide flickering palette
	QPainter painter(this);
	painter.eraseRect(0, menubar->heightForWidth( width() ), width(),
height());
    }

    QApplication::setOverrideCursor( waitCursor ); // this might take time
*/    
    img->qpm = new QPixmap();
    if ( img->qpm->convertFromImage(*(img->qImg), 0) )
    {
	//img->qpmScaled = QPixmap();
	//scale();
	success = TRUE;
    } else {
	img->qpm->resize(0,0);				// couldn't load image
    }
//    updateStatus();
//    QApplication::restoreOverrideCursor();	// restore original cursor

//    if ( useColorContext() )
//	QColor::leaveAllocContext();

    return success;				// TRUE if loaded OK
}



void*
Java_java_awt_Toolkit_imgCreateFromFile ( JNIEnv* env, jclass clazz, jstring fileName )
{
  
  Image *img = 0;
  int   infile;
  char  *fn = java2CString( env, X, fileName);
  unsigned char  sig[SIG_LENGTH];

  printf("imgCreateFromFile file=%s\n",fn);
  QImage *image = new QImage();
  bool ok = FALSE;
#if 0  
  if ( (infile = AWT_OPEN( fn)) >= 0 ) {
	if ( AWT_READ( infile, sig, sizeof(sig)) == sizeof(sig) ) {
	  AWT_REWIND( infile);  /* some native converters can't skip the signature read */

	  switch ( imageFormat( sig) ) {
	  case SIG_GIF:
		//fprintf(stderr,"got gif!!!!\n");
		//img = readGifFile( infile,fn);
		//fprintf(stderr,"img=%p\n",img);
		//if(img->next) break;
                //AWT_FREE(img);
	  case SIG_JPEG:
		//img = readJpegFile( infile);
		//break;
	  case SIG_PNG:
		//img = readPngFile( infile);
		//break;
		//QApplication::setOverrideCursor( waitCursor );
#endif
		ok = image->load( fn);
		//QApplication::restoreOverrideCursor();
		if (ok) {
		  img = createImage( image->width(),image->height());
		  img->qImg = image;

                  ok = reconvertImage(img);
		  if ( !ok ) img->qpm->resize(0,0);
		}
#if 0
		break;
	  default:
		img = unknownImage;
	  }
	}
	AWT_CLOSE( infile);
  }
#endif  
  return img;
}

void*
Java_java_awt_Toolkit_imgCreateFromData ( JNIEnv* env, jclass clazz,
										  jbyteArray jbuffer, jint off, jint len )
{
  printf("imgCreateFromData\n");
  Image *img = 0;
  jboolean isCopy;
  jint   length = env->GetArrayLength( jbuffer);
  jbyte  *jb = env->GetByteArrayElements( jbuffer, &isCopy);
  unsigned char *buf = (unsigned char*) jb + off;

  
  /* in case of a buffer overrun, we probably have a JPEG read error, anyway */
  if ( (off + len) <= length ) {
	//QByteArray a;
	//a.setRawData((char *) buf, sizeof(buf));
	QImage *image = new QImage();
        image->loadFromData(buf, len);
	bool ok = FALSE;
#if 0
	switch ( imageFormat( buf) ) {
	case SIG_GIF:
	  //img = readGifData( buf, len);
	  //if(img->next) break;
          //AWT_FREE(img);
	case SIG_JPEG:
//	  img = readJpegData( buf, len);
//	  break;
	case SIG_PNG:
//	  img = readPngData( buf, len);
//	  break;
#endif
	img = createImage( image->width(),image->height());
        img->qImg = image;
   	ok = reconvertImage(img);
   	if ( !ok ) img->qpm->resize(0,0);
#if 0 
  	break;

	default:
	  img = unknownImage;
	}
#endif
  }

  env->ReleaseByteArrayElements( jbuffer, jb, JNI_ABORT);
  return img;  
}

void*
Java_java_awt_Toolkit_imgSetFrame ( JNIEnv* env, jclass clazz, Image* img, int frameNo )
{
  printf("imgSetFrame\n");
  Image *imgCur = img;

  if ( !img->next )
	return img;

  while ( img->frame != frameNo ) {
	img = img->next;
	if ( img == imgCur ) {
		break;
	}
  }

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

jboolean
Java_java_awt_Toolkit_imgIsMultiFrame ( JNIEnv* env, jclass clazz, Image* img)
{
  return (img->next != 0);
}

jint
Java_java_awt_Toolkit_imgGetLatency ( JNIEnv* env, jclass clazz, Image* img)
{
  return img->latency;
}

void*
Java_java_awt_Toolkit_imgGetNextFrame ( JNIEnv* env, jclass clazz, Image* img )
{
  return img->next;   /* next in the ring */
}
