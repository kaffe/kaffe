package java.awt;

import java.awt.event.AdjustmentListener;

/**
 * Adjustable - 
 *
 * Copyright (c) 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 *
 * @author P. Mehlitz
 */
public interface Adjustable
{
	final public static int HORIZONTAL = 0;
	final public static int VERTICAL = 1;

public void addAdjustmentListener ( AdjustmentListener listener );

public int getBlockIncrement ();

public int getMaximum ();

public int getMinimum ();

public int getOrientation ();

public int getUnitIncrement ();

public int getValue ();

public int getVisibleAmount ();

public void removeAdjustmentListener ( AdjustmentListener listener );

public void setBlockIncrement ( int inc );

public void setMaximum ( int max );

public void setMinimum ( int min );

public void setUnitIncrement ( int inc );

public void setValue ( int v );

public void setVisibleAmount ( int a );
}
