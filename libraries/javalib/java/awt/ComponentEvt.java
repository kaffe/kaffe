package java.awt;

import java.awt.event.ComponentEvent;

class ComponentEvt
  extends ComponentEvent
{
	static ComponentEvt cache;

ComponentEvt ( Component c, int id ){
	super( c, id);
}

protected void dispatch () {
	((Component)source).processEvent( this);

	if ( (Defaults.RecycleEvents & AWTEvent.COMPONENT_EVENT_MASK) != 0 )	recycle();
}

static synchronized ComponentEvt getEvent ( Component source, int id ){
	if ( cache == null ){
		return new ComponentEvt( source, id);
	}
	else {
		ComponentEvt e = cache;
		cache = (ComponentEvt)e.next;
		e.next = null;
		
		e.source = source;
		e.id = id;
		
		return e;
	}	
}

static synchronized ComponentEvt getEvent ( int srcIdx, int id, int x, int y, int width, int height ){
	// This is exclusively called by the native event emitter

	ComponentEvt   e;
	Component      source = sources[srcIdx];

	if ( (Toolkit.flags & Toolkit.EXTERNAL_DECO) != 0 ) {
		Rectangle      d = source.deco;
		x -= d.x;
		y -= d.y;
		width  += d.width;
		height += d.height;
	}

	if ( (width != source.width) || (height != source.height) )
		source.invalidate();

	source.x = x;
	source.y = y;
	source.width = width;
	source.height = height;

	if ( (id == COMPONENT_MOVED) || (id == COMPONENT_RESIZED) )
		PopupWindow.checkPopup( source); // close any open popups

	if ( (source.flags & Component.IS_VALID) == 0 ){
		// if source is already visible, we wait for the subsequent
		// expose (fake repaint by temp changing visibility)
		if ( (source.flags & Component.IS_VISIBLE) != 0 ) {
			source.flags &= ~Component.IS_PARENT_SHOWING;
			source.propagateParentShowing();

			source.validate();

			source.flags |= Component.IS_PARENT_SHOWING;
			source.propagateParentShowing();
		}
		else {
			source.validate();
		}
	}
	
	if ( cache == null ){
		e = new ComponentEvt( source, id);
	}
	else {
		e = cache;
		cache = (ComponentEvt)e.next;
		e.next = null;

		e.source = source;
		e.id = id;
	}

	if ( (Toolkit.flags & Toolkit.NATIVE_DISPATCHER_LOOP) != 0 ) {
		// this is not used as a direct return value for EventQueue.getNextEvent(), 
		// it has to be Java-queued by the native layer
		Toolkit.eventQueue.postEvent( e);
	}

	return e;
}

protected void recycle () {
	synchronized ( ComponentEvt.class ) {
		source = null;

		next = cache;	
		cache = this;
	}
}
}
