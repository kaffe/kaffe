package java.awt;

import java.awt.event.ItemListener;

/**
 * ItemSelectable - 
 *
 * Copyright (c) 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 *
 * @author P. Mehlitz
 */
public interface ItemSelectable
{
public void addItemListener ( ItemListener listener );

public Object[] getSelectedObjects ();

public void removeItemListener ( ItemListener listener );
}
