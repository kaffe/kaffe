/**
* cmnwnd.hpp - basic window functions header
*
* Copyright (c) 1999
*      Transvirtual Technologies, Inc.  All rights reserved.
*
* See the file "license.terms" for information on usage and redistribution 
* of this file. 
*/

#ifndef __cmnwnd_h
#define __cmnwnd_h

#include "toolkit.hpp"

#define DEF_WND_STYLE	WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS

#ifdef MAIN

jclass     AWTEvent;
jclass     ComponentEvent;
jclass     MouseEvent;
jclass     FocusEvent;
jclass     WindowEvent;
jclass     KeyEvent;
jclass     PaintEvent;
jclass     WMEvent;
jclass	   ActionEvent;
jclass	   ItemEvent;
jclass	   AdjustmentEvent;

jmethodID  getComponentEvent;
jmethodID  getMouseEvent;
jmethodID  getFocusEvent;
jmethodID  getWindowEvent;
jmethodID  getKeyEvent;
jmethodID  getPaintEvent;
jmethodID  getWMEvent;
jmethodID  dispatch;
jmethodID  getActionEvent;
jmethodID  getItemEvent;
jmethodID  getAdjustmentEvent;

#else

extern jclass     AWTEvent;
extern jclass     ComponentEvent;
extern jclass     MouseEvent;
extern jclass     FocusEvent;
extern jclass     WindowEvent;
extern jclass     KeyEvent;
extern jclass     PaintEvent;
extern jclass     WMEvent;
extern jclass	  ActionEvent;
extern jclass	  ItemEvent;
extern jclass	  AdjustmentEvent;

extern jmethodID  getComponentEvent;
extern jmethodID  getMouseEvent;
extern jmethodID  getFocusEvent;
extern jmethodID  getWindowEvent;
extern jmethodID  getKeyEvent;
extern jmethodID  getPaintEvent;
extern jmethodID  getWMEvent;
extern jmethodID  dispatch;
extern jmethodID  getActionEvent;
extern jmethodID  getItemEvent;
extern jmethodID  getAdjustmentEvent;

#endif



#define		SELECTED	1
#define		DESELECTED	2

#define    COMPONENT_RESIZED    101

#define    WINDOW_CLOSING       201
#define    WINDOW_CLOSED        202
#define    WINDOW_ICONIFIED     203
#define    WINDOW_DEICONIFIED   204
#define    WINDOW_ACTIVATED		205
#define    WINDOW_DEACTIVATED   206

#define    KEY_PRESSED          401
#define    KEY_RELEASED         402

#define    MOUSE_PRESSED        501
#define    MOUSE_RELEASED       502
#define    J_MOUSE_MOVED          503
#define    MOUSE_ENTERED        504
#define    MOUSE_EXITED         505

#define    PAINT                800
#define    UPDATE               801

#define    FOCUS_GAINED        1004
#define    FOCUS_LOST          1005

#define	   WM_EVT_DESTROY			1902
#define    WM_EVT_KILLED           1905


class KWnd {
	
public:
	HWND		hwnd;
	int			srcIdx;
	HMENU		popup;
#if defined(UNDER_CE)
	HWND		hbar;
#endif
	RECT		insets;
	COLORREF	fg;
	COLORREF	bg;
	HFONT		fnt;
	HCURSOR		cursor;
	HBRUSH		brush;
	WNDPROC		defWndProc;
	
	KWnd() {
		insets.left = insets.top = insets.bottom = insets.right = 0;
	}
	
	virtual bool	OnPosChanged( int x, int y, int width, int height);
	virtual bool	OnPaint( int x, int y, int width, int height);
	virtual bool	OnEraseBkgnd( HDC hdc);
	virtual bool	OnOwnerDraw( LPDRAWITEMSTRUCT di);
	virtual bool	OnMouseDown( int btn, int x, int y);
	virtual bool	OnMouseUp( int btn, int x, int y);
	virtual bool	OnMouseMove( int x, int y);
	virtual bool	OnMouseEnter( int x, int y);
	virtual bool	OnMouseExit( int x, int y);
	virtual bool	OnContextMenu( int x, int y);
	virtual bool	OnFocusChange ( bool gain);
	virtual bool	OnActivate( int flags, bool min, HWND handle);
	virtual bool	OnClose();
	virtual bool	OnCommand( int cmd);
	virtual bool	OnScroll( int pos, int op, bool vert);
	virtual bool	OnSetCursor();
	virtual bool	OnChar( TCHAR c, int data);
	virtual bool	OnKeyDown( int virtKey, int keyData);
	virtual bool	OnKeyUp( int virtKey, int keyData);
	virtual HBRUSH	OnCtlColor( HDC hdc);
	
	virtual bool	updateInsets();
	virtual int		convertKey( int virtKey, int* scan, int* chr);
	virtual int		getKeyMods();
	virtual TCHAR*	getText();
	virtual void	setText(jstring text);
    virtual void	setVisible(jboolean b);
	virtual void	setFocus();
    virtual void    setEnabled(jboolean b);
    virtual void	setBounds(int x, int y, int width, int height);
    virtual void	setForeground(COLORREF c);
    virtual void	setBackground(COLORREF c);
    virtual void	setFont(HFONT f);
    virtual void 	setCursor(HCURSOR cursor);
	virtual jobject getPreferredSize();
	virtual bool	processMenuAction( HMENU m, int idx);
	
};

#endif