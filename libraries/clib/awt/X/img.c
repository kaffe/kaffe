/**
 * img.c - 
 *
 * Copyright (c) 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */


#include "toolkit.h"


/* interfaces of image conversion functions */
Image* readGifFile ( int fd );
Image* readGifData ( unsigned char*, long len );
Image* readJpegFile ( int fd );
Image* readJpegData ( unsigned char*, long len );
Image* readPngFile ( int fd );
Image* readPngData ( unsigned char*, long len );

/************************************************************************************
 * own auxiliary funcs
 */

Image*
createImage ( int width, int height )
{
  Image * img = AWT_CALLOC( 1, sizeof( Image));

  img->trans  = -1;     /* default to no alpha */

  img->width = width;   /* we need to (temp) store them for subsequent X image creation */
  img->height = height;

  return img;
}

static int
createShmXImage ( Toolkit* tk, Image* img, int depth, int isMask )
{
#if defined(USE_XSHM_EXTENSION)
  Visual  *vis = DefaultVisual( tk->dsp, DefaultScreen( tk->dsp));
  XShmSegmentInfo* shmi = (XShmSegmentInfo*) AWT_MALLOC( sizeof(XShmSegmentInfo));
  XImage *xim;
  unsigned int    nBytes;

  if ( isMask ) {
	xim = XShmCreateImage( tk->dsp, vis, depth, XYBitmap, 0, shmi, img->width, img->height);
  }
  else {
    xim = XShmCreateImage( tk->dsp, vis, depth, ZPixmap, 0, shmi, img->width, img->height);
  }

  nBytes = xim->bytes_per_line * img->height;
  shmi->shmid = shmget( IPC_PRIVATE, nBytes, (IPC_CREAT | 0777));

  /*
   * It is essential to check if shmget failed, because shared memory is usually
   * a scarce resource
   */
  if ( shmi->shmid == -1 ) {
	XShmDetach( tk->dsp, shmi);
	xim->data = 0;
	XDestroyImage( xim);
	AWT_FREE( shmi);

	tk->shm = SUSPEND_SHM;
	return 0;
  }

  xim->data = shmi->shmaddr = shmat( shmi->shmid, 0, 0);
  shmi->readOnly = False;
  XShmAttach( tk->dsp, shmi);
  /*
   * make sure it will be freed automatically once the attachment count comes
   * down to 0 (either by explicit imgFreeImage or by process termination)
   */
  shmctl( shmi->shmid, IPC_RMID, 0);

  if ( isMask ) {
	memset( xim->data, 0xff, nBytes);
	img->shmiMask = shmi;
	img->xMask = xim;
  }
  else {
	img->shmiImg = shmi;
	img->xImg = xim;
  }

  return 1;
#else
  return 0;
#endif
}


static void
destroyShmXImage ( Toolkit* tk, Image* img, int isMask )
{
#if defined(USE_XSHM_EXTENSION)
  XShmSegmentInfo *shmi;
  XImage          *xim;

  if ( isMask ) {
	shmi = img->shmiMask;
	xim  = img->xMask;
	img->shmiMask = 0;
  }
  else {
	shmi = img->shmiImg;
	xim  = img->xImg;
	img->shmiImg = 0;
  }

  XShmDetach( tk->dsp, shmi);
  xim->data = 0;
  XDestroyImage( xim);
  /* we created it as 'deleted', so we just have to detach here */
  shmdt( shmi->shmaddr);
  AWT_FREE( shmi);

  /* if we have suspended shm, give it a try again */
  if ( tk->shm == SUSPEND_SHM )
	tk->shm = USE_SHM;
#endif
}



void
createXImage ( Toolkit* tk, Image* img )
{
  int bitmap_pad;
  int bytes_per_line;
  int bytes_per_pix;
  unsigned int nPix;
  char *data;
  Visual *vis  = DefaultVisual( tk->dsp, DefaultScreen( tk->dsp));
  int    depth = DefaultDepth(  tk->dsp, DefaultScreen( tk->dsp));
	
  if ( depth <= 8)	      bytes_per_pix = 1;
  else if ( depth <= 16)  bytes_per_pix = 2;
  else			          bytes_per_pix = 4;

  bytes_per_line = bytes_per_pix * img->width;
  bitmap_pad = bytes_per_pix * 8;
  nPix = img->width * img->height;

  if ( (tk->shm == USE_SHM) && (nPix > tk->shmThreshold) && (img->alpha == 0) ) {
	if ( createShmXImage( tk, img, depth, False) ){
	  DBG( AWT_IMG, printf("alloc Shm: %p %p %p (%dx%d) \n", img, img->xImg, img->shmiImg,
					  img->width, img->height));
	  return;
	}
  }

  data = AWT_CALLOC( nPix, bytes_per_pix);
  img->xImg = XCreateImage( tk->dsp, vis, depth, ZPixmap, 0,
							data, img->width, img->height, bitmap_pad, bytes_per_line);
  DBG( AWT_IMG, printf( "alloc: %p %p (%dx%d)\n", img, img->xImg, img->width, img->height));
}

void
createXMaskImage ( Toolkit* tk, Image* img )
{
  int     bytes_per_line;
  unsigned int nBytes, nPix;
  char    *data;
  Visual  *vis = DefaultVisual( tk->dsp, DefaultScreen( tk->dsp));

  bytes_per_line = (img->width + 7) / 8;
  nPix   = img->width * img->height;
  nBytes = bytes_per_line * img->height;

  if ( (tk->shm == USE_SHM) && (nPix > tk->shmThreshold) ) {
	if ( createShmXImage( tk, img, 1, True) ){
	  DBG( AWT_IMG, printf( "alloc Shm mask: %p %p %p (%dx%d) \n", img, img->xMask, img->shmiMask,
					  img->width, img->height));
	  return;
	}
  }

  data = AWT_MALLOC( nBytes);
  memset( data, 0xff, nBytes);

  img->xMask = XCreateImage( tk->dsp, vis, 1, XYBitmap, 0,
							 data, img->width, img->height, 8, bytes_per_line );
  DBG( AWT_IMG, printf( "alloc mask: %p %p (%dx%d)\n", img, img->xMask, img->width, img->height));
}


void
createAlphaImage ( Toolkit* tk, Image *img )
{
  int nBytes = img->width * img->height;

  img->alpha = AWT_MALLOC( sizeof( AlphaImage));

  img->alpha->width  = img->width;
  img->alpha->height = img->height;
  img->alpha->buf = AWT_MALLOC( nBytes);
  memset( img->alpha->buf, 0xff, nBytes);
}

/*
 * For images with a full alpha channel, check if we really need an alpha byte for
 * each pel, or if a mask bitmap (alpha 0x00 / 0xff) will be sufficient
 */
int
needsFullAlpha ( Toolkit* tk, Image *img, double threshold )
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
reduceAlpha ( Toolkit* tk, Image* img, int threshold )
{
  int i, j, a;

  if ( !img->alpha )
	return;

  createXMaskImage( tk, img);

  for ( i=0; i<img->height; i++ ) {
	for ( j=0; j<img->width; j++ ) {
	  a = GetAlpha( img->alpha, j, i);
	  if ( a < threshold ) {
		XPutPixel( img->xImg, j, i, 0);
		XPutPixel( img->xMask, j, i, 0);
	  }
	}
  }

  AWT_FREE( img->alpha->buf);
  AWT_FREE( img->alpha);
  img->alpha = 0;
}


static inline int
interpolate ( int ul, int ur, int ll, int lr, double dx, double dy )
{
  double u = ul + (double)(ur - ul) * dx;
  double l = ll + (double)(lr - ll) * dx;

  return (int) (u + (l - u) * dy  + 0.5);
}

static unsigned int
getScaledAlpha ( Toolkit* tk, Image* img, int x, int y, double dx, double dy )
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
getScaledPixel ( Toolkit* tk, Image* img, int x, int y, double dx, double dy )
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
	rgbValues( tk, ul, &r, &g, &b);
  }
  else {
	rgbValues( tk, ul, &ulR, &ulG, &ulB);
	rgbValues( tk, ur, &urR, &urG, &urB);
	rgbValues( tk, ll, &llR, &llG, &llB);
	rgbValues( tk, lr, &lrR, &lrG, &lrB);

	r = interpolate( ulR, urR, llR, lrR, dx, dy);
	g = interpolate( ulG, urG, llG, lrG, dx, dy);
	b = interpolate( ulB, urB, llB, lrB, dx, dy);
  }

  return pixelValue( tk, (r << 16) | (g << 8) | b);
}



void
initScaledImage ( Toolkit* tk, Image *tgt, Image *src,
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
	syDelta = (sy < (sy1-1)) ? sY - sy : 0;

	for ( dx=dx0; dx != dx1; dx += dxInc ) {
	  sX = sx0 + (dx - dx0) / xScale;
	  sx = (int) sX;
	  sxDelta = (sx < (sx1-1)) ? sX - sx : 0;

	  if ( (c = getScaledPixel( tk, src, sx, sy, sxDelta, syDelta)) != -1 ){
  		XPutPixel( tgt->xImg, dx, dy, c);
		if ( src->alpha )
		  PutAlpha( tgt->alpha, dx, dy, getScaledAlpha( tk, src, sx, sy, sxDelta, syDelta));
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


jobject
Java_java_awt_Toolkit_imgCreateImage ( JNIEnv* env, jclass clazz, jint width, jint height )
{
  Image *img = createImage( width, height);
  createXImage( X, img);
  return JCL_NewRawDataObject(env, img);
}


jobject
Java_java_awt_Toolkit_imgCreateScreenImage ( JNIEnv* env, jclass clazz, jint width, jint height )
{
  Image  *img = createImage( width, height);
  int    depth = DefaultDepth(  X->dsp, DefaultScreen( X->dsp));

  img->pix  = XCreatePixmap( X->dsp, X->root, width, height, depth);
  return JCL_NewRawDataObject(env, img);
}


/* generic (ImageProducer) based image construction */

void
Java_java_awt_Toolkit_imgSetIdxPels ( JNIEnv* env, jclass clazz, jobject nimg,
		jint x, jint y, jint w, jint h,
		jintArray clrMap, jbyteArray idxPels, jint trans,
		jint off, jint scan)
{
  Image *img = UNVEIL_IMG(nimg);
  register int    row, col;
  unsigned long   pix;
  jint            rgb;
  jboolean        isCopy;
  jint            *clr = (*env)->GetIntArrayElements( env, clrMap, &isCopy);
  jbyte           *pel = (*env)->GetByteArrayElements( env, idxPels, &isCopy);
  unsigned char   *idx = (unsigned char*)(pel + off);
  int             maxCol = x + w;
  int             maxRow = y + h;
  unsigned char   curPel;

  if ( (trans >= 0) && !img->xMask )
	createXMaskImage( X, img);

  for ( row = y; row < maxRow; row++) {
    for ( col = x; col < maxCol; col++) {
      curPel = idx[col + row * scan];
      rgb = clr[curPel];
      pix = pixelValue( X, rgb);
      if ( trans >= 0 ) {
		if ( curPel == trans ){
		  pix = 0;
		  XPutPixel( img->xMask, col, row, 0);
		}
      }
      XPutPixel( img->xImg, col, row, pix);
    }
  }

  (*env)->ReleaseIntArrayElements( env, clrMap, clr, JNI_ABORT);
  (*env)->ReleaseByteArrayElements( env, idxPels, pel, JNI_ABORT);
}


void
Java_java_awt_Toolkit_imgSetRGBPels ( JNIEnv* env, jclass clazz, jobject nimg,
				      jint x, jint y, jint w, jint h,
				      jintArray rgbPels, jint off, jint scan)
{
  Image *img = UNVEIL_IMG(nimg);
  register int    row, col;
  unsigned long   pix = 0;
  jboolean        isCopy;
  jint            *rgbs = (*env)->GetIntArrayElements( env, rgbPels, &isCopy);
  jint            *rgb = rgbs + off;
  int             maxCol = x + w;
  int             maxRow = y + h;
  jint		  val;

  for ( row = y; row < maxRow; row++) {
    for ( col = x; col < maxCol; col++) {
	  val = rgb[col + row * scan];
	  if ( (val & 0xff000000) == 0xff000000 ) {
		pix = pixelValue( X, val);
		XPutPixel( img->xImg, col, row, pix);
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
		XPutPixel( img->xImg, col, row, pix);

		/*
		if ( !img->xMask )
		  createXMaskImage( X, img);
		XPutPixel( img->xMask, col, row, 0);
		XPutPixel( img->xImg, col, row, 0);
		*/
	  }
    }
  }

  (*env)->ReleaseIntArrayElements( env, rgbPels, rgbs, JNI_ABORT);
}

void
Java_java_awt_Toolkit_imgComplete( JNIEnv* env, jclass clazz, jobject nimg, jint status )
{
  Image *img = UNVEIL_IMG(nimg);
  /*
   * Check for alpha channel reduction. Note that full alpha images aren't created
   * with Shm (by policy), so you might loose the Shm speed factor. This method is just
   * called for external (generic) production, since our own prod facilities usually
   * know better if and how to do alpha support
   */
  if ( img->alpha &&  !needsFullAlpha( X, img, 0.0) )
	reduceAlpha( X, img, 128);
}

void
Java_java_awt_Toolkit_imgFreeImage(JNIEnv *env, jclass clazz UNUSED, jobject nimg)
{
  imgFreeImage(UNVEIL_IMG(nimg));
}

void
imgFreeImage( Image *img )
{
  Image *next, *first = img;

  /* we have to be aware of image rings (GIF movies), iterate */
  do {
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


jobject
Java_java_awt_Toolkit_imgCreateScaledImage ( JNIEnv* env, jclass clazz,
					     jobject nimg, int width, int height )
{
  Image *img = UNVEIL_IMG(nimg);
  int depth;

  Image *scaledImg = createImage( width, height);

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

  return JCL_NewRawDataObject(env, scaledImg);
}


void
Java_java_awt_Toolkit_imgProduceImage ( JNIEnv* env, jclass clazz, jobject producer, jobject nimg )
{
  Image *img = UNVEIL_IMG(nimg);
  int            i, j;
  int            r, g, b;
  unsigned long  pix;
  jclass         prodClazz, modelClazz;
  jmethodID      modelCtor, setDim, setCM, setHints, setPix, imgCompl;
  jobject        model;
  jintArray      pelArray;
  jint           *pels;
  jboolean       isCopy;

  prodClazz  = (*env)->GetObjectClass( env, producer);
  modelClazz = (*env)->FindClass( env, "kaffe/awt/JavaColorModel");

  modelCtor = (*env)->GetStaticMethodID( env, modelClazz, "getSingleton", "()Lkaffe/awt/JavaColorModel;");
  setDim    = (*env)->GetMethodID( env, prodClazz, "setDimensions", "(II)V");
  setCM     = (*env)->GetMethodID( env, prodClazz, "setColorModel", "(Ljava/awt/image/ColorModel;)V");
  setHints  = (*env)->GetMethodID( env, prodClazz, "setHints", "(I)V");
  setPix    = (*env)->GetMethodID( env, prodClazz, "setPixels", "(IIIILjava/awt/image/ColorModel;[III)V");
  imgCompl  = (*env)->GetMethodID( env, prodClazz, "imageComplete", "(I)V");

  model     = (*env)->CallStaticObjectMethod( env, modelClazz, modelCtor);

  /* for JDK compat, the pixel buffer has to be large enough to hold the *complete* image */
  pelArray  = (*env)->NewIntArray( env, img->width * img->height);
  pels = (*env)->GetIntArrayElements( env, pelArray, &isCopy);

  (*env)->CallVoidMethod( env, producer, setDim, img->width, img->height);
  (*env)->CallVoidMethod( env, producer, setCM, model);
  (*env)->CallVoidMethod( env, producer, setHints, 6); /* TOPDOWNLEFTRIGHT | COMPLETESCANLINES */

  if ( img->pix && !img->xImg ) {
	img->xImg = XGetImage( X->dsp, img->pix, 0, 0, img->width, img->height, 0xffffffff, ZPixmap);
  }

  if ( img->xImg ) {
	for ( j=0; j<img->height; j++ ) {
	  for ( i=0; i<img->width; i++ ) {
		if ( (img->xMask == 0) || XGetPixel( img->xMask, i, j) ) {
		  pix = XGetPixel( img->xImg, i, j);
		  rgbValues( X, pix, &r, &g, &b);
		  pels[j*img->width+i] = (0xff000000 | (r << 16) | (g << 8) | b);
		}
		else {
		  pels[j*img->width+i] = 0;
		}
	  }
	}
  }

  if ( img->pix && img->xImg ) {
	XDestroyImage( img->xImg);
	img->xImg = 0;
  }

  if ( isCopy ) {
    (*env)->ReleaseIntArrayElements( env, pelArray, pels, JNI_COMMIT);
  }

  (*env)->CallVoidMethod( env, producer, setPix, 0, 0, img->width, img->height, model, pelArray, 0, img->width);
  (*env)->CallVoidMethod( env, producer, imgCompl, 3); /* 3 = STATICIMAGEDONE */
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

jobject
Java_java_awt_Toolkit_imgCreateFromFile ( JNIEnv* env, jclass clazz, jstring fileName )
{
  Image *img = 0;
  int   infile;
  char  *fn = java2CString( env, X, fileName);
  unsigned char  sig[SIG_LENGTH];

  if (!X->colorMode)
	initColorMapping( env, clazz, X );

  if ( (infile = AWT_OPEN( fn)) >= 0 ) {
	if ( AWT_READ( infile, sig, sizeof(sig)) == sizeof(sig) ) {
	  AWT_REWIND( infile);  /* some native converters can't skip the signature read */

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
	AWT_CLOSE( infile);
  }

  if (img != NULL)
    return JCL_NewRawDataObject(env, img);

  return NULL;
}

jobject
Java_java_awt_Toolkit_imgCreateFromData ( JNIEnv* env, jclass clazz,
					  jbyteArray jbuffer, jint off, jint len )
{
  Image *img = NULL;
  jobject nimg = NULL;
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
  if (img != NULL)
     nimg = JCL_NewRawDataObject(env, img);
  return nimg;
}

jobject
Java_java_awt_Toolkit_imgSetFrame ( JNIEnv* env, jclass clazz, jobject nimg, int frameNo )
{
  Image *img = UNVEIL_IMG(nimg);
  Image *imgCur = img;

  if ( !img->next )
	return img;

  while ( img->frame != frameNo ) {
	img = img->next;
	if ( img == imgCur ) {
		break;
	}
  }

  return nimg;
}


/************************************************************************************
 * field access
 */

jint
Java_java_awt_Toolkit_imgGetWidth ( JNIEnv* env, jclass clazz, jobject nimg)
{
  return UNVEIL_IMG(nimg)->width;
}

jint
Java_java_awt_Toolkit_imgGetHeight ( JNIEnv* env, jclass clazz, jobject nimg)
{
  return UNVEIL_IMG(nimg)->height;
}

jboolean
Java_java_awt_Toolkit_imgIsMultiFrame ( JNIEnv* env, jclass clazz, jobject nimg)
{
  return (UNVEIL_IMG(nimg)->next != 0);
}

jint
Java_java_awt_Toolkit_imgGetLatency ( JNIEnv* env, jclass clazz, jobject nimg)
{
  return UNVEIL_IMG(nimg)->latency;
}

void*
Java_java_awt_Toolkit_imgGetNextFrame ( JNIEnv* env, jclass clazz, jobject nimg)
{
  return UNVEIL_IMG(nimg)->next;   /* next in the ring */
}
