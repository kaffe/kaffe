package java.awt.event;

import java.util.EventListener;

/**
 * interface MouseListener - 
 *
 * Copyright (c) 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 *
 * @author P.C.Mehlitz
 */
public interface MouseListener
  extends EventListener
{
void mouseClicked ( MouseEvent evt );

/**
 * This method informs the listener that the mouse has entered
 * the source Component
 */
void mouseEntered ( MouseEvent evt );

void mouseExited ( MouseEvent evt );

void mousePressed ( MouseEvent evt );

void mouseReleased ( MouseEvent evt );
}
