package kaffe.awt;

import java.awt.datatransfer.Clipboard;
import java.awt.datatransfer.ClipboardOwner;
import java.awt.datatransfer.DataFlavor;
import java.awt.datatransfer.Transferable;
import java.awt.datatransfer.UnsupportedFlavorException;
import java.io.IOException;

abstract public class ExportTransferable
  implements Transferable, ClipboardOwner
{
	protected Object data;
	protected DataFlavor[] flavors;
	protected FlavorConverter[] converters;

protected ExportTransferable ( Object data,
                               DataFlavor[] flavors,
                               FlavorConverter[] converters ) {
	this.data = data;
	this.flavors = flavors;
	this.converters = converters;
}

public byte[] getNativeData ( String mimeType ) {
	for ( int i=0; i<flavors.length; i++) {
		if ( flavors[i].isMimeTypeEqual( mimeType) ) {
			if ( converters[i] != null )
				return converters[i].exportObject( data);
		}
	}

	return null;
}

public Object getTransferData ( DataFlavor flavor )
         throws UnsupportedFlavorException, IOException{
	return null;
}

public DataFlavor[] getTransferDataFlavors() {
	return flavors;
}

public boolean isDataFlavorSupported ( DataFlavor flavor ) {
	for ( int i=0; i<flavors.length; i++ ) {
		if ( flavors[i].equals( flavor) )
			return true;
	}
	
	return false;
}

public void lostOwnership( Clipboard clipboard, Transferable contents) {
	// Hrmm, don't know why StringSelections should own the clipboard..
	// (since they are passed into the clipboard via setContents() by the real
	// owner). Probably just a convenience feature (so that the originator
	// does not have to be a ClipboardOwner)
}

public String toString () {
	return getClass().getName() + " [" + data + ']';
}
}
