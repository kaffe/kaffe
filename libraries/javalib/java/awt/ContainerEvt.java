package java.awt;

import java.awt.event.ContainerEvent;

class ContainerEvt
  extends ContainerEvent
{
	static ContainerEvt cache;

ContainerEvt ( Component src, int evtId, Component child ){
	super( src, evtId, child);
}

protected void dispatch () {
	((Component)source).process( this);

	if ( (Defaults.RecycleEvents & AWTEvent.CONTAINER_EVENT_MASK) != 0 ) recycle();
}

static synchronized ContainerEvt getEvent ( Component source, int id, Component child ){
	if ( cache == null ){
		return new ContainerEvt( source, id, child);
	}
	else {
		ContainerEvt e = cache;
		cache = (ContainerEvt)e.next;
		e.next = null;
		
		e.source = source;
		e.id = id;
		e.child = child;
		
		return e;
	}	
}

protected void recycle () {
	synchronized ( ContainerEvt.class ) {
		source = null;

		next = cache;	
		cache = this;
	}
}
}
