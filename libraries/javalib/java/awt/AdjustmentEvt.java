package java.awt;

import java.awt.event.AdjustmentEvent;

class AdjustmentEvt
  extends AdjustmentEvent
{
	static AdjustmentEvt cache;

AdjustmentEvt ( Adjustable src, int evtId, int adjType, int adjVal ) {
	super( src, evtId, adjType, adjVal);
}

protected void dispatch () {
	((Component)source).processEvent( this);
	
	if ( (Defaults.RecycleEvents & AWTEvent.ADJUSTMENT_EVENT_MASK) != 0 ) recycle();
}

static synchronized AdjustmentEvt getEvent ( Adjustable source, int id, int adjType, int adjVal ){
	if ( cache == null ){
		return new AdjustmentEvt( source, id, adjType, adjVal);
	}
	else {
		AdjustmentEvt e = cache;
		cache = (AdjustmentEvt)e.next;
		e.next = null;
		
		e.source = source;
		e.id = id;
		e.adjType = adjType;
		e.adjVal = adjVal;
		
		return e;
	}	
}

protected void recycle () {
	synchronized ( AdjustmentEvt.class ) {
		source = null;

		next = cache;	
		cache = this;
	}
}

void setAdjustmentEvent( int adjType, int adjVal) {
	this.adjType = adjType;
	this.adjVal = adjVal;
}
}
