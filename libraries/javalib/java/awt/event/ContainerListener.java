package java.awt.event;

import java.util.EventListener;

/**
 * ContainerListener - 
 *
 * Copyright (c) 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 *
 * @author P. Mehlitz
 */
public interface ContainerListener
  extends EventListener
{
void componentAdded ( ContainerEvent e );

void componentRemoved ( ContainerEvent e );
}
