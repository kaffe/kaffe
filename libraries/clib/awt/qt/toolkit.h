/**
 * toolkit.h - 
 *
 * Copyright (c) 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * Copyright (c) 2002, 2003, 2004, 2005
 *	Kaffe.org contributors, see ChangeLog for details.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#ifndef __toolkit_h
#define __toolkit_h

#ifdef __cplusplus
#define HAVE_BOOL
#endif

#include "defs.h"

BEGIN_C_DECLS

#include "config.h"
#include "config-std.h"
#include "config-mem.h"
#include "config-setjmp.h"

#include <jni.h>
#include <native.h>
#include "gtypes.h"
#include "gc.h"
#include "thread.h"
#include "debug.h"

#include <fcntl.h>
#include <stdarg.h>

/**
 * image handling structures
 */
typedef struct _AlphaImage {       /* storage for full alpha channel images */
  unsigned char *buf;
  int           width, height;
} AlphaImage;

typedef struct _Image {
  
  QPixmap          *qpm;
  QPixmap	   *qpmScaled;
  QImage	   *qImg;
  QImage 	   *qImg_AlphaMask;

  AlphaImage       *alpha;         /* full alpha channel (for alpha != 0x00 or 0xff) */

  int              trans;          /* transparent index */
  int              left, top;      /* some GIF movies require drawing offsets */
  int              width, height;  /* we need this in case we are a pixmap */

  short            latency;        /* between image flips, for "gif-movies" */
  short            frame;          /* frame number for animations */
  struct _Image    *next;          /* next movie-frame */
} Image;


/**
 * structure to store guessed and computed Frame/Dialog insets (titlebar, borders)
 */
typedef struct _DecoInset {
  int            left;
  int            top;
  int            right;
  int            bottom;
  char           guess;
} DecoInset;           


/**
 * Kaffe uses its own structure instead of directly storing handles from window
 * system to enable attach and query attributes (owners, states, flags..) to 
 * particular window instances. We could do this by means of X properties, but 
 * this might end up in costly round-trips and even more memory (than we trade 
 * for speed here).
 * It's a must have for our popup key/focus event forwarding (see wnd.c)
 */
typedef struct _WindowRec {
  void*          w;
  unsigned       flags;
  void*          owner;
} WindowRec;

/**
 * this is the master AWT structure (singleton object), glueing it al together
 */
typedef struct _Toolkit {
  char           *buf;
  unsigned int   nBuf;

  int            colorMode;      /* refers to CM_xx constants, not X visuals */

  int            shm;
  int            shmThreshold;

  int            cursors[14];

  DecoInset      frameInsets;
  DecoInset      dialogInsets;

  char           preFetched;
  char           blocking;
  int            pending;
  int            evtId;
  fd_set         rfds;           /* used to select-check availability of X input (pthreads) */

  void*          lastWindow;
  int            srcIdx;
  WindowRec      *windows;
  int            nWindows;

  void*          cbdOwner;
  void*          wakeUp;

  void*          focus;     /* this is the real focus, if it is in our process */
  void*          focusFwd;  /* this might be a (owned) window we forward the focus to */
  int            fwdIdx;    /* cached index of the focus forward window */
} Toolkit;


/**
 * Exported JNI Functions
 */

/* Clipboard */
typedef void ClipBoard;
jobject Java_java_awt_Toolkit_cbdInitClipboard ( JNIEnv* env, jclass clazz );
void Java_java_awt_Toolkit_cbdFreeClipboard ( JNIEnv* env, jclass clazz,
  ClipBoard* cbd);
jboolean Java_java_awt_Toolkit_cbdSetOwner ( JNIEnv* env, jclass clazz,
  ClipBoard* cbd);
jobject Java_java_awt_Toolkit_cbdGetContents ( JNIEnv* env, jclass clazz,
  ClipBoard* cbd);

/* Color */
jint Java_java_awt_Toolkit_clrGetPixelValue ( JNIEnv* env, jclass clazz, jint rgb);
void Java_java_awt_Toolkit_clrSetSystemColors ( JNIEnv* env, jclass clazz,
  jintArray sysClrs);
jlong Java_java_awt_Toolkit_clrBright ( JNIEnv* env, jclass clazz, jint rgb);
jlong Java_java_awt_Toolkit_clrDark ( JNIEnv* env, jclass clazz, jint rgb);
jobject Java_java_awt_Toolkit_clrGetColorModel ( JNIEnv* env, jclass clazz);

/* Event */
jobject Java_java_awt_Toolkit_evtInit ( JNIEnv* env, jclass clazz);
jobject Java_java_awt_Toolkit_evtGetNextEvent ( JNIEnv* env, jclass clazz);
jobject Java_java_awt_Toolkit_evtPeekEvent ( JNIEnv* env, jclass clazz);
jobject Java_java_awt_Toolkit_evtPeekEventId ( JNIEnv* env, jclass clazz,
  jint id);
void Java_java_awt_Toolkit_evtWakeup ( JNIEnv* env, jclass clazz);
void Java_java_awt_Toolkit_evtSendWMEvent ( JNIEnv* env, jclass clazz,
  jobject wmEvt);
jint Java_java_awt_Toolkit_evtRegisterSource ( JNIEnv* env, jclass clazz,
  void* wnd);
jint Java_java_awt_Toolkit_evtUnregisterSource ( JNIEnv* env, jclass clazz,
  void* wnd);

/* Font */
class QFont;
void* Java_java_awt_Toolkit_fntInitFont ( JNIEnv* env, jclass clazz,
  jstring jSpec, jint style, jint size);
void Java_java_awt_Toolkit_fntFreeFont ( JNIEnv* env, jclass clazz,
  QFont* fs);
void* Java_java_awt_Toolkit_fntInitFontMetrics ( JNIEnv* env, jclass clazz,
  void* fs);
void Java_java_awt_Toolkit_fntFreeFontMetrics ( JNIEnv* env, jclass clazz,
  QFont* fs);
jint Java_java_awt_Toolkit_fntGetAscent ( JNIEnv* env, jclass clazz,
  QFont* fs);
jint Java_java_awt_Toolkit_fntGetDescent ( JNIEnv* env, jclass clazz,
  QFont* fs);
jint Java_java_awt_Toolkit_fntGetFixedWidth ( JNIEnv* env, jclass clazz,
  QFont* fs);
jint Java_java_awt_Toolkit_fntGetHeight ( JNIEnv* env, jclass clazz,
  QFont* fs);
jint Java_java_awt_Toolkit_fntGetLeading ( JNIEnv* env, jclass clazz,
  QFont* fs);
jint Java_java_awt_Toolkit_fntGetMaxAdvance ( JNIEnv* env, jclass clazz,
  QFont* fs);
jint Java_java_awt_Toolkit_fntGetMaxAscent ( JNIEnv* env, jclass clazz,
  QFont* fs);
jint Java_java_awt_Toolkit_fntGetMaxDescent ( JNIEnv* env, jclass clazz,
  QFont* fs);
jboolean Java_java_awt_Toolkit_fntIsWideFont ( JNIEnv* env, jclass clazz,
  QFont* fs);
jobject Java_java_awt_Toolkit_fntGetWidths ( JNIEnv* env, jclass clazz,
  QFont* fs);
jint Java_java_awt_Toolkit_fntBytesWidth ( JNIEnv* env, jclass clazz,
  QFont* fs, jbyteArray jBytes, jint off, jint len);
jint Java_java_awt_Toolkit_fntCharWidth ( JNIEnv* env, jclass clazz,
  QFont* fs, jchar jChar);
jint Java_java_awt_Toolkit_fntCharsWidth ( JNIEnv* env, jclass clazz,
  QFont* fs, jcharArray jChars, jint off, jint len);
jint Java_java_awt_Toolkit_fntStringWidth ( JNIEnv* env, jclass clazz,
  QFont* fs, jstring jStr);

/* Graphics */
typedef struct {
  QPaintDevice   *drw;
  QPainter  *painter;
  int       fg;
  int       bg;
  char      xor_mode;
  int       xclr;
  int       x0;
  int       y0;
} Graphics;

void* Java_java_awt_Toolkit_graInitGraphics ( JNIEnv* env, jclass clazz,
  Graphics* gr, jobject tgt, jint tgtType, jint xOff, jint yOff,
  jint xClip, jint yClip, jint wClip, jint hClip, jobject fnt,
  jint fg, jint bg, jboolean blank);
void Java_java_awt_Toolkit_graFreeGraphics ( JNIEnv* env, jclass clazz,
  Graphics* gr);
void Java_java_awt_Toolkit_graCopyArea ( JNIEnv* env, jclass clazz,
  Graphics* gr, jint x, jint y, jint width, jint height,
  jint xDelta, jint yDelta);
void Java_java_awt_Toolkit_graClearRect ( JNIEnv* env, jclass clazz,
  Graphics* gr, jint x, jint y, jint width, jint height);
void Java_java_awt_Toolkit_graDrawBytes ( JNIEnv* env, jclass clazz,
  Graphics* gr, jbyteArray jBytes, jint offset, jint len, jint x, jint y);
void Java_java_awt_Toolkit_graDrawChars ( JNIEnv* env, jclass clazz,
  Graphics* gr, jcharArray jChars, jint offset, jint len, jint x, jint y);
void Java_java_awt_Toolkit_graDrawString ( JNIEnv* env, jclass clazz,
  Graphics* gr, jstring str, jint x, jint y);
void Java_java_awt_Toolkit_graDrawLine ( JNIEnv* env, jclass clazz,
  Graphics* gr, jint x1, jint y1, jint x2, jint y2);
void Java_java_awt_Toolkit_graDrawArc ( JNIEnv* env, jclass clazz,
  Graphics* gr, jint x, jint y, jint width, jint height,
  jint startAngle, jint arcAngle);
void Java_java_awt_Toolkit_graFillArc ( JNIEnv* env, jclass clazz,
  Graphics* gr, jint x, jint y, jint width, jint height,
  jint startAngle, jint arcAngle);
void Java_java_awt_Toolkit_graDrawOval ( JNIEnv* env, jclass clazz,
  Graphics* gr, jint x, jint y, jint width, jint height);
void Java_java_awt_Toolkit_graFillOval ( JNIEnv* env, jclass clazz,
  Graphics* gr, jint x, jint y, jint width, jint height);
void Java_java_awt_Toolkit_graDrawPolygon ( JNIEnv* env, jclass clazz,
  Graphics* gr, jintArray xPoints, jintArray yPoints, jint nPoints);
void Java_java_awt_Toolkit_graDrawPolyline ( JNIEnv* env, jclass clazz,
  Graphics* gr, jintArray xPoints, jintArray yPoints, jint nPoints);
void Java_java_awt_Toolkit_graFillPolygon ( JNIEnv* env, jclass clazz,
  Graphics* gr, jintArray xPoints, jintArray yPoints, jint nPoints);
void Java_java_awt_Toolkit_graDrawRect ( JNIEnv* env, jclass clazz,
  Graphics* gr, jint x, jint y, jint width, jint height);
void Java_java_awt_Toolkit_graFillRect ( JNIEnv* env, jclass clazz,
  Graphics* gr, jint x, jint y, jint width, jint height);
void Java_java_awt_Toolkit_graDrawRoundRect( JNIEnv* env, jclass clazz,
  Graphics* gr, jint x, jint y, jint width, jint height,
  jint wArc, jint hArc);
void Java_java_awt_Toolkit_graFillRoundRect( JNIEnv* env, jclass clazz,
  Graphics* gr, jint x, jint y, jint width, jint height,
  jint wArc, jint hArc);
void Java_java_awt_Toolkit_graDraw3DRect( JNIEnv* env, jclass clazz,
  Graphics* gr, jint x, jint y, jint width, jint height,
  jboolean raised, jint rgb);
void Java_java_awt_Toolkit_graFill3DRect( JNIEnv* env, jclass clazz,
  Graphics* gr, jint x, jint y, jint width, jint height,
  jboolean raised, jint rgb);
void Java_java_awt_Toolkit_graAddClip ( JNIEnv* env, jclass clazz,
  Graphics* gr, jint xClip, jint yClip, jint wClip, jint hClip);
void Java_java_awt_Toolkit_graSetClip ( JNIEnv* env, jclass clazz,
  Graphics* gr, jint xClip, jint yClip, jint wClip, jint hClip);
void Java_java_awt_Toolkit_graSetColor ( JNIEnv* env, jclass clazz,
  Graphics* gr, jint clr);
void Java_java_awt_Toolkit_graSetBackColor ( JNIEnv* env, jclass clazz,
  Graphics* gr, jint clr);
void Java_java_awt_Toolkit_graSetFont ( JNIEnv* env, jclass clazz,
  Graphics* gr, jobject fnt);
void Java_java_awt_Toolkit_graSetOffset ( JNIEnv* env, jclass clazz,
  Graphics* gr, jint xOff, jint yOff);
void Java_java_awt_Toolkit_graSetPaintMode ( JNIEnv* env, jclass clazz,
  Graphics* gr);
void Java_java_awt_Toolkit_graSetXORMode ( JNIEnv* env, jclass clazz,
  Graphics* gr, jint xorClr);
void Java_java_awt_Toolkit_graSetVisible ( JNIEnv* env, jclass clazz,
  Graphics* gr, jint isVisible);
void Java_java_awt_Toolkit_graDrawImage ( JNIEnv* env, jclass clazz,
  Graphics* gr, Image* img, jint srcX, jint srcY, jint dstX, jint dstY,
  jint width, jint height, jint bgval);
void Java_java_awt_Toolkit_graDrawImageScaled ( JNIEnv* env, jclass clazz,
  Graphics* gr, Image* img, jint dx0, jint dy0, jint dx1, jint dy1,
  jint sx0, jint sy0, jint sx1, jint sy1, jint bgval);


/* Image */
void* Java_java_awt_Toolkit_imgCreateImage ( JNIEnv* env, jclass clazz,
  jint width, jint height);
void* Java_java_awt_Toolkit_imgCreateScreenImage ( JNIEnv* env, jclass clazz,
  jint width, jint height);
void Java_java_awt_Toolkit_imgSetIdxPels ( JNIEnv* env, jclass clazz,
  Image * img, jint x, jint y, jint w, jint h, jintArray clrMap,
  jbyteArray idxPels, jint trans, jint off, jint scan);
void Java_java_awt_Toolkit_imgSetRGBPels ( JNIEnv* env, jclass clazz,
  Image * img, jint x, jint y, jint w, jint h, jintArray rgbPels, jint off,
  jint scan);
void Java_java_awt_Toolkit_imgComplete( JNIEnv* env, jclass clazz, Image * img,
  jint status);
void Java_java_awt_Toolkit_imgFreeImage( JNIEnv* env, jclass clazz, Image * img);
void *Java_java_awt_Toolkit_imgCreateScaledImage ( JNIEnv* env, jclass clazz,
  Image* img, int width, int height);
void Java_java_awt_Toolkit_imgProduceImage ( JNIEnv* env, jclass clazz,
  jobject producer, Image* img);
void* Java_java_awt_Toolkit_imgCreateFromFile ( JNIEnv* env, jclass clazz,
  jstring fileName);
void* Java_java_awt_Toolkit_imgCreateFromData ( JNIEnv* env, jclass clazz,
  jbyteArray jbuffer, jint off, jint len);
void* Java_java_awt_Toolkit_imgSetFrame ( JNIEnv* env, jclass clazz,
  Image* img, int frameNo);
jint Java_java_awt_Toolkit_imgGetWidth ( JNIEnv* env, jclass clazz, Image* img);
jint Java_java_awt_Toolkit_imgGetHeight ( JNIEnv* env, jclass clazz, Image* img);
jboolean Java_java_awt_Toolkit_imgIsMultiFrame ( JNIEnv* env, jclass clazz, Image* img);
jint Java_java_awt_Toolkit_imgGetLatency ( JNIEnv* env, jclass clazz, Image* img);
void* Java_java_awt_Toolkit_imgGetNextFrame ( JNIEnv* env, jclass clazz, Image* img);

/* Toolkit */
jint Java_java_awt_Toolkit_tlkProperties ( JNIEnv* env, jclass clazz);
jboolean Java_java_awt_Toolkit_tlkInit ( JNIEnv* env, jclass clazz,
  jstring name);
void Java_java_awt_Toolkit_tlkTerminate ( JNIEnv* env, jclass clazz);
jstring Java_java_awt_Toolkit_tlkVersion ( JNIEnv* env, jclass clazz);
jint Java_java_awt_Toolkit_tlkGetResolution ( JNIEnv* env, jclass clazz);
jint Java_java_awt_Toolkit_tlkGetScreenHeight ( JNIEnv* env, jclass clazz);
jint Java_java_awt_Toolkit_tlkGetScreenWidth ( JNIEnv* env, jclass clazz);
void Java_java_awt_Toolkit_tlkSync ( JNIEnv* env, jclass clazz);
void Java_java_awt_Toolkit_tlkFlush ( JNIEnv* env, jclass clazz);
void Java_java_awt_Toolkit_tlkBeep ( JNIEnv* env, jclass clazz);
void Java_java_awt_Toolkit_tlkDisplayBanner ( JNIEnv* env, jclass clazz,
  jstring banner);

/* Window */
void Java_java_awt_Toolkit_wndSetTitle ( JNIEnv* env, jclass clazz, void* wnd,
  jstring jTitle);
void Java_java_awt_Toolkit_wndSetResizable ( JNIEnv* env, jclass clazz,
  void* wnd, jboolean isResizable, int x, int y, int width, int height);
void* Java_java_awt_Toolkit_wndCreateFrame ( JNIEnv* env, jclass clazz,
  jstring jTitle, jint x, jint y, jint width, jint height,
  jint jCursor, jint clrBack, jboolean isResizable);
void* Java_java_awt_Toolkit_wndCreateWindow ( JNIEnv* env, jclass clazz,
  void* owner, jint x, jint y, jint width, jint height,
  jint jCursor, jint clrBack);
void* Java_java_awt_Toolkit_wndCreateDialog ( JNIEnv* env, jclass clazz,
  void* owner, jstring jTitle, jint x, jint y, jint width, jint height,
  jint jCursor, jint clrBack, jboolean isResizable);
void Java_java_awt_Toolkit_wndDestroyWindow ( JNIEnv* env, jclass clazz,
  void* wnd);
void Java_java_awt_Toolkit_wndRequestFocus ( JNIEnv* env, jclass clazz,
  void* wnd);
void Java_java_awt_Toolkit_wndSetFrameInsets ( JNIEnv* env, jclass clazz,
  jint top, jint left, jint bottom, jint right);
void Java_java_awt_Toolkit_wndSetDialogInsets ( JNIEnv* env, jclass clazz,
  jint top, jint left, jint bottom, jint right);
void Java_java_awt_Toolkit_wndSetBounds ( JNIEnv* env, jclass clazz,
  void* wnd, jint x, jint y, jint width, jint height, jboolean isResizable);
void Java_java_awt_Toolkit_wndRepaint ( JNIEnv* env, jclass clazz, void* wnd,
  jint x, jint y, jint width, jint height);
void Java_java_awt_Toolkit_wndSetIcon ( JNIEnv* env, jclass clazz, void* wnd,
  void* img);
void Java_java_awt_Toolkit_wndSetVisible ( JNIEnv* env, jclass clazz,
  void* wnd, jboolean showIt);
void Java_java_awt_Toolkit_wndToBack ( JNIEnv* env, jclass clazz, void* wnd);
void Java_java_awt_Toolkit_wndToFront ( JNIEnv* env, jclass clazz, void* wnd);
void Java_java_awt_Toolkit_wndSetCursor ( JNIEnv* env, jclass clazz,
  void* wnd, jint jCursor);

/**
 * global data def/decl
 */
#ifdef MAIN

Toolkit   XTk;
Toolkit   *X = &XTk;
jclass  AWTError;
JNIEnv  *JniEnv;

#else

extern Toolkit* X;
extern jclass AWTError;
extern JNIEnv* JniEnv;

#endif /* MAIN */

extern long StdEvents, PopupEvents;


/**
 * heap wrapper macros
 */
static inline void* _awt_malloc_wrapper ( size_t size )
{
  void *adr;
  enterUnsafeRegion();
  adr = malloc( size);
  leaveUnsafeRegion();
  DBG( AWT_MEM, printf("malloc: %ld  -> %p\n", (unsigned long) size, adr));
  return adr;
}

static inline void* _awt_calloc_wrapper ( int n, size_t size )
{
  void *adr;
  enterUnsafeRegion();
  adr = calloc( n, size);
  leaveUnsafeRegion();
  DBG( AWT_MEM, printf("calloc: %ld,%ld  -> %p\n", (unsigned long) n, (unsigned long) size, adr));
  return adr;
}

static inline void _awt_free_wrapper ( void* adr )
{
  DBG( AWT_MEM, printf("free: %p\n", adr));
  enterUnsafeRegion();
  free( adr);
  leaveUnsafeRegion();
}

#define qqDebug(format, args...) \
  do { \
    qDebug("[%s:%d] ", __FILE__, __LINE__); \
    qDebug(format, ## args); \
  } while (0)
 

#define AWT_MALLOC(_n) \
  _awt_malloc_wrapper( _n)

#define AWT_CALLOC(_n,_sz) \
  _awt_calloc_wrapper( _n, _sz)

#define AWT_FREE(_adr) \
  _awt_free_wrapper( _adr);


static inline char* java2CString ( JNIEnv *env, Toolkit* X, const jstring jstr ) {
  jboolean isCopy;
  register unsigned int i;
  unsigned int      n = env->GetStringLength(jstr);
  const jchar    *jc = env->GetStringChars(jstr, &isCopy);

  if ( n >= X->nBuf ) {
	if ( X->buf )
	  AWT_FREE( X->buf);
	X->buf = (char*)AWT_MALLOC( n+1);
	X->nBuf = n+1;
  }

  for ( i=0; i<n; i++ ) X->buf[i] = (char) jc[i];
  X->buf[i] = 0;
  env->ReleaseStringChars( jstr, jc);

  return X->buf;
}

static inline char* jchar2CString ( Toolkit* X, const jchar* jc, int len ) {
  register int i;
  unsigned int n = len+1;
  
  if ( n > X->nBuf ) {
	if ( X->buf )
	  AWT_FREE( X->buf);
	X->buf  = (char*)AWT_MALLOC( n);
	X->nBuf = n;
  }

  for ( i=0; i<len; i++ ) X->buf[i] = (char) jc[i];
  X->buf[i] = 0;

  return X->buf;
}

static inline void* getBuffer ( Toolkit* X, unsigned int nBytes ) {
  if ( nBytes > X->nBuf ) {
	if ( X->buf )
	  AWT_FREE( X->buf);
	X->buf  = (char*)AWT_MALLOC( nBytes);
	X->nBuf = nBytes;
  }
  return X->buf;
}


/**
 * color functions & defines
 */
void initColorMapping ( JNIEnv* env, jclass clazz, Toolkit* X);

#define JRGB(_r,_g,_b)  (_r<<16 | _g<<8 | (_b))
#define JRED(_rgb)      (((_rgb) & 0xff0000) >> 16)
#define JGREEN(_rgb)    (((_rgb) & 0x00ff00) >> 8)
#define JBLUE(_rgb)     ((_rgb) & 0x0000ff)


/**
 * image functions
 */
int needsFullAlpha(Toolkit*, Image*, double);
Image* createImage ( int width, int height);
void createXMaskImage ( Toolkit* X, Image* img );
void createXImage ( Toolkit* X, Image* img );
void createAlphaImage ( Toolkit* X, Image* img );
void initScaledImage ( Toolkit* X, Image *tgt, Image *src,
					   int dx0, int dy0, int dx1, int dy1,
					   int sx0, int sy0, int sx1, int sy1 );

bool reconvertImage(Image* img);

static inline void
PutAlpha ( AlphaImage* img, int col, int row, unsigned char alpha )
{
  img->buf[ row*img->width + col ] = alpha;
}

static inline int
GetAlpha ( AlphaImage* img, int col, int row )
{
  return img->buf[ row*img->width + col];
}

/**
 * clipboard functions
 */
jobject selectionClear ( JNIEnv* env, Toolkit* X );
jobject selectionRequest ( JNIEnv* env, Toolkit* X );


/**
 * file io wrapper macros (for image production)
 */
#define AWT_OPEN(_file)               open(_file, 0)
#define AWT_REWIND(_fd)               lseek(_fd, 0, SEEK_SET)
#define AWT_SETPOS(_fd,_off)          lseek(_fd, _off, SEEK_CUR)
#define AWT_READ(_fd,_buf,_count)     read(_fd,_buf,_count)
#define AWT_CLOSE(_fd)                close(_fd)


/**
 * macros to manage the source table (conversion of X windows to/from indices, which
 * are consistent with the AWTEvent.sources array)
 */
#define WND_FRAME      0x01
#define WND_WINDOW     0x02
#define WND_DIALOG     0x04

#define WND_MAPPED     0x08
#define WND_DESTROYED  0x10

static inline int getFreeSourceIdx ( Toolkit* X, void* wnd ) {
  register int i, n;

  /*
   * we don't use a double hashing here because collisions are very unlikely
   * (window IDs usually already are hashed, so it does not make sense to
   * hash them again - we just could make it worse
   */
  for ( i = (unsigned long)wnd, n=0; n < X->nWindows; i++, n++ ) {
	i %= X->nWindows;
	if ( (long)(X->windows[i].w) <= 0 ) {
	  X->srcIdx = i;
	  X->lastWindow = wnd;

	  return i;
	}
  }

  return 0xffffffff;
}

static inline int getSourceIdx ( Toolkit* X, void* w )
{
  int      n;
  register int i;

  if ( w == X->lastWindow ){
	return X->srcIdx;
  }
  else {
	for ( i = (unsigned long) w, n=0; n < X->nWindows; i++, n++ ) {
	  i %= X->nWindows;
	  if ( X->windows[i].w == w ){
		X->srcIdx = i;
		X->lastWindow = w;
		return X->srcIdx;
	  }
	  else if ( X->windows[i].w == 0 ){
		return 0xffffffff;
	  }
	}
	return 0xffffffff;
  }
}

static inline int checkSource ( Toolkit* X, int idx )
{
  return ( (idx >= 0) && (idx < X->nWindows) && (X->windows[idx].w) );
}


/**
 * focus forwarding
 */
#define FWD_SET    0  /* set focus forwarding */
#define FWD_CLEAR  1  /* reset focus forwarding */
#define FWD_REVERT 2  /* reset focus on owner */

static inline void resetFocusForwarding ( Toolkit* X )
{
  X->fwdIdx = 0xffffffff;
  X->focusFwd = 0;
}


/**
 * Macros to implement keyboard handling for owned windows (which don't ever get
 * the X focus). This is done by means of forwarding and generation of "artificial"
 * focus events (generated from clientMessages.)
 */

#define	USE_POLLING_AWT	1


END_C_DECLS

#endif
