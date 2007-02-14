
/*
 * Copyright (c) 2006 - 2007
 *	Alper Akcan <alper@kaffe.org>, All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#include "toolkit.h"

jclass     ComponentEvent;
jclass     MouseEvent;
jclass     FocusEvent;
jclass     WindowEvent;
jclass     KeyEvent;
jclass     PaintEvent;
jclass     WMEvent;

jmethodID  getComponentEvent;
jmethodID  getMouseEvent;
jmethodID  getFocusEvent;
jmethodID  getWindowEvent;
jmethodID  getKeyEvent;
jmethodID  getPaintEvent;
jmethodID  getWMEvent;

typedef jobject (*event_handler) (JNIEnv *, xynth_event_t *);

typedef enum {
	JNONE_EVENT   = 0x0,
	JQUIT_EVENT   = 0x1,
	JKEYBD_EVENT  = 0x2,
	JMOUSE_EVENT  = 0x3,
	JEXPOSE_EVENT = 0x4,
	JCONFIG_EVENT = 0x5,
	JFOCUS_EVENT  = 0x6
} S_JEVENT;

S_JEVENT event_handler_number (s_event_t *event)
{
	S_EVENT event_type;
	event_type = event->type & (QUIT_EVENT | KEYBD_EVENT | MOUSE_EVENT | EXPOSE_EVENT | CONFIG_EVENT | FOCUS_EVENT);
	switch (event_type) {
		case QUIT_EVENT:  return JQUIT_EVENT;
		case KEYBD_EVENT: return JKEYBD_EVENT;
		case MOUSE_EVENT: return JMOUSE_EVENT;
		case EXPOSE_EVENT:return JEXPOSE_EVENT;
		case CONFIG_EVENT:return JCONFIG_EVENT;
		case FOCUS_EVENT: return JFOCUS_EVENT;
		default:          break;
	}
	DEBUGF("unknown event: 0x%08x", event->type); 
	return JNONE_EVENT;
}

jobject event_handler_none (JNIEnv *env, xynth_event_t *xevent)
{
	DEBUGF("Enter");
	DEBUGF("Leave");
	return NULL;
}

jobject event_handler_quit (JNIEnv *env, xynth_event_t *xevent)
{
	int idx;
	jobject jevent;
	s_event_t *event;
	DEBUGF("Enter");
	idx = source_idx_get(xynth, xevent->window);
	if (idx < 0) {
		return NULL;
	}
	jevent = (*env)->CallStaticObjectMethod(env, WMEvent, getWMEvent, idx, JQUIT);
	DEBUGF("Leave");
	return jevent;
}

static inline int key_mod (int keyState)
{
	int mod = 0;
	if (keyState & KEYCODE_SHIFTF)   mod |= 1;
	if (keyState & KEYCODE_CTRLF)    mod |= 2;
	if (keyState & KEYCODE_ALTGRF)   mod |= 4;
	if (keyState & KEYCODE_ALTF)     mod |= 8;
	return mod;
}

jobject event_handler_keybd (JNIEnv *env, xynth_event_t *xevent)
{
	int idx;
	jobject jevent;
	s_event_t *event;
	DEBUGF("Enter");
	idx = source_idx_get(xynth, xevent->window);
	if (idx < 0) {
		return NULL;
	}
	jevent = NULL;
	event = xevent->event;
	jevent = (*env)->CallStaticObjectMethod(env, KeyEvent, getKeyEvent, idx,
	                                        (event->type & KEYBD_PRESSED) ? JKEY_PRESSED : JKEY_RELEASED,
						xynth->keymap[event->keybd->keycode],
						event->keybd->ascii,
						key_mod(event->keybd->flag));
	DEBUGF("Leave");
	return jevent;
}

jobject event_handler_mouse (JNIEnv *env, xynth_event_t *xevent)
{
	int idx;
	jobject jevent;
	s_event_t *event;
	DEBUGF("Enter");
	idx = source_idx_get(xynth, xevent->window);
	if (idx < 0) {
		return NULL;
	}
	jevent = NULL;
	event = xevent->event;
	if ((event->mouse->x >= xevent->window->surface->buf->x) &&
	    (event->mouse->y >= xevent->window->surface->buf->y) &&
	    (event->mouse->x <= (xevent->window->surface->buf->x + xevent->window->surface->buf->w - 1)) &&
	    (event->mouse->y <= (xevent->window->surface->buf->y + xevent->window->surface->buf->h - 1))) {
		event->mouse->x -= xevent->window->surface->buf->x;
		event->mouse->y -= xevent->window->surface->buf->y;
		if (event->type & MOUSE_RELEASED) {
			jevent = (*env)->CallStaticObjectMethod(env, MouseEvent, getMouseEvent, idx, JMOUSE_RELEASED, event->mouse->b, event->mouse->x, event->mouse->y);
		} else if (event->type & MOUSE_PRESSED) {
			jevent = (*env)->CallStaticObjectMethod(env, MouseEvent, getMouseEvent, idx, JMOUSE_PRESSED, event->mouse->b, event->mouse->x, event->mouse->y);
		} else if (event->type & MOUSE_ENTERED) {
			jevent = (*env)->CallStaticObjectMethod(env, MouseEvent, getMouseEvent, idx, JMOUSE_ENTERED, 0, event->mouse->x, event->mouse->y);
		} else if (event->type & MOUSE_EXITED) {
			jevent = (*env)->CallStaticObjectMethod(env, MouseEvent, getMouseEvent, idx, JMOUSE_EXITED, 0, event->mouse->x, event->mouse->y);
		} else {
			jevent = (*env)->CallStaticObjectMethod(env, MouseEvent, getMouseEvent, idx, JMOUSE_MOVED, 0, event->mouse->x, event->mouse->y);
		}
	}
	DEBUGF("Leave %d", xevent->window->id);
	return jevent;
}

jobject event_handler_expose (JNIEnv *env, xynth_event_t *xevent)
{
	int idx;
	jobject jevent;
	s_event_t *event;
	DEBUGF("Enter");
	idx = source_idx_get(xynth, xevent->window);
	if (idx < 0) {
		return NULL;
	}
	event = xevent->event;
	event->expose->rect->x = event->expose->rect->x - xevent->window->surface->buf->x;
	event->expose->rect->y = event->expose->rect->y - xevent->window->surface->buf->y;
	jevent = (*env)->CallStaticObjectMethod( env, PaintEvent, getPaintEvent, idx, JUPDATE, event->expose->rect->x, event->expose->rect->y, event->expose->rect->w, event->expose->rect->h);
	DEBUGF("Leave");
	return jevent;
}

jobject event_handler_config (JNIEnv *env, xynth_event_t *xevent)
{
	int idx;
	jobject jevent;
	s_event_t *event;
	DEBUGF("Enter");
	idx = source_idx_get(xynth, xevent->window);
	if (idx < 0) {
		return NULL;
	}
	if (xevent->event->type & (CONFIG_CHNGW | CONFIG_CHNGH)) {
		free(xevent->window->surface->vbuf);
		xevent->window->surface->width = xevent->event->expose->rect->w;
		xevent->window->surface->height = xevent->event->expose->rect->h;
		xevent->window->surface->vbuf = (char *) malloc(xevent->window->surface->width * xevent->window->surface->height * xevent->window->surface->bytesperpixel);
	}
	event = xevent->event;
	jevent = (*env)->CallStaticObjectMethod(env, ComponentEvent, getComponentEvent, idx, JCOMPONENT_RESIZED, event->expose->rect->x, event->expose->rect->y, event->expose->rect->w, event->expose->rect->h);
	DEBUGF("Leave");
	return jevent;
}

jobject event_handler_focus (JNIEnv *env, xynth_event_t *xevent)
{
	int idx;
	jobject jevent;
	s_event_t *event;
	DEBUGF("Enter");
	idx = source_idx_get(xynth, xevent->window);
	if (idx < 0) {
		return NULL;
	}
	event = xevent->event;
	if (xevent->window->pri == 0) {
		jevent = (*env)->CallStaticObjectMethod(env, FocusEvent, getFocusEvent, idx, JFOCUS_GAINED, JNI_FALSE);
	} else {
		jevent = (*env)->CallStaticObjectMethod(env, FocusEvent, getFocusEvent, idx, JFOCUS_LOST, JNI_FALSE);
	}
	DEBUGF("Leave");
	return jevent;
}

event_handler process_event[] = {
	event_handler_none,
	event_handler_quit,
	event_handler_keybd,
	event_handler_mouse,
	event_handler_expose,
	event_handler_config,
	event_handler_focus
};

jobject Java_java_awt_Toolkit_evtInit (JNIEnv* env, jclass clazz UNUSED)
{
	jclass Component;
	
	DEBUGF("Enter");
	
	if (ComponentEvent != NULL) {
		DEBUGF("evtInit called twice");
		return NULL;
	}
	
	ComponentEvent = (*env)->FindClass( env, "java/awt/ComponentEvt");
	assert(ComponentEvent != NULL);
	MouseEvent     = (*env)->FindClass( env, "java/awt/MouseEvt");
	assert(MouseEvent != NULL);
	FocusEvent     = (*env)->FindClass( env, "java/awt/FocusEvt");
	assert(FocusEvent != NULL);
	WindowEvent    = (*env)->FindClass( env, "java/awt/WindowEvt");
	assert(WindowEvent != NULL);  
	KeyEvent       = (*env)->FindClass( env, "java/awt/KeyEvt");
	assert(KeyEvent != NULL);
	PaintEvent     = (*env)->FindClass( env, "java/awt/PaintEvt");
	assert(PaintEvent != NULL);
	WMEvent        = (*env)->FindClass( env, "java/awt/WMEvent");
	assert(WMEvent != NULL);
	
	getComponentEvent = (*env)->GetStaticMethodID( env, ComponentEvent, "getEvent", "(IIIIII)Ljava/awt/ComponentEvt;");
	getMouseEvent     = (*env)->GetStaticMethodID( env, MouseEvent, "getEvent", "(IIIII)Ljava/awt/MouseEvt;");
	getFocusEvent     = (*env)->GetStaticMethodID( env, FocusEvent, "getEvent", "(IIZ)Ljava/awt/FocusEvt;");
	getWindowEvent    = (*env)->GetStaticMethodID( env, WindowEvent, "getEvent", "(II)Ljava/awt/WindowEvt;");
	getKeyEvent       = (*env)->GetStaticMethodID( env, KeyEvent, "getEvent", "(IIIII)Ljava/awt/KeyEvt;");
	getPaintEvent     = (*env)->GetStaticMethodID( env, PaintEvent, "getEvent", "(IIIIII)Ljava/awt/PaintEvt;");
	getWMEvent        = (*env)->GetStaticMethodID( env, WMEvent, "getEvent", "(II)Ljava/awt/WMEvent;");
	
	Component = (*env)->FindClass( env, "java/awt/Component");

	DEBUGF("Leave");

	return (*env)->NewObjectArray( env, xynth->nwindows, Component, NULL);
}

jint Java_java_awt_Toolkit_evtRegisterSource (JNIEnv* env UNUSED, jclass clazz UNUSED, jobject nativeWnd)
{
	int id;
	DEBUGF("Enter");
	s_window_t *window;
	window = UNVEIL_WINDOW(nativeWnd);
	id = source_idx_get(xynth, window);
	DEBUGF("Leave");
	return id;
}

void xynth_kaffe_atevent (s_window_t *window, s_event_t *event)
{
	xynth_event_t *xevent;
	DEBUGF("Enter id: %d", window->id);
	switch (event->type & EVENT_MASK) {
		case QUIT_EVENT:
		case MOUSE_EVENT:
		case KEYBD_EVENT:
		case CONFIG_EVENT:
		case FOCUS_EVENT:
			xevent = (xynth_event_t *) AWT_MALLOC(sizeof(xynth_event_t));
			if (xevent != NULL) {
				if (!s_event_init(&(xevent->event))) {
					xevent->window = window;
					xevent->event->type = event->type;
					memcpy(xevent->event->mouse, event->mouse, sizeof(s_mouse_t));
					memcpy(xevent->event->keybd, event->keybd, sizeof(s_keybd_t));
					memcpy(xevent->event->expose->rect, event->expose->rect, sizeof(s_rect_t));
					s_thread_mutex_lock(xynth->eventq->mut);
					s_list_add(xynth->eventq->list, xevent, -1);
					s_thread_mutex_unlock(xynth->eventq->mut);
				} else {
					AWT_FREE(xevent);
				}
			}
			break;
		case EXPOSE_EVENT: /* nothing */
		default:
			break;
	}
	if (event->type & (CONFIG_CHNGW | CONFIG_CHNGH)) {
		xevent = (xynth_event_t *) AWT_MALLOC(sizeof(xynth_event_t));
		if (xevent != NULL) {
			if (!s_event_init(&(xevent->event))) {
				xevent->window = window;
				xevent->event->type = EXPOSE_EVENT;
				memcpy(xevent->event->mouse, event->mouse, sizeof(s_mouse_t));
				memcpy(xevent->event->keybd, event->keybd, sizeof(s_keybd_t));
				memcpy(xevent->event->expose->rect, event->expose->rect, sizeof(s_rect_t));
				s_thread_mutex_lock(xynth->eventq->mut);
				s_list_add(xynth->eventq->list, xevent, -1);
				s_thread_mutex_unlock(xynth->eventq->mut);
			} else {
				AWT_FREE(xevent);
			}
		}
	}
	DEBUGF("Leave");
}

jobject Java_java_awt_Toolkit_evtGetNextEvent (JNIEnv *env, jclass clazz)
{
	jobject jevent;
	xynth_event_t *xevent;
	S_JEVENT jevent_type;
	jevent = NULL;
	s_thread_mutex_lock(xynth->eventq->mut);
        while (!s_list_eol(xynth->eventq->list, 0)) {
		xevent = (xynth_event_t *) s_list_get(xynth->eventq->list, 0);
		jevent_type = event_handler_number(xevent->event);
		jevent = process_event[jevent_type](env, xevent);
		s_list_remove(xynth->eventq->list, 0);
		s_event_uninit(xevent->event);
		AWT_FREE(xevent);
		if (jevent != NULL) {
			break;
		} else {
			DEBUGF("ERROR");
		}
	}
	s_thread_mutex_unlock(xynth->eventq->mut);
	return jevent;
}

jint Java_java_awt_Toolkit_evtUnregisterSource (JNIEnv *env UNUSED, jclass clazz UNUSED, jobject nwnd)
{
	int i;
	s_window_t *window;
	DEBUGF("Enter");
	window = UNVEIL_WINDOW(nwnd);
	i = source_idx_get(xynth, window);
	if (i >= 0) {
		xynth->windows[i].window = 0;
		xynth->windows[i].owner = 0;
	}
	if (xynth->last_window == window) {
		xynth->last_window = 0;
	}
	DEBUGF("Leave");
	return i;
}
