/**
 * imggif.c - interface for libgif input routines
 *
 * Copyright (c) 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#include "defs.h"
#include "toolkit.h"

#include "qimage.h"

#if defined(HAVE_GIF_LIB_H) && (defined(HAVE_LIBGIF) || defined(HAVE_LIBUNGIF))
#define INCLUDE_GIF 1
#endif

#if defined(INCLUDE_GIF)
BEGIN_C_DECLS

#include "gif_lib.h"

END_C_DECLS
#endif


#if defined(INCLUDE_GIF)
static int iOffset[] = { 0, 4, 2, 1 };
static int iJumps[] = { 8, 8, 4, 2 };
#endif

int colordepth;
char *s;
/**************************************************************************************
 * internal functions
 */

#if defined(INCLUDE_GIF)
static void
writeRow ( Image* img, GifPixelType* rowBuf, GifColorType* cm, int row )
{
  int             idx, col;
  jint            rgb;
  unsigned long   pix;
  unsigned char   *imgData;
  imgData = img->qImg->scanLine(row);
  //fprintf(stderr,"img w=%d h=%d imgData %p\n",img->qImg->width(),img->qImg->height(),imgData);
//  memset(imgData,(char *)rowBuf,img->width);
  for ( col=0; col < img->width; col++ ) {
	idx = rowBuf[col];
	if ( img->qImg_AlphaMask/*img->xMask*/ && (idx == img->trans) ) {
	  pix = 0;
	  //fprintf(stderr,"alpha %d %d\n",col,row);
          img->qImg_AlphaMask->setPixel((int)col,(int)row,(uint)0);
	  //XPutPixel( img->xMask, col, row, 0);
	}
	else {
	  rgb = (cm[idx].Red << 16) | (cm[idx].Green << 8) | (cm[idx].Blue);
//	  pix = pixelValue( X, rgb);
	}
	//fprintf(stderr,"setting pixel %d %d %d\n",col,row,rgb);
	img->qImg->setPixel((int)col,(int)row,idx);	
//	XPutPixel( img->xImg, col, row, pix);
  }
}


#define CHECK(gifOp) \
  if ( gifOp == GIF_ERROR ) { \
    if ( img )    Java_java_awt_Toolkit_imgFreeImage( 0, 0, img); \
    if ( rowBuf ) AWT_FREE( rowBuf); \
    return 0; \
  }


static Image*
readGif ( GifFileType *gf )
{
  Image*          firstImg = 0;
  Image*	  img = 0;
  int             i, extCode, width, height, row, cmapSize;
  int             trans = -1, nFrames = 0, delay = 0;
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
		img->qImg = new QImage(width,height,colordepth);
		if(img->qImg->load(s)==FALSE) fprintf(stderr,"load image %s failed\n",s);
	  }
	  else {                 /* this is a subsequent gif-movie frame, link it in */
		img->next = createImage( width, height);
		img->next->qImg = new QImage(width,height,colordepth);
		if(img->next->qImg->load(s)==FALSE) fprintf(stderr,"load image %s failed\n",s);
		bool ok = FALSE;
		ok = reconvertImage(img);
		if (ok == FALSE) img->qpm->resize(0,0);
		img = img->next;
	  }

	  /*
	   * The trans index might have been cached by a preceeding extension record. Now
	   * that we have the Image object, it's time to store it in img and to create the
	   * mask
	   */
	  if ( trans != -1 ) {
		img->trans = trans;
		createXMaskImage( X, img);
		img->qImg->setAlphaBuffer(TRUE);
		img->qImg_AlphaMask = new QImage();
		if(img->qImg && img->qImg->hasAlphaBuffer());
		*(img->qImg_AlphaMask) = img->qImg->createAlphaMask();
		trans = -1;
	  }

	  /*
	   * Some browsers seem to assume minimal values, and some animations
	   * seem to rely on this. But there's no safe guess, so we
	   * skip it completely
	   */
/*
	  if ( delay == 0 )
		delay = 1000;
	  else if ( delay < 100 )
		delay = 100;
*/

	  img->latency = delay;
	  img->left = gf->Image.Left;
	  img->top = gf->Image.Top;
	  img->frame = nFrames;

	  nFrames++;
	  createXImage( X, img);
	  img->qImg = new QImage();
	  if(img->qImg->load(s) == FALSE) fprintf(stderr,"load %s failed",s);
	  /*
	   * start reading in the image data
	   */
	  if ( gf->Image.Interlace ) {
		/* Need to perform 4 passes on the images: */
		for ( i = 0; i < 4; i++ ) {
		  for (row = iOffset[i]; row < height; row += iJumps[i]) {
			memset( rowBuf, gf->SBackGroundColor, width);
			memset(img->qImg->scanLine(row),gf->SBackGroundColor,width);
			CHECK( DGifGetLine( gf, rowBuf, width));
			writeRow( img, rowBuf, clrs, row);
		  }
		}
	  }
	  else {
		for ( row = 0; row < height; row++) {
		  memset( rowBuf, gf->SBackGroundColor, width);
		  CHECK( DGifGetLine(gf, rowBuf, width));
		  memset (img->qImg->scanLine(row),gf->SBackGroundColor,width);
		  writeRow( img, rowBuf, clrs, row);
		}
	  }
	  break;

	case EXTENSION_RECORD_TYPE:
	  CHECK( DGifGetExtension( gf, &extCode, &ext));

	  if ( extCode == 0xf9 ) {   /* graphics extension */
		/*
		 * extension record with transparency spec are preceeding description records
		 * (which create new Images), so just cache the transp index, here
		 */
		if ( ext[1] & 1 ) {      /* transparent index following */
		  trans = ext[4];
		}
		delay = ((ext[3] << 8) | ext[2]) * 10; /* delay in 1/100 secs */
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
  //fprintf(stderr,"total frame = %d\n",nFrames);
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

static int
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

static int
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
readGifFile ( int infile, char *fn )
{
  Image          *img = 0;

#if defined(INCLUDE_GIF)

  /*
   * we don't use DGifOpenFile because file io might be intercepted
   * (because of threading)
   */
  FileSource     fileSrc;
  GifFileType    *gf;
  s=fn;
  QImage *tmp2 = new QImage(fn);
  colordepth = tmp2->depth();
  //fprintf(stderr,"colordepth %d %d %d\n",colordepth,tmp2->width(),tmp2->height());
  delete tmp2;
  fileSrc.fd = infile;

  if ( !(gf = DGifOpen( &fileSrc, readGifFileSource)) )
	{
	fprintf(stderr,"can't open gif file!!\n");
	return 0;
	}
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
  QByteArray a;
  a.setRawData((char *)buf,sizeof(buf));
  QImage *tmp2 = new QImage(a);
  colordepth = tmp2->depth();
  delete tmp2;
  bufSrc.buf = bufSrc.p = buf;
  bufSrc.remain = len;

  if ( !(gf = DGifOpen( &bufSrc, readGifBuffer)) )
	return 0;

  img = readGif( gf);

  DGifCloseFile( gf);
  
#endif /* INCLUDE_GIF */

  return img;
}
