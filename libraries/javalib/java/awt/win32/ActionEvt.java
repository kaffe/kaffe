package java.awt;

import java.awt.AWTEvent;
import java.awt.Component;
import java.awt.MenuItem;
import java.awt.Toolkit;

/**
 * class ActionEvt -
 *
 * Copyright (c) 1999
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

class ActionEvt
  extends java.awt.event.ActionEvent
{
	static ActionEvt cache;

ActionEvt ( Object src, int evtId, String cmd, int mod ) {
	super( src, evtId, cmd, mod);
}

protected void dispatch () {
	if ( source instanceof Component )
		((Component)source).process( this);
	else if ( source instanceof MenuItem )
		((MenuItem)source).process( this);

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

static synchronized ActionEvt getEvent ( int srcIdx, String cmd, int mods){
	// This is exclusively called by the native event emitter

	ActionEvt   e;
	Component      source = sources[srcIdx];

	if ( cache == null ){
		e = new ActionEvt( source, ACTION_PERFORMED, cmd,mods );
	}
	else {
		e = cache;
		cache = (ActionEvt)e.next;
		e.next = null;

		e.source = source;
		e.id = ACTION_PERFORMED;
		e.cmd = cmd;
		e.mods = mods;
	}

	if ( (Toolkit.flags & Toolkit.NATIVE_DISPATCHER_LOOP) != 0 ) {
		// this is not used as a direct return value for EventQueue.getNextEvent(), 
		// it has to be Java-queued by the native layer
		Toolkit.eventQueue.postEvent( e);
	}

	return e;
}

static synchronized ActionEvt getMenuEvent ( MenuItem source){
	// This is exclusively called by the native event emitter
	// resolved because MenuItems are no registered awt sources
	// ( not reachable by source index )

	ActionEvt   e;
	String ac = source.getActionCommand();

	if ( cache == null ){
		e = new ActionEvt( source, ACTION_PERFORMED, ac, 0 );
	}
	else {
		e = cache;
		cache = (ActionEvt)e.next;
		e.next = null;

		e.source = source;
		e.id = ACTION_PERFORMED;
		e.cmd = ac;
		e.mods = 0;
	}

	if ( (Toolkit.flags & Toolkit.NATIVE_DISPATCHER_LOOP) != 0 ) {
		// this is not used as a direct return value for EventQueue.getNextEvent(), 
		// it has to be Java-queued by the native layer
		Toolkit.eventQueue.postEvent( e);
	}

	return e;
}

protected void recycle () {
	synchronized ( ActionEvt.class ) {
		source = null;

		next = cache;	
		cache = this;
	}
}
}
