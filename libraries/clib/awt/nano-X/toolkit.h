/**
 * nano-X AWT backend for Kaffe.
 *
 * Copyright (c) 2001
 *	Exor International Inc. All rights reserved.
 *
 * Copyright (c) 2001
 *	Sinby Corporatin, All rights reserved.
 *
 * Copyright (c) 2005
 *	Kaffe.org contributors, see ChangeLog for details.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

#ifndef __toolkit_h
#define __toolkit_h

#define MWINCLUDECOLORS
#include <nano-X.h>
#include "awt_const.h"

#include "gtypes.h"
#include "gc.h"
#include "thread.h"
#include "debug.h"

typedef struct _TLKWindow {
	GR_WINDOW_ID wid;
	GR_WINDOW_ID owner;
	unsigned int flags;
} TLKWindow;

typedef unsigned long TLK_GRAPHICS_ID;

#define TLK_GR_EVENT_MASK_ALL (GR_EVENT_MASK_ALL & ~( GR_EVENT_MASK_MOUSE_MOTION | GR_EVENT_MASK_CHLD_UPDATE | GR_EVENT_MASK_TIMEOUT | GR_EVENT_MASK_SCREENSAVER| GR_EVENT_TYPE_CLIENT_DATA | GR_EVENT_TYPE_SELECTION_CHANGED))

typedef struct _TLKGraphics {
	int flags;
	GR_ID target;
	int targetType;
	GR_GC_ID gc;
	GR_REGION_ID region;
	unsigned int off_x;
	unsigned int off_y;
	GR_COLOR fg_color;
	GR_COLOR bg_color;
	GR_COLOR xor_color;

	TLK_GRAPHICS_ID gid;
	unsigned int magicNo;
} TLKGraphics;

#define TLK_GRAPHICS_FLAG_XOR_MODE	0x0001

#define TLK_GRAPHICS_MAGIC_NO	0x47524150 // GRAP
#define TLK_TARGET_TYPE_WINDOW		0
#define TLK_TARGET_TYPE_IMAGE		1
#define TLK_TARGET_TYPE_GRAPHICS	2

#define TLK_GET_GRAPHICS(gid)
static inline 
TLKGraphics *tlk_get_graphics(TLK_GRAPHICS_ID gid)
{
	TLKGraphics *gp;

	gp = (TLKGraphics *)gid;
	if ( gp == NULL ) {
		return NULL;
	}
	if ( gid != gp->gid ) {
		return NULL;
	}
	if ( gp->magicNo != TLK_GRAPHICS_MAGIC_NO ) {
		return NULL;
	}

	return gp;
}

typedef struct _DecoInset {
	int left;
	int top;
	int right;
	int bottom;
	int guess;
} DecoInset;           

typedef struct _Toolkit {
	int         maxWindowN;
	int         windowN;
	TLKWindow	*windowsP;

	DecoInset	frameInsets;
	DecoInset	dialogInsets;
} Toolkit;

#define MAX_WINDOW_N 32
extern Toolkit nanoX;

/**
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

#define TLK_MALLOC(_n) \
	_awt_malloc_wrapper(_n)

#define TLK_CALLOC(_n,_sz) \
	_awt_calloc_wrapper(_n, _sz)

#define TLK_FREE(_adr) \
	_awt_free_wrapper(_adr);

#define TLK_SignalError SignalError
extern void SignalError(const char *, const char *);

#define TLK_IS_BLOCKING				1
#define TLK_IS_DISPATCH_EXCLUSIVE	2
#define TLK_NEEDS_FLUSH				4
#define TLK_NATIVE_DISPATCHER_LOOP	8
#define TLK_EXTERNAL_DECO			16

#define TLK_FONT_PLAIN	0
#define TLK_FONT_BOLD	1
#define TLK_FONT_ITAILC	2

#define TLK_CURSOR_DEFAULT		0
#define TLK_CURSOR_CROSSHAIR	1
#define TLK_CURSOR_TEXT			2
#define TLK_CURSOR_WAIT			3
#define TLK_CURSOR_SW_RESIZE	4
#define TLK_CURSOR_SE_RESIZE	5
#define TLK_CURSOR_NW_RESIZE	6
#define TLK_CURSOR_NE_RESIZE	7
#define TLK_CURSOR_N_RESIZE		8
#define TLK_CURSOR_S_RESIZE		9
#define TLK_CURSOR_W_RESIZE		10
#define TLK_CURSOR_E_RESIZE		11
#define TLK_CURSOR_HAND			12
#define TLK_CURSOR_MOVE			13

#define TLK_SOURCE_NONE       0x00
#define TLK_SOURCE_FRAME      0x01
#define TLK_SOURCE_WINDOW     0x02
#define TLK_SOURCE_DIALOG     0x04
#define TLK_SOURCE_MAPPED     0x08
#define TLK_SOURCE_DESTROYED  0x10

#define TLK_WINDOW_REGISTER 0x8000

#define getCachedSourceIndex getSourceIndex

static inline int 
getSourceIndex(GR_WINDOW_ID wid)
{
	int i;
	for ( i = 0; i < nanoX.maxWindowN; i++ ) {
		if (( nanoX.windowsP[i].flags != TLK_SOURCE_NONE ) &&
			( nanoX.windowsP[i].wid == wid )) {
			return i;
		}
	}

	return -1;
}

static inline int 
getFreeSourceIndex(void) 
{
	int i;
	for ( i = nanoX.windowN; i < nanoX.maxWindowN; i++ ) {
		if ( nanoX.windowsP[i].flags == TLK_SOURCE_NONE ) {
			return i;
		}
	}

	return -1;
}

static inline int
registerSource(GR_WINDOW_ID wid, GR_WINDOW_ID owner, int flags)
{
	int i;
	i = getFreeSourceIndex();

	if ( i >= 0 ) {
		nanoX.windowsP[i].wid = wid;
		nanoX.windowsP[i].owner = owner;
		nanoX.windowsP[i].flags = flags;
		nanoX.windowN++;
		return i;
	} else {
		return -1;
	}
}

#define JAVA_RGB(_r,_g,_b)  (_r<<16 | _g<<8 | _b)
#define JAVA_RED(_rgb)      ((_rgb & 0xff0000) >> 16)
#define JAVA_GREEN(_rgb)    ((_rgb & 0x00ff00) >> 8)
#define JAVA_BLUE(_rgb)      (_rgb & 0x0000ff)

#endif /* __toolkit_h */
