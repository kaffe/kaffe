/**
 * SystemColor -
 *
 * Copyright (c) 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 *
 * @author P.C.Mehlitz
 */

package java.awt;


final public class SystemColor
  extends Color
{
	int idx;
	static int[] sysColors;
	private static final long serialVersionUID = 4503142729533789064L;
	final public static int DESKTOP = 0;
	final public static int ACTIVE_CAPTION = 1;
	final public static int ACTIVE_CAPTION_TEXT = 2;
	final public static int ACTIVE_CAPTION_BORDER = 3;
	final public static int INACTIVE_CAPTION = 4;
	final public static int INACTIVE_CAPTION_TEXT = 5;
	final public static int INACTIVE_CAPTION_BORDER = 6;
	final public static int WINDOW = 7;
	final public static int WINDOW_BORDER = 8;
	final public static int WINDOW_TEXT = 9;
	final public static int MENU = 10;
	final public static int MENU_TEXT = 11;
	final public static int TEXT = 12;
	final public static int TEXT_TEXT = 13;
	final public static int TEXT_HIGHLIGHT = 14;
	final public static int TEXT_HIGHLIGHT_TEXT = 15;
	final public static int TEXT_INACTIVE_TEXT = 16;
	final public static int CONTROL = 17;
	final public static int CONTROL_TEXT = 18;
	final public static int CONTROL_HIGHLIGHT = 19;
	final public static int CONTROL_LT_HIGHLIGHT = 20;
	final public static int CONTROL_SHADOW = 21;
	final public static int CONTROL_DK_SHADOW = 22;
	final public static int SCROLLBAR = 23;
	final public static int INFO = 24;
	final public static int INFO_TEXT = 25;
	final public static int NUM_COLORS = 26;
	final public static SystemColor desktop;
	final public static SystemColor activeCaption;
	final public static SystemColor activeCaptionText;
	final public static SystemColor activeCaptionBorder;
	final public static SystemColor inactiveCaption;
	final public static SystemColor inactiveCaptionText;
	final public static SystemColor inactiveCaptionBorder;
	final public static SystemColor window;
	final public static SystemColor windowBorder;
	final public static SystemColor windowText;
	final public static SystemColor menu;
	final public static SystemColor menuText;
	final public static SystemColor text;
	final public static SystemColor textText;
	final public static SystemColor textHighlight;
	final public static SystemColor textHighlightText;
	final public static SystemColor textInactiveText;
	final public static SystemColor control;
	final public static SystemColor controlText;
	final public static SystemColor controlHighlight;
	final public static SystemColor controlLtHighlight;
	final public static SystemColor controlShadow;
	final public static SystemColor controlDkShadow;
	final public static SystemColor scrollbar;
	final public static SystemColor info;
	final public static SystemColor infoText;

static {
	sysColors = new int[NUM_COLORS];
	
	// set reasonable (fallback) defaults
  sysColors[0]  = 0xff0000ff;  // desktop
  sysColors[1]  = 0xffa00000;  // active_caption
  sysColors[2]  = 0xffffff00;  // active_caption_text
  sysColors[3]  = 0xffa00000;  // active_caption_border
  sysColors[4]  = 0xffc0c0c0;  // inactive_caption
  sysColors[5]  = 0xff000000;  // inactive_caption_text
  sysColors[6]  = 0xffc0c0c0;  // inactive_caption_border
  sysColors[7]  = 0xffffffff;  // window
  sysColors[8]  = 0xff000000;  // window_border
  sysColors[9]  = 0xff000000;  // window_text
  sysColors[10] = 0xffc0c0c0;  // menu
  sysColors[11] = 0xff000000;  // menu_text
  sysColors[12] = 0xffffffff;  // text
  sysColors[13] = 0xff000000;  // text_text
  sysColors[14] = 0xff000000;  // text_highlight
  sysColors[15] = 0xffffffff;  // text_highlight_text
  sysColors[16] = 0xff808080;  // text_inactive_text
  sysColors[17] = 0xffa0a0a0;  // control
  sysColors[18] = 0xff000000;  // controlText
  sysColors[19] = 0xffffffff;  // control_highlight
  sysColors[20] = 0xffe0e0e0;  // control_lt_highlight
  sysColors[21] = 0xff808080;  // control_shadow
  sysColors[22] = 0xff000000;  // control_dk_shadow
  sysColors[23] = 0xffa0a0a0;  // scrollbar
  sysColors[24] = 0xffc0c0c0;  // info
  sysColors[25] = 0xff0000ff;  // info_text
	
	// check if there are native defaults
	Toolkit.getDefaultToolkit().loadSystemColors( sysColors);

	// now create the SystemColor objects according to the above rgbs
	desktop               = new SystemColor( DESKTOP);
  activeCaption         = new SystemColor( ACTIVE_CAPTION);
  activeCaptionText     = new SystemColor( ACTIVE_CAPTION_TEXT);
  activeCaptionBorder   = new SystemColor( ACTIVE_CAPTION_BORDER);
  inactiveCaption       = new SystemColor( INACTIVE_CAPTION);
  inactiveCaptionText   = new SystemColor( INACTIVE_CAPTION_TEXT);
  inactiveCaptionBorder = new SystemColor( INACTIVE_CAPTION_BORDER);
  window                = new SystemColor( WINDOW);
  windowBorder          = new SystemColor( WINDOW_BORDER);
  windowText            = new SystemColor( WINDOW_TEXT);
  menu                  = new SystemColor( MENU);
  menuText              = new SystemColor( MENU_TEXT);
  text                  = new SystemColor( TEXT);
  textText              = new SystemColor( TEXT_TEXT);
  textHighlight         = new SystemColor( TEXT_HIGHLIGHT);
  textHighlightText     = new SystemColor( TEXT_HIGHLIGHT_TEXT);
  textInactiveText      = new SystemColor( TEXT_INACTIVE_TEXT);
  control               = new SystemColor( CONTROL);
  controlText           = new SystemColor( CONTROL_TEXT);
  controlHighlight      = new SystemColor( CONTROL_HIGHLIGHT);
  controlLtHighlight    = new SystemColor( CONTROL_LT_HIGHLIGHT);
  controlShadow         = new SystemColor( CONTROL_SHADOW);
  controlDkShadow       = new SystemColor( CONTROL_DK_SHADOW);
  scrollbar             = new SystemColor( SCROLLBAR);
  info                  = new SystemColor( INFO);
  infoText              = new SystemColor( INFO_TEXT);
}

private SystemColor ( int idx ) {
	super( sysColors[idx]);
	this.idx = idx;
}

public String toString () {
	return "SystemColor [r=" + getRed() + 
	             ",g=" + getGreen() +
	             ",b=" + getBlue() + ",idx=" + idx + ']';
}
}
