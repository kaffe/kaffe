package java.awt.event;

import java.awt.Component;
import java.awt.Event;

/**
 *
 * Copyright (c) 1998
 *   Transvirtual Technologies Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 * @author P.C.Mehlitz
 */
public class KeyEvent
  extends InputEvent
{
	protected int keyCode;
	protected char keyChar;
	final public static int KEY_FIRST = 400;
	final public static int KEY_LAST = 402;
	final public static int KEY_TYPED = KEY_FIRST;
	final public static int KEY_PRESSED = 1 + KEY_FIRST;
	final public static int KEY_RELEASED = 2 + KEY_FIRST;
	final public static int VK_ENTER = '\n';
	final public static int VK_BACK_SPACE = '\b';
	final public static int VK_TAB = '\t';
	final public static int VK_CANCEL = 0x03;
	final public static int VK_CLEAR = 0x0C;
	final public static int VK_SHIFT = 0x10;
	final public static int VK_CONTROL = 0x11;
	final public static int VK_ALT = 0x12;
	final public static int VK_PAUSE = 0x13;
	final public static int VK_CAPS_LOCK = 0x14;
	final public static int VK_ESCAPE = 0x1B;
	final public static int VK_SPACE = 0x20;
	final public static int VK_PAGE_UP = 0x21;
	final public static int VK_PAGE_DOWN = 0x22;
	final public static int VK_END = 0x23;
	final public static int VK_HOME = 0x24;
	final public static int VK_LEFT = 0x25;
	final public static int VK_UP = 0x26;
	final public static int VK_RIGHT = 0x27;
	final public static int VK_DOWN = 0x28;
	final public static int VK_COMMA = 0x2C;
	final public static int VK_PERIOD = 0x2E;
	final public static int VK_SLASH = 0x2F;
	final public static int VK_0 = 0x30;
	final public static int VK_1 = 0x31;
	final public static int VK_2 = 0x32;
	final public static int VK_3 = 0x33;
	final public static int VK_4 = 0x34;
	final public static int VK_5 = 0x35;
	final public static int VK_6 = 0x36;
	final public static int VK_7 = 0x37;
	final public static int VK_8 = 0x38;
	final public static int VK_9 = 0x39;
	final public static int VK_SEMICOLON = 0x3B;
	final public static int VK_EQUALS = 0x3D;
	final public static int VK_A = 0x41;
	final public static int VK_B = 0x42;
	final public static int VK_C = 0x43;
	final public static int VK_D = 0x44;
	final public static int VK_E = 0x45;
	final public static int VK_F = 0x46;
	final public static int VK_G = 0x47;
	final public static int VK_H = 0x48;
	final public static int VK_I = 0x49;
	final public static int VK_J = 0x4A;
	final public static int VK_K = 0x4B;
	final public static int VK_L = 0x4C;
	final public static int VK_M = 0x4D;
	final public static int VK_N = 0x4E;
	final public static int VK_O = 0x4F;
	final public static int VK_P = 0x50;
	final public static int VK_Q = 0x51;
	final public static int VK_R = 0x52;
	final public static int VK_S = 0x53;
	final public static int VK_T = 0x54;
	final public static int VK_U = 0x55;
	final public static int VK_V = 0x56;
	final public static int VK_W = 0x57;
	final public static int VK_X = 0x58;
	final public static int VK_Y = 0x59;
	final public static int VK_Z = 0x5A;
	final public static int VK_OPEN_BRACKET = 0x5B;
	final public static int VK_BACK_SLASH = 0x5C;
	final public static int VK_CLOSE_BRACKET = 0x5D;
	final public static int VK_NUMPAD0 = 0x60;
	final public static int VK_NUMPAD1 = 0x61;
	final public static int VK_NUMPAD2 = 0x62;
	final public static int VK_NUMPAD3 = 0x63;
	final public static int VK_NUMPAD4 = 0x64;
	final public static int VK_NUMPAD5 = 0x65;
	final public static int VK_NUMPAD6 = 0x66;
	final public static int VK_NUMPAD7 = 0x67;
	final public static int VK_NUMPAD8 = 0x68;
	final public static int VK_NUMPAD9 = 0x69;
	final public static int VK_MULTIPLY = 0x6A;
	final public static int VK_ADD = 0x6B;
	final public static int VK_SEPARATER = 0x6C;
	final public static int VK_SUBTRACT = 0x6D;
	final public static int VK_DECIMAL = 0x6E;
	final public static int VK_DIVIDE = 0x6F;
	final public static int VK_F1 = 0x70;
	final public static int VK_F2 = 0x71;
	final public static int VK_F3 = 0x72;
	final public static int VK_F4 = 0x73;
	final public static int VK_F5 = 0x74;
	final public static int VK_F6 = 0x75;
	final public static int VK_F7 = 0x76;
	final public static int VK_F8 = 0x77;
	final public static int VK_F9 = 0x78;
	final public static int VK_F10 = 0x79;
	final public static int VK_F11 = 0x7A;
	final public static int VK_F12 = 0x7B;
	final public static int VK_DELETE = 0x7F;
	final public static int VK_NUM_LOCK = 0x90;
	final public static int VK_SCROLL_LOCK = 0x91;
	final public static int VK_PRINTSCREEN = 0x9A;
	final public static int VK_INSERT = 0x9B;
	final public static int VK_HELP = 0x9C;
	final public static int VK_META = 0x9D;
	final public static int VK_BACK_QUOTE = 0xC0;
	final public static int VK_QUOTE = 0xDE;
	final public static int VK_FINAL = 0x18;
	final public static int VK_CONVERT = 0x1C;
	final public static int VK_NONCONVERT = 0x1D;
	final public static int VK_ACCEPT = 0x1E;
	final public static int VK_MODECHANGE = 0x1F;
	final public static int VK_KANA = 0x15;
	final public static int VK_KANJI = 0x19;
	final public static int VK_UNDEFINED = 0x0;
	final public static char CHAR_UNDEFINED = 0x0;

public KeyEvent ( Component src, int evtId, long time, int mods, int kCode) {
	super( src, evtId);
	
	when = time;
	modifiers = mods;
	keyCode = kCode;
	keyChar = (char)kCode;
}

public KeyEvent ( Component src, int evtId, long time, int mods, int kCode, char kChar ) {
	super( src, evtId);
	
	when = time;
	modifiers = mods;
	keyCode = kCode;
	keyChar = kChar;
}

public char getKeyChar() {
	return keyChar;
}

public int getKeyCode() {
	return keyCode;
}

public static String getKeyModifiersText ( int modifiers ) {
	String s = "";
	int i = 0;

	if ( (modifiers & META_MASK) != 0 ){
		i++;
		s += "Meta";
	}

	if ( (modifiers & CTRL_MASK) != 0 ) {
		if ( i++ > 0 ) s += '+';
		s += "Ctrl";
	}

	if ( (modifiers & ALT_MASK) != 0 ) {
		if ( i++ > 0 ) s += '+';
		s += "Alt";
	}

	if ( (modifiers & InputEvent.SHIFT_MASK) != 0 ){
		if ( i++ > 0 ) s += '+';
		s += "Shift";
	}

	return s;
}

public static String getKeyText ( int keyCode ) {
	if ( (keyCode >= VK_0 && keyCode <= VK_9) || (keyCode >= VK_A && keyCode <= VK_Z) ||
       keyCode == ',' || keyCode == '.' || keyCode == '/' || keyCode == ';' ||
	     keyCode == '=' || keyCode == '[' || keyCode == '\\' || keyCode == ']' )
		return String.valueOf((char)keyCode);

	switch(keyCode) {
	case VK_ENTER:      return "Enter";
	case VK_BACK_SPACE: return "Backspace";
	case VK_TAB:        return "Tab";
	case VK_CANCEL:     return "Cancel";
	case VK_CLEAR:      return "Clear";
	case VK_SHIFT:      return "Shift";
	case VK_CONTROL:    return "Control";
	case VK_ALT:        return "Alt";
	case VK_PAUSE:      return "Pause";
	case VK_CAPS_LOCK:  return "Caps Lock";
	case VK_ESCAPE:     return "Escape";
	case VK_SPACE:      return "Space";
	case VK_PAGE_UP:    return "Page Up";
	case VK_PAGE_DOWN:  return "Page Down";
	case VK_END:        return "End";
	case VK_HOME:       return "Home";
	case VK_LEFT:       return "Left";
	case VK_UP:         return "Up";
	case VK_RIGHT:      return "Right";
	case VK_DOWN:       return "Down";

	case VK_MULTIPLY:   return "NumPad *";
	case VK_ADD:        return "NumPad +";
	case VK_SEPARATER:  return "NumPad ,";
	case VK_SUBTRACT:   return "NumPad -";
	case VK_DECIMAL:    return "NumPad .";
	case VK_DIVIDE:     return "NumPad /";

	case VK_F1:         return "F1";
	case VK_F2:         return "F2";
	case VK_F3:         return "F3";
	case VK_F4:         return "F4";
	case VK_F5:         return "F5";
	case VK_F6:         return "F6";
	case VK_F7:         return "F7";
	case VK_F8:         return "F8";
	case VK_F9:         return "F9";
	case VK_F10:        return "F10";
	case VK_F11:        return "F11";
	case VK_F12:        return "F12";
	case VK_DELETE:     return "Delete";
	case VK_NUM_LOCK:   return "Num Lock";
	case VK_SCROLL_LOCK: return "Scroll Lock";
	case VK_PRINTSCREEN: return "Print Screen";
	case VK_INSERT:     return "Insert";
	case VK_HELP:       return "Help";
	case VK_META:       return "Meta";
	case VK_BACK_QUOTE: return "Back Quote";
	case VK_QUOTE:      return "Quote";

	case VK_FINAL:      return "Final";
	case VK_CONVERT:    return "Convert";
	case VK_NONCONVERT: return "No Convert";
	case VK_ACCEPT:     return "Accept";
	case VK_MODECHANGE: return "Mode Change";
	case VK_KANA:       return "Kana";
	case VK_KANJI:      return "Kanji";
	}

	if ( keyCode >= VK_NUMPAD0 && keyCode <= VK_NUMPAD9 ) {
		return "NumPad-" + (char)(keyCode - VK_NUMPAD0 + '0');
	}

	return "Unknown keyCode: 0x" + Integer.toString(keyCode, 16);
}

protected Event initOldEvent ( Event e ) {
	if ( keyChar == 0 )
		return null;

	e.target = source;
	e.id = id;
	
	e.when = when;
	e.modifiers = modifiers;
	e.key = keyChar;
	
	return e;
}

public boolean isActionKey () {
	int kc = keyCode;

	if ( (kc >= VK_PAUSE) && (kc <= VK_CAPS_LOCK) ) return true;
	if ( (kc >= VK_PAGE_UP) && (kc <= VK_DOWN) ) return true;
	if ( (kc >= VK_F1) && (kc <= VK_F12) ) return true;
	if ( (kc >= VK_NUM_LOCK) && (kc <= VK_INSERT) ) return true;

	return false;
}

public String paramString () {
	String s;
	int kc = keyCode;
	int k = keyChar;
	
	switch(id) {
	case KEY_PRESSED:		s = "KEY_PRESSED";		break;
	case KEY_RELEASED:	s = "KEY_RELEASED";		break;
	case KEY_TYPED:		  s = "KEY_TYPED";		  break;
	default:		        s = "unknown type";
	}

	s += ",keyCode=" + keyCode;
	
	if ( isActionKey() || kc == VK_ENTER || kc == VK_BACK_SPACE || 
	     kc == VK_TAB || kc == VK_ESCAPE || kc == VK_DELETE ||
	     (kc >= VK_NUMPAD0 && kc <= VK_NUMPAD9) ) {
		s += ',' + getKeyText( kc);
	}
	else if ( k == '\n' || k == '\b' || k == '\t' || k == VK_ESCAPE || k == VK_DELETE) {
		s += ',' + getKeyText( k);
	}
	else {
		s += ",keyChar='" + keyChar + "'";
	}
	
	if ( modifiers > 0 ) {
		s += ",modifiers=" + getKeyModifiersText( modifiers);
	}
	
	return s;
}

public void setKeyChar ( char kChar ) {
	keyChar = kChar;
}

public void setKeyCode ( int kCode ) {
	keyCode = kCode;
}

public void setModifiers ( int mods ) {
	modifiers = mods;
}
}
