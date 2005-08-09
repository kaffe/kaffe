/**
 * NativeClipboard - System Clipboard (to handle native IPC)
 *
 * Copyright (c) 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 *
 * @author P. Mehlitz
 */

package java.awt;

import java.awt.datatransfer.Clipboard;
import java.awt.datatransfer.ClipboardOwner;
import java.awt.datatransfer.Transferable;

import kaffe.awt.ExportTransferable;
import gnu.classpath.Pointer;

class NativeClipboard
  extends Clipboard
{
	Pointer nativeData;
	static NativeClipboard singleton;

NativeClipboard () {
	super( "System");
	
	nativeData = Toolkit.cbdInitClipboard();
	singleton = this;
}

static Transferable createTransferable ( String mimeType, byte[] data ) {
	return new NativeSelection( mimeType, data);	
}

void dispose () {
	if ( nativeData != null ) {
		Toolkit.cbdFreeClipboard( nativeData);
		nativeData = null;
	}
}

public Transferable getContents ( Object requestor ) {
	if ( owner != null ) {  // can handle this internally, again
		return contents;
	}
	else {                  // check external sources
		return Toolkit.cbdGetContents( nativeData);
	}
}

static byte[] getNativeData ( String mimeType ) {
	Transferable t = singleton.contents;

	if ( t != null ) {
		if ( t instanceof ExportTransferable )
			return ((ExportTransferable)t).getNativeData( mimeType);
			
		// we should probably try to hardcode some "standard" conversion here, too
	}

	return null;
}

static void lostOwnership () {
	if ( singleton.owner != null ) {
		singleton.owner.lostOwnership( singleton, singleton.contents);
		singleton.owner = null;
		singleton.contents = null;
	}
}

public void setContents ( Transferable data, ClipboardOwner newOwner ) {
	if ( owner != null ) {          // can be handled internally
		if ( owner != newOwner ) {
			owner.lostOwnership( this, contents);
			owner = newOwner;
		}
		
		contents = data;
	}
	else {
		Toolkit.startDispatch(); // in case we don't have a eventThread yet
	
		if ( Toolkit.cbdSetOwner( nativeData) ) {
			owner = newOwner;
			contents = data;
		}
	}
}
}

