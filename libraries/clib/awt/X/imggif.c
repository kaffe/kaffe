/**
 * imggif.c - interface for libgif input routines
 *
 * Copyright (c) 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#include <stdio.h>
#include "config.h"

#if defined(HAVE_GIF_LIB_H)
#include "gif_lib.h"
#endif

#include "toolkit.h"

#if defined(HAVE_GIF_LIB_H)
static int iOffset[] = { 0, 4, 2, 1 };
static int iJumps[] = { 8, 8, 4, 2 };
#endif

/* references to img.c */
Image* createImage ( int width, int height );
void Java_java_awt_Toolkit_imgFreeImage( JNIEnv* env, jclass clazz, Image * img);
XImage* createXImage ( Toolkit* X, int width, int height );
XImage* createXMaskImage ( Toolkit* X, int width, int height );

/**************************************************************************************
 * internal functions
 */

#if defined(HAVE_GIF_LIB_H)
void
writeRow ( Image* img, GifPixelType* rowBuf, GifColorType* cm, int row )
{
  int             idx, col;
  jint            rgb;
  unsigned long   pix;

  for ( col=0; col < img->width; col++ ) {
	idx = rowBuf[col];
	if ( img->xMask && (idx == img->trans) ) {
	  pix = 0;
	  XPutPixel( img->xMask, col, row, 0);
	}
	else {
	  rgb = (cm[idx].Red << 16) | (cm[idx].Green << 8) | (cm[idx].Blue);
	  pix = pixelValue( X, rgb);
	}
	XPutPixel( img->xImg, col, row, pix);
  }
}


#define CHECK(gifOp) \
  if ( gifOp == GIF_ERROR ) { \
    if ( img )    Java_java_awt_Toolkit_imgFreeImage( 0, 0, img); \
    if ( rowBuf ) free( rowBuf); \
    return 0; \
  }


Image*
readGif ( GifFileType *gf )
{
  Image           *img = 0;
  int             i, extCode, width, height, row, col, cmapSize;
  GifRecordType   rec;
  GifByteType     *ext;
  ColorMapObject  *cmap;
  GifColorType    *clrs;
  GifPixelType    *rowBuf = (GifPixelType*) malloc( gf->SWidth * sizeof( GifPixelType) );

  img = createImage( gf->SWidth, gf->SHeight);
  img->xImg = createXImage( X, gf->SWidth, gf->SHeight);

  do {
	CHECK( DGifGetRecordType( gf, &rec));

	switch ( rec ) {
	case IMAGE_DESC_RECORD_TYPE:
	  CHECK( DGifGetImageDesc( gf));

	  width    = gf->Image.Width;
	  height   = gf->Image.Height;

      cmap     = (gf->Image.ColorMap) ? gf->Image.ColorMap : gf->SColorMap;
	  clrs     = cmap->Colors;
      cmapSize = cmap->ColorCount;

	  if ( gf->Image.Interlace ) {
		/* Need to perform 4 passes on the images: */
		for ( i = 0; i < 4; i++ ) {
		  for (row = iOffset[i]; row < height; row += iJumps[i]) {
			memset( rowBuf, gf->SBackGroundColor, width);
			CHECK( DGifGetLine( gf, rowBuf, width));

			writeRow( img, rowBuf, clrs, row);
		  }
		}
	  }
	  else {
		for ( row = 0; row < height; row++) {
		  memset( rowBuf, gf->SBackGroundColor, width);
		  CHECK( DGifGetLine(gf, rowBuf, width));

		  writeRow( img, rowBuf, clrs, row);
		}
	  }
	  break;

	case EXTENSION_RECORD_TYPE:
	  CHECK( DGifGetExtension( gf, &extCode, &ext));

	  if ( extCode == 0xf9 ) {   /* graphics extension */
		if ( ext[1] & 1 ) {      /* transparent index following */
		  img->trans = ext[4];
		  img->xMask = createXMaskImage( X, img->width, img->height);
		}
	  }

	  while ( ext != NULL ) {
		CHECK( DGifGetExtensionNext( gf, &ext));
	  }
	  break;

	case TERMINATE_RECORD_TYPE:
	  break;

	default:                /* Should be traps by DGifGetRecordType. */
	  break;
	}
  } while ( rec != TERMINATE_RECORD_TYPE );

  return img;
}

/**************************************************************************************
 * memory buffer IO
 */

typedef struct {
  unsigned char *buf;
  unsigned char *p;
  long          remain;
} BufferSource;

int
readGifBuffer ( GifFileType *gf, GifByteType* buf, int length )
{
  BufferSource *psource = (BufferSource*)gf->UserData;

  if ( psource && (psource->remain >= length) ) {
	memcpy( buf, psource->p, length);
	psource->p += length;
	psource->remain -= length;

	return length;
  }
  else {
	return 0;
  }
}

#endif /* HAVE_GIF_LIB_H */

/**************************************************************************************
 * these are the "exported" production interface functions
 */

Image*
readGifFile ( FILE* infile )
{
  Image          *img = 0;

#if defined(HAVE_GIF_LIB_H)
  GifFileType    *gf;

  if ( !(gf = DGifOpenFileHandle( fileno( infile))) )
	return 0;

  img = readGif( gf);

  DGifCloseFile( gf);
#endif

  return img;
}


Image*
readGifData ( unsigned char* buf, long len )
{
  Image          *img = 0;
#if defined(HAVE_GIF_LIB_H)
  BufferSource   bufSrc;
  GifFileType    *gf;

  /*
   * we don't have a enhanced GIF lib (capable of alternate input methods), backup
   * to a plain old temp file
   */
  char  *tmp = "tmp.gif";
  FILE  *tmpFile = fopen( tmp, "w+");

  fwrite( buf, len, 1, tmpFile);
  fflush( tmpFile);
  rewind( tmpFile);

  img = readGifFile( tmpFile);
  fclose( tmpFile);
  remove( tmp);
#endif /* HAVE_GIF_LIB_H */

  return img;
}
