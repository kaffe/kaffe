/*
 * Java core library component.
 *
 * Copyright (c) 1997, 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */


package java.awt;


public class MenuShortcut
{
	int key;
	boolean usesShiftMod;

public MenuShortcut( int key) {
	this( key, false);
}

public MenuShortcut( int key, boolean useShiftMod) {
	this.key = key;
	usesShiftMod = useShiftMod;
}

public boolean equals( MenuShortcut s) {
	if ( s == null )
		return false;
	return ( (s.key == key) && (s.usesShiftMod == usesShiftMod) );
}

public int getKey() {
	return key;
}

public String toString() {
	return ( " " + key);
}

public boolean usesShiftModifier() {
	return usesShiftMod;
}
}
