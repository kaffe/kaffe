package java.awt.event;

import java.util.EventListener;

/**
 * interface ActionListener - 
 *
 * Copyright (c) 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 *
 * @author P.C.Mehlitz
 */

public interface ActionListener
  extends EventListener
{
void actionPerformed(ActionEvent e);
}
