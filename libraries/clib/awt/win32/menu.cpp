/**
* menu.cpp - menu support functions
*
* Copyright (c) 1999
*      Transvirtual Technologies, Inc.  All rights reserved.
*
* See the file "license.terms" for information on usage and redistribution 
* of this file. 
*/

#include "toolkit.hpp"
#include "cmnwnd.hpp"

#if defined(UNDER_CE)
#include <Commctrl.h>
extern "C" HINSTANCE appInstance;
#endif

extern UINT KWM_CONTEXTMENU;

class KMenu {
public:	
	HMENU hMenu;
	jboolean isPopup;
#if defined(UNDER_CE)
	KWnd* parent;
#endif
	
	KMenu( jboolean popup) {
		isPopup = popup;
		if ( isPopup ) {
			hMenu = CreatePopupMenu();
		}
		else {
			hMenu = CreateMenu();
		}
	}
	
	void KMenu::removeItem( USHORT id) {
		DeleteMenu( hMenu, id, MF_BYCOMMAND);
	}
	
	void KMenu::checkItem( USHORT id, jboolean state) {
		int ws = state ? MF_CHECKED : MF_UNCHECKED;
		CheckMenuItem( hMenu, id, MF_BYCOMMAND | ws);
	}
	
	void KMenu::enableItem( USHORT id, jboolean state) {
		int ws = state ? MF_ENABLED : MF_GRAYED;
		EnableMenuItem( hMenu, id, MF_BYCOMMAND | ws);
	}
	
	void KMenu::insertItem( TCHAR* item, int id, int pos, HMENU sub) {

#if !defined(UNDER_CE)
		MENUITEMINFO mi;
		mi.cbSize = sizeof( MENUITEMINFO);
		mi.fMask = MIIM_ID;
		mi.wID	 = (USHORT)id;
		
		if ( (item == NULL) || (*item == '-') ) {
			mi.fType = MFT_SEPARATOR;
			mi.fMask |= MIIM_TYPE;
		}
		else if ( sub) {
			mi.fType = MFT_STRING;
			mi.fMask |= MIIM_TYPE | MIIM_SUBMENU;
			mi.hSubMenu = sub;
			mi.dwTypeData = item;
			mi.cch = _tcslen( item);
		}
		else {
			mi.fMask |= MIIM_DATA | MIIM_TYPE;
			mi.fType = MFT_STRING;
			mi.dwItemData = id;
			mi.dwTypeData = item;
			mi.cch = _tcslen( item);
		}
		if ( pos == -1 ) {
			InsertMenuItem( hMenu, mi.wID, FALSE, &mi);
		}
		else {
			InsertMenuItem( hMenu, pos, TRUE, &mi);
		}
#else
		/* CE doesn't have InsertMenuItem yet, revert to old Append/InsertMenu
		 */
        UINT flags = (!item || (*item=='-')) ? MF_SEPARATOR : (sub ? MF_POPUP : MF_STRING);
		UINT mid = (UINT) (sub ? (UINT)sub : id);

		if ( pos == -1 ) {
			AppendMenu( hMenu, flags, mid, item);
		}
		else {
			InsertMenu( hMenu, pos, flags, mid, item);
		}
#endif
	}
	
	void KMenu::assign( KWnd* parent, int x, int y) {
		
		if ( ! isPopup) {
#if !defined(UNDER_CE)
			SetMenu( parent->hwnd, hMenu);
#else
			/* CE doesn't attach menubars to Frames, but to "CommandBars", filled
			 * with menus and buttons.
			 */
			CommandBar_InsertMenubarEx(parent->hbar, NULL, (TCHAR*)hMenu, 0);
			this->parent = parent;
#endif
			parent->updateInsets();
		}
		else {
			parent->popup = hMenu;
			x -= parent->insets.left;
			y -= parent->insets.top;
			PostMessage( parent->hwnd, KWM_CONTEXTMENU, 
				(WPARAM)parent->hwnd, MAKELPARAM( x, y) );
		}
	}
	
	KMenu::~KMenu() {
		DestroyMenu( hMenu);
	}
	
	
};

extern "C" {
	
	void* __cdecl
		Java_java_awt_Toolkit_menuCreateMenu ( JNIEnv* env, jclass clazz, jboolean popup)
	{
		return ( new KMenu( popup));
	}
	
	void __cdecl
		Java_java_awt_Toolkit_menuRedraw ( JNIEnv* env, jclass clazz, KWnd* parent)
	{
#if defined(UNDER_CE)
		CommandBar_DrawMenuBar(parent->hbar, 0);
#endif
	}
	
	void __cdecl
		Java_java_awt_Toolkit_menuInsertItem ( JNIEnv* env, jclass clazz, KMenu* menu, KMenu* sub, jstring item, int pos, jobject id)
	{
		TCHAR* lbl = item ? java2WinString( env, X, item) : NULL;
		menu->insertItem( lbl, (int)id, pos, sub ? sub->hMenu : NULL);
	}
	
	void __cdecl
		Java_java_awt_Toolkit_menuRemoveItem ( JNIEnv* env, jclass clazz, KMenu* menu, jobject id)
	{
		menu->removeItem( (USHORT)id);
	}
	
	void __cdecl
		Java_java_awt_Toolkit_menuDeleteMenu ( JNIEnv* env, jclass clazz, KMenu* menu)
	{
		delete( menu) ;
	}
	
	void __cdecl
		Java_java_awt_Toolkit_menuCheckItem ( JNIEnv* env, jclass clazz, KMenu* menu, jobject id, jboolean state)
	{
		menu->checkItem( (USHORT)id, state);
	}
	
	void __cdecl
		Java_java_awt_Toolkit_menuEnableItem ( JNIEnv* env, jclass clazz, KMenu* menu, jobject id, jboolean state)
	{
		menu->enableItem( (USHORT)id, state);
	}
	
	void __cdecl
		Java_java_awt_Toolkit_menuAssign ( JNIEnv* env, jclass clazz, KWnd* parent, KMenu* menu, jint x, jint y)
	{
		if ( ! menu) {
#if !defined(UNDER_CE)
			SetMenu( parent->hwnd, NULL);
#else
			CommandBar_InsertMenubarEx(parent->hbar, appInstance, NULL, 0);
#endif
			parent->updateInsets();
		}
		else {
#if defined(UNDER_CE)
			if ( (parent->hbar == NULL) && !menu->isPopup ) {
				parent->hbar = CommandBar_Create(appInstance, parent->hwnd, 0);
			}
#endif
			menu->assign( parent, x, y);
		}
	}
	
}
