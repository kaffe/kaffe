package java.awt;

import java.awt.event.KeyEvent;

class KeyEvt
  extends KeyEvent
{
	static KeyEvt cache;

KeyEvt ( Component src, int evtId, long time, int mods, int kCode, char kChar ) {
	super( src, evtId, time, mods, kCode, kChar);
}

protected void dispatch () {
	if ( keyTgt != null )           // do we have a focus window?
		source = keyTgt;
	else
		keyTgt = (Component)source;   // hmm, just a defense line

	if ( id == KEY_PRESSED ) {
		accelHint = true;
		keyTgt.processEvent( this);
		
		if ( keyChar != 0 ) {         // printable key
			if ( keyTgt != null ) {     // maybe a fast finger pulled the keyTgt under our feet
				id = KEY_TYPED;
				keyCode = 0;
				keyTgt.processEvent( this);
			}
		}
		else {                        // function key, update modifiers
			switch ( keyCode ) {
			case VK_SHIFT:        inputModifier |= SHIFT_MASK; break;
			case VK_CONTROL:      inputModifier |= CTRL_MASK;  break;
			case VK_ALT:          inputModifier |= ALT_MASK;   break;
			case VK_META:         inputModifier |= META_MASK;
			}
		}
	}
	else if ( id == KEY_RELEASED ) {
		accelHint = false;
		keyTgt.processEvent( this);
	
	 	if ( keyChar == 0 ) {
			switch ( keyCode ) {
			case VK_SHIFT:        inputModifier &= ~SHIFT_MASK; break;
			case VK_CONTROL:      inputModifier &= ~CTRL_MASK;  break;
			case VK_ALT:          inputModifier &= ~ALT_MASK;   break;
			case VK_META:         inputModifier &= ~META_MASK;
			}
		}
	}
	
	if ( !consumed )
		HotKeyHandler.handle( this);
	
	recycle();
}

static synchronized KeyEvt getEvent ( int srcIdx, int id, int keyCode, int keyChar, int modifier ) {
	Component source = sources[srcIdx];
	long      when = System.currentTimeMillis();
	
	// Check for modifier keystrokes which have been "eaten" by the native window system.
	// Unfortunately, this can happen if the window manager temporarily grabs the keyboard
	// (e.g. fvwm2 during an initial window positioning)
	if ( (modifier == 0) && (inputModifier != 0) )
		inputModifier = 0;
	
	if ( cache == null ) {
		return new KeyEvt( source, id, when, inputModifier, keyCode, (char)keyChar);
	}
	else {
		KeyEvt e = cache;
		cache = (KeyEvt) e.next;
		e.next = null;
	
		e.id         = id;
		e.source     = source;
		e.when       = when;
		e.modifiers  = inputModifier;
		e.keyCode    = keyCode;
		e.keyChar    = (char)keyChar;
		e.consumed	 = false;
	
		return e;
	}
}

protected void recycle () {
	synchronized ( KeyEvt.class ) {
		source = null;

		next  = cache;
		cache = this;
	}
}
}
