/**
* toolkit.hpp - 
*
* Copyright (c) 1999
*      Transvirtual Technologies, Inc.  All rights reserved.
*
* See the file "license.terms" for information on usage and redistribution 
* of this file. 
*/

#ifndef __toolkit_h
#define __toolkit_h

#include "config.h"
#include "config-std.h"
#include "config-io.h"
#include "config-mem.h"
#include "jni.h"
#include <tchar.h>
#include "cmnwnd.hpp"

extern "C" {
#include "..\..\..\..\kaffe\kaffevm\gtypes.h"
#include "..\..\..\..\kaffe\kaffevm\locks.h"
#include "..\..\..\..\include\java_lang_Object.h"
#include "..\..\..\..\kaffe\kaffevm\gc.h"

#include "..\..\..\..\kaffe\kaffevm\debug.h"
}

extern "C" void kprintf( const char*, ...); 
// #define DBG(_topic,_msgSpec) printf _msgSpec
// #define DBG_ACTION(_topic,__action)


#define D_LOOP_MODE 1
extern "C" LRESULT WINAPI WndProc( HWND, UINT, WPARAM, LPARAM);

/*******************************************************************************
* image handling structures
*/

typedef struct _AlphaImage {       /* storage for full alpha channel images */
	unsigned char *buf;
	int           width, height;
} AlphaImage;


typedef struct _Image {
	int	          trans;          /* transparent index */
	int	          left, top;
	int           width, height;  /* we need this in case we are a pixmap */
	
	HBITMAP	      bmp;
	HBITMAP	      mask;
	
	HDC		      dcMask;
	HDC		      dc;
	
	int           latency;        /* between image flips, for "gif-movies" */
	int			  frame;
	struct _Image *next;          /* next movie-frame */
} Image;


/*******************************************************************************
* structure to store guessed and computed Frame/Dialog insets (titlebar, borders)
*/

typedef struct _DecoInset {
	int           left;
	int           top;
	int           right;
	int           bottom;
	char          guess;
} DecoInset;           


/*******************************************************************************
* this is the master AWT structure (singleton object), glueing it al together
*/

typedef struct _Toolkit {
	HDC	          display;
	TCHAR         *buf;
	unsigned int  nBuf;
	
	HCURSOR		  cursors[14];
	
	DecoInset     frameInsets;
	DecoInset     dialogInsets;
	
	KWnd          **windows;
	int           nWindows;
	
	HWND          lastHwnd;
	KWnd          *lastWnd;
	
	
	jobject		  jEvt;
	int			  dspTid;
	
	HWND          wakeUp;
} Toolkit;


/*******************************************************************************
* font struct
*/
typedef struct _Font {
	HFONT fnt;
	TEXTMETRIC fm;
} Font;


/*******************************************************************************
* global data def/decl
*/

#ifdef MAIN
Toolkit   XTk;
Toolkit   *X = &XTk;
jclass    Tlk;
JNIEnv    *JniEnv;
#else
extern Toolkit  *X;
extern jclass   Tlk;
extern JNIEnv   *JniEnv;
#endif


/*****************************************************************************************
* heap wrapper macros
*/

#undef malloc
#undef calloc
#undef free

static inline void* _awt_malloc_wrapper ( size_t size )
{
	void *adr = KMALLOC( size);
	DBG( awt_mem, ("malloc: %d  -> %x\n", size, adr));
	return adr;
}

static inline void* _awt_calloc_wrapper ( int n, size_t size )
{
	void *adr = KCALLOC( n, size);
	DBG( awt_mem, ("calloc: %d,%d  -> %x\n", n, size, adr));
	return adr;
}

static inline void _awt_free_wrapper ( void* adr )
{
	DBG( awt_mem, ("free: %x\n", adr));
	KFREE( adr);
}

#define AWT_MALLOC(_n) \
_awt_malloc_wrapper( _n)

#define AWT_CALLOC(_n,_sz) \
_awt_calloc_wrapper( _n, _sz)

#define AWT_FREE(_adr) \
_awt_free_wrapper( _adr);


static inline void* getBuffer ( Toolkit* X, unsigned int nBytes ) {
	if ( nBytes > X->nBuf ) {
		if ( X->buf )
			AWT_FREE( X->buf);
		X->buf  = (TCHAR*)AWT_MALLOC( nBytes * sizeof(TCHAR));
		X->nBuf = nBytes;
	}
	return X->buf;
}

static inline char* java2CString ( JNIEnv *env, Toolkit* X, jstring jstr ) {
  jboolean isCopy;
  register unsigned i;
  unsigned      n = env->GetStringLength( jstr);
  const jchar    *jc = env->GetStringChars( jstr, &isCopy);
  char         *c;

  if ( n >= X->nBuf ) {
	if ( X->buf )
	  AWT_FREE( X->buf);
	X->buf = (TCHAR*) AWT_MALLOC( n+1);
	X->nBuf = n+1;
  }

  for ( i=0, c=(char*)X->buf; i<n; i++ ) *c++ = (char) jc[i];
  *c = 0;
  env->ReleaseStringChars( jstr, jc);

  return (char*)X->buf;
}

static inline TCHAR* java2WinString ( JNIEnv *env, Toolkit* X, jstring jstr ) {
	jboolean isCopy;
	UINT i, n;
	const jchar *jc;
	
	if ( ! jstr) {
		X->buf[0] = 0;
		return X->buf;
	}
	
	n = env->GetStringLength( jstr);
    jc = env->GetStringChars( jstr, &isCopy);
	
	getBuffer( X, n+1);
	
	for ( i=0; i<n; i++ ) X->buf[i] = (TCHAR) jc[i];
    X->buf[i] = 0;
    env->ReleaseStringChars( jstr, jc);
	
    return X->buf;
}

static inline TCHAR* jbyte2WinString ( Toolkit* X, jbyte* jb, int len ) {
	register int i;
	
	getBuffer( X, len+1);
	
	for ( i=0; i<len; i++ ) {
		X->buf[i] = (TCHAR) jb[i];
	}
	X->buf[i] = 0;
	return (X->buf);
}

static inline TCHAR* jchar2WinString ( Toolkit* X, jchar* jc, int len ) {
	register int i;
	
	getBuffer( X, len+1);
	
	for ( i=0; i<len; i++ ) {
		X->buf[i] = (TCHAR) jc[i];
	}
	X->buf[i] = 0;
	return (X->buf);
}

static inline char* winString2UTF(Toolkit* X, TCHAR* str, int len) {
#if !defined(_UNICODE)
	return (str);
#else
	register int i;
	char* buf;
	
	getBuffer(X, len+1);
	buf = (char*)X->buf;
	
	for (i = 0; i < len; i++) {
		buf[i] = (char)str[i];
	}
	buf[i] = 0;
	return (buf);
#endif
}

static inline HCURSOR getCursor ( jint jCursor )
{
	HCURSOR cursor;
	
	if ( jCursor > 13 ) jCursor = 0;
	
	if ( !(cursor = X->cursors[jCursor]) ){
		LPCTSTR shape;
#if defined(UNDER_CE)
		shape = IDC_WAIT;
#else
		switch ( jCursor ) {
		case  0: shape = IDC_ARROW; break;		/*  0: DEFAULT_CURSOR     */
		case  1: shape = IDC_CROSS; break;		/*  1: CROSSHAIR_CURSOR   */
		case  2: shape = IDC_IBEAM; break;		/*  2: TEXT_CURSOR        */
		case  3: shape = IDC_WAIT; break;		/*  3: WAIT_CURSOR        */
		case  4: shape = IDC_SIZENESW; break;	/*  4: SW_RESIZE_CURSOR   */
		case  5: shape = IDC_SIZENWSE; break;	/*  5: SE_RESIZE_CURSOR   */
		case  6: shape = IDC_SIZENWSE; break;	/*  6: NW_RESIZE_CURSOR   */
		case  7: shape = IDC_SIZENESW; break;	/*  7: NE_RESIZE_CURSOR   */
		case  8: shape = IDC_SIZENS; break;		/*  8: N_RESIZE_CURSOR    */
		case  9: shape = IDC_SIZENS; break;		/*  9: S_RESIZE_CURSOR    */
		case 10: shape = IDC_SIZEWE; break;		/* 10: W_RESIZE_CURSOR    */
		case 11: shape = IDC_SIZEWE; break;		/* 11: E_RESIZE_CURSOR    */
		case 12: shape = IDC_UPARROW; break;	/* 12: HAND_CURSOR        */
		case 13: shape = IDC_SIZEALL; break;	/* 13: MOVE_CURSOR        */
		default: shape = IDC_ARROW;
		}
#endif		
		cursor = X->cursors[jCursor] = LoadCursor( 0, shape);
	}
	return cursor;
}

static inline KWnd* getWnd ( HWND hwnd )
{
	if ( hwnd != X->lastHwnd ){
		X->lastWnd = (KWnd*)GetWindowLong( hwnd, GWL_USERDATA);
		X->lastHwnd = hwnd;
	}

	return X->lastWnd;
}

static inline void setWnd ( HWND hwnd, KWnd* wnd )
{
	X->lastWnd = wnd;
	X->lastHwnd = hwnd;

    SetWindowLong( hwnd, GWL_USERDATA, (long)wnd);
}


extern "C" {
	jlong Java_java_awt_Toolkit_clrBright ( JNIEnv* env, jclass clazz, jint rgb );
	jlong Java_java_awt_Toolkit_clrDark ( JNIEnv* env, jclass clazz, jint rgb );
}

static inline void
rgbValues ( Toolkit* X, unsigned long pixel, int* r, int* g, int* b )
{
	*r = GetRValue( pixel);
	*g = GetGValue( pixel);
	*b = GetBValue( pixel);
}

static inline int
ColorJ2N(int jrgb) {
	return ( RGB( GetBValue( jrgb), GetGValue( jrgb), GetRValue( jrgb) ));
}

static inline int
ColorN2J(int nrgb) {
	return ( RGB( GetBValue( nrgb), GetGValue( nrgb), GetRValue( nrgb) ));
}

static inline int
ColorN(int nrgb) {
	return (nrgb & 0x00FFFFFF);
}


/*****************************************************************************************
* image functions
*/

Image* createImage ( int width, int height);
void createMaskImage( Toolkit* X, Image* img);
void createClrImage ( Toolkit* X, Image* img);
void freeImage( Toolkit* X, Image* img);


/*****************************************************************************************
* clipboard functions
*/

jobject selectionClear ( JNIEnv* env, Toolkit* X );
jobject selectionRequest ( JNIEnv* env, Toolkit* X );


/*****************************************************************************************
 * file io wrapper macros (for image production)
 */

#define AWT_OPEN(_file)               open(_file, O_RDONLY|O_BINARY)
#define AWT_REWIND(_fd)               lseek(_fd, 0, SEEK_SET)
#define AWT_SETPOS(_fd,_off)          lseek(_fd, _off, SEEK_CUR)
#define AWT_READ(_fd,_buf,_count)     read(_fd,_buf,_count)
#define AWT_CLOSE(_fd)                close(_fd)


/*****************************************************************************************
 * CE hacks
 */

#if defined(UNDER_CE)
#define GetMenu(X)              (CommandBar_GetMenu(this->hbar, 0))
#define WS_THICKFRAME           0
#define WS_OVERLAPPEDWINDOW     (WS_OVERLAPPED|WS_THICKFRAME|WS_CAPTION|WS_SYSMENU)
#define WND_PROC				WNDPROC
#define PIXEL(a,b,c,d)			SetPixel(a,b,c,d)
#define GFLUSH()
#define APPICON					(HICON)0
#define BEEP()
#else
#define WND_PROC				FARPROC
#define PIXEL(a,b,c,d)			SetPixelV(a,b,c,d)
#define GFLUSH()				GdiFlush()
#define APPICON					LoadIcon( 0, IDI_APPLICATION)
#define BEEP()					Beep( 700, 70)
#endif

#endif

