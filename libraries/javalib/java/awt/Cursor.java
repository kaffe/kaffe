package java.awt;


/**
 * Cursor - class to access predefined standard native cursors
 *
 * Copyright (c) 1998
 *   Transvirtual Technologies Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 * @author P.C.Mehlitz
 */
public class Cursor implements java.io.Serializable
{
	final public static int CUSTOM_CURSOR = -1;
	final public static int DEFAULT_CURSOR = 0;
	final public static int CROSSHAIR_CURSOR = 1;
	final public static int TEXT_CURSOR = 2;
	final public static int WAIT_CURSOR = 3;
	final public static int SW_RESIZE_CURSOR = 4;
	final public static int SE_RESIZE_CURSOR = 5;
	final public static int NW_RESIZE_CURSOR = 6;
	final public static int NE_RESIZE_CURSOR = 7;
	final public static int N_RESIZE_CURSOR = 8;
	final public static int S_RESIZE_CURSOR = 9;
	final public static int W_RESIZE_CURSOR = 10;
	final public static int E_RESIZE_CURSOR = 11;
	final public static int HAND_CURSOR = 12;
	final public static int MOVE_CURSOR = 13;

	/** @serial 
	 * The chosen cursor type intially set to the DEFAULT_CURSOR.
	 */
	int type;

	/** @serial
	 * The user-visible name of the cursor.
	 */
	protected String name;

	protected static Cursor[] predefined = new Cursor[14];
	static Cursor defaultCursor;
	private static final long serialVersionUID = 8028237497568985504L;

static {
	defaultCursor = predefined[DEFAULT_CURSOR] = new Cursor( DEFAULT_CURSOR);
	
	for ( int i=1; i<= MOVE_CURSOR; i++ )
		predefined[i] = new Cursor( i);
}

public Cursor ( int cursorType ) {
	if ( (cursorType < 0) || (cursorType > MOVE_CURSOR) )
		type = DEFAULT_CURSOR;
	else
		type = cursorType;
}

public static Cursor getDefaultCursor() {
	return defaultCursor;
}

public static Cursor getPredefinedCursor ( int cursorType ) {
	if ( (cursorType < 1) || (cursorType > MOVE_CURSOR) )
		return defaultCursor;
	else
		return predefined[cursorType];
}

public int getType () {
	return type;
}
}
