package java.util;


/*
 * Java core library component.
 *
 * Copyright (c) 1997, 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */
public class Observable
{
	Observer client;
	boolean hasChanged;

public Observable() {
}

public synchronized void addObserver ( Observer newClient ) {
	ObservableClient oc;

	if ( client == null ) {       // this is the first one (standard case)
		client = newClient;
	}
	else {
		if ( client == newClient )  // are you kidding? we don't add twice
			return;

		if ( client instanceof ObservableClient ){ // third and subsequent clients
			for ( oc = (ObservableClient)client; oc.next != null; oc = oc.next );
			oc.next = new ObservableClient( newClient);
		}
		else {                      // second client, open a list
			oc = new ObservableClient( client);
			oc.next = new ObservableClient( newClient);
			client = oc;
		}
	}
}

protected void clearChanged(){
	hasChanged = false;
}

public synchronized int countObservers(){
	ObservableClient oc;
	int n;
	
	if ( client == null ) return 0;
	
	if ( client instanceof ObservableClient ) {
		for ( oc=(ObservableClient)client, n=1; oc.next != null; oc = oc.next, n++ );
		return n;
	}
	else {
		return 1;
	}
}

public synchronized void deleteObserver ( Observer oldClient ) {
	ObservableClient oc;
	
	if ( client != null ) {
		if ( client instanceof ObservableClient ) {
			oc = (ObservableClient)client;
			
			if ( (oc.client == oldClient) && (oc.next == null ) ){
				client = null;
			}
			else {
				for ( ;oc.next != null; oc = oc.next ){
					if ( oc.next.client == oldClient ){
						oc.next = oc.next.next;
						break;
					}
				}
			}
		}
		else {
			if ( client == oldClient )
				client = null;
		}
	}
}

public synchronized void deleteObservers(){
	client = null;
}

public boolean hasChanged(){
	return hasChanged;
}

public void notifyObservers () {
	notifyObservers( null);
}

public synchronized void notifyObservers ( Object arg ) {
	if ( hasChanged && (client != null) ) {
		client.update( this, arg);
		hasChanged = false;
	}
}

protected void setChanged () {
	hasChanged = true;
}
}

class ObservableClient
  implements Observer
{
	Observer client;
	ObservableClient next;

ObservableClient ( Observer client ) {
	this.client = client;
}

public void update ( Observable server, Object arg ) {
	ObservableClient oc;
	
	for ( oc = this; oc != null; oc = oc.next )
		oc.client.update( server, arg);
}
}
