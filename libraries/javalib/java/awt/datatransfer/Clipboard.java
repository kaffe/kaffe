package java.awt.datatransfer;

import java.lang.String;

/**
 * Clipboard - 
 *
 * Copyright (c) 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 *
 * @author P. Mehlitz
 */
public class Clipboard
{
	protected String name;
	protected Transferable contents;
	protected ClipboardOwner owner;

public Clipboard( String name) {
	this.name = name;
}

public Transferable getContents( Object requestor) {
	return contents;
}

public String getName() {
	return name;
}

public void setContents ( Transferable newContents, ClipboardOwner newOwner ) {
	if ( (owner != null) && (owner != newOwner) )
		owner.lostOwnership( this, contents);

	owner = newOwner;
	contents = newContents;
}
}
