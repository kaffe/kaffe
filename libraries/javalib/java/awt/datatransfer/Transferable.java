package java.awt.datatransfer;

import java.io.IOException;

/**
 * interface Transferable - 
 *
 * Copyright (c) 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 *
 * @author J.Mehlitz
 */
public interface Transferable
{
abstract public Object getTransferData ( DataFlavor flavor ) throws UnsupportedFlavorException, IOException;

abstract public DataFlavor[] getTransferDataFlavors();

abstract public boolean isDataFlavorSupported ( DataFlavor flavor );
}
