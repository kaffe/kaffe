package java.awt.event;


/**
 * class WindowAdapter - 
 *
 * Copyright (c) 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 *
 * @author P.C.Mehlitz
 */

abstract public class WindowAdapter
  implements WindowListener
{
public void windowActivated ( WindowEvent evt ) {
}

public void windowClosed ( WindowEvent evt ) {
}

public void windowClosing ( WindowEvent evt ) {
}

public void windowDeactivated ( WindowEvent evt ) {
}

public void windowDeiconified ( WindowEvent evt ) {
}

public void windowIconified ( WindowEvent evt ) {
}

public void windowOpened ( WindowEvent evt ) {
}
}
