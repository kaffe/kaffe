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
//	protected int keyLocation; // JDK 1.4
	protected char keyChar;
	private static final long serialVersionUID = -2352130953028126954L;
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
	final public static int VK_MINUS = 0x2D; // 1.2
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
	final public static int VK_SEPARATER = 0x6C; // 1.1 compatibility (misspelled)
	final public static int VK_SEPARATOR = 0x6C; // 1.4
	final public static int VK_SUBTRACT = 0x6D;
	final public static int VK_DECIMAL = 0x6E;
	final public static int VK_DIVIDE = 0x6F;
	final public static int VK_DELETE = 0x7F;
	final public static int VK_NUM_LOCK = 0x90;
	final public static int VK_SCROLL_LOCK = 0x91;
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

	final public static int VK_F13 = 0xF000; // 1.2
	final public static int VK_F14 = 0xF001; // 1.2
	final public static int VK_F15 = 0xF002; // 1.2
	final public static int VK_F16 = 0xF003; // 1.2
	final public static int VK_F17 = 0xF004; // 1.2
	final public static int VK_F18 = 0xF005; // 1.2
	final public static int VK_F19 = 0xF006; // 1.2
	final public static int VK_F20 = 0xF007; // 1.2
	final public static int VK_F21 = 0xF008; // 1.2
	final public static int VK_F22 = 0xF009; // 1.2
	final public static int VK_F23 = 0xF00A; // 1.2
	final public static int VK_F24 = 0xF00B; // 1.2

	final public static int VK_PRINTSCREEN = 0x9A;
	final public static int VK_INSERT = 0x9B;
	final public static int VK_HELP = 0x9C;
	final public static int VK_META = 0x9D;
	final public static int VK_BACK_QUOTE = 0xC0;
	final public static int VK_QUOTE = 0xDE;

	final public static int VK_KP_UP = 0xE0; // 1.2
	final public static int VK_KP_DOWN = 0xE1; // 1.2
	final public static int VK_KP_LEFT = 0xE2; // 1.2
	final public static int VK_KP_RIGHT = 0xE3; // 1.2
	final public static int VK_DEAD_GRAVE = 0x80; // 1.2
	final public static int VK_DEAD_ACUTE = 0x81; // 1.2
	final public static int VK_DEAD_CIRCUMFLEX = 0x82; // 1.2
	final public static int VK_DEAD_TILDE = 0x83; // 1.2
	final public static int VK_DEAD_MACRON = 0x84; // 1.2
	final public static int VK_DEAD_BREVE = 0x85; // 1.2
	final public static int VK_DEAD_ABOVEDOT = 0x86; // 1.2
	final public static int VK_DEAD_DIAERESIS = 0x87; // 1.2
	final public static int VK_DEAD_ABOVERING = 0x88; // 1.2
	final public static int VK_DEAD_DOUBLEACUTE = 0x89; // 1.2
	final public static int VK_DEAD_CARON = 0x8A; // 1.2
	final public static int VK_DEAD_CEDILLA = 0x8B; // 1.2
	final public static int VK_DEAD_OGONEK = 0x8C; // 1.2
	final public static int VK_DEAD_IOTA = 0x8D; // 1.2
	final public static int VK_DEAD_VOICED_SOUND = 0x8E; // 1.2
	final public static int VK_DEAD_SEMIVOICED_SOUND = 0x8F; // 1.2
	final public static int VK_AMPERSAND = 0x96; // 1.2
	final public static int VK_ASTERISK = 0x97; // 1.2
	final public static int VK_QUOTEDBL = 0x98; // 1.2
	final public static int VK_LESS = 0x99; // 1.2
	final public static int VK_GREATER = 0xA0; // 1.2
	final public static int VK_BRACELEFT = 0xA1; // 1.2
	final public static int VK_BRACERIGHT = 0xA2; // 1.2
	final public static int VK_AT = 0x200; // 1.2
	final public static int VK_COLON = 0x201; // 1.2
	final public static int VK_CIRCUMFLEX = 0x202; // 1.2
	final public static int VK_DOLLAR = 0x203; // 1.2
	final public static int VK_EURO_SIGN = 0x204; // 1.2
	final public static int VK_EXCLAMATION_MARK = 0x205; // 1.2
	final public static int VK_INVERTED_EXCLAMATION_MARK = 0x206; // 1.2
	final public static int VK_LEFT_PARENTHESIS = 0x207; // 1.2
	final public static int VK_NUMBER_SIGN = 0x208; // 1.2
	final public static int VK_PLUS = 0x209; // 1.2
	final public static int VK_RIGHT_PARENTHESIS = 0x20A; // 1.2
	final public static int VK_UNDERSCORE = 0x20B; // 1.2

	final public static int VK_FINAL = 0x18;
	final public static int VK_CONVERT = 0x1C;
	final public static int VK_NONCONVERT = 0x1D;
	final public static int VK_ACCEPT = 0x1E;
	final public static int VK_MODECHANGE = 0x1F;
	final public static int VK_KANA = 0x15;
	final public static int VK_KANJI = 0x19;

	final public static int VK_ALPHANUMERIC = 0xF0; // 1.2
	final public static int VK_KATAKANA = 0xF1; // 1.2
	final public static int VK_HIRAGANA = 0xF2; // 1.2
	final public static int VK_FULL_WIDTH = 0xF3; // 1.2
	final public static int VK_HALF_WIDTH = 0xF4; // 1.2
	final public static int VK_ROMAN_CHARACTERS = 0xF5; // 1.2
	final public static int VK_ALL_CANDIDATES = 0x100; // 1.2
	final public static int VK_PREVIOUS_CANDIDATE = 0x101; // 1.2
	final public static int VK_CODE_INPUT = 0x102; // 1.2
	final public static int VK_JAPANESE_KATAKANA = 0x103; // 1.2
	final public static int VK_JAPANESE_HIRAGANA = 0x104; // 1.2
	final public static int VK_JAPANESE_ROMAN = 0x105; // 1.2
	final public static int VK_KANA_LOCK = 0x106; // 1.2
	final public static int VK_INPUT_METHOD_ON_OFF = 0x107; // 1.2
	final public static int VK_CUT = 0xFFD1; // 1.2
	final public static int VK_COPY = 0xFFCD; // 1.2
	final public static int VK_PASTE = 0xFFCF; // 1.2
	final public static int VK_UNDO = 0xFFCB; // 1.2
	final public static int VK_AGAIN = 0xFFC9; // 1.2
	final public static int VK_FIND = 0xFFD0; // 1.2
	final public static int VK_PROPS = 0xFFCA; // 1.2
	final public static int VK_STOP = 0xFFC8; // 1.2
	final public static int VK_COMPOSE = 0xFF20; // 1.2
	final public static int VK_ALT_GRAPH = 0xFF7E; // 1.2

	final public static int VK_UNDEFINED = 0x0;
	final public static char CHAR_UNDEFINED = 0xffff;

	final public static int KEY_LOCATION_UNKNOWN = 0; // 1.4
	final public static int KEY_LOCATION_STANDARD = 1; // 1.4
	final public static int KEY_LOCATION_LEFT = 2; // 1.4
	final public static int KEY_LOCATION_RIGHT = 3; // 1.4
	final public static int KEY_LOCATION_NUMPAD = 4; // 1.4

public KeyEvent ( Component src, int evtId, long time, int mods, int kCode) {
	super( src, evtId);
	
	when = time;
	modifiers = mods;
	keyCode = kCode;
	keyChar = (char)kCode;
//	keyLocation = KEY_LOCATION_UNKNOWN;
}

public KeyEvent ( Component src, int evtId, long time, int mods, int kCode, char kChar ) {
	super( src, evtId);
	
	when = time;
	modifiers = mods;
	keyCode = kCode;
	keyChar = kChar;
//	keyLocation = KEY_LOCATION_UNKNOWN;
}

public KeyEvent ( Component src, int evtId, long time, int mods, int kCode, char kChar, int kLoc ) {
	super( src, evtId);
	
	when = time;
	modifiers = mods;
	keyCode = kCode;
	keyChar = kChar;
//	keyLocation = kLoc;
}

public char getKeyChar() {
	return keyChar;
}

public int getKeyCode() {
	return keyCode;
}

public static String getKeyModifiersText ( int modifiers ) {
	StringBuffer sb = new StringBuffer();
	int i = 0;

	if ( (modifiers & META_MASK) != 0 ){
		i++;
		sb.append ("Meta");
	}

	if ( (modifiers & CTRL_MASK) != 0 ) {
		if ( i++ > 0 ) sb.append ('+');
		sb.append ("Ctrl");
	}

	if ( (modifiers & ALT_MASK) != 0 ) {
		if ( i++ > 0 ) sb.append ('+');
		sb.append ("Alt");
	}

	if ( (modifiers & InputEvent.SHIFT_MASK) != 0 ){
		if ( i++ > 0 ) sb.append ('+');
		sb.append ("Shift");
	}

	return sb.toString();
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

	case VK_KP_LEFT:    return "Left";
	case VK_KP_UP:      return "Up";
	case VK_KP_RIGHT:   return "Right";
	case VK_KP_DOWN:    return "Down";

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
	// JDK 1.2+
	case VK_F13:        return "F13";
	case VK_F14:        return "F14";
	case VK_F15:        return "F15";
	case VK_F16:        return "F16";
	case VK_F17:        return "F17";
	case VK_F18:        return "F18";
	case VK_F19:        return "F19";
	case VK_F20:        return "F20";
	case VK_F21:        return "F21";
	case VK_F22:        return "F22";
	case VK_F23:        return "F23";
	case VK_F24:        return "F24";

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

	// Java2 additions
	case VK_AGAIN:		return "Again";
	case VK_ALL_CANDIDATES:	return "All Candidates";
	case VK_ALPHANUMERIC:	return "Alphanumeric";
	case VK_ALT_GRAPH:	return "Alt Graph";
	case VK_AMPERSAND:	return "Ampersand";
	case VK_ASTERISK:	return "Asterisk";
	case VK_AT:		return "At";
	case VK_BRACELEFT:	return "Left Brace";
	case VK_BRACERIGHT:	return "Right Brace";
	case VK_CIRCUMFLEX:	return "Circumflex";
	case VK_CODE_INPUT:	return "Code Input";
	case VK_COLON:		return "Colon";
	case VK_COMPOSE:	return "Compose ";
	case VK_COPY:		return "Copy";
	case VK_CUT:		return "Cut";
	case VK_DEAD_ABOVEDOT:	return "Dead Above Dot";
	case VK_DEAD_ABOVERING:	return "Dead Above Ring";
	case VK_DEAD_ACUTE:	return "Dead Acute";
	case VK_DEAD_BREVE:	return "Dead Breve";
	case VK_DEAD_CARON:	return "Dead Caron";
	case VK_DEAD_CEDILLA:	return "Dead Cedilla";
	case VK_DEAD_CIRCUMFLEX: return "Dead Circumflex";
	case VK_DEAD_DIAERESIS:	return "Dead Diaeresis";
	case VK_DEAD_DOUBLEACUTE: return "Dead Double Acute";
	case VK_DEAD_GRAVE:	return "Dead Grave";
	case VK_DEAD_IOTA:	return "Dead Iota";
	case VK_DEAD_MACRON:	return "Dead Macron";
	case VK_DEAD_OGONEK:	return "Dead Ogonek";
	case VK_DEAD_SEMIVOICED_SOUND: return "Dead Semivoiced Sound";
	case VK_DEAD_TILDE:	return "Dead Tilde";
	case VK_DEAD_VOICED_SOUND: return "Dead Voiced Sound";
	case VK_DOLLAR:		return "Dollar";
	case VK_EURO_SIGN:	return "Euro";
	case VK_EXCLAMATION_MARK: return "Exclamation Mark";
	case VK_FIND:		return "Find";
	case VK_FULL_WIDTH:	return "Full-Width";
	case VK_GREATER:	return "Greater";
	case VK_HALF_WIDTH:	return "Half-Width";
	case VK_HIRAGANA:	return "Hiragana";
	case VK_INPUT_METHOD_ON_OFF: return "Input Method On/Off";
	case VK_INVERTED_EXCLAMATION_MARK: return "Inverted Exclamation Mark";
	case VK_JAPANESE_HIRAGANA: return "Japanese Hiragana";
	case VK_JAPANESE_KATAKANA: return "Japanese Katakana";
	case VK_JAPANESE_ROMAN:	return "Japanese Roman";
	case VK_KANA_LOCK:	return "Kana Lock";
	case VK_KATAKANA:	return "Katakana";
	case VK_LEFT_PARENTHESIS: return "Left Parenthesis";
	case VK_LESS:		return "Less";
	case VK_MINUS:		return "Minus";
	case VK_NUMBER_SIGN:	return "Number Sign";
	case VK_PASTE:		return "Paste";
	case VK_PLUS:		return "Plus";
	case VK_PREVIOUS_CANDIDATE: return "Previous Candidate";
	case VK_PROPS:		return "Props";
	case VK_QUOTEDBL:	return "Double Quote";
	case VK_RIGHT_PARENTHESIS: return "Right Parenthesis";
	case VK_ROMAN_CHARACTERS: return "Roman Characters";
	case VK_STOP:		return "Stop";
	case VK_UNDERSCORE:	return "Underscore";
	case VK_UNDO:		return "Undo";
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

	// we need to set x & y coordinates too,
	// since some events may be cached.
	e.x = ((Component) getSource()).getX();
	e.y = ((Component) getSource()).getY();

	return e;
}

public boolean isActionKey () {
	int kc = keyCode;

	// Pop out most common alphanumerics first
	if ( kc == VK_SPACE ) return false; // Space-bar
	if ( (kc > VK_DOWN) && (kc < VK_F1) ) return false; // Alphanum
	if ( kc < VK_PAUSE ) return false; // Newline
	if ( (kc > VK_KANJI) && (kc < VK_CONVERT) ) return false; // Escape
	if ( (kc > VK_KANA) && (kc < VK_FINAL) ) return false; // None
	if ( kc <= VK_F12 ) return true; // Everything else common

	if ( (kc >= VK_NUM_LOCK) && (kc <= VK_SCROLL_LOCK) ) return true;
	if ( (kc >= VK_PRINTSCREEN) && (kc <= VK_HELP) ) return true;
	if ( (kc >= VK_KP_UP) && (kc <= VK_KP_RIGHT) ) return true;
	if ( (kc >= VK_ALPHANUMERIC) && (kc <= VK_ROMAN_CHARACTERS) ) return true;
	if ( (kc >= VK_ALL_CANDIDATES) && (kc <= VK_INPUT_METHOD_ON_OFF) ) return true;
	if ( (kc >= VK_F13) && (kc <= VK_F24) ) return true;
	if ( (kc >= VK_STOP) && (kc <= VK_UNDO) ) return true;
	if ( kc == VK_COPY ) return true;
	if ( (kc >= VK_PASTE) && (kc <= VK_CUT) ) return true;

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

public int getKeyLocation() {
//	return keyLocation;
	return KEY_LOCATION_UNKNOWN;
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
