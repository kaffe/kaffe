/**
* cmnwnd.cpp - basic window functions
*
* Copyright (c) 1999
*      Transvirtual Technologies, Inc.  All rights reserved.
*
* See the file "license.terms" for information on usage and redistribution 
* of this file. 
*/

#include "toolkit.hpp"

extern UINT KWM_REQUEST_FOCUS;
extern UINT KWM_DESTROY;

bool KWnd::OnCommand( int cmd) {
//	if ( popup )
//		processMenuAction( popup, cmd);
	return false;
}

bool KWnd::OnScroll( int pos, int op, bool vert) {
	return false;
}

bool KWnd::OnSetCursor() {
	SetCursor( cursor);
	return true;
}

bool KWnd::OnActivate( int flags, bool min, HWND handle ) {
	return false;
}

bool KWnd::OnPosChanged( int x, int y, int width, int height) {
	// done by java layouting
	return false;
}

bool KWnd::OnPaint( int x, int y, int width, int height){

	/*
	* paint native *before* java painting
	*/
	CallWindowProc( defWndProc, hwnd, WM_PAINT, 0, 0);

	X->jEvt = JniEnv->CallStaticObjectMethod( PaintEvent, getPaintEvent,
		srcIdx, PAINT,
		x, y, width, height );			
	
	return true;
}

bool KWnd::OnOwnerDraw( LPDRAWITEMSTRUCT di) {
	return false;
}

bool KWnd::OnEraseBkgnd( HDC hdc){
	return false;
}

HBRUSH KWnd::OnCtlColor( HDC hdc) {
	SetBkColor( hdc, bg);
	SetTextColor( hdc, fg);
	return brush;
}

bool KWnd::OnContextMenu( int x, int y) {
	POINT pt = { x, y };
	int retID;
	
	if ( popup ) {
		MapWindowPoints( hwnd, HWND_DESKTOP, &pt, 1);
		retID = TrackPopupMenuEx( popup, TPM_LEFTALIGN | TPM_TOPALIGN | TPM_RETURNCMD,
			pt.x, pt.y, hwnd, NULL);
		processMenuAction( popup, retID);
	}
	
	return true;
}

bool KWnd::OnMouseDown( int btn, int x, int y){
	int jBtn = ( btn & MK_RBUTTON ) ? 3 : 1;
	X->jEvt = JniEnv->CallStaticObjectMethod( MouseEvent, getMouseEvent,
		srcIdx, MOUSE_PRESSED,
		jBtn, x+insets.left, y+insets.top );
	return false;
}

bool KWnd::OnMouseUp( int btn, int x, int y){
	int jBtn = ( btn & MK_RBUTTON ) ? 3 : 1;
	X->jEvt = JniEnv->CallStaticObjectMethod( MouseEvent, getMouseEvent,
		srcIdx, MOUSE_RELEASED,
		jBtn, x+insets.left, y+insets.top );
	return false;
}

bool KWnd::OnMouseEnter( int x, int y){
	return false;
}

bool KWnd::OnMouseExit( int x, int y){
	return false;
}

bool KWnd::OnMouseMove( int x, int y){
	X->jEvt = JniEnv->CallStaticObjectMethod( MouseEvent, getMouseEvent,
		srcIdx, J_MOUSE_MOVED,
		1, x+insets.left, y+insets.top);
	return false;
}

bool KWnd::OnFocusChange ( bool gain) {
	X->jEvt = JniEnv->CallStaticObjectMethod( FocusEvent, getFocusEvent,
		srcIdx, gain ? FOCUS_GAINED : FOCUS_LOST, false);
	return false;
}

bool KWnd::OnChar( TCHAR c, int data) {
	return false;
}

bool KWnd::OnKeyDown( int virtKey, int data) {
	int scan, chr, mods;

	mods = convertKey( virtKey, &scan, &chr);
	//kprintf( "OnKeyDown: scan:%d[%x] data:%x chr:%d[%x] mods:%x\n", scan, scan, (data & 0x00ff0000) >> 16, chr, chr, mods);

	X->jEvt = JniEnv->CallStaticObjectMethod( KeyEvent, getKeyEvent,
		srcIdx, KEY_PRESSED, scan, chr, mods );
	
	return false;
}

bool KWnd::OnKeyUp( int virtKey, int data) {
	int scan, chr, mods;
	
	mods = convertKey( virtKey, &scan, &chr);
	//kprintf( "OnKeyUp: scan:%d[%x] chr:%d[%x] mods:%x\n", scan, scan, chr, chr, mods);
	
	X->jEvt = JniEnv->CallStaticObjectMethod( KeyEvent, getKeyEvent,
		srcIdx, KEY_RELEASED, scan, chr, mods );
	
	return false;
}

bool KWnd::OnClose() {
	return false;
}

int KWnd::getKeyMods() {
	int mods = 0;
	//kprintf( "%x %x %x\n", GetKeyState( VK_SHIFT), GetKeyState( VK_CONTROL), GetKeyState( VK_MENU) );
	if ( GetKeyState( VK_SHIFT) & 0xffffff00)	mods |= 1;
	if ( GetKeyState( VK_CONTROL) & 0xffffff00)	mods |= 2;
	if ( GetKeyState( VK_MENU) & 0xffffff00)	mods |= 8;
	
	return mods;
}

int KWnd::convertKey( int virtKey, int* scan, int* chr ) {
	static BYTE states[256];
	int mods, cpy;
	WORD ac;

	mods = getKeyMods();
	GetKeyboardState( states);
	cpy = ToAscii( virtKey, 0, states, &ac, 0);

	if ( cpy == 0 ) {
		*chr = 0;

		switch ( virtKey ) {
		case VK_DELETE:
			virtKey = 0x7F;
			break;
		case VK_INSERT:
			virtKey = 0x9B;
			break;
		case VK_TAB:
			//ctrl-tab KEY_TYPED generation ( tabbing in MLE )
			*chr = '\t';
			break;
		}
		*scan = virtKey;
	}
	else if ( cpy < 0 ) {	//dead key
	}
	else {
		ac &= 0x00FF;
		switch ( ac) {
		case VK_RETURN:
			virtKey = ac = '\n';
			break;
		}
		*scan = virtKey;
		*chr = ac;
	}

	return mods;

	/*
	if ( *chr = MapVirtualKey( virtKey, 2) ) {
		switch ( *chr ) {
		case VK_RETURN:
			*chr = '\n';
			break;
		}

		*scan = *chr;
		if ( (mods & 1) == 0 ) {
			if ( IsCharUpper( *chr) )
				*chr |= 0x20;
		}
	}
	else {
		switch ( virtKey ) {
		case VK_DELETE:
			virtKey = 0x7F;
			break;
		case VK_INSERT:
			virtKey = 0x9B;
			break;
		}
		*scan = virtKey;
		*chr = 0;
	}
	return mods;
	*/
}

jobject KWnd::getPreferredSize() {
	jclass dc = JniEnv->FindClass( "java/awt/Dimension");
	jmethodID ct = JniEnv->GetMethodID( dc, "<init>", "(II)V;");
	return (JniEnv->NewObject( dc, ct, 10, 10));
}

void KWnd::setText( jstring text) {
	if ( text) {
		SetWindowText( hwnd, java2WinString( JniEnv, X, text));
	}
	else {
		SetWindowText( hwnd, TEXT("") );
	}
}

TCHAR* KWnd::getText() {
	UINT len = GetWindowTextLength( hwnd);
	getBuffer(X, len+1);
	GetWindowText( hwnd, X->buf, len+1);
	return X->buf;
}

void KWnd::setVisible( jboolean b) {
	ShowWindow( hwnd, b ? SW_SHOWNORMAL : SW_HIDE);
}

void KWnd::setEnabled( jboolean b) {
	EnableWindow( hwnd, b);
}

void KWnd::setBounds(int x, int y, int width, int height) {
	KWnd* parent = (KWnd*)GetWindowLong( GetParent( hwnd), GWL_USERDATA );
	if ( parent ) {
		x -= parent->insets.left;
		y -= parent->insets.top;
	}
	
	SetWindowPos( hwnd, HWND_TOP, x, y, width, height, SWP_NOZORDER); // | SWP_NOREDRAW);
}

void KWnd::setForeground(COLORREF c) {
	fg = c;
}

void KWnd::setBackground(COLORREF c) {
	bg = c;
	DeleteObject( brush);
	brush = CreateSolidBrush( bg);
}

void KWnd::setFont(HFONT f) {
	fnt = f;
	SendMessage( hwnd, WM_SETFONT, (WPARAM)fnt, MAKELPARAM( true, 0) );
}

void KWnd::setCursor(HCURSOR cursor) {
	this->cursor = cursor;
}

void KWnd::setFocus() {
	PostMessage( X->wakeUp, KWM_REQUEST_FOCUS, (WPARAM)hwnd, 0);
}

bool KWnd::processMenuAction( HMENU menu, int id) {
	MENUITEMINFO mi;
	jmethodID getEvt;
	
	if ( menu ) {
		mi.cbSize = sizeof( MENUITEMINFO);
		mi.fMask = MIIM_DATA;
		
		if ( GetMenuItemInfo( menu, id, false, &mi) ) {
			getEvt = JniEnv->GetStaticMethodID( ActionEvent, "getMenuEvent",
				"(Ljava/awt/MenuItem;)Ljava/awt/ActionEvt;");
			JniEnv->CallStaticObjectMethod( ActionEvent, getEvt, (jobject)mi.dwItemData);
			
			return true;
		}
	}
	return false;
}

bool KWnd::updateInsets() {
	return false;
}

/*************************
*	exported functions
**************************/
extern "C" {
	
	void __cdecl
		Java_java_awt_Toolkit_cmnDestroyWindow ( JNIEnv* env, jclass clazz, KWnd* wnd )
	{
		PostMessage( X->wakeUp, KWM_DESTROY, (WPARAM)wnd->hwnd, 0);
	}

	void* __cdecl
		Java_java_awt_Toolkit_cmnGetPreferredSize( JNIEnv* env, jclass clazz, KWnd* wnd)
	{
		return wnd->getPreferredSize();
	}
	
	void __cdecl
		Java_java_awt_Toolkit_cmnSetBounds ( JNIEnv* env, jclass clazz, KWnd* wnd, jint x, jint y,
		jint width, jint height)
	{
		wnd->setBounds( x, y, width, height);
	}
	
	void __cdecl
		Java_java_awt_Toolkit_cmnSetForeground ( JNIEnv* env, jclass clazz, KWnd* wnd, jint clr)
	{
		wnd->setForeground( clr);
	}
	
	void __cdecl
		Java_java_awt_Toolkit_cmnSetBackground ( JNIEnv* env, jclass clazz, KWnd* wnd, jint clr)
	{
		wnd->setBackground( clr);
	}
	
	void __cdecl
		Java_java_awt_Toolkit_cmnSetFont ( JNIEnv* env, jclass clazz, KWnd* wnd, Font* fnt)
	{
		wnd->setFont( fnt->fnt);
	}
	
	void __cdecl
		Java_java_awt_Toolkit_cmnSetCursor ( JNIEnv* env, jclass clazz, KWnd* wnd, jint cursor)
	{
		wnd->setCursor( getCursor( cursor));
	}
	
	void __cdecl
		Java_java_awt_Toolkit_cmnSetFocus ( JNIEnv* env, jclass clazz, KWnd* wnd)
	{
		wnd->setFocus();
	}
	
	void __cdecl
		Java_java_awt_Toolkit_cmnSetVisible ( JNIEnv* env, jclass clazz, KWnd* wnd, jboolean vis)
	{
		wnd->setVisible( vis);
	}
	
	void __cdecl
		Java_java_awt_Toolkit_cmnSetEnabled ( JNIEnv* env, jclass clazz, KWnd* wnd, jboolean en)
	{
		wnd->setEnabled( en);
	}
	
	void __cdecl
		Java_java_awt_Toolkit_cmnValidate( JNIEnv* env, jclass clazz, KWnd* wnd)
	{
		UpdateWindow( wnd->hwnd);
	}
	
}
