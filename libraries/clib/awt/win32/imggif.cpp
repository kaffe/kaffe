/**
* imggif.cpp - interface for libgif input routines
*
* Copyright (c) 1999
*      Transvirtual Technologies, Inc.  All rights reserved.
*
* See the file "license.terms" for information on usage and redistribution 
* of this file. 
*/

#include "toolkit.hpp"

#if defined(HAVE_GIF_LIB_H) && (defined(HAVE_LIBGIF) || defined(HAVE_LIBUNGIF))
#define INCLUDE_GIF 1
#endif

extern "C" {
	
#if defined(INCLUDE_GIF)
#undef DrawText	//winuser defined
#include "gif_lib.h"
#endif
	
	static int iOffset[] = { 0, 4, 2, 1 };
	static int iJumps[] = { 8, 8, 4, 2 };
	
	Java_java_awt_Toolkit_imgFreeImage( JNIEnv* X, jclass clazz, Image* img);
	
	/**************************************************************************************
	* internal functions
	*/
	
#if defined(INCLUDE_GIF)
	void
		writeRow ( Image* img, GifPixelType* rowBuf, GifColorType* cm, int row )
	{
		int             idx, col;
		unsigned long   pix = 0;
		
		for ( col=0; col < img->width; col++ ) {
			idx = rowBuf[col];
			if ( (img->mask) && (idx == img->trans) ) {
				/* set mask WHITE and image BLACK */
				PIXEL( img->dcMask, col, row, 0x00ffffff);
				PIXEL( img->dc, col, row, 0);
			}
			else {
				pix = (cm[idx].Blue << 16) | (cm[idx].Green << 8) | (cm[idx].Red);
				PIXEL( img->dc, col, row, pix);
			}
		}
	}
	
	
#define CHECK(gifOp) \
	if ( gifOp == GIF_ERROR ) { \
	if ( img )    Java_java_awt_Toolkit_imgFreeImage( 0, 0, img); \
    if ( rowBuf ) AWT_FREE( rowBuf); \
    return 0; \
	}
	
	
	Image*
		readGif ( GifFileType *gf )
	{
		Image           *firstImg = 0, *img;
		int             i, extCode, width, height, row, cmapSize;
		int			  trans = -1, nFrames = 0, delay = 0;
		GifRecordType   rec;
		GifByteType     *ext;
		ColorMapObject  *cmap;
		GifColorType    *clrs;
		GifPixelType    *rowBuf = (GifPixelType*) AWT_MALLOC( gf->SWidth * sizeof( GifPixelType) );
		
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
				
				/*
				* create our image objects and keep track of frames 
				*/
				if ( !firstImg ) {     /* this is the first (maybe only) frame */
					firstImg = img = createImage( width, height);
				}
				else {                 /* this is a subsequent gif-movie frame, link it in */
					img->next = createImage( width, height);
					img = img->next;
				}
				
				/*
				* The trans index might have been cached by a preceeding extension record. Now
				* that we have the Image object, it's time to store it in img and to create the
				* mask
				*/
				if ( trans != -1 ) {
					img->trans = trans;
					createMaskImage( X, img);
					trans = -1;
				}
				
				if ( delay < 100 ) {
					delay = 100;
				}

				img->latency = delay;
				img->left = gf->Image.Left;
				img->top = gf->Image.Top;
				img->frame = nFrames;
				
				nFrames++;
				
				
				/*
				* start reading in the image data
				*/
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
						trans = ext[4];
						delay = ((ext[3] << 8) | ext[2]) * 10; /* delay in 1/100 secs */
					}
				}
				else if ( extCode == 0xff ) {  /* application extension block */
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

		if ( firstImg && (img != firstImg) ){
			img->next = firstImg;   /* link it together (as a ring) */
		}

		return firstImg;
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


/*
 * intercept all file io and map it to AWT_xx file io maros
 * (in order to obtain thread safety)
 */
typedef struct {
  int fd;
} FileSource;

int
readGifFileSource( GifFileType *gf, GifByteType* buf, int length )
{
  FileSource *psource = (FileSource*)gf->UserData;
  int n =  AWT_READ( psource->fd, buf, length);

  return (n < 0) ? 0 : n;
}

#endif /* INCLUDE_GIF */

/**************************************************************************************
* these are the "exported" production interface functions
*/

Image*
readGifFile ( int infile )
{
	Image          *img = 0;
	
#if defined(INCLUDE_GIF)
  /*
   * we don't use DGifOpenFile because file io might be intercepted
   * (because of threading)
   */
  FileSource     fileSrc;
  GifFileType    *gf;

  fileSrc.fd = infile;

  if ( !(gf = DGifOpen( &fileSrc, readGifFileSource)) )
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
#if defined(INCLUDE_GIF)
	BufferSource   bufSrc;
	GifFileType    *gf;
	
	bufSrc.buf = bufSrc.p = buf;
	bufSrc.remain = len;
	
	if ( !(gf = DGifOpen( &bufSrc, readGifBuffer)) )
		return 0;
	
	img = readGif( gf);
	
	DGifCloseFile( gf);
	
#endif /* INCLUDE_GIF */
	
	return img;
}

}
