package java.awt;

import java.awt.event.TextEvent;

class TextEvt
  extends java.awt.event.TextEvent
{
	static TextEvt cache;

TextEvt ( Object src, int evtId ) {
	super( src, evtId);
}

protected void dispatch () {
	((Component)source).process( this);

	if ( (Defaults.RecycleEvents & AWTEvent.TEXT_EVENT_MASK) != 0 )	recycle();
}

static synchronized TextEvt getEvent ( Object source, int id ){
	if ( cache == null ){
		return new TextEvt( source, id);
	}
	else {
		TextEvt e = cache;
		cache = (TextEvt)e.next;
		e.next = null;
		
		e.source = source;
		e.id = id;
		
		return e;
	}	
}

protected void recycle () {
	synchronized ( TextEvt.class ) {
		source = null;

		next = cache;	
		cache = this;
	}
}
}
