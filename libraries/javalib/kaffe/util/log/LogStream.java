package kaffe.util.log;

import java.io.OutputStream;
import java.util.Vector;
import kaffe.util.DoubleLinkedObject;

public class LogStream
  extends OutputStream
{
	DoubleLinkedObject first;
	DoubleLinkedObject last;
	StringBuffer current = new StringBuffer( 80);
	int nLines;
	int limit;
	Object client;

public LogStream ( int maxLines ) {
	limit = maxLines;
}

public LogStream ( int maxLines, LogClient client ) {
	this.limit = maxLines;
	this.client = client;
}

public void addClient ( LogClient newClient ) {
	if ( client == null ) {
		client = newClient;
	}
	else if ( client instanceof Vector ) {
		((Vector)client).addElement( newClient);
	}
	else {
		Vector v = new Vector( 2);
		v.addElement( client);
		v.addElement( newClient);
		client = v;
	}
}

public DoubleLinkedObject getFirstLine () {
	return first;
}

public DoubleLinkedObject getLastLine () {
	return last;
}

void notifyClient () {
	if ( client != null ) {
		if ( client instanceof LogClient ) {
			((LogClient)client).newLogLine( this);
		}
		else if ( client instanceof Vector ) {
			Vector v = (Vector) client;
			int i, n = v.size();
			for ( i=0; i<n; i++ ){
				((LogClient)v.elementAt( i)).newLogLine( this);
			}
		}
	} 
}

public void removeClient ( LogClient oldClient ) {
	if ( client != null ) {
		if ( client instanceof Vector ) {
			((Vector)client).removeElement( oldClient);
		}
		else if ( client == oldClient ) {
			client = null;
		}
	}
}

public void write ( int b ) {
	if ( b == '\n' ) {
		if ( nLines > limit )
			first = first.next;
		
		DoubleLinkedObject newLine = new DoubleLinkedObject( new String( current), last, null);
		
		if ( first == null ){
			first = last = newLine;
		}
		else {
			last.next = newLine;
		}
		
		current.setLength( 0);
		nLines++;
		notifyClient();
	}
	else {
		current.append( (char)b);
	}
}
}
