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

#define TLK_AWT_EVENT_WINDOW_CLOSING       201
#define TLK_AWT_EVENT_WINDOW_CLOSED        202
#define TLK_AWT_EVENT_WINDOW_ICONIFIED     203
#define TLK_AWT_EVENT_WINDOW_DEICONIFIED   204

#define TLK_AWT_EVENT_KEY_PRESSED		401
#define TLK_AWT_EVENT_KEY_RELEASED		402

#define TLK_AWT_EVENT_MOUSE_CLICKED		500
#define TLK_AWT_EVENT_MOUSE_PRESSED		501
#define TLK_AWT_EVENT_MOUSE_RELEASED	502
#define TLK_AWT_EVENT_MOUSE_MOVED		503
#define TLK_AWT_EVENT_MOUSE_ENTERED		504
#define TLK_AWT_EVENT_MOUSE_EXITED		505
#define TLK_AWT_EVENT_MOUSE_DRAGGED		506
#define TLK_AWT_EVENT_MOUSE_FIRST TLK_AWT_EVENT_MOUSE_CLICKED
#define TLK_AWT_EVENT_MOUSE_LAST TLK_AWT_EVENT_MOUSE_DRAGGED

#define TLK_AWT_EVENT_PAINT_FIRST 800
#define TLK_AWT_EVENT_PAINT 800
#define TLK_AWT_EVENT_UPDATE 801

#define TLK_AWT_EVENT_FOCUS_GAINED        1004
#define TLK_AWT_EVENT_FOCUS_LOST          1005

/*
#define TLK_AWT_EVENT_COMPONENT_RESIZED    101
#define    WM_KILLED           1905
*/

#define TLK_AWT_IC_RANDOMPIXELORDER		1
#define TLK_AWT_IC_TOPDOWNLEFTRIGHT		2
#define TLK_AWT_IC_COMPLETESCANLINES	4
#define TLK_AWT_IC_SINGLEPASS			8
#define TLK_AWT_IC_SINGLEFRAME			16

#define TLK_AWT_IC_IMAGEERROR		1
#define TLK_AWT_IC_SINGLEFRAMEDONE	2
#define TLK_AWT_IC_STATICIMAGEDONE	3
#define TLK_AWT_IC_IMAGEABORTED		4

#define TLK_AWT_KM_SHIFT_MASK	1
#define TLK_AWT_KM_CTRL_MASK	2
#define TLK_AWT_KM_META_MASK	4
#define TLK_AWT_KM_ALT_MASK		8
