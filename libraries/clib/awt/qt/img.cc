/*
 * img.c - 
 *
 * Copyright (c) 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * Copyright (c) 2002, 2003, 2004
 *	Kaffe.org contributors, see ChangeLog for details.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#include <stdio.h>

#include <qimage.h>

#include "toolkit.h"

/**
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


void
createXImage ( Toolkit* X, Image* img )
{
  int bitmap_pad;
  int bytes_per_line;
  int bytes_per_pix;
  unsigned int nPix;
  char *data;
  img->qImg = new QImage();
  DBG( AWT_IMG, qqDebug("alloc: %p %p (%dx%d)\n", img, img->qImg, img->width, img->height));
}

void
createXMaskImage ( Toolkit* X, Image* img )
{
  int     bytes_per_line;
  unsigned int nBytes, nPix;
  char    *data;

  bytes_per_line = (img->width + 7) / 8;
  nPix   = img->width * img->height;
  nBytes = bytes_per_line * img->height;

  data = (char*)AWT_MALLOC( nBytes);
  memset( data, 0xff, nBytes);

  img->qImg_AlphaMask = new QImage();
  DBG( AWT_IMG, qqDebug( "alloc mask: %p %p (%dx%d)\n", img, img->qImg_AlphaMask, img->width, img->height));
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

  max = (img->width * img->height) * (int)threshold;

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

  img->qImg->setAlphaBuffer(TRUE);
  *(img->qImg_AlphaMask) = img->qImg->createAlphaMask();

  for ( i=0; i<img->height; i++ ) {
	for ( j=0; j<img->width; j++ ) {
	  a = GetAlpha( img->alpha, j, i);
	  if ( a < threshold ) {
 		DBG( AWT_IMG, qqDebug("reduce alpha! %d %d",img->qImg->width(),img->qImg->height()) );
		img->qImg->setPixel(j,i,0);
	  }
	}
  }

  AWT_FREE( img->alpha->buf);
  AWT_FREE( img->alpha);
  img->alpha = 0;
}

/**
 * exported native methods
 */


void*
Java_java_awt_Toolkit_imgCreateImage ( JNIEnv* env, jclass clazz, jint width, jint height )
{
  DBG( AWT_IMG, qqDebug("imgCreateImage w=%d h=%d\n",(int)width,(int)height));
  Image *img = createImage( width, height);
  createXImage( X, img);
  return img;
}


void*
Java_java_awt_Toolkit_imgCreateScreenImage ( JNIEnv* env, jclass clazz, jint width, jint height )
{
  DBG( AWT_IMG, qqDebug("imgCreateScreenImage w=%d h=%d\n",(int)width,(int)height));
  Image  *img = createImage( width, height);
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
  DBG( AWT_IMG, qqDebug("imgSetIdxPels\n"));
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

  if ( (trans >= 0) && !img->qImg_AlphaMask)
	img->qImg_AlphaMask = new QImage();

  for ( row = y; row < maxRow; row++) {
    for ( col = x; col < maxCol; col++) {
      curPel = idx[col + row * scan];
      rgb = clr[curPel];
      if ( trans >= 0 ) {
		if ( curPel == trans ){
		  pix = 0;
		  img->qImg_AlphaMask->setPixel(col,row,0);
		}
      }
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
  DBG( AWT_IMG, qqDebug("imgSetRGBPels\n"));
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
		
		img->qImg->setPixel(col,row,val);
		
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
  DBG( AWT_IMG, qqDebug("imgComplete\n"));
  if ( img->alpha &&  !needsFullAlpha( X, img, 0.0) )
	reduceAlpha( X, img, 128);
}

void
Java_java_awt_Toolkit_imgFreeImage( JNIEnv* env, jclass clazz, Image * img)
{
  Image *next, *first = img;

  /* we have to be aware of image rings (GIF movies), iterate */
  do {
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
  DBG( AWT_IMG, qqDebug("imgCreateScaledImage img=%p w=%d h=%d\n",img, (int)width, (int)height));
  Image *scaledImg = createImage( width, height);

  *(scaledImg->qImg) = img->qImg->smoothScale(width,height);
  reconvertImage(scaledImg);
  
  return scaledImg;
}


void
Java_java_awt_Toolkit_imgProduceImage ( JNIEnv* env, jclass clazz, jobject producer, Image* img )
{
  DBG( AWT_IMG, qqDebug("imgProduceImage\n"));
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
  
  if ( img->qImg ) {
	for ( j=0; j<img->height; j++ ) {
	  for ( i=0; i<img->width; i++ ) {
		if ( (img->qImg_AlphaMask == 0) || img->qImg_AlphaMask->pixel( i, j) ) {
		  pix = img->qImg->pixel(i,j);
		  pels[j*img->width+i] = (0xff000000 | (r << 16) | (g << 8) | b);
		}
		else {
		  pels[j*img->width+i] = 0;
		}
	  }
	}
  }
  if ( isCopy ) {
    env->ReleaseIntArrayElements( pelArray, pels, JNI_COMMIT);
  }

  env->CallVoidMethod( producer, setPix, 0, 0, img->width, img->height, model, pelArray, 0, img->width);
  env->CallVoidMethod( producer, imgCompl, 3); /* 3 = STATICIMAGEDONE */
}



/**
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
/* XXX:
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
	// img->qpmScaled = QPixmap();
	// scale();
	success = TRUE;
    } else {
	img->qpm->resize(0,0);				// couldn't load image
    }
// XXX:
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

  DBG( AWT_IMG, qqDebug("imgCreateFromFile file=%s\n",fn));
  QImage *image = new QImage();
  bool ok = FALSE;
#if 0  
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
  DBG( AWT_IMG, qqDebug("imgCreateFromData\n"));
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
  DBG(AWT_IMG, qqDebug("imgSetFrame\n"));
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


/**
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

