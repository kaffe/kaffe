/**
* scroll.cpp - scrollbar support
*
* Copyright (c) 1999
*      Transvirtual Technologies, Inc.  All rights reserved.
*
* See the file "license.terms" for information on usage and redistribution 
* of this file. 
*/

#include "toolkit.hpp"
#include "cmnwnd.hpp"

#define UNIT_INCREMENT		1;
#define UNIT_DECREMENT		2;
#define BLOCK_DECREMENT		3;
#define BLOCK_INCREMENT     4;
#define TRACK				5;

class KScroll:public KWnd {
public:		
	int min;
	int max;
	int pos;
	int page;
	SCROLLINFO si;
	
	KScroll( HWND parent, jboolean vert) {
		int style = vert ? SBS_VERT : SBS_HORZ;
		style |= DEF_WND_STYLE;
		hwnd = CreateWindow( TEXT("SCROLLBAR"), NULL, style, 0, 0, 0, 0,
			parent, (HMENU)0, NULL, NULL);
		defWndProc = (WNDPROC)SetWindowLong( hwnd, GWL_WNDPROC, (LONG)WndProc);
		initScroll();
	}
	
	bool KScroll::OnScroll( int trackPos, int op, bool vert) {
		int jop = -1;
		
		switch ( op) {
		case SB_PAGEDOWN:
			if ( pos < max-page+1) {
				jop = BLOCK_INCREMENT;
				pos = min( pos+page, max-page+1);
			}
			break;
		case SB_PAGEUP:
			if ( pos > min) {
				jop = BLOCK_DECREMENT;
				pos = max( pos-page, min);
			}
			break;
		case SB_LINEDOWN:
			if ( pos < max-page+1 ) {
				jop = UNIT_INCREMENT;
				pos = min( pos+1, max-page+1);
			}
			break;
		case SB_LINEUP:
			if ( pos > min ) {
				jop = UNIT_DECREMENT;
				pos = max( pos-1, min);
			}
			break;
		case SB_TOP:
			pos = min;
			break;
		case SB_BOTTOM:
			pos = max-page;
			break;
		case SB_THUMBTRACK:
			jop = TRACK;
			pos = trackPos;
			break;
		case SB_ENDSCROLL:
			break;
		case SB_THUMBPOSITION:
			break;
		}
		
		if ( jop > -1 ) {
			setPos( pos);
			JniEnv->CallStaticObjectMethod( AdjustmentEvent, getAdjustmentEvent,
				srcIdx, jop, pos );
		}
		
		return true;
	}

	void KScroll::initScroll() {
		si.cbSize = sizeof( SCROLLINFO);
		setValues( 0, 0, 100, 10);
	}

	void KScroll::setScroll() {
		if ( page < max-min ) {
			SetScrollInfo( hwnd, SB_CTL, &si, true);
		}
		else { 		//range completely visible
			EnableScrollBar( hwnd, SB_CTL, ESB_DISABLE_BOTH );
		}

	}

	int KScroll::getPos() {
		si.fMask = SIF_POS;
		GetScrollInfo( hwnd, SB_CTL, &si);
		return (si.nPos);
	}
	
	void KScroll::setPos( int pos) {
		si.fMask = SIF_POS;
		this->pos = si.nPos = pos;

		setScroll();
	}
	
	void KScroll::setValues( int pos, int min, int max, int page) {
		si.fMask = SIF_POS | SIF_RANGE | SIF_PAGE;
		this->min = si.nMin = min;
		this->max = si.nMax = max;
		this->pos = si.nPos = pos;
		this->page = si.nPage = page;

		setScroll();
	}
	
	void KScroll::setRange( int min, int max) {
		si.fMask = SIF_RANGE;
		this->min = si.nMin = min;
		this->max = si.nMax = max;

		setScroll();
	}
	
	void KScroll::setPage( int page) {
		si.fMask = SIF_PAGE;
		this->page = si.nPage = page;

		setScroll();
	}
	};
	
	
	
	
	/*************************
	*	exported functions
	**************************/
	extern "C" {
		
		void* __cdecl
			Java_java_awt_Toolkit_scrollCreateScrollbar ( JNIEnv* env, jclass clazz, KWnd* parent, jboolean vert)
		{
			return ( new KScroll( parent->hwnd, vert));
		}
		
		void __cdecl
			Java_java_awt_Toolkit_scrollSetValues ( JNIEnv* env, jclass clazz, KScroll* wnd, jint pos, jint min, jint max, jint page)
		{
			wnd->setValues( pos, min, max, page);
		}
		
		void __cdecl
			Java_java_awt_Toolkit_scrollSetPos ( JNIEnv* env, jclass clazz, KScroll* wnd, jint pos)
		{
			wnd->setPos( pos);
		}
		
		void __cdecl
			Java_java_awt_Toolkit_scrollSetRange ( JNIEnv* env, jclass clazz, KScroll* wnd, jint min, jint max)
		{
			wnd->setRange( min, max);
		}
		
		void __cdecl
			Java_java_awt_Toolkit_scrollSetPage ( JNIEnv* env, jclass clazz, KScroll* wnd, jint page)
		{
			wnd->setPage( page);
		}
		
		void __cdecl
			Java_java_awt_Toolkit_scrollSetOrientation ( JNIEnv* env, jclass clazz, KScroll* wnd, jboolean vert)
		{
			int cs = GetWindowLong( wnd->hwnd, GWL_STYLE);
			
			if ( vert )	{
				cs |= SBS_VERT;
				cs &= ~SBS_HORZ;
			}
			else {
				cs |= SBS_HORZ;
				cs &= ~SBS_VERT;
			}
			
			SetWindowLong( wnd->hwnd, GWL_STYLE, cs);
		}
		
	}
