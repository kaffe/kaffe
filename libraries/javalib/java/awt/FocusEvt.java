package java.awt;

import java.awt.event.FocusEvent;
import java.awt.event.WindowEvent;

class FocusEvt
  extends FocusEvent
{
	static FocusEvt cache;
	static Component keyTgtRequest;
	static Window newActiveWindow;
	static boolean temporaryHint;

FocusEvt ( Component src, int evtId ) {
	super( src, evtId);
}

static void checkActiveWindow( Component c) {
	Component top = c.getToplevel();

	if ( (top != AWTEvent.activeWindow) && (top != null) ) {
		FocusEvt e = getEvent( top, FocusEvent.FOCUS_GAINED);
		e.dispatch();
	}
}

protected void dispatch () {
	WindowEvt  we;
	Component  src = (Component) source;
	Component  lastTgt;

	// We don't support 'isTemporary' in the native layer. The spec says that it
	// should be set for events where the focus is changed as "a indirect result
	// of another operation.. and will automatically be restored once this operation
	// is finished". Oh, great! That is exactly were the native windowing system
	// kicks in, and there are not even two of them doing this in the same way. The best
	// approximation we can do is to set this if we show owned Windows, but even here the
	// focus could wander away to a external toplevel, never to be restored. Anyway, this
	// simple approach makes Swing happy for the moment
	if ( temporaryHint ) {
		isTemporary = true;
		temporaryHint = false;
	}

	if ( id == FOCUS_GAINED ) {
		if ( (keyTgtRequest != null) ) {
			// There is an obscure WM behavior (e.g. in KWM), first sending
			// a FOCUS_LOST to a freshly opened window. If we just reset keyTgtRequest
			// on FOCUS_LOST, we loose the forwarding. If we don't do that, we better
			// check here that the keyTgtRequest is for us
			if ( getToplevel( keyTgtRequest) == src )
				src = keyTgtRequest;
			keyTgtRequest = null;
		}		

		if ( (AWTEvent.keyTgt != null) && (AWTEvent.keyTgt != src) ) {
			id = FOCUS_LOST;
			source = lastTgt = AWTEvent.keyTgt;
			AWTEvent.keyTgt = src;
			lastTgt.processEvent( this);
		  id = FOCUS_GAINED;
		}

		// we should make the next active window known (within java.awt)
		// prior to sending the DEACTIVATED messages because RootWindow
		// instances might have to react on this
		newActiveWindow = (Window) getToplevel( src);
		if ( (AWTEvent.activeWindow != null) && (newActiveWindow != AWTEvent.activeWindow)
		       && ((newActiveWindow instanceof Frame) || (newActiveWindow == root)) ) {
			we = WindowEvt.getEvent( AWTEvent.activeWindow, WindowEvent.WINDOW_DEACTIVATED);
			AWTEvent.activeWindow.processEvent( we);
		}

		AWTEvent.keyTgt = src;
		source = src;
		src.processEvent( this);
		
		ShortcutHandler.buildCodeTable( src);
		
		if ( (newActiveWindow != AWTEvent.activeWindow) && (newActiveWindow instanceof Frame) ) {
			we = WindowEvt.getEvent( newActiveWindow, WindowEvent.WINDOW_ACTIVATED);
			newActiveWindow.processEvent( we);
			AWTEvent.activeWindow = (Window) newActiveWindow;
		}
		else if ( newActiveWindow == root )
			AWTEvent.activeWindow = null;
	}
	else if ( id == FOCUS_LOST ) {
		if ( src == AWTEvent.activeWindow ) { // native generated focus lost
			we = WindowEvt.getEvent( AWTEvent.activeWindow, WindowEvent.WINDOW_DEACTIVATED);
			AWTEvent.activeWindow.processEvent( we);
			AWTEvent.activeWindow = null;
			
			// seems to be silly, but JDK marks *all* toplevel losts as temporary
			// we wouldn't need that for swing, since we try harder with 'temporaryHint', but
			// there are some native window managers (Enlightenment) having trouble with
			// 'owned' window focus
			isTemporary = true;
		}

		if ( AWTEvent.keyTgt != null ) {
			src    = AWTEvent.keyTgt;
			source = src;
			AWTEvent.keyTgt = null;
			inputModifier = 0; // just a matter of safety (a reset point)

			src.processEvent( this);			
		}
	}
	
	if ( (Defaults.RecycleEvents & AWTEvent.FOCUS_EVENT_MASK) != 0 )	recycle();
}

static synchronized FocusEvt getEvent ( Component source, int id ) {
	if ( cache == null ){
		return new FocusEvt( source, id);
	}
	else {
		FocusEvt e = cache;
		cache = (FocusEvt) e.next;
		e.next = null;
		
		e.id = id;
		e.source = source;
		e.isTemporary = false;

		return e;
	}
}

static synchronized FocusEvt getEvent ( int srcIdx, int id ) {
	FocusEvt  e;
	Component source = sources[srcIdx];

	if ( source == null ) return null;

	if ( cache == null ){
		e = new FocusEvt( source, id);
	}
	else {
		e = cache;
		cache = (FocusEvt) e.next;
		e.next = null;
		
		e.id = id;
		e.source = source;
		e.isTemporary = false;
	}

	if ( (Toolkit.flags & Toolkit.NATIVE_DISPATCHER_LOOP) != 0 ) {
		// this is not used as a direct return value for EventQueue.getNextEvent(), 
		// it has to be Java-queued by the native layer
		Toolkit.eventQueue.postEvent( e);
	}
	
	return e;
}

protected boolean isLiveEventFor ( Object src ) {
	return (source == src);
}

protected void recycle () {
	synchronized ( FocusEvt.class ) {
		source = null;

		next = cache;	
		cache = this;
	}
}

static void sendFocusEvent ( Component src, boolean focusGained, boolean sync ) {
	int id = focusGained ? FOCUS_GAINED : FOCUS_LOST;
	FocusEvt e = getEvent( src, id);

	if ( sync )
		e.dispatch();
	else
		Toolkit.eventQueue.postEvent( e);
}
}
