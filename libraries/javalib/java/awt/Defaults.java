/**
 * Defaults - adaption parameters of package java.awt
 *
 * Be very carefully to add additional java.awt types, since this might
 * introduce recursive class init problems. This is especially
 * true if static fields of such classes are involved (like Color), since
 * they might be still "null" (if they in turn refer to Toolkit/Defaults)
 * when referenced inside of this class.
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

import java.awt.event.KeyEvent;
import java.util.Hashtable;

class Defaults
{
	static boolean RedirectStreams = false;
	static boolean AutoStop = true;
	static int ClickInterval = 250;
	static int EventPollingRate = 30;
/**
 * physical screen size in 1/10 inches (used for various scaling purposes)
 */
	static int ScreenSize = 130;
	static int ScreenWidth;
	static int ScreenHeight;
	static int XResolution;
	static int YResolution;
	static int TitleBarHeight = 20;
	static int MenuBarHeight = 20;
	static int FrameBorderWidth = 4;
	static int BottomBarHeight;
	static String FsDefault;
	static String FsMonospaced;
	static String FsSansSerif;
	static String FsSerif;
	static String FsDialog;
	static String FsDialogInput;
	static String FsZapfDingbats;
	static int[] RgbRequests = {
	// note that requested colors can't refer to Color objects (which
	// require an already initialized Color mapping)

	195<<16 | 195<<8 | 195,     // lightgray
	230<<16 | 230<<8 | 230,     // lighter lightGray
	163<<16 | 163<<8 | 163,     // darker lightGray
	190<<16                     // dark red
};
	static Color WndForeground = Color.black;
	static Color WndBackground = Color.lightGray;
	static Font WndFont;
	static FontMetrics WndFontMetrics;
	static Font MenuFont;
	static Color MenuTxtClr = Color.black;
	static Color MenuBgClr = Color.lightGray;
	static boolean MenuTxtCarved = true;
	static Font TextFont;
	static Color FocusClr = new Color( 191, 0, 0);
	static Color TextAreaBgClr = Color.white;
	static Color TextAreaTxtClr = Color.black;
	static Font TextAreaFont;
	static Color TextAreaSelBgClr = Color.lightGray;
	static Color TextAreaSelTxtClr = Color.black;
	static Color TextCursorClr = Color.blue;
	static Color TextCursorInactiveClr = Color.gray;
	static boolean ShowInactiveCursor = false;
	static boolean FocusScrolls = false;
	static Color TextFieldBgClr = Color.white;
	static Color TextFieldTxtClr = Color.black;
	static Color TextFieldSelBgClr = Color.lightGray;
	static Color TextFieldSelTxtClr = Color.black;
	static Font TextFieldFont;
	static Font ListFont;
	static Color ListBgClr = new Color( 223, 223, 223);
	static Color ListTxtClr = Color.black;
	static Color ListSelBgClr = new Color( 163, 163, 163);
	static Color ListSelTxtClr = Color.black;
	static Color ListFlyOverBgClr = new Color( 214, 214, 214);
	static Color ListFlyOverTxtClr = Color.black;
	static boolean ListFlyOverInset = false;
	static Color LabelClr = Color.black;
	static Font LabelFont;
	static boolean LabelsCarved = true;
	static Font BtnFont;
	static Color BtnClr = new Color( 163, 163, 163);
	static Color BtnTxtClr = Color.black;
	static Color BtnPointClr = new Color( 180, 180, 180);
	static Color BorderClr = new Color( 159, 159, 159);
/**
 * grx only
 * @grx
 */
	static String RootWindowClass = "java.awt.RootWindow";
	static Color RootWindowClr = Color.blue;
	static String RootWindowImg = "marble.jpg";
/**
 * grx only
 * @grx
 */
	static String KeyTabClass = "java.awt.KeyTabGr";
/**
 * grx only
 * @grx
 */
	static Color WndActiveClr = new Color( 191, 0, 0);
/**
 * grx only
 * @grx
 */
	static Color WndActiveTitleClr = Color.yellow;
/**
 * grx only
 * @grx
 */
	static Color WndInactiveClr = Color.lightGray;
/**
 * grx only
 * @grx
 */
	static Color WndInactiveTitleClr = Color.black;
	static String ControlBoxClass = "java.awt.WinTaskBar";
	static String MinBoxClass = "java.awt.WinTaskBar";

static {
	// these are the computed values
	Dimension sd = Toolkit.getDefaultToolkit().getScreenSize();

	ScreenWidth  = sd.width;
	ScreenHeight = sd.height;

	// HACK HACK HACK XXX
	if (ScreenSize == 0) {
		ScreenSize = 130;
	}
	
	// we assume a 4:3 width/height ratio of the display
	XResolution = (int)((double) ScreenWidth  * 10 / ( 0.8 * ScreenSize)); // dpi
	YResolution = (int)((double) ScreenHeight * 10 / ( 0.6 * ScreenSize)); // dpi
	
	String version = Toolkit.tlkVersion();
	if ( version.startsWith( "grx") ) {
		FsDefault = "helv%d%s%s.fnt";
		FsMonospaced = "xm7x13b.fnt";
		FsSansSerif = "helv%d%s%s.fnt";
		FsSerif = "tms%d%s%s.fnt";
		FsDialog = "cour%d%s%s.fnt";
		FsDialogInput = "cour%d%s%s.fnt";
		FsZapfDingbats = "helv%d%s%s.fnt";
	}
	else if ( version.startsWith( "X") ) {
		FsDefault = "-b&h-lucida-%s-%s-*-*-%d-*-*-*-*-*-*-*";
		FsMonospaced = "-misc-fixed-%s-%s-*-*-%d-*-*-*-*-*-*-*";
		FsSansSerif = "-adobe-helvetica-%s-%s-*-*-%d-*-*-*-*-*-*-*";
		FsSerif = "-adobe-times-%s-%s-*-*-%d-*-*-*-*-*-*-*";
		FsDialog = "-misc-fixed-%s-%s-*-*-%d-*-*-*-*-*-*-*";
		FsDialogInput = "-b&h-lucidatypewriter-%s-%s-*-*-%d-*-*-*-*-*-*-*";
		FsZapfDingbats = "-adobe-new century schoolbook-%s-%s-*-*-%d-*-*-*-*-*-*-*";
	}
	else {
		throw new AWTError( "unknown native toolkit: "+ version);
	}

	WndFont = new Font( "Default", Font.BOLD, 12);
	MenuFont = new Font( "Default", Font.BOLD, 10);
	TextFont = new Font( "Default", Font.BOLD, 10);
	TextAreaFont = new Font( "Default", Font.BOLD, 11);
	TextFieldFont = new Font( "Default", Font.BOLD, 11);
	ListFont = new Font( "Default", Font.BOLD, 11);
	LabelFont = new Font( "Default", Font.BOLD, 11);
	BtnFont = new Font( "Default", Font.BOLD, 11);
	
	WndFontMetrics = Toolkit.getDefaultToolkit().getFontMetrics( WndFont);
}
}
