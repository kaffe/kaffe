/**
 * class GridBagConstraints - 
 *
 * Copyright (c) 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 *
 * @author J.Mehlitz
 */

package java.awt;

import java.io.Serializable;

public class GridBagConstraints
  implements Cloneable, Serializable
{

private static final long serialVersionUID = -1000070633030801713L;
final public static int RELATIVE = -1;
final public static int REMAINDER = 0;
final public static int NONE = 0;
final public static int BOTH = 1;
final public static int HORIZONTAL = 2;
final public static int VERTICAL = 3;

final public static int CENTER = 10;
final public static int NORTH = 11;
final public static int NORTHEAST = 12;
final public static int EAST = 13;
final public static int SOUTHEAST = 14;
final public static int SOUTH = 15;
final public static int SOUTHWEST = 16;
final public static int WEST = 17;
final public static int NORTHWEST = 18;

public int gridx = RELATIVE;
public int gridy = RELATIVE;
public int gridwidth = 1;
public int gridheight = 1;
public double weightx = 0;
public double weighty = 0;
public int anchor = CENTER;
public int fill = NONE;
public int ipadx = 0;
public int ipady = 0;
public Insets insets = new Insets( 0, 0, 0, 0);

public GridBagConstraints() {
}

public Object clone() {
	try { 
		GridBagConstraints gc = (GridBagConstraints) super.clone();
		gc.insets = (Insets) insets.clone();
		return gc;
	}
	catch ( CloneNotSupportedException e) {
		return null;
	}
}
}
