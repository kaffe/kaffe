package java.awt.datatransfer;


/**
 * UnsupportedFlavorException - 
 *
 * Copyright (c) 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 *
 * @author J.Mehlitz
 */
public class UnsupportedFlavorException
  extends Exception
{
public UnsupportedFlavorException ( DataFlavor flavor ) {
	super( "unsupported DataFlavor: " + flavor.getHumanPresentableName());
}
}
