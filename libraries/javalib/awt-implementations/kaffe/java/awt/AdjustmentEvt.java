package java.awt;

import java.awt.event.AdjustmentEvent;

class AdjustmentEvt
  extends AdjustmentEvent
{
	private static AdjustmentEvt cache;

AdjustmentEvt ( Adjustable src, int evtId, int adjType, int adjVal ) {
	super( src, evtId, adjType, adjVal);
}

protected void dispatch () {
	((Component)source).process( this);
	
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

static synchronized AdjustmentEvt getEvent ( int srcIdx, int op, int val){
	// This is exclusively called by the native event emitter

        	AdjustmentEvt   e;
	Component      source = sources[srcIdx];

	if ( cache == null ){
		e = new AdjustmentEvt( (Adjustable)source, ADJUSTMENT_VALUE_CHANGED, op,val );
	}
	else {
		e = cache;
		cache = (AdjustmentEvt)e.next;
		e.next = null;

		e.source = source;
		e.id = ADJUSTMENT_VALUE_CHANGED;
		e.adjType = op;
		e.adjVal = val;
	}

	if ( (Toolkit.flags & Toolkit.NATIVE_DISPATCHER_LOOP) != 0 ) {
		// this is not used as a direct return value for EventQueue.getNextEvent(), 
		// it has to be Java-queued by the native layer
		Toolkit.eventQueue.postEvent( e);
	}

        return e;
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
