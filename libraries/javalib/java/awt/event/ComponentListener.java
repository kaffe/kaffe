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
public void componentHidden ( ComponentEvent evt );

public void componentMoved ( ComponentEvent evt );

public void componentResized ( ComponentEvent evt );

public void componentShown ( ComponentEvent evt );
}
