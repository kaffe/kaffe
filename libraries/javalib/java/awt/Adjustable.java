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
	int HORIZONTAL = 0;
	int VERTICAL = 1;

void addAdjustmentListener ( AdjustmentListener listener );

int getBlockIncrement ();

int getMaximum ();

int getMinimum ();

int getOrientation ();

int getUnitIncrement ();

int getValue ();

int getVisibleAmount ();

void removeAdjustmentListener ( AdjustmentListener listener );

void setBlockIncrement ( int inc );

void setMaximum ( int max );

void setMinimum ( int min );

void setUnitIncrement ( int inc );

void setValue ( int v );

void setVisibleAmount ( int a );
}
