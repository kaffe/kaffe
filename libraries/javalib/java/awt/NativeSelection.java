/**
 * NativeSelection - wrapper for external clipboard data
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

import java.awt.datatransfer.DataFlavor;
import java.awt.datatransfer.Transferable;
import java.awt.datatransfer.UnsupportedFlavorException;
import java.io.ByteArrayInputStream;

import kaffe.awt.FlavorConverter;
import kaffe.awt.ObjectSelection;
import kaffe.awt.SerializerConverter;

class NativeSelection
  implements Transferable
{
	byte[] data;
	String mimeType;
	DataFlavor[] flavors;
	FlavorConverter[] converters;
	static DataFlavor[] textFlavors;
	static FlavorConverter[] textConverters;
	static DataFlavor[] objectFlavors;
	static FlavorConverter[] objectConverters;

static {
	textFlavors = new DataFlavor[2];
	textFlavors[0] = DataFlavor.stringFlavor;
	textFlavors[1] = DataFlavor.plainTextFlavor;
	
	textConverters = new FlavorConverter[2];
	textConverters[0] = new FlavorConverter () {
		public Object importBytes( byte[] data) {	return new String( data);	}
		public byte[] exportObject ( Object obj ) { return null; }
	};
	textConverters[1] = new FlavorConverter () {
		public Object importBytes( byte[] data) { return new ByteArrayInputStream( data); }
		public byte[] exportObject ( Object obj) { return null; }
	};
	
	objectFlavors = new DataFlavor[1];
	objectFlavors[0] = ObjectSelection.objectFlavor;
	
	objectConverters = new FlavorConverter[1];
	objectConverters[0] = new SerializerConverter();
}

NativeSelection ( String mimeType, byte[] data ) {
	this.mimeType = mimeType;
	this.data = data;
	
	setFlavors();
}

public Object getTransferData ( DataFlavor flavor )
													throws UnsupportedFlavorException {
	for ( int i=0; i<flavors.length; i++) {
		if ( flavors[i].equals( flavor) )
			return converters[i].importBytes( data);
	}
	
	throw new UnsupportedFlavorException( flavor);
}

public DataFlavor[] getTransferDataFlavors() {
	return flavors;
}

public boolean isDataFlavorSupported ( DataFlavor flavor ) {
	for ( int i=0; i<flavors.length; i++) {
		if ( flavors[i].equals( flavor) )
			return true;
	}

	return false;
}

void setFlavors() {
	if ( mimeType.startsWith( "text/plain") ) {
		flavors    = textFlavors;
		converters = textConverters;
	}
	else if ( mimeType.startsWith( "application/x-java-serialized-object") ) {
		flavors    = objectFlavors;
		converters = objectConverters;
	}
	else {
		flavors    = new DataFlavor[0];
		converters = new FlavorConverter[0];
	}
}

public String toString () {
	int i;
	StringBuffer s = new StringBuffer( 50);
	
	s.append( "NativeSelection [length: ");
	s.append( data.length);
	s.append( ", ");

	for ( i=0; i<8 && i<data.length; i++ ){
		s.append( Integer.toHexString( data[i] & 0xff));
		s.append( ' ');
	}
	if ( i < data.length )
		s.append( "...");

	s.append( ']');
	
	return s.toString();
}
}
