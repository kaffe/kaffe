package java.awt;


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
public class GridBagConstraints
  implements Cloneable
{
	final public static int RELATIVE = -1;
	final public static int REMAINDER = 0;
	final public static int NONE = 0;
	final public static int HORIZONTAL = 1;
	final public static int VERTICAL = 2;
	final public static int BOTH = 3;
	final public static int NORTHWEST = 0x00;
	final public static int NORTH = 0x01;
	final public static int NORTHEAST = 0x02;
	final public static int WEST = 0x10;
	final public static int CENTER = 0x11;
	final public static int EAST = 0x12;
	final public static int SOUTHWEST = 0x20;
	final public static int SOUTH = 0x21;
	final public static int SOUTHEAST = 0x22;
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
