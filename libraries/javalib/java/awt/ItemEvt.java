package java.awt;

import java.awt.event.ItemEvent;

class ItemEvt
  extends ItemEvent
{
	static ItemEvt cache;

ItemEvt ( ItemSelectable src, int evtId, Object item, int state ) {
	super( src, evtId, item, state);
}

protected void dispatch () {
	if ( source instanceof Component )
		((Component)source).process( this);
	else if ( source instanceof CheckboxMenuItem )
		((CheckboxMenuItem)source).process( this);
	
	if ( (Defaults.RecycleEvents & AWTEvent.ITEM_EVENT_MASK) != 0 )	recycle();
}

static synchronized ItemEvt getEvent ( ItemSelectable source, int id, Object item, int state ){
	if ( cache == null ){
		return new ItemEvt( source, id, item, state);
	}
	else {
		ItemEvt e = cache;
		cache = (ItemEvt)e.next;
		e.next = null;
		
		e.source = source;
		e.id = id;
		e.obj = item;
		e.op = state;
		
		return e;
	}	
}

static synchronized ItemEvt getEvent ( int srcIdx, int op, int idx ){
	// This is exclusively called by the native event emitter

	ItemEvt   e;
	Component      source = sources[srcIdx];
	Object 	           item = new Integer( idx);

	if ( cache == null ){
                e = new ItemEvt( (ItemSelectable)source, ITEM_STATE_CHANGED, item, op );
	}
	else {
		e = cache;
		cache = (ItemEvt)e.next;
		e.next = null;

		e.source = source;
                e.id = ITEM_STATE_CHANGED;
		e.obj = item;
                e.op = op;
	}

	if ( (Toolkit.flags & Toolkit.NATIVE_DISPATCHER_LOOP) != 0 ) {
		// this is not used as a direct return value for EventQueue.getNextEvent(), 
		// it has to be Java-queued by the native layer
		Toolkit.eventQueue.postEvent( e);
	}

        return e;
}

protected void recycle () {
	synchronized ( ItemEvt.class ) {
		source = null;

		next = cache;	
		cache = this;
	}
}

void setItemEvent( Object obj, int op) {
	this.obj = obj;
	this.op = op;
}
}
