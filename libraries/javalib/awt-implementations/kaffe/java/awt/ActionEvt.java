package java.awt;


class ActionEvt
  extends java.awt.event.ActionEvent
{
	private static ActionEvt cache;

ActionEvt ( Object src, int evtId, String cmd, int mod ) {
	super( src, evtId, cmd, mod);
}

protected void dispatch () {
	if ( source instanceof Component ){
		((Component)source).process( this);
	}
	else if ( source instanceof MenuItem ){
		((MenuItem)source).process( this);
	}

	if ( (Defaults.RecycleEvents & AWTEvent.ACTION_EVENT_MASK) != 0 )	recycle();
}

static synchronized ActionEvt getEvent ( Object source, int id, String cmd, int mods ){
	if ( cache == null ){
		return new ActionEvt( source, id, cmd, mods);
	}
	else {
		ActionEvt e = cache;
		cache = (ActionEvt)e.next;
		e.next = null;
		
		e.source = source;
		e.id = id;
		e.cmd = cmd;
		e.mods = mods;
		
		return e;
	}	
}

protected void recycle () {
	synchronized ( ActionEvt.class ) {
		source = null;

		next = cache;	
		cache = this;
	}
}
}
