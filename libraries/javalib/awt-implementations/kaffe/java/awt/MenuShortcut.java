package java.awt;

import java.awt.event.KeyEvent;

public class MenuShortcut implements java.io.Serializable
{
	/* XXX implement serial form 
	 * int key;		result of getKey()
	 * boolean usesShift;	result of usesShiftModifier()
	 */
	int keyCode;
	transient int mods;
	transient MenuShortcut next;
	transient MenuShortcut nextMod;
	transient ShortcutConsumer consumer;
	private static final long serialVersionUID = 143448358473180225L;

MenuShortcut( MenuShortcut ms) {
	mods = ms.mods;
	keyCode = ms.keyCode;
}

MenuShortcut( ShortcutConsumer consumer, int keyCode, int mods) {
	this.consumer = consumer;
	this.keyCode = keyCode;
	this.mods = mods;
}

public MenuShortcut( int keyCode) {
	this( keyCode, false);
}

public MenuShortcut( int keyCode, boolean useShiftMod) {
	this.keyCode = keyCode;
	
	mods = KeyEvent.CTRL_MASK;
	if ( useShiftMod )
		mods |= KeyEvent.SHIFT_MASK;
}

public boolean equals( MenuShortcut s) {
	if ( s == null )
		return false;
	return ( (s.keyCode == keyCode) && (s.mods == mods) );
}

public int getKey() {
	return keyCode;
}

void process() {
	consumer.handleShortcut( this);
}

public String toString() {
	return ( getClass().getName() + " code: " + keyCode + ", mods: " + mods);
}

public boolean usesShiftModifier() {
	return ((mods & KeyEvent.SHIFT_MASK) > 0);
}
}
