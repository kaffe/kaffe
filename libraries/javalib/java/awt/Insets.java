package java.awt;


/**
 * class Insets - decribes constraints of usable Container space
 *
 * Copyright (c) 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 *
 * @author P.C.Mehlitz
 */
public class Insets
  implements Cloneable, java.io.Serializable
{

	/** @serial
	 * The inset from the top.  This value is added to the Top of 
	 * the rectangle to yield a new location for the Top.
	 */
	public int top;

	/** @serial
	 * The inset from the left.  This value is added to the left of 
	 * the rectangle to yield a new location for the left edge.
	 */
	public int left;

	/** @serial
	 *  The inset from the bottom.  This value is added to the Bottom 
	 *  of the rectangle to yield a new location for the Bottom.
	 */
	public int bottom;

	/** @serial
	 *  The inset from the left.  This value is added to the left of 
	 *  the rectangle to yield a new location for the left edge.
	 */
	public int right;

	static Insets noInsets = new Insets();
	private static final long serialVersionUID = -2272572637695466749L;

Insets () {
}

public Insets ( int top, int left, int bottom, int right ) {
	this.top    = top;
	this.left   = left;
	this.bottom = bottom;
	this.right  = right;
}

public Object clone() { 
	try { 
		return super.clone();
	}
	catch ( CloneNotSupportedException x ) {
		return null;
	}
}

public boolean equals ( Object obj ) {
	if ( obj instanceof Insets ) {
		Insets in = (Insets) obj;
		return ((left == in.left) && (top == in.top) &&
	 	        (bottom == in.bottom) && (right == in.right));
	}
	else
		return false;
}

public String toString() {
	return getClass().getName() 
		+ "[top="  + top + ",left=" + left
		+ ",bottom=" + bottom + ",right=" + right + ']';
}
}
