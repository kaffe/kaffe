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
