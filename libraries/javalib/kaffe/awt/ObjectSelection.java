package kaffe.awt;

import java.awt.datatransfer.ClipboardOwner;
import java.awt.datatransfer.DataFlavor;
import java.awt.datatransfer.Transferable;

public class ObjectSelection
  extends ExportTransferable
  implements Transferable, ClipboardOwner
{
	static DataFlavor[] objectFlavors;
	static FlavorConverter[] objectConverters;
	public static DataFlavor objectFlavor;

static {
	try {
		objectFlavor = new DataFlavor( Class.forName( "java.lang.Object"), 
		                                   "serialized Java object");
	}
	catch ( ClassNotFoundException x ) {}
	
	objectFlavors = new DataFlavor[1];
	objectFlavors[0] = objectFlavor;
	
	objectConverters = new FlavorConverter[1];
	objectConverters[0] = new SerializerConverter();
}

public ObjectSelection ( Object selData ) {
	super( selData, objectFlavors, objectConverters);
}
}
