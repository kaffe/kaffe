package java.awt.datatransfer;


/**
 * interface ClipboardOwner - 
 *
 * Copyright (c) 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 *
 * @author P.C.Mehlitz
 */
public interface ClipboardOwner
{
void lostOwnership ( Clipboard clipboard, Transferable contents );
}
