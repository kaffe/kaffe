package java.awt.event;

import java.util.EventListener;

/**
 * interface ComponentListener - 
 *
 * Copyright (c) 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 *
 * @author P.C.Mehlitz
 */

public interface ComponentListener
 extends EventListener
{
void componentHidden ( ComponentEvent evt );

void componentMoved ( ComponentEvent evt );

void componentResized ( ComponentEvent evt );

void componentShown ( ComponentEvent evt );
}
