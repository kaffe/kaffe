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
	if ( AWTEvent.keyTgt != null )           // do we have a focus window?
		source = AWTEvent.keyTgt;
	else {
		if ( source == null )
			return;
		AWTEvent.keyTgt = (Component)source;   // hmm, just a defense line
	}

	if ( id == KEY_PRESSED ) {
		if ( keyChar == 0 ){
			// update modifiers for function keys
			switch ( keyCode ) {
			case VK_SHIFT:        inputModifier |= SHIFT_MASK; break;
			case VK_CONTROL:      inputModifier |= CTRL_MASK;  break;
			case VK_ALT:          inputModifier |= ALT_MASK;   break;
			case VK_META:         inputModifier |= META_MASK;
			}
		}

		// we interpret the setEnabled spec in a way that we don't emit events for disabled comps
		if ( (AWTEvent.keyTgt.eventMask & AWTEvent.DISABLED_MASK) == 0 ) {
			AWTEvent.keyTgt.process( this);

			if ( !consumed && !ShortcutHandler.handle( this) ) {
				if ( keyChar != 0 ) {         // printable key
					if ( AWTEvent.keyTgt != null ) {     // maybe a fast finger pulled the keyTgt under our feet
						KeyEvt typedEvt = getEvent((Component)source, KEY_TYPED, 0, keyChar, modifiers);
						AWTEvent.keyTgt.process(typedEvt);
					}
				}
			}
		}	
	}
	else if ( id == KEY_RELEASED ) {	
		if ( (AWTEvent.keyTgt.eventMask & AWTEvent.DISABLED_MASK) == 0 ) {
			AWTEvent.keyTgt.process( this);
		}

		if ( keyChar == 0 ) {
			switch ( keyCode ) {
			case VK_SHIFT:        inputModifier &= ~SHIFT_MASK; break;
			case VK_CONTROL:      inputModifier &= ~CTRL_MASK;  break;
			case VK_ALT:          inputModifier &= ~ALT_MASK;   break;
			case VK_META:         inputModifier &= ~META_MASK;
			}
		}
	}

	if ( (Defaults.RecycleEvents & AWTEvent.KEY_EVENT_MASK) != 0 )
		recycle();
}

static synchronized KeyEvt getEvent ( Component source, int id, int keyCode, int keyChar, int modifier ) {
	KeyEvt    e;
	long      when = System.currentTimeMillis();
	
	if ( cache == null ) {
		e = new KeyEvt( source, id, when, modifier, keyCode, (char)keyChar);
	}
	else {
		e = cache;
		cache = (KeyEvt) e.next;
		e.next = null;
	
		e.id         = id;
		e.source     = source;
		e.when       = when;
		e.modifiers  = modifier;
		e.keyCode    = keyCode;
		e.keyChar    = (char)keyChar;
		e.consumed	 = false;
	}

	return e;
}

static synchronized KeyEvt getEvent ( int srcIdx, int id, int keyCode, int keyChar, int modifier ) {
	KeyEvt    e;
	Component source = sources[srcIdx];
	long      when = System.currentTimeMillis();
	
	// Check for modifier keystrokes which have been "eaten" by the native window system.
	// Unfortunately, this can happen if the window manager temporarily grabs the keyboard
	// (e.g. fvwm2 during an initial window positioning)
	if ( (modifier == 0) && (inputModifier != 0) )
		inputModifier = 0;
	
	if ( cache == null ) {
		e = new KeyEvt( source, id, when, inputModifier, keyCode, (char)keyChar);
	}
	else {
		e = cache;
		cache = (KeyEvt) e.next;
		e.next = null;
	
		e.id         = id;
		e.source     = source;
		e.when       = when;
		e.modifiers  = inputModifier;
		e.keyCode    = keyCode;
		e.keyChar    = (char)keyChar;
		e.consumed	 = false;
	}

	if ( (Toolkit.flags & Toolkit.NATIVE_DISPATCHER_LOOP) != 0 ) {
		// this is not used as a direct return value for EventQueue.getNextEvent(), 
		// it has to be Java-queued by the native layer
		Toolkit.eventQueue.postEvent( e);
	}

	return e;
}

protected void recycle () {
	synchronized ( KeyEvt.class ) {
		source = null;

		next  = cache;
		cache = this;
	}
}
}
