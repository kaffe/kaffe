package java.awt;

import java.awt.event.ComponentEvent;

class ComponentEvt
  extends ComponentEvent
{
	int x;
	int y;
	int width;
	int height;
	static ComponentEvt cache;

ComponentEvt ( Component c, int id ){
	super( c, id);
	
	x = c.x;
	y = c.y;
	width = c.width;
	height = c.height;
}

protected void dispatch () {
	Component src = (Component) source;

	if ( (src.parent == null) && ((id == COMPONENT_MOVED) || (id == COMPONENT_RESIZED)) ) {
		// Check for toplevel resizes and moves BEFORE processing this event. Each resize
		// has to cause a validation. This has been moved out of the getEvent() (which is
		// native called and therefore Toolkit synchronized), to avoid deadlock problems with
		// treelock synchronized methods (e.g. invalidate, validate, which might be called by
		// non-dispatcher threads, too)
	
		if ( (width != src.width) || (height != src.height) ) {
			src.invalidate();

			src.width = width;
			src.height = height;
		}

		src.x = x;
		src.y = y;

		if ( (src.flags & Component.IS_VALID) == 0 ){
			// if source is already visible, we wait for the subsequent
			// expose (fake repaint by temp changing visibility)
			if ( (src.flags & Component.IS_VISIBLE) != 0 ) {
				// The 'parentShowing' jitter is a waz to prevent superfluous
				// redraws and resident Graphics updates. Note that the final
				// propagateParentShowing has to be called with a "false" parameter
				// (to make sure it forces the required updates)
				src.flags &= ~Component.IS_PARENT_SHOWING;
				src.propagateParentShowing( true);

				src.validate();

				src.flags |= Component.IS_PARENT_SHOWING;
				src.propagateParentShowing( false);
			}
			else {
				src.validate();
			}
		}
	}

	src.process( this);

	if ( (Defaults.RecycleEvents & AWTEvent.COMPONENT_EVENT_MASK) != 0 )	recycle();
}

static synchronized ComponentEvt getEvent ( Component source, int id ){
	ComponentEvt e;

	if ( cache == null ){
		e = new ComponentEvt( source, id);
	}
	else {
		e = cache;
		cache = (ComponentEvt)e.next;
		e.next = null;
		
		e.source = source;
		e.id = id;
		
		e.x = source.x;
		e.y = source.y;
		e.width = source.width;
		e.height = source.height;
	}

	return e;
}

static synchronized ComponentEvt getEvent ( int srcIdx, int id, int x, int y, int width, int height ){
	// This is exclusively called by the native event emitter, be aware of the fact that
	// it therefore is Toolkit synchronized.

	ComponentEvt   e;
	Component      source = sources[srcIdx];

	if ( (Toolkit.flags & Toolkit.EXTERNAL_DECO) != 0 ) {
		// revert deco fake (the native side may not know about it)
		Rectangle      d = source.deco;
		x -= d.x;
		y -= d.y;
		width  += d.width;
		height += d.height;
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

  // store new coordinates for subsequent update / validation in dispatch()
	// (not here anymore because of deadlock problems for non-Toolkit treelocks)
	// !! Don't do any other native called (e.g. getEvent triggered) processing
	// !! (except of generating/queueing events) because we are now inconsistent
	// !! with the native layer until dispatch() is called
	e.x = x;
	e.y = y;
	e.width = width;
	e.height = height;


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
