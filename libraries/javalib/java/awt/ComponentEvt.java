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

	recycle();
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
	Component      source = sources[srcIdx];
	Rectangle      d = source.deco;
	int            w, h;

	w = width + d.width;
	h = height + d.height;

	if ( (w != source.width) || (h != source.height) )
		source.invalidate();

	source.x = x - d.x;
	source.y = y - d.y;	
	source.width = w;
	source.height = h;
	
	if ( !source.isValid ){
		// if source is already visible, we wait for the subsequent
		// expose (fake repaint by temp changing visibility)
		if ( source.isVisible ) {
			source.isVisible = false;
			source.validate();
			source.isVisible = true;
		}
		else {
			source.validate();
		}
	}
	
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

protected void recycle () {
	synchronized ( ComponentEvt.class ) {
		source = null;

		next = cache;	
		cache = this;
	}
}
}
