package java.awt;

import java.io.Serializable;

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
  implements Cloneable, Serializable
{
	final private static long serialVersionUID = -1000070633030801713L;
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
	public double weightx;
	public double weighty;
	public int anchor = CENTER;
	public int fill = NONE;
	public int ipadx;
	public int ipady;
	public Insets insets = new Insets( 0, 0, 0, 0);

public GridBagConstraints() {
}

/**
 * Creates a GridBagConstraints object with all of its fields set to the
 * passed-in arguments.
 * Note: Because the use of this constructor hinders readability of source
 * code, this constructor should only be used by automatic source code
 * generation tools.
 *
 * @since JDK1.2
 */
public GridBagConstraints(int gridx,
			  int gridy,
			  int gridwidth,
			  int gridheight,
			  double weightx,
			  double weighty,
			  int anchor,
			  int fill,
			  Insets insets,
			  int ipadx,
			  int ipady)
{
	this.gridx = gridx;
	this.gridy = gridy;
	this.gridwidth = gridwidth;
	this.gridheight = gridheight;
	this.weightx = weightx;
	this.weighty = weighty;
	this.anchor = anchor;
	this.fill = fill;
	this.insets = insets;
	this.ipadx = ipadx;
	this.ipady = ipady;
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

public String toString() {
	String san, sfl;
	
	switch ( anchor ) {
		case NORTH:
			san = "north";
			break;
		case NORTHEAST:
			san = "northeast";
			break;
		case EAST:
			san = "east";
			break;
		case SOUTHEAST:
			san = "southeast";
			break;
		case SOUTH:
			san = "south";
			break;
		case SOUTHWEST:
			san = "southwest";
			break;
		case WEST:
			san = "west";
			break;
		case NORTHWEST:
			san = "northwest";
			break;
		default:
			san = "center";
	}
	
	switch ( fill ) {
		case HORIZONTAL:
			sfl = "horz";
			break;
		case VERTICAL:
			sfl = "vert";
			break;
		case BOTH:
			sfl = "both";
			break;
		default:
			sfl = "none";			
	}
	
	return ("Constraint:" + gridx + "," + gridy + " " + gridwidth + "x" + gridheight +
			",fill:" + sfl + ",anchor:" + san + ",weight:" + weightx + "x" + weighty);
}
}
