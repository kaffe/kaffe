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
  implements Cloneable
{
	public int top;
	public int left;
	public int bottom;
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
	return getClass().getName() + " ["  + top + ',' + left
	                            + ',' + bottom + ',' + right + ']';
}
}
