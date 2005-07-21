/**
 * toolkit.h - 
 *
 * Copyright (c) 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#ifndef __toolkit_h
#define __toolkit_h

#include "config.h"
#include "config-std.h"
#include "config-mem.h"
#include "config-setjmp.h"

#include "jcl.h"
#undef DBG

#include <X11/Xlib.h>
#include <X11/Xutil.h>

// #if defined(HAVE_LIBXEXT) && defined(HAVE_SYS_IPC_H) && defined(HAVE_SYS_SHM_H) && defined(HAVE_X11_EXTENSIONS_XSHM_H)
// #define USE_XSHM_EXTENSION 1
#include <sys/ipc.h>
#include <sys/shm.h>
#if defined(HAVE_LIBXEXT)
#if defined(HAVE_X11_EXTENSIONS_XSHM_H)
#include <X11/extensions/XShm.h>
#endif /* defined(HAVE_X11_EXTENSIONS_XSHM_H) */
#else
// #undef USE_XSHM_EXTENSION
#define	XShmGetImage(A,B,C,D,E,F)		0
#define	XShmPutImage(A,B,C,D,E,F,G,H,I,J,K)	0
#define	XShmSegmentInfo				void
#endif

#include <jni.h>

#include "gtypes.h"
#include "gc.h"
#include "thread.h"
#include "debug.h"

#define	DBG_ACTION(A,B)

#ifdef HAVE_FCNTL_H
#include <fcntl.h>
#endif

#define KAFFE_I18N	1 /* make Kaffe to be able to support i18n */

/*******************************************************************************
 * color conversion structures
 */

#define N_DIRECT 256

typedef struct _Rgb2Direct {
  unsigned char red[N_DIRECT];
  unsigned char redPix[N_DIRECT];
  int           redShift;
  int           nRed;
  unsigned char green[N_DIRECT];
  unsigned char greenPix[N_DIRECT];  
  int           greenShift;
  int           nGreen;
  unsigned char blue[N_DIRECT];
  unsigned char bluePix[N_DIRECT];
  int           blueShift;
  int           nBlue;
} Rgb2Direct;

typedef struct _Rgb2True {
  unsigned int   redMask;
  unsigned int   greenMask;
  unsigned int   blueMask;
  int            blueShift;
  int            redShift;
  int            greenShift;
} Rgb2True;


typedef struct _RgbColor {
  unsigned char r;
  unsigned char g;
  unsigned char b;
} RgbColor;


typedef struct _Rgb2Pseudo {
  RgbColor       rgb[256];
  unsigned char  pix[8][8][8];
} Rgb2Pseudo;


/*******************************************************************************
 * image handling structures
 */

typedef struct _AlphaImage {       /* storage for full alpha channel images */
  unsigned char *buf;
  int           width, height;
} AlphaImage;

#define NO_SHM       0             /* we don't have MIT-Shm support in the X server */
#define USE_SHM      1             /* we have support, use it */
#define SUSPEND_SHM  2             /* we have support, but it ran out of space */

typedef struct _Image {
  Pixmap           pix;            /* pixmap for screen images */

  XImage           *xImg;          /* "real" image */
  XShmSegmentInfo  *shmiImg;       /* Shm info for shared mem real image */

  XImage           *xMask;         /* mask image for reduced alpha (on/off) images */
  XShmSegmentInfo  *shmiMask;      /* Shm info for shared mem mask image */

  AlphaImage       *alpha;         /* full alpha channel (for alpha != 0x00 or 0xff) */

  int              trans;          /* transparent index */
  int              left, top;      /* some GIF movies require drawing offsets */
  int              width, height;  /* we need this in case we are a pixmap */

  short            latency;        /* between image flips, for "gif-movies" */
  short            frame;          /* frame number for animations */
  struct _Image    *next;          /* next movie-frame */
} Image;


/*******************************************************************************
 * structure to store guessed and computed Frame/Dialog insets (titlebar, borders)
 */

typedef struct _DecoInset {
  int            left;
  int            top;
  int            right;
  int            bottom;
  char           guess;
} DecoInset;           


/*******************************************************************************
 * We use  our own structure instead of directly storing X window handles, to
 * enable us to attach and query attributes (owners, states, flags..) to particular
 * X window instances. We could do this by means of X properties, but this might
 * end up in costly round-trips and even more memory (than we trade for speed here).
 * It's a must have for our popup key/focus event forwarding (see wnd.c)
 */

typedef struct _WindowRec {
  Window         w;
  unsigned       flags;
  Window         owner;
} WindowRec;

/*******************************************************************************
 * this is the master AWT structure (singleton object), glueing it al together
 */

typedef struct _Toolkit {
  Display        *dsp;
  Window         root;

  char           *buf;
  unsigned int   nBuf;

  int            colorMode;      /* refers to CM_xx constants, not X visuals */
  Rgb2True       *tclr;
  Rgb2Pseudo     *pclr;
  Rgb2Direct     *dclr;

  int            shm;
  int            shmThreshold;

  Cursor         cursors[14];

  DecoInset      frameInsets;
  DecoInset      dialogInsets;

  XEvent         event;
  char           preFetched;
  char           blocking;
  int            pending;
  int            evtId;
  fd_set         rfds;           /* used to select-check availability of X input (pthreads) */

  Window         lastWindow;
  int            srcIdx;
  WindowRec      *windows;
  int            nWindows;

  Window         cbdOwner;
  Window         wakeUp;

  Window         focus;     /* this is the real focus, if it is in our process */
  Window         focusFwd;  /* this might be a (owned) window we forward the focus to */
  int            fwdIdx;    /* cached index of the focus forward window */
#ifdef KAFFE_I18N
  XOM            xom;       /* X Output Method */
#endif  
} Toolkit;


/*******************************************************************************
 * global data def/decl
 */
#ifdef MAIN

Toolkit   XTk;
Toolkit   *X = &XTk;

Atom WM_PROTOCOLS;
Atom WM_DELETE_WINDOW;
Atom WM_TAKE_FOCUS;
Atom WAKEUP;
Atom RETRY_FOCUS;
Atom FORWARD_FOCUS;
Atom SELECTION_DATA;
Atom JAVA_OBJECT;

jclass  AWTError;
JNIEnv  *JniEnv;

#else

extern Toolkit* X;

extern Atom WM_PROTOCOLS;
extern Atom WM_DELETE_WINDOW;
extern Atom WM_TAKE_FOCUS;
extern Atom WAKEUP;
extern Atom RETRY_FOCUS;
extern Atom FORWARD_FOCUS;
extern Atom SELECTION_DATA;
extern Atom JAVA_OBJECT;

extern jclass AWTError;
extern JNIEnv* JniEnv;

#endif /* MAIN */

extern long StdEvents, PopupEvents;


/*****************************************************************************************
 * heap wrapper macros
 */

static inline void* _awt_malloc_wrapper ( size_t size )
{
  void *adr;
  enterUnsafeRegion();
  adr = malloc( size);
  leaveUnsafeRegion();
  DBG( AWT_MEM, printf("malloc: %zi -> %p\n", size, adr));
  return adr;
}

static inline void* _awt_calloc_wrapper ( size_t n, size_t size )
{
  void *adr;
  enterUnsafeRegion();
  adr = calloc( n, size);
  leaveUnsafeRegion();
  DBG( AWT_MEM, printf("calloc: %zi,%zi -> %p\n", n, size, adr));
  return adr;
}

static inline void _awt_free_wrapper ( void* adr )
{
  DBG( AWT_MEM, printf("free: %p\n", adr));
  enterUnsafeRegion();
  free( adr);
  leaveUnsafeRegion();
}


#define AWT_MALLOC(_n) \
  _awt_malloc_wrapper( _n)

#define AWT_CALLOC(_n,_sz) \
  _awt_calloc_wrapper( _n, _sz)

#define AWT_FREE(_adr) \
  _awt_free_wrapper( _adr);


/*******************************************************************************
 *
 */


static inline char* java2CString ( JNIEnv *env, Toolkit* tk, jstring jstr ) {
  jboolean isCopy;
  register unsigned int i;
  jsize n = (*env)->GetStringLength( env, jstr);
  const jchar    *jc = (*env)->GetStringChars( env, jstr, &isCopy);

  if ( n >= tk->nBuf ) {
	if ( tk->buf )
	  AWT_FREE( tk->buf);
	tk->buf = AWT_MALLOC( n+1);
	tk->nBuf = n+1;
  }

  for ( i=0; i<n; i++ ) tk->buf[i] = (char) jc[i];
  tk->buf[i] = 0;
  (*env)->ReleaseStringChars( env, jstr, jc);

  return tk->buf;
}

static inline char* jchar2CString ( Toolkit* tk, jchar* jc, size_t len ) {
  register size_t i;
  size_t  n = len+1;
  
  if ( n > tk->nBuf ) {
	if ( tk->buf )
	  AWT_FREE( tk->buf);
	tk->buf  = AWT_MALLOC( n);
	tk->nBuf = n;
  }

  for ( i=0; i<len; i++ ) tk->buf[i] = (char) jc[i];
  tk->buf[i] = 0;

  return tk->buf;
}

static inline void* getBuffer ( Toolkit* tk, unsigned int nBytes ) {
  if ( nBytes > tk->nBuf ) {
	if ( tk->buf )
	  AWT_FREE( tk->buf);
	tk->buf  = AWT_MALLOC( nBytes);
	tk->nBuf = nBytes;
  }
  return tk->buf;
}

#ifdef KAFFE_I18N
static inline wchar_t* jchar2wchar( const jchar *jc, int len ) {
  int i;
  wchar_t *wch = (wchar_t *)malloc(sizeof(wchar_t)*len);
  for( i=0; i < len; i++ ) wch[i] = jc[i];
  return wch;
}

static inline wchar_t* jbyte2wchar( jbyte *jc, int len ) {
  int i;
  wchar_t *wch = (wchar_t *)malloc(sizeof(wchar_t)*(len));
  for( i=0; i < len; i++ ) wch[i] = jc[i];
  return wch;
}
#endif

/*****************************************************************************************
 * color functions & defines
 */

/*
 * These are our directly supported visuals / color modes. Note that there is
 * no more 1-1 correspondence to X visuals, since we do a categorisation with
 * respect to our internal RGB <-> pixel conversion. All visuals not listed
 * explicitly are handled via the generic XAllocColor/XQueryColor (which might
 * slow down images considerably)
 *
 * NOTE: these values have to be != 0, since '0' is used to trigger color init
 */
#define CM_PSEUDO_256   1  /* PseudoColor visual */
#define CM_TRUE         2  /* general TrueColor visual */
#define CM_TRUE_888     3  /* special 8-8-8 bit TrueColor visual */
#define CM_DIRECT       4
#define CM_GENERIC      5  /* grays, DirectColor (packed) etc. */

int needsFullAlpha ( Toolkit* tk, Image *img, double threshold );

void initColorMapping ( JNIEnv* env, jclass clazz, Toolkit* tk);
jlong Java_java_awt_Toolkit_clrBright ( JNIEnv* env, jclass clazz, jint rgb );
jlong Java_java_awt_Toolkit_clrDark ( JNIEnv* env, jclass clazz, jint rgb );

#define JRGB(_r,_g,_b)  (_r<<16 | _g<<8 | _b)
#define JRED(_rgb)      ((_rgb & 0xff0000) >> 16)
#define JGREEN(_rgb)    ((_rgb & 0x00ff00) >> 8)
#define JBLUE(_rgb)     (_rgb & 0x0000ff)

#define D8   36.43  /* 255 / 7 */
#define D16  18.21

#define JI8(_v) (int)((_v + D16) / D8)
#define XI8(_v) (int)(((_v>>8) + D16) / D8)

#define ROUND_SHORT2CHAR(_n) \
  ((unsigned short)_n >= 0xff70 ) ? 0xff : (unsigned char)(((unsigned short)_n + 0x80) >> 8)

/* shift ops with negative values have undefined results */
#define SIGNED_RSHIFT(_var,_val,_shift)   \
   _var = _val;                           \
   if ( _shift > 0 )	 _var >>= _shift; \
   else                  _var <<= -_shift;

#define SIGNED_LSHIFT(_var,_val,_shift)   \
   _var = _val;                           \
   if ( _shift > 0 )	 _var <<= _shift; \
   else                  _var >>= -_shift;


static inline jint
pixelValue ( Toolkit* tk, jint rgb )
{
  int      r,g,b;
  XColor   xclr;

  switch ( tk->colorMode ) {
  case CM_PSEUDO_256:
	return tk->pclr->pix [JI8(JRED(rgb))] [JI8(JGREEN(rgb))] [JI8(JBLUE(rgb))];

  case CM_TRUE:
	SIGNED_RSHIFT( b, (rgb & tk->tclr->blueMask), tk->tclr->blueShift);
	SIGNED_RSHIFT( g, (rgb & tk->tclr->greenMask), tk->tclr->greenShift);
	SIGNED_RSHIFT( r, (rgb & tk->tclr->redMask), tk->tclr->redShift);
	return ( b | g | r );

  case CM_TRUE_888:
	return (rgb & 0xffffff);

  case CM_DIRECT:
	return (((jint)tk->dclr->redPix[JRED(rgb)]     << tk->dclr->redShift) |
			((jint)tk->dclr->greenPix[JGREEN(rgb)] << tk->dclr->greenShift) |
			((jint)tk->dclr->bluePix[JBLUE(rgb)]   << tk->dclr->blueShift));

  default:
	/*
	 * this is a generic fallback for "exotic" visuals and might be *awefully*
	 * slow (esp. for images) because XAllocColor is a roundtrip
	 */
	xclr.red = (rgb & 0xff0000) >> 8;
	xclr.green = (rgb & 0xff00);
	xclr.blue = (rgb & 0xff) << 8;
	xclr.flags = DoRed | DoGreen | DoBlue;
	XAllocColor( tk->dsp, DefaultColormapOfScreen( DefaultScreenOfDisplay( tk->dsp)), &xclr);
	return xclr.pixel;
  }
}

static inline void
rgbValues ( Toolkit* tk, unsigned long pixel, int* r, int* g, int* b )
{
  Visual         *v;
  XColor         xclr;

  switch ( tk->colorMode ) {
  case CM_PSEUDO_256:
	*r = tk->pclr->rgb[(unsigned char)pixel].r;
	*g = tk->pclr->rgb[(unsigned char)pixel].g;
	*b = tk->pclr->rgb[(unsigned char)pixel].b;
	break;

  case CM_TRUE:
	v = DefaultVisual( tk->dsp, DefaultScreen( tk->dsp));
	SIGNED_LSHIFT( *r, (pixel & v->red_mask), tk->tclr->redShift);
	SIGNED_LSHIFT( *g, (pixel & v->green_mask), tk->tclr->greenShift);
	SIGNED_LSHIFT( *b, (pixel & v->blue_mask), tk->tclr->blueShift);
	*r >>= 16;
	*g >>= 8;
	break;
/*
	*r = ((pixel & v->red_mask)   << tk->tclr->redShift)   >> 16;
	*g = ((pixel & v->green_mask) << tk->tclr->greenShift) >> 8;
	*b = ((pixel & v->blue_mask)  << tk->tclr->blueShift);
	break;
*/
  case CM_TRUE_888:
	*r = JRED( pixel);
	*g = JGREEN( pixel);
	*b = JBLUE( pixel);
	break;

  case CM_DIRECT:
	v = DefaultVisual( tk->dsp, DefaultScreen( tk->dsp));
	*r = tk->dclr->red[   ((pixel & v->red_mask) >> tk->dclr->redShift) ];
	*g = tk->dclr->green[ ((pixel & v->green_mask) >> tk->dclr->greenShift) ];
	*b = tk->dclr->blue[  ((pixel & v->blue_mask) >> tk->dclr->blueShift) ];
	break;

  default:
	/*
	 * this is a generic fallback for "exotic" visuals and might be *awefully*
	 * slow (esp. for images) because XAllocColor is a roundtrip
	 */
	xclr.pixel = pixel;
	XQueryColor( tk->dsp, DefaultColormapOfScreen( DefaultScreenOfDisplay( tk->dsp)), &xclr);
	*r = xclr.red >> 8;
	*g = xclr.green >> 8;
	*b = xclr.blue >> 8;

    break;
  }
}


/*****************************************************************************************
 * image functions
 */

Image* createImage ( int width, int height);
void createXMaskImage ( Toolkit* tk, Image* img );
void createXImage ( Toolkit* tk, Image* img );
void createAlphaImage ( Toolkit* tk, Image* img );
void initScaledImage ( Toolkit* tk, Image *tgt, Image *src,
					   int dx0, int dy0, int dx1, int dy1,
					   int sx0, int sy0, int sx1, int sy1 );


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

/*****************************************************************************************
 * clipboard functions
 */

jobject selectionClear ( JNIEnv* env, Toolkit* tk );
jobject selectionRequest ( JNIEnv* env, Toolkit* tk );


/*****************************************************************************************
 * async (multithreaded) macros
 * this can be used to solve the problem of deferred drawing requests, not being
 * flushed because of infrequent (non-polled) XNextEvent calls.
 * (for now, we go with a backgound flush thread)
 */

#define XFLUSH(_X,_force)



/*****************************************************************************************
 * file io wrapper macros (for image production)
 */

#define AWT_OPEN(_file)               open(_file, 0)
#define AWT_REWIND(_fd)               lseek(_fd, 0, SEEK_SET)
#define AWT_SETPOS(_fd,_off)          lseek(_fd, _off, SEEK_CUR)
#define AWT_READ(_fd,_buf,_count)     read(_fd,_buf,_count)
#define AWT_CLOSE(_fd)                close(_fd)


/*****************************************************************************************
 * macros to manage the source table (conversion of X windows to/from indices, which
 * are consistent with the AWTEvent.sources array)
 */

#define WND_FRAME      0x01
#define WND_WINDOW     0x02
#define WND_DIALOG     0x04

#define WND_MAPPED     0x08
#define WND_DESTROYED  0x10

static inline int getFreeSourceIdx ( Toolkit* tk, Window wnd ) {
  register int i, n;

  /*
   * we don't use a double hashing here because collisions are very unlikely
   * (window IDs usually already are hashed, so it does not make sense to
   * hash them again - we just could make it worse
   */
  for ( i = (unsigned long)wnd, n=0; n < tk->nWindows; i++, n++ ) {
	i %= tk->nWindows;
	if ( (int)(tk->windows[i].w) <= 0 ) {
	  tk->srcIdx = i;
	  tk->lastWindow = wnd;

	  return i;
	}
  }

  return -1;
}

static inline int getSourceIdx ( Toolkit* tk, Window w )
{
  int      n;
  register int i;

  if ( w == X->lastWindow ){
	return X->srcIdx;
  }
  else {
	for ( i = (unsigned long) w, n=0; n < tk->nWindows; i++, n++ ) {
	  i %= tk->nWindows;
	  if ( tk->windows[i].w == w ){
		tk->srcIdx = i;
		tk->lastWindow = w;
		return tk->srcIdx;
	  }
	  else if ( tk->windows[i].w == 0 ){
		return -1;
	  }
	}
	return -1;
  }
}

static inline int checkSource ( Toolkit* tk, int idx )
{
  return ( (idx >= 0) && (idx < tk->nWindows) && (tk->windows[idx].w) );
}


/*****************************************************************************************
 * focus forwarding
 */

#define FWD_SET    0  /* set focus forwarding */
#define FWD_CLEAR  1  /* reset focus forwarding */
#define FWD_REVERT 2  /* reset focus on owner */

static inline void resetFocusForwarding ( Toolkit* tk )
{
  tk->fwdIdx = -1;
  tk->focusFwd = 0;
}
extern void forwardFocus( int cmd, Window wnd );

/*****************************************************************************************
 * Macros to implement keyboard handling for owned windows (which don't ever get
 * the X focus). This is done by means of forwarding and generation of "artificial"
 * focus events (generated from clientMessages.
 */

#define	USE_POLLING_AWT	1

/* Clipboard */

typedef void ClipBoard;
extern jobject Java_java_awt_Toolkit_cbdInitClipboard ( JNIEnv* env, jclass clazz );
extern void Java_java_awt_Toolkit_cbdFreeClipboard ( JNIEnv* env, jclass clazz, ClipBoard* cbd );
extern jboolean Java_java_awt_Toolkit_cbdSetOwner ( JNIEnv* env, jclass clazz, ClipBoard* cbd );
extern jobject Java_java_awt_Toolkit_cbdGetContents ( JNIEnv* env, jclass clazz, ClipBoard* cbd );

/* Color */

extern jint Java_java_awt_Toolkit_clrGetPixelValue ( JNIEnv* env, jclass clazz, jint rgb );
extern void Java_java_awt_Toolkit_clrSetSystemColors ( JNIEnv* env, jclass clazz, jintArray sysClrs );
extern jobject Java_java_awt_Toolkit_clrGetColorModel ( JNIEnv* env, jclass clazz );

/* Event */

extern jobject Java_java_awt_Toolkit_evtInit ( JNIEnv* env, jclass clazz );
extern jobject Java_java_awt_Toolkit_evtGetNextEvent ( JNIEnv* env, jclass clazz );
extern jobject Java_java_awt_Toolkit_evtPeekEvent ( JNIEnv* env, jclass clazz );
extern jobject Java_java_awt_Toolkit_evtPeekEventId ( JNIEnv* env, jclass clazz, jint id );
extern void Java_java_awt_Toolkit_evtWakeup ( JNIEnv* env, jclass clazz );
extern void Java_java_awt_Toolkit_evtSendWMEvent ( JNIEnv* env, jclass clazz, jobject wmEvt );
extern jint Java_java_awt_Toolkit_evtRegisterSource ( JNIEnv* env, jclass clazz, jobject wnd );
extern jint Java_java_awt_Toolkit_evtUnregisterSource ( JNIEnv* env, jclass clazz, jobject wnd );

/* Font */
extern void* Java_java_awt_Toolkit_fntInitFont ( JNIEnv* env, jclass clazz, jstring jSpec, jint style, jint size );

#ifdef KAFFE_I18N
#define KAFFE_FONT_FUNC_DECL( ret, name, args... ) ret name( JNIEnv* env, jclass clazz, jobject  xoc , ## args )
#define UNVEIL_XOC(xoc)  ((XOC)JCL_GetRawData (env, xoc))
#else
#define KAFFE_FONT_FUNC_DECL( ret, name, args... ) ret name( JNIEnv* env, jclass clazz, jobject  fs , ## args )
#define UNVEIL_FS(fs)  ((XFontStruct *)JCL_GetRawData (env, fs))
#endif

extern KAFFE_FONT_FUNC_DECL( void, Java_java_awt_Toolkit_fntFreeFont );
extern KAFFE_FONT_FUNC_DECL( void *, Java_java_awt_Toolkit_fntInitFontMetrics );
extern KAFFE_FONT_FUNC_DECL( void, Java_java_awt_Toolkit_fntFreeFontMetrics );
extern KAFFE_FONT_FUNC_DECL( jint, Java_java_awt_Toolkit_fntGetAscent );
extern KAFFE_FONT_FUNC_DECL( jint, Java_java_awt_Toolkit_fntGetDescent );
extern KAFFE_FONT_FUNC_DECL( jint, Java_java_awt_Toolkit_fntGetFixedWidth );
extern KAFFE_FONT_FUNC_DECL( jint, Java_java_awt_Toolkit_fntGetHeight );
extern KAFFE_FONT_FUNC_DECL( jint, Java_java_awt_Toolkit_fntGetLeading );
extern KAFFE_FONT_FUNC_DECL( jint, Java_java_awt_Toolkit_fntGetMaxAdvance );
extern KAFFE_FONT_FUNC_DECL( jint, Java_java_awt_Toolkit_fntGetMaxAscent );
extern KAFFE_FONT_FUNC_DECL( jint, Java_java_awt_Toolkit_fntGetMaxDescent );
extern KAFFE_FONT_FUNC_DECL( jboolean, Java_java_awt_Toolkit_fntIsWideFont );
extern KAFFE_FONT_FUNC_DECL( jobject, Java_java_awt_Toolkit_fntGetWidths );
extern KAFFE_FONT_FUNC_DECL( jint, Java_java_awt_Toolkit_fntBytesWidth, jbyteArray jBytes, jint off, jint len );
extern KAFFE_FONT_FUNC_DECL( jint, Java_java_awt_Toolkit_fntCharWidth, jchar jChar );
extern KAFFE_FONT_FUNC_DECL( jint, Java_java_awt_Toolkit_fntCharsWidth, jcharArray jChars, jint off, jint len );
extern KAFFE_FONT_FUNC_DECL( jint, Java_java_awt_Toolkit_fntStringWidth, jstring jStr );

/* Graphics */

typedef struct {
  GC        gc;
  Drawable  drw;
#ifdef KAFFE_I18N
  XOC       oc;
#endif  
  int       fg;
  int       bg;
  char      xor;
  int       xclr;
  int       x0;
  int       y0;
} Graphics;

#define  WINDOW   0
#define  IMAGE    1
#define  GRAPHICS 2

#define KAFFE_GRA_FUNC_DECL( ret, name, args... ) ret name( JNIEnv* env, jclass clazz, jobject ngr , ## args )
#define UNVEIL_GR(ngr) ((Graphics *)JCL_GetRawData(env, ngr))

extern KAFFE_GRA_FUNC_DECL( jobject, Java_java_awt_Toolkit_graInitGraphics, jobject tgt, jint tgtType, jint xOff, jint yOff, jint xClip, jint yClip, jint wClip, jint hClip, jobject fnt, jint fg, jint bg, jboolean blank );
extern KAFFE_GRA_FUNC_DECL( void, Java_java_awt_Toolkit_graFreeGraphics );
extern KAFFE_GRA_FUNC_DECL( void, Java_java_awt_Toolkit_graCopyArea, jint x, jint y, jint width, jint height, jint xDelta, jint yDelta );
extern KAFFE_GRA_FUNC_DECL( void, Java_java_awt_Toolkit_graClearRect, jint x, jint y, jint width, jint height );
extern KAFFE_GRA_FUNC_DECL( void, Java_java_awt_Toolkit_graDrawBytes, jbyteArray jBytes, jint offset, jint len, jint x, jint y );
extern KAFFE_GRA_FUNC_DECL( void, Java_java_awt_Toolkit_graDrawChars, jcharArray jChars, jint offset, jint len, jint x, jint y );
extern KAFFE_GRA_FUNC_DECL( void, Java_java_awt_Toolkit_graDrawString, jstring str, jint x, jint y );
extern KAFFE_GRA_FUNC_DECL( void, Java_java_awt_Toolkit_graDrawLine, jint x1, jint y1, jint x2, jint y2 );
extern KAFFE_GRA_FUNC_DECL( void, Java_java_awt_Toolkit_graDrawArc, jint x, jint y, jint width, jint height, jint startAngle, jint arcAngle );
extern KAFFE_GRA_FUNC_DECL( void, Java_java_awt_Toolkit_graFillArc, jint x, jint y, jint width, jint height, jint startAngle, jint arcAngle );
extern KAFFE_GRA_FUNC_DECL( void, Java_java_awt_Toolkit_graDrawOval, jint x, jint y, jint width, jint height );
extern KAFFE_GRA_FUNC_DECL( void, Java_java_awt_Toolkit_graFillOval, jint x, jint y, jint width, jint height );
extern KAFFE_GRA_FUNC_DECL( void, Java_java_awt_Toolkit_graDrawPolygon, jintArray xPoints, jintArray yPoints, jint nPoints );
extern KAFFE_GRA_FUNC_DECL( void, Java_java_awt_Toolkit_graDrawPolyline, jintArray xPoints, jintArray yPoints, jint nPoints );
extern KAFFE_GRA_FUNC_DECL( void, Java_java_awt_Toolkit_graFillPolygon, jintArray xPoints, jintArray yPoints, jint nPoints );
extern KAFFE_GRA_FUNC_DECL( void, Java_java_awt_Toolkit_graDrawRect, jint x, jint y, jint width, jint height );
extern KAFFE_GRA_FUNC_DECL( void, Java_java_awt_Toolkit_graFillRect, jint x, jint y, jint width, jint height );
extern KAFFE_GRA_FUNC_DECL( void, Java_java_awt_Toolkit_graDrawRoundRect, jint x, jint y, jint width, jint height, jint wArc, jint hArc );
extern KAFFE_GRA_FUNC_DECL( void, Java_java_awt_Toolkit_graFillRoundRect, jint x, jint y, jint width, jint height, jint wArc, jint hArc );
extern KAFFE_GRA_FUNC_DECL( void, Java_java_awt_Toolkit_graDraw3DRect, jint x, jint y, jint width, jint height, jboolean raised, jint rgb );
extern KAFFE_GRA_FUNC_DECL( void, Java_java_awt_Toolkit_graFill3DRect, jint x, jint y, jint width, jint height, jboolean raised, jint rgb );
extern KAFFE_GRA_FUNC_DECL( void, Java_java_awt_Toolkit_graAddClip, jint xClip, jint yClip, jint wClip, jint hClip );
extern KAFFE_GRA_FUNC_DECL( void, Java_java_awt_Toolkit_graSetClip, jint xClip, jint yClip, jint wClip, jint hClip );
extern KAFFE_GRA_FUNC_DECL( void, Java_java_awt_Toolkit_graSetColor, jint clr );
extern KAFFE_GRA_FUNC_DECL( void, Java_java_awt_Toolkit_graSetBackColor, jint clr );
extern KAFFE_GRA_FUNC_DECL( void, Java_java_awt_Toolkit_graSetFont, jobject fnt );
extern KAFFE_GRA_FUNC_DECL( void, Java_java_awt_Toolkit_graSetOffset, jint xOff, jint yOff );
extern KAFFE_GRA_FUNC_DECL( void, Java_java_awt_Toolkit_graSetPaintMode );
extern KAFFE_GRA_FUNC_DECL( void, Java_java_awt_Toolkit_graSetXORMode, jint xorClr );
extern KAFFE_GRA_FUNC_DECL( void, Java_java_awt_Toolkit_graSetVisible, jint isVisible );
extern KAFFE_GRA_FUNC_DECL( void, Java_java_awt_Toolkit_graDrawImage, jobject img, jint srcX, jint srcY, jint dstX, jint dstY, jint width, jint height, jint bgval );
extern KAFFE_GRA_FUNC_DECL( void, Java_java_awt_Toolkit_graDrawImageScaled, jobject img, jint dx0, jint dy0, jint dx1, jint dy1, jint sx0, jint sy0, jint sx1, jint sy1, jint bgval );

  /* Image */
#define KAFFE_CREATE_IMG_FUNC_DECL( ret, name, args... ) ret name( JNIEnv* env, jclass clazz , ## args )
#define KAFFE_IMG_FUNC_DECL( ret, name, args... ) ret name( JNIEnv* env, jclass clazz, jobject nimg , ## args )
#define UNVEIL_IMG(nimg) ((Image *)JCL_GetRawData(env, nimg))

extern KAFFE_CREATE_IMG_FUNC_DECL( jobject, Java_java_awt_Toolkit_imgCreateImage, jint width, jint height );
extern KAFFE_CREATE_IMG_FUNC_DECL( jobject, Java_java_awt_Toolkit_imgCreateScreenImage, jint width, jint height );
extern KAFFE_IMG_FUNC_DECL( void, Java_java_awt_Toolkit_imgSetIdxPels, jint x, jint y, jint w, jint h, jintArray clrMap, jbyteArray idxPels, jint trans, jint off, jint scan );
extern KAFFE_IMG_FUNC_DECL( void, Java_java_awt_Toolkit_imgSetRGBPels, jint x, jint y, jint w, jint h, jintArray rgbPels, jint off, jint scan );
extern KAFFE_IMG_FUNC_DECL( void, Java_java_awt_Toolkit_imgComplete, jint status );
extern KAFFE_IMG_FUNC_DECL( void, Java_java_awt_Toolkit_imgFreeImage );
extern void imgFreeImage( Image *img );
extern KAFFE_IMG_FUNC_DECL( jobject, Java_java_awt_Toolkit_imgCreateScaledImage, int width, int height );
extern KAFFE_CREATE_IMG_FUNC_DECL( void, Java_java_awt_Toolkit_imgProduceImage, jobject producer, jobject nimg );
extern KAFFE_CREATE_IMG_FUNC_DECL( jobject, Java_java_awt_Toolkit_imgCreateFromFile, jstring fileName );
extern KAFFE_CREATE_IMG_FUNC_DECL( jobject, Java_java_awt_Toolkit_imgCreateFromData, jbyteArray jbuffer, jint off, jint len );
extern KAFFE_IMG_FUNC_DECL( jobject, Java_java_awt_Toolkit_imgSetFrame, int frameNo );
extern KAFFE_IMG_FUNC_DECL( jint, Java_java_awt_Toolkit_imgGetWidth );
extern KAFFE_IMG_FUNC_DECL( jint, Java_java_awt_Toolkit_imgGetHeight );
extern KAFFE_IMG_FUNC_DECL( jboolean, Java_java_awt_Toolkit_imgIsMultiFrame );
extern KAFFE_IMG_FUNC_DECL( jint, Java_java_awt_Toolkit_imgGetLatency );
extern KAFFE_IMG_FUNC_DECL( jobject, Java_java_awt_Toolkit_imgGetNextFrame );

/* Window */

#define KAFFE_GLBL_WND_FUNC_DECL( ret, name, args... ) ret name( JNIEnv* env, jclass clazz, ## args )
#define KAFFE_WND_FUNC_DECL( ret, name, args... ) ret name( JNIEnv* env, jclass clazz, jobject nwnd , ## args )
#define UNVEIL_WND(wnd) ((Window)JCL_GetRawData(env, wnd))

extern KAFFE_WND_FUNC_DECL( void, Java_java_awt_Toolkit_wndSetTitle, jstring jTitle );
extern KAFFE_WND_FUNC_DECL( void, Java_java_awt_Toolkit_wndSetResizable, jboolean isResizable, int x, int y, int width, int height );
extern KAFFE_GLBL_WND_FUNC_DECL( jobject, Java_java_awt_Toolkit_wndCreateFrame, jstring jTitle, jint x, jint y, jint width, jint height, jint jCursor, jint clrBack, jboolean isResizable );
extern KAFFE_GLBL_WND_FUNC_DECL( jobject, Java_java_awt_Toolkit_wndCreateWindow, jobject nowner, jint x, jint y, jint width, jint height, jint jCursor, jint clrBack );
extern KAFFE_GLBL_WND_FUNC_DECL( jobject, Java_java_awt_Toolkit_wndCreateDialog, jobject nowner, jstring jTitle, jint x, jint y, jint width, jint height, jint jCursor, jint clrBack, jboolean isResizable );
extern KAFFE_WND_FUNC_DECL( void, Java_java_awt_Toolkit_wndDestroyWindow );
extern KAFFE_WND_FUNC_DECL( void, Java_java_awt_Toolkit_wndRequestFocus );
extern KAFFE_GLBL_WND_FUNC_DECL( void, Java_java_awt_Toolkit_wndSetFrameInsets, jint top, jint left, jint bottom, jint right );
extern KAFFE_GLBL_WND_FUNC_DECL( void, Java_java_awt_Toolkit_wndSetDialogInsets, jint top, jint left, jint bottom, jint right );
extern KAFFE_WND_FUNC_DECL( void, Java_java_awt_Toolkit_wndSetBounds, jint x, jint y, jint width, jint height, jboolean isResizable );
extern KAFFE_WND_FUNC_DECL( void, Java_java_awt_Toolkit_wndRepaint, jint x, jint y, jint width, jint height );
extern KAFFE_WND_FUNC_DECL( void, Java_java_awt_Toolkit_wndSetIcon, jobject nimg );
extern KAFFE_WND_FUNC_DECL( void, Java_java_awt_Toolkit_wndSetVisible, jboolean showIt );
extern KAFFE_WND_FUNC_DECL( void, Java_java_awt_Toolkit_wndToBack );
extern KAFFE_WND_FUNC_DECL( void, Java_java_awt_Toolkit_wndToFront );
extern KAFFE_WND_FUNC_DECL( void, Java_java_awt_Toolkit_wndSetCursor, jint jCursor );

/* Toolkit */

#define KAFFE_TLK_FUNC_DECL( ret, name, args... ) ret name( JNIEnv* env, jclass clazz , ## args )

extern KAFFE_TLK_FUNC_DECL( jint, Java_java_awt_Toolkit_tlkProperties );
extern KAFFE_TLK_FUNC_DECL( jboolean, Java_java_awt_Toolkit_tlkInit, jstring name );
extern KAFFE_TLK_FUNC_DECL( void, Java_java_awt_Toolkit_tlkTerminate );
extern KAFFE_TLK_FUNC_DECL( jstring, Java_java_awt_Toolkit_tlkVersion );
extern KAFFE_TLK_FUNC_DECL( jint, Java_java_awt_Toolkit_tlkGetResolution );
extern KAFFE_TLK_FUNC_DECL( jint, Java_java_awt_Toolkit_tlkGetScreenHeight );
extern KAFFE_TLK_FUNC_DECL( jint, Java_java_awt_Toolkit_tlkGetScreenWidth );
extern KAFFE_TLK_FUNC_DECL( void, Java_java_awt_Toolkit_tlkSync );
extern KAFFE_TLK_FUNC_DECL( void, Java_java_awt_Toolkit_tlkFlush );
extern KAFFE_TLK_FUNC_DECL( void, Java_java_awt_Toolkit_tlkBeep );
extern KAFFE_TLK_FUNC_DECL( void, Java_java_awt_Toolkit_tlkDisplayBanner, jstring banner );

#endif
