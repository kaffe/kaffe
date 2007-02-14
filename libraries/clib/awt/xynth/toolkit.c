
/*
 * Copyright (c) 2006 - 2007
 *	Alper Akcan <alper@kaffe.org>, All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#include "toolkit.h"

jint Java_java_awt_Toolkit_tlkProperties (JNIEnv *env UNUSED, jclass clazz UNUSED)
{
	jint props;
	DEBUGF("Enter");
	props = TLK_EXTERNAL_DECO;
	DEBUGF("Leave");
	return props;
}

jboolean Java_java_awt_Toolkit_tlkInit (JNIEnv *env UNUSED, jclass clazz UNUSED, jstring name UNUSED)
{
	DEBUGF("Enter");
	xynth = AWT_MALLOC(sizeof(xynth_toolkit_t));
	xynth->eventq = (xynth_eventq_t *) AWT_MALLOC(sizeof(xynth_eventq_t));
	s_list_init(&(xynth->eventq->list));
	s_thread_mutex_init(&(xynth->eventq->mut));
	s_window_init(&(xynth->root));
	s_window_new(xynth->root, WINDOW_MAIN, NULL);
	s_thread_create(s_window_main, xynth->root);
	xynth->nwindows = XYNTH_NWINDOWS;
	xynth->windows = AWT_CALLOC(xynth->nwindows, sizeof(window_rec_t));
	
	xynth->keymap[S_KEYCODE_ESCAPE] = -0x1b;
	xynth->keymap[S_KEYCODE_ONE] = 0x31;
	xynth->keymap[S_KEYCODE_TWO] = 0x32;
	xynth->keymap[S_KEYCODE_THREE] = 0x33;
	xynth->keymap[S_KEYCODE_FOUR] = 0x34;
	xynth->keymap[S_KEYCODE_FIVE] = 0x35;
	xynth->keymap[S_KEYCODE_SIX] = 0x36;
	xynth->keymap[S_KEYCODE_SEVEN] = 0x37;
	xynth->keymap[S_KEYCODE_EIGHT] = 0x38;
	xynth->keymap[S_KEYCODE_NINE] = 0x39;
	xynth->keymap[S_KEYCODE_ZERO] = 0x30;
	xynth->keymap[S_KEYCODE_MINUS] = 0x0;
	xynth->keymap[S_KEYCODE_EQUAL] = 0x3d;
	xynth->keymap[S_KEYCODE_DELETE] = 0x08;
	xynth->keymap[S_KEYCODE_TAB] = 0x9;
	xynth->keymap[S_KEYCODE_q] = 0x51;
	xynth->keymap[S_KEYCODE_w] = 0x57;
	xynth->keymap[S_KEYCODE_e] = 0x45;
	xynth->keymap[S_KEYCODE_r] = 0x52;
	xynth->keymap[S_KEYCODE_t] = 0x54;
	xynth->keymap[S_KEYCODE_y] = 0x59;
	xynth->keymap[S_KEYCODE_u] = 0x55;
	xynth->keymap[S_KEYCODE_i] = 0x49;
	xynth->keymap[S_KEYCODE_o] = 0x4f;
	xynth->keymap[S_KEYCODE_p] = 0x50;
	xynth->keymap[S_KEYCODE_BRACKETLEFT] = 0x5b;
	xynth->keymap[S_KEYCODE_BRACKETRIGHT] = 0x5d;
	xynth->keymap[S_KEYCODE_RETURN] = 0xa;
	xynth->keymap[S_KEYCODE_LEFTCONTROL] = 0x11;
	xynth->keymap[S_KEYCODE_a] = 0x41;
	xynth->keymap[S_KEYCODE_s] = 0x53;
	xynth->keymap[S_KEYCODE_d] = 0x44;
	xynth->keymap[S_KEYCODE_f] = 0x46;
	xynth->keymap[S_KEYCODE_g] = 0x47;
	xynth->keymap[S_KEYCODE_h] = 0x48;
	xynth->keymap[S_KEYCODE_j] = 0x4a;
	xynth->keymap[S_KEYCODE_k] = 0x4b;
	xynth->keymap[S_KEYCODE_l] = 0x4c;
	xynth->keymap[S_KEYCODE_SEMICOLON] = 0x3b;
	xynth->keymap[S_KEYCODE_APOSTROPHE] = 0xde;
	xynth->keymap[S_KEYCODE_GRAVE] = 0xc0;
	xynth->keymap[S_KEYCODE_LEFTSHIFT] = 0x10;
	xynth->keymap[S_KEYCODE_BACKSLASH] = 0x5c;
	xynth->keymap[S_KEYCODE_z] = 0x5a;
	xynth->keymap[S_KEYCODE_x] = 0x58;
	xynth->keymap[S_KEYCODE_c] = 0x43;
	xynth->keymap[S_KEYCODE_v] = 0x56;
	xynth->keymap[S_KEYCODE_b] = 0x42;
	xynth->keymap[S_KEYCODE_n] = 0x4e;
	xynth->keymap[S_KEYCODE_m] = 0x4d;
	xynth->keymap[S_KEYCODE_COMMA] = 0x2c;
	xynth->keymap[S_KEYCODE_PERIOD] = 0x2e;
	xynth->keymap[S_KEYCODE_SLASH] = 0x2f;
	xynth->keymap[S_KEYCODE_RIGHTSHIFT] = 0x10;
	xynth->keymap[S_KEYCODE_KP_MULTIPLY] = 0x6a;
	xynth->keymap[S_KEYCODE_ALT] = 0x12;
	xynth->keymap[S_KEYCODE_SPACE] = 0x20;
	xynth->keymap[S_KEYCODE_CAPS_LOCK] = 0x14;
	xynth->keymap[S_KEYCODE_F1] = 0x70;
	xynth->keymap[S_KEYCODE_F2] = 0x71;
	xynth->keymap[S_KEYCODE_F3] = 0x72;
	xynth->keymap[S_KEYCODE_F4] = 0x73;
	xynth->keymap[S_KEYCODE_F5] = 0x74;
	xynth->keymap[S_KEYCODE_F6] = 0x75;
	xynth->keymap[S_KEYCODE_F7] = 0x76;
	xynth->keymap[S_KEYCODE_F8] = 0x77;
	xynth->keymap[S_KEYCODE_F9] = 0x78;
	xynth->keymap[S_KEYCODE_F10] = 0x79;
	xynth->keymap[S_KEYCODE_NUM_LOCK] = 0x90;
	xynth->keymap[S_KEYCODE_SCROLL_LOCK] = 0x91;
	xynth->keymap[S_KEYCODE_KP_7] = 0x67;
	xynth->keymap[S_KEYCODE_KP_8] = 0x68;
	xynth->keymap[S_KEYCODE_KP_9] = 0x69;
	xynth->keymap[S_KEYCODE_KP_SUBTRACT] = 0x6d;
	xynth->keymap[S_KEYCODE_KP_4] = 0x64;
	xynth->keymap[S_KEYCODE_KP_5] = 0x65;
	xynth->keymap[S_KEYCODE_KP_6] = 0x66;
	xynth->keymap[S_KEYCODE_KP_ADD] = 0x6b;
	xynth->keymap[S_KEYCODE_KP_1] = 0x61;
	xynth->keymap[S_KEYCODE_KP_2] = 0x62;
	xynth->keymap[S_KEYCODE_KP_3] = 0x63;
	xynth->keymap[S_KEYCODE_KP_0] = 0x60;
	xynth->keymap[S_KEYCODE_KP_PERIOD] = 0x6c;
	xynth->keymap[S_KEYCODE_LAST_CONSOLE] = 0x0;
	xynth->keymap[S_KEYCODE_LESS] = 0x0;
	xynth->keymap[S_KEYCODE_F11] = 0x7a;
	xynth->keymap[S_KEYCODE_F12] = 0x7b;
	xynth->keymap[S_KEYCODE_KP_ENTER] = -'\n';
	xynth->keymap[S_KEYCODE_RIGHTCONTROL] = 0x11;
	xynth->keymap[S_KEYCODE_KP_DIVIDE] = 0x6f;
	xynth->keymap[S_KEYCODE_VOIDSYMBOL] = 0x9a;
	xynth->keymap[S_KEYCODE_ALTGR] = 0x9d;
	xynth->keymap[S_KEYCODE_BREAK] = 0x0;
	xynth->keymap[S_KEYCODE_HOME] = 0x24;
	xynth->keymap[S_KEYCODE_UP] = 0x26;
	xynth->keymap[S_KEYCODE_PAGEUP] = 0x21;
	xynth->keymap[S_KEYCODE_LEFT] = 0x25;
	xynth->keymap[S_KEYCODE_RIGHT] = 0x27;
	xynth->keymap[S_KEYCODE_END] = 0x23;
	xynth->keymap[S_KEYCODE_DOWN] = 0x28;
	xynth->keymap[S_KEYCODE_PAGEDOWN] = 0x22;
	xynth->keymap[S_KEYCODE_INSERT] = 0x9b;
	xynth->keymap[S_KEYCODE_REMOVE] = -0x7f;
	xynth->keymap[S_KEYCODE_PAUSE] = 0x13;
	xynth->keymap[S_KEYCODE_EXCLAM] = 0x0;
	xynth->keymap[S_KEYCODE_AT] = 0x0;
	xynth->keymap[S_KEYCODE_NUMBERSIGN] = 0x0;
	xynth->keymap[S_KEYCODE_DOLLAR] = 0x0;
	xynth->keymap[S_KEYCODE_PERCENT] = 0x0;
	xynth->keymap[S_KEYCODE_ASCIICIRCUM] = 0x0;
	xynth->keymap[S_KEYCODE_AMPERSAND] = 0x0;
	xynth->keymap[S_KEYCODE_ASTERISK] = 0x0;
	xynth->keymap[S_KEYCODE_PARENLEFT] = 0x0;
	xynth->keymap[S_KEYCODE_PARENRIGHT] = 0x0;
	xynth->keymap[S_KEYCODE_UNDERSCORE] = 0x0;
	xynth->keymap[S_KEYCODE_PLUS] = 0x0;
	xynth->keymap[S_KEYCODE_Q] = 0x0;
	xynth->keymap[S_KEYCODE_W] = 0x0;
	xynth->keymap[S_KEYCODE_E] = 0x0;
	xynth->keymap[S_KEYCODE_R] = 0x0;
	xynth->keymap[S_KEYCODE_T] = 0x0;
	xynth->keymap[S_KEYCODE_Y] = 0x0;
	xynth->keymap[S_KEYCODE_U] = 0x0;
	xynth->keymap[S_KEYCODE_I] = 0x0;
	xynth->keymap[S_KEYCODE_O] = 0x0;
	xynth->keymap[S_KEYCODE_P] = 0x0;
	xynth->keymap[S_KEYCODE_BRACELEFT] = 0x0;
	xynth->keymap[S_KEYCODE_BRACERIGHT] = 0x0;
	xynth->keymap[S_KEYCODE_A] = 0x0;
	xynth->keymap[S_KEYCODE_S] = 0x0;
	xynth->keymap[S_KEYCODE_D] = 0x0;
	xynth->keymap[S_KEYCODE_F] = 0x0;
	xynth->keymap[S_KEYCODE_G] = 0x0;
	xynth->keymap[S_KEYCODE_H] = 0x0;
	xynth->keymap[S_KEYCODE_J] = 0x0;
	xynth->keymap[S_KEYCODE_K] = 0x0;
	xynth->keymap[S_KEYCODE_L] = 0x0;
	xynth->keymap[S_KEYCODE_COLON] = 0x0;
	xynth->keymap[S_KEYCODE_QUOTEDBL] = 0x0;
	xynth->keymap[S_KEYCODE_ASCIITILDE] = 0x0;
	xynth->keymap[S_KEYCODE_BAR] = 0x0;
	xynth->keymap[S_KEYCODE_Z] = 0x0;
	xynth->keymap[S_KEYCODE_X] = 0x0;
	xynth->keymap[S_KEYCODE_C] = 0x0;
	xynth->keymap[S_KEYCODE_V] = 0x0;
	xynth->keymap[S_KEYCODE_B] = 0x0;
	xynth->keymap[S_KEYCODE_N] = 0x0;
	xynth->keymap[S_KEYCODE_M] = 0x0;
	xynth->keymap[S_KEYCODE_GREATER] = 0x0;
	xynth->keymap[S_KEYCODE_QUESTION] = 0x0;
	xynth->keymap[S_KEYCODE_F13] = 0x0;
	xynth->keymap[S_KEYCODE_F14] = 0x0;
	xynth->keymap[S_KEYCODE_F15] = 0x0;
	xynth->keymap[S_KEYCODE_F16] = 0x0;
	xynth->keymap[S_KEYCODE_F17] = 0x0;
	xynth->keymap[S_KEYCODE_F18] = 0x0;
	xynth->keymap[S_KEYCODE_F19] = 0x0;
	xynth->keymap[S_KEYCODE_F20] = 0x0;
	xynth->keymap[S_KEYCODE_F21] = 0x0;
	xynth->keymap[S_KEYCODE_F22] = 0x0;
	xynth->keymap[S_KEYCODE_SHOW_MEMORY] = 0x0;
	xynth->keymap[S_KEYCODE_F23] = 0x0;
	xynth->keymap[S_KEYCODE_F24] = 0x0;
	xynth->keymap[S_KEYCODE_SCROLL_BACKWARD] = 0x0;
	xynth->keymap[S_KEYCODE_SCROLL_FORWARD] = 0x0;
	xynth->keymap[S_KEYCODE_CURRENCY] = 0x0;
	xynth->keymap[S_KEYCODE_CENT] = 0x0;
	xynth->keymap[S_KEYCODE_HEX_C] = 0x0;
	xynth->keymap[S_KEYCODE_HEX_A] = 0x0;
	xynth->keymap[S_KEYCODE_SHOW_REGISTERS] = 0x0;
	xynth->keymap[S_KEYCODE_HEX_7] = 0x0;
	xynth->keymap[S_KEYCODE_HEX_8] = 0x0;
	xynth->keymap[S_KEYCODE_HEX_9] = 0x0;
	xynth->keymap[S_KEYCODE_HEX_D] = 0x0;
	xynth->keymap[S_KEYCODE_HEX_4] = 0x0;
	xynth->keymap[S_KEYCODE_HEX_5] = 0x0;
	xynth->keymap[S_KEYCODE_HEX_6] = 0x0;
	xynth->keymap[S_KEYCODE_HEX_E] = 0x0;
	xynth->keymap[S_KEYCODE_HEX_1] = 0x0;
	xynth->keymap[S_KEYCODE_HEX_2] = 0x0;
	xynth->keymap[S_KEYCODE_HEX_3] = 0x0;
	xynth->keymap[S_KEYCODE_HEX_0] = 0x0;
	xynth->keymap[S_KEYCODE_HEX_F] = 0x0;
	xynth->keymap[S_KEYCODE_HEX_B] = 0x0;
	xynth->keymap[S_KEYCODE_USER0] = 0x0;
	xynth->keymap[S_KEYCODE_USER1] = 0x0;
	xynth->keymap[S_KEYCODE_USER2] = 0x0;
	xynth->keymap[S_KEYCODE_USER3] = 0x0;
	xynth->keymap[S_KEYCODE_USER4] = 0x0;
	xynth->keymap[S_KEYCODE_USER5] = 0x0;
	xynth->keymap[S_KEYCODE_USER6] = 0x0;
	xynth->keymap[S_KEYCODE_USER7] = 0x0;
	xynth->keymap[S_KEYCODE_USER8] = 0x0;
	xynth->keymap[S_KEYCODE_USER9] = 0x0;
	xynth->keymap[S_KEYCODE_USER10] = 0x0;
	xynth->keymap[S_KEYCODE_USER11] = 0x0;
	xynth->keymap[S_KEYCODE_USER12] = 0x0;
	xynth->keymap[S_KEYCODE_USER13] = 0x0;
	xynth->keymap[S_KEYCODE_USER14] = 0x0;
	xynth->keymap[S_KEYCODE_NOCODE] = 0x0;

	DEBUGF("Leave");
	return JNI_TRUE;
}

jint Java_java_awt_Toolkit_tlkGetScreenWidth (JNIEnv *env UNUSED, jclass clazz UNUSED)
{
	DEBUGF("Enter");
	DEBUGF("Leave");
	return xynth->root->surface->linear_buf_width;
}

jint Java_java_awt_Toolkit_tlkGetScreenHeight (JNIEnv *env UNUSED, jclass clazz UNUSED)
{
	DEBUGF("Enter");
	DEBUGF("Leave");
	return xynth->root->surface->linear_buf_height;
}

jint Java_java_awt_Toolkit_tlkGetResolution (JNIEnv *env UNUSED, jclass clazz UNUSED)
{
	DEBUGF("Enter");
	DEBUGF("Leave");
	return 0;
}

jstring Java_java_awt_Toolkit_tlkVersion (JNIEnv *env, jclass clazz UNUSED)
{
	DEBUGF("Enter");
	DEBUGF("Leave");
	return (*env)->NewStringUTF(env, "Xynth Windowing System");
}

void Java_java_awt_Toolkit_tlkDisplayBanner (JNIEnv* env UNUSED, jclass clazz UNUSED, jstring banner UNUSED)
{
	DEBUGF("Enter");
	DEBUGF("Leave");
}

void Java_java_awt_Toolkit_tlkBeep (JNIEnv *env UNUSED, jclass clazz UNUSED)
{
	DEBUGF("Enter");
	DEBUGF("Leave");
}
