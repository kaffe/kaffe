/**
* img.cpp - 
*
* Copyright (c) 1999
*      Transvirtual Technologies, Inc.  All rights reserved.
*
* See the file "license.terms" for information on usage and redistribution 
* of this file. 
*/

#include "toolkit.hpp"

/* interfaces of image conversion functions */

extern "C" {
	Image* readGifFile ( int );
	Image* readGifData ( unsigned char*, long len );
	Image* readJpegFile ( int );
	Image* readJpegData ( unsigned char*, long len );
	
}

/*
Image* readPngFile ( FILE* );
Image* readPngData ( unsigned char*, long len );
*/

/************************************************************************************
* own auxiliary funcs
*/

Image*
createImage ( int width, int height )
{
	Image * img = (Image*)AWT_CALLOC( 1, sizeof( Image));
	
	img->trans  = -1;     /* default to no alpha */
	
	img->width = width;
	img->height = height;
	
	img->dc = CreateCompatibleDC( 0);
	img->bmp = CreateCompatibleBitmap( X->display, img->width, img->height);
	
	SelectObject( img->dc, img->bmp);
	return img;
}

void
createMaskImage( Toolkit* X, Image* img) {
	
	if ( ! img->mask) {
		img->dcMask = CreateCompatibleDC( 0);


#if defined ( UNDER_95 )

		img->mask = CreateCompatibleBitmap( X->display, img->width, img->height);

#else

		/* for use of monochrome masks ( MaskBlt) */

		img->mask = CreateBitmap( img->width, img->height, 1, 1, NULL);
#endif	

		
		SelectObject( img->dcMask, img->mask);
		PatBlt( img->dcMask, 0, 0, img->width, img->height, BLACKNESS);
	}
}

void
freeImage( Toolkit* X, Image* img) {
	Image *next, *first = img;
	
	/* we have to be aware of image rings (GIF movies), iterate */
	do {
		DeleteObject( img->bmp);
		DeleteDC( img->dc);
		img->bmp = 0;
		img->dc = 0;
		if ( img->mask) {
			DeleteObject( img->mask);
			DeleteDC( img->dcMask);
			img->mask = 0;
			img->dcMask = 0;
		}
		next = img->next;
		AWT_FREE( img);
		img = next;
	} while ( (img != 0) && (img != first) );
}


/************************************************************************************
* exported native methods
*/

extern "C" {
	
	void*
		Java_java_awt_Toolkit_imgCreateImage ( JNIEnv* env, jclass clazz, jint width, jint height )
	{
		Image *img = createImage( width, height);
		
		
		return img;
	}
	
	
	void*
		Java_java_awt_Toolkit_imgCreateScreenImage ( JNIEnv* env, jclass clazz, jint width, jint height )
	{
		Image  *img = createImage( width, height);
		
		return img;
	}
	
	
	/* generic (ImageProducer) based image construction */
	
	void
		Java_java_awt_Toolkit_imgSetIdxPels ( JNIEnv* env, jclass clazz, Image * img,
		jint x, jint y, jint w, jint h,
		jintArray clrMap, jbyteArray idxPels, jint trans,
		jint off, jint scan)
	{
		register int    row, col;
		COLORREF		pix;
		jboolean        isCopy;
		jint            *clr = env->GetIntArrayElements( clrMap, &isCopy);
		jbyte           *pel = env->GetByteArrayElements( idxPels, &isCopy);
		unsigned char   *idx = (unsigned char*)(pel + off);
		int             maxCol = x + w;
		int             maxRow = y + h;
		unsigned char   curPel;
		
		if ( (trans >= 0) && !img->mask)
			createMaskImage( X, img);
		
		for ( row = y; row < maxRow; row++) {
			for ( col = x; col < maxCol; col++) {
				curPel = idx[col + row * scan];
				pix = ColorJ2N( clr[curPel] );
				if ( trans >= 0 ) {
					if ( curPel == trans ){
						pix = 0;
						PIXEL( img->dcMask, col, row, 0x00ffffff);
						
					}
					
				}
				PIXEL( img->dc, col, row, pix);
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
		register int    row, col;
		COLORREF		   pix;
		jboolean        isCopy;
		jint            *rgbs = env->GetIntArrayElements( rgbPels, &isCopy);
		jint            *rgb = rgbs + off;
		int             maxCol = x + w;
		int             maxRow = y + h;
		

		for ( row = y; row < maxRow; row++) {
			for ( col = x; col < maxCol; col++) {
				pix = rgb[col + row * scan];
				if ( (pix & 0xff000000) == 0xff000000 ) {					
					PIXEL( img->dc, col, row, pix);
				}
				else {
					if ( !img->mask )
						createMaskImage( X, img);
					PIXEL( img->dcMask, col, row, 0x00ffffff);
					PIXEL( img->dc, col, row, 0);					
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
		//if ( img->alpha &&  !needsFullAlpha( X, img, 0.0) )
		//	reduceAlpha( X, img, 128);
	}
	
	void
		Java_java_awt_Toolkit_imgFreeImage( JNIEnv* env, jclass clazz, Image * img)
	{
		freeImage( X, img);
	}	
	
	void*
		Java_java_awt_Toolkit_imgCreateScaledImage ( JNIEnv* env, jclass clazz,
		Image* img, int width, int height )
	{
		Image *si = createImage( width, height);
		si->bmp = CreateCompatibleBitmap( X->display, width, height);
		
		SelectObject( si->dc, si->bmp);
		StretchBlt( si->dc, 0, 0, width, height,
			img->dc, 0, 0, img->width, img->height, SRCCOPY);
		
		if ( img->mask) {
			createMaskImage( X, si);
			StretchBlt( si->dcMask, 0, 0, width, height,
				img->dcMask, 0, 0, img->width, img->height, SRCCOPY);
		}
		
		return si;
	}
	
	
	void
		Java_java_awt_Toolkit_imgProduceImage ( JNIEnv* env, jclass clazz, jobject producer, Image* img )
	{
		int       i, j;
		int       r, g, b;
		unsigned long pix;
		jboolean  isCopy;
		jclass    prodClazz  = env->GetObjectClass( producer);
		jclass    modelClazz = env->FindClass( "java/awt/image/ColorModel");
		
		jmethodID modelCtor = env->GetStaticMethodID( modelClazz, "getRGBdefault", "()Ljava/awt/image/ColorModel;");
		jmethodID setDim    = env->GetMethodID( prodClazz, "setDimensions", "(II)V");
		jmethodID setCM     = env->GetMethodID( prodClazz, "setColorModel", "(Ljava/awt/image/ColorModel;)V");
		jmethodID setHints  = env->GetMethodID( prodClazz, "setHints", "(I)V");
		jmethodID setPix    = env->GetMethodID( prodClazz, "setPixels", "(IIIILjava/awt/image/ColorModel;[III)V");
		jmethodID imgCompl  = env->GetMethodID( prodClazz, "imageComplete", "(I)V");
		
		jobject   model = env->CallStaticObjectMethod( modelClazz, modelCtor);
		
		/* for JDK compat, the pixel buffer has to be large enough to hold the *complete* image */
		jintArray pelArray  = env->NewIntArray( img->width * img->height);
		jint*     pels = env->GetIntArrayElements( pelArray, &isCopy);
		
		env->CallVoidMethod( producer, setDim, img->width, img->height);
		env->CallVoidMethod( producer, setCM, model);
		env->CallVoidMethod( producer, setHints, 6); /* TOPDOWNLEFTRIGHT | COMPLETESCANLINES */
		
		if ( img->bmp ) {
			for ( j=0; j<img->height; j++ ) {
				for ( i=0; i<img->width; i++ ) {
					if ( (img->mask == 0) || GetPixel( img->dcMask, i, j) ) {
						pix = GetPixel( img->dc, i, j);
						rgbValues( X, pix, &r, &g, &b);
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
		int   infile;
		char  *fn = java2CString( env, X, fileName);
		unsigned char  sig[SIG_LENGTH];
		
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
					//img = readPngFile( infile);
					break;
				default:
					img = unknownImage;
				}
			}
			AWT_CLOSE( infile);
		}
		
		return img;
	}
	
	void*
		Java_java_awt_Toolkit_imgCreateFromData ( JNIEnv* env, jclass clazz,
		jbyteArray jbuffer, jint off, jint len )
	{
		Image *img = 0;
		jboolean isCopy;
		jint   length = env->GetArrayLength( jbuffer);
		jbyte  *jb = env->GetByteArrayElements( jbuffer, &isCopy);
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
				//	  img = readPngData( buf, len);
				break;
			default:
				img = unknownImage;
			}
		}
		
		env->ReleaseByteArrayElements( jbuffer, jb, JNI_ABORT);
		return img;  
	}
	
	void*
		Java_java_awt_Toolkit_imgSetFrame ( JNIEnv* env, jclass clazz, Image* img, int frameNo )	
	{
		Image *imgCur = img;
		
		if ( !img->next )	
			return img;
		
		while ( img->frame != frameNo ) {	
			img = img->next;
			
			if ( img == imgCur )	
				break;	
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
		return ((jboolean)img->next != NULL );
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
}
