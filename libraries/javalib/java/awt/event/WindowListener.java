package java.awt.event;

import java.util.EventListener;

/**
 * interface WindowListener - 
 *
 * Copyright (c) 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 *
 * @author P.C.Mehlitz
 */

public interface WindowListener
 extends EventListener
{
void windowActivated ( WindowEvent evt );

void windowClosed ( WindowEvent evt );

void windowClosing ( WindowEvent evt );

void windowDeactivated ( WindowEvent evt );

void windowDeiconified ( WindowEvent evt );

void windowIconified ( WindowEvent evt );

void windowOpened ( WindowEvent evt );
}
