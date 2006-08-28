
/*
 * Copyright (c) 2006
 *	Alper Akcan <alper@kaffe.org>, All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#include "config.h"
#include "config-std.h"
#include "config-mem.h"
#include "config-setjmp.h"

#include "jcl.h"
#include "native.h"

#include <xynth.h>

#define XYNTH_NWINDOWS 100

typedef struct graphics_s {
	s_surface_t *surface;
	s_font_t *font;
	int fg;
	int bg;
	int x0;
	int y0;
	s_rect_t clip;
} graphics_t;

typedef struct window_rec_s {
	s_window_t *window;
	s_window_t *owner;
} window_rec_t;

typedef struct deco_inset_s {
	int left;
	int top;
	int right;
	int bottom;
	char guess;
} deco_inset_t;

typedef struct xynth_event_s {
	s_event_t *event;
	s_window_t *window;
} xynth_event_t;

typedef struct xynth_eventq_s {
	s_list_t *list;
	s_thread_mutex_t *mut;
} xynth_eventq_t;

typedef struct xynth_toolkit_s {
	deco_inset_t frame_insets;
	deco_inset_t dialog_insets;
	unsigned int nwindows;
	window_rec_t *windows;
	s_window_t *last_window;
	unsigned int last_idx;
	s_window_t *root;
	xynth_eventq_t *eventq;
	int keymap[S_KEYCODE_CODES];
} xynth_toolkit_t;

xynth_toolkit_t *xynth;

//#define DEBUGF(a...)	printf(a); printf(" (%s [%s:%d])\n", __FUNCTION__, __FILE__, __LINE__);
#define DEBUGF(a...)

#define TLK_IS_BLOCKING			1
#define TLK_IS_DISPATCH_EXCLUSIVE	2
#define TLK_NEEDS_FLUSH			4
#define TLK_NATIVE_DISPATCHER_LOOP	8
#define TLK_EXTERNAL_DECO		16

#define JCOMPONENT_RESIZED 101
#define JFOCUS_GAINED      1004
#define JFOCUS_LOST        1005
#define JUPDATE            801
#define JQUIT              202
#define JMOUSE_PRESSED     501
#define JMOUSE_RELEASED    502
#define JMOUSE_MOVED       503
#define JMOUSE_ENTERED     504
#define JMOUSE_EXITED      505
#define JKEY_PRESSED       401
#define JKEY_RELEASED      402

static inline void * _awt_malloc_wrapper (size_t size)
{
	void *adr;
	enterUnsafeRegion();
	adr = malloc(size);
	leaveUnsafeRegion();
	return adr;
}

static inline void * _awt_calloc_wrapper (size_t n, size_t size)
{
	void *adr;
	enterUnsafeRegion();
	adr = calloc(n, size);
	leaveUnsafeRegion();
	return adr;
}

static inline void _awt_free_wrapper (void *adr)
{
	enterUnsafeRegion();
	free(adr);
	leaveUnsafeRegion();
}

#define AWT_MALLOC(_n)		_awt_malloc_wrapper(_n)
#define AWT_CALLOC(_n,_sz)	_awt_calloc_wrapper(_n, _sz)
#define AWT_FREE(_adr)		_awt_free_wrapper(_adr);

static inline char * java2CString (JNIEnv *env, jstring jstr)
{
	char *buf;
	jboolean isCopy;
	register int i;
	jsize n = (*env)->GetStringLength(env, jstr);
	const jchar *jc = (*env)->GetStringChars(env, jstr, &isCopy);
	buf = AWT_MALLOC(n + 1);
	for (i = 0; i < n; i++) {
		buf[i] = (char) jc[i];
	}
	buf[i] = 0;
	(*env)->ReleaseStringChars(env, jstr, jc);
	return buf;
}

#define KAFFE_FONT_FUNC_DECL(ret, name, args...) ret name(JNIEnv *env, jclass clazz UNUSED, jobject jfont, ##args)
#define UNVEIL_FONT(jfont) ((s_font_t *) JCL_GetRawData(env, jfont))

#define KAFFE_IMG_FUNC_DECL(ret, name, args...) ret name(JNIEnv *env, jclass clazz UNUSED, jobject jimg, ##args)
#define UNVEIL_IMG(jimg) ((s_image_t *) JCL_GetRawData(env, jimg))

#define UNVEIL_WINDOW(jwin) ((s_window_t *) JCL_GetRawData(env, jwin))

#define KAFFE_GR_FUNC_DECL(ret, name, args...) ret name(JNIEnv *env, jclass clazz UNUSED, jobject jgr, ##args)
#define UNVEIL_GR(jgraphics) ((graphics_t *) JCL_GetRawData(env, jgraphics));

static inline int source_idx_free (xynth_toolkit_t *tk, s_window_t *win)
{
	register unsigned int n;
	for (n = 0; n < tk->nwindows; n++) {
		if (tk->windows[n].window == NULL) {
			tk->last_idx = n;
			tk->last_window = win;
			return n;
		}
	}
	return -1;
}

static inline int source_idx_register (xynth_toolkit_t *tk, s_window_t *window, s_window_t *owner)
{
	int i;
	i = source_idx_free(tk, window);
	if (i >= 0) {
		tk->windows[i].window = window;
		tk->windows[i].owner = owner;
		return i;
	} else {
		DEBUGF("window table out of spafe: %d", tk->nwindows);
		return -1;
	}
}

static inline int source_idx_get (xynth_toolkit_t *tk, s_window_t *win)
{
	register unsigned int n;
	if (win == tk->last_window) {
		return tk->last_idx;
	} else {
		for (n = 0; n < tk->nwindows; n++) {
			if (tk->windows[n].window == win) {
				tk->last_idx = n;
				tk->last_window = win;
				return tk->last_idx;
			} else if (tk->windows[n].window == 0) {
				return -1;
			}
		}
		return -1;
	}
}
