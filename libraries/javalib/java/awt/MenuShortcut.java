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
