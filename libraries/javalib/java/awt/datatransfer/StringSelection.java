/**
 * StringSelection - 
 *
 * Copyright (c) 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 *
 * @author P. Mehlitz
 */

package java.awt.datatransfer;

import java.lang.String;
import java.io.StringReader;
import kaffe.awt.ExportTransferable;
import kaffe.awt.FlavorConverter;
import kaffe.awt.SerializerConverter;

public class StringSelection
  extends ExportTransferable
  implements Transferable, ClipboardOwner
{
	static DataFlavor[] stringFlavors;
	static FlavorConverter[] stringConverters;

static {
	stringFlavors = new DataFlavor[2];
	stringFlavors[0] = DataFlavor.stringFlavor;
	stringFlavors[1] = DataFlavor.plainTextFlavor;
	
	stringConverters = new FlavorConverter[2];
	stringConverters[0] = new SerializerConverter ();
	stringConverters[1] = new FlavorConverter () {
		public Object importBytes( byte[] data) {	return null;	}
		public byte[] exportObject ( Object obj ) {
			return ((String)obj).getBytes();
		}	
	};
}

public StringSelection ( String selData ) {
	super( selData, stringFlavors, stringConverters);
}

public Object getTransferData ( DataFlavor flavor ) throws UnsupportedFlavorException {

	if ( flavor.equals( DataFlavor.stringFlavor) )
		return data;
	else if	( flavor.equals( DataFlavor.plainTextFlavor) ) {
		// StringSel data requested as plainText appears to be returned as
		// StringReader instance - but how does this comply to the DataFlavor
		// representationClass 'java.io.InputStream' ?
		return new StringReader( (String)data);
	}
	
	throw new UnsupportedFlavorException( flavor);
}
}
