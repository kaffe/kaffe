package java.awt;

import java.awt.event.KeyEvent;

public class MenuShortcut
{
	int mods;
	int keyCode;
	MenuShortcut next;
	MenuShortcut nextMod;
	ShortcutConsumer consumer;

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
