/**
 * nano-X AWT backend for Kaffe.
 *
 * Copyright (c) 2001
 *	Exor International Inc. All rights reserved.
 *
 * Copyright (c) 2001
 *	Sinby Corporatin, All rights reserved.
 *
 * Copyright (c) 2005
 *	Kaffe.org contributors, see ChangeLog for details.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

unsigned int keymap[] = {
	0x2500, // VK_LEFT
	0x2700, // VK_RIGHT
	0x2600, // VK_UP
	0x2800, // VK_DOWN
	0x9B00, // VK_INSERT
	0x7F7F, // VK_DELETE
	0x2400, // VK_HOME
	0x2300, // VK_END

	0x2100, // VK_UP
	0x2200, // VK_DOWN
	0x6030, // VK_NUMPAD0
	0x6131, // VK_NUMPAD1
	0x6232, // VK_NUMPAD2
	0x6333, // VK_NUMPAD3
	0x6434, // VK_NUMPAD4
	0x6535, // VK_NUMPAD5

	0x6636, // VK_NUMPAD6
	0x6737, // VK_NUMPAD7
	0x6838, // VK_NUMPAD8
	0x6939, // VK_NUMPAD9
	0x2E2E, // VK_PERIOD
	0x6F6F, // VK_DIVIDE
	0x6A6A, // VK_MULTIPLY
	0x6D6D, // VK_SUBTRACT

	0x6B6B, // VK_ADD
	('\n' << 8 ) | '\n', // VK_ENTER
	0x3D3D, // VK_EQUALS
	0x7000, // VK_F1
	0x7100, // VK_F2
	0x7200, // VK_F3
	0x7300, // VK_F4
	0x7400, // VK_F5

	0x7500, // VK_F6
	0x7600, // VK_F7
	0x7700, // VK_F8
	0x7800, // VK_F9
	0x7900, // VK_F10
	0x7A00, // VK_F11
	0x0000, // Reserved, maybe bug
	0x7B00, // VK_F12

	0x9000, // VK_NUM_LOCK
	0x1400, // VK_CAPS_LOCK
	0x9100, // VK_SCROLL_LOCK
	0x1000, // VK_SHIFT
	0x1000, // VK_SHIFT
	0x1100, // VK_CONTROL
	0x1100, // VK_CONTROL
	0x1200, // VK_ALT

	0x1200, // VK_ALT
	0x9D00, // VK_META
	0x9D00, // VK_META
	0x1200, // VK_ALT
	0x9A00, // VK_ALT
	0x0000, // SYSREQ, nothing
	0x1300, // VK_PUASE
	0x0000, // BREAK

	0x0000, // QUIT
	0x0000, // MENU
	0x0000, // REDRAW
	0x0000, // Reserved
	0x0000, // Reserved
	0x0000, // Reserved
	0x0000, // Reserved
	0x0000, // Reserved

	0x0000, // Record
	0x0000, // Play
	0x0000, // Contrast
	0x0000, // Brightness
	0x0000, // Selectup
	0x0000, // Selectdown
	0x1E00, // VK_ACCEPT
	0x0300, // VK_CANCEL

	0x0000, // App1
	0x0000  // App2
};
