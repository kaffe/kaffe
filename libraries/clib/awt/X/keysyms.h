/**
 * keysyms.h - X keycode -> Java keycode mapping
 *
 * Copyright (c) 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */


#ifndef __keysyms_h
#define __keysyms_h

/*
 * "function" keycodes (keysym values > 0xff)
 *
 * The index into this array is the normalized X keysym value (XK_xx & 0xff), the
 * value is the Java keycode.
 * Negative values indicate control/function keys which should produce a non-zero
 * keyChar value (AKA "control-keys"), i.e. which should generate KEY_TYPED
 * events.
 * Note that we can't mix in latin keycodes, because even upper case letters would
 * overlap (XK_P = 0x50, XK_home = 0xff50), we have to keep them in a separate table
 *
 * Changed the FKeyCode from char to short; on a system where the char is 8bit, some
 * of the code become negative (because they are bigger than 127) and
 * produce a keytyped of unrelated characters (like "Ins", produce an 'e');
 * (MDC, Mandrakesoft).
 */

signed short FKeyCode [256] = {
0,
0,
0,
0,
0,
0,
0,
0,
-'\b',               /* BACK_SPACE      XK_BackSpace       0xFF08  back space, back char  */
-'\t',               /* TAB             XK_Tab             0xFF09                         */
-'\n',               /* ENTER           XK_Linefeed        0xFF0A  Linefeed, LF           */
0x0c,                /* CLEAR           XK_Clear           0xFF0B                         */
0,
-'\n',               /* ENTER           XK_Return          0xFF0D  Return, enter          */
0,
0,

0,                   /*                                    0xFF10                         */
0,
0,
0x13,                /* PAUSE           XK_Pause           0xFF13  Pause, hold            */
0x91,                /* SCROLL_LOCK     XK_Scroll_Lock     0xFF14                         */
0,                   /*                 XK_Sys_Req         0xFF15                         */
0,
0,
0,
0,
0,
-0x1b,               /* ESCAPE          XK_Escape          0xFF1B                         */
0,
0,
0,
0,

0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,  /* 0xFF20 */
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,  /* 0xFF30 */
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,  /* 0xFF40 */

0x24,               /* HOME            XK_Home            0xFF50                         */
0x25,               /* LEFT            XK_Left            0xFF51  Move left, left arrow  */
0x26,               /* UP              XK_Up              0xFF52  Move up, up arrow      */
0x27,               /* RIGHT           XK_Right           0xFF53  Move right, right arrow */
0x28,               /* DOWN            XK_Down            0xFF54  Move down, down arrow  */
0x21,               /* PAGE_UP         XK_Prior Page_Up   0xFF55  Prior, previous        */
0x22,               /* PAGE_DOWN       XK_Next Page_Down  0xFF56  Next                   */
0x23,               /* END             XK_End             0xFF57  EOL                    */
0,                  /*                 XK_Begin           0xFF58  BOL                    */
0,
0,
0,
0,
0,
0,
0,

0,                  /*                 XK_Select          0xFF60  Select, mark           */
0x9a,               /* PRINTSCREEN     XK_Print           0xFF61                         */
0,                  /*                 XK_Execute         0xFF62  Execute, run, do       */
0x9b,               /* INSERT          XK_Insert          0xFF63  Insert, insert here    */
0,
0,                  /*                 XK_Undo            0xFF65  Undo, oops             */
0,                  /*                 XK_Redo            0xFF66  redo, again            */
0,                  /*                 XK_Menu            0xFF67                         */
0,                  /*                 XK_Find            0xFF68  Find, search           */
0x03,               /* CANCEL          XK_Cancel          0xFF69  Cancel, stop, abort, exit */
0x9c,               /* HELP            XK_Help            0xFF6A  Help                   */
0,                  /*                 XK_Break           0xFF6B                         */
0,
0,
0,
0,

0,                  /* 0xFF70                                                            */
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0x1f,               /* MODECHANGE      XK_Mode_switch, script_switch    0xFF7E  Character set switch   */
0x90,               /* NUM_LOCK        XK_Num_Lock        0xFF7F                         */

-' ',               /* SPACE           XK_KP_Space        0xFF80  space                  */
0,
0,
0,
0,
0,
0,
0,
0,
-'\t',               /* TAB             XK_KP_Tab          0xFF89                         */
0,
0,
0,
-'\n',               /* ENTER           XK_KP_Enter        0xFF8D  enter                  */
0,
0,

0,                 /*                                     0xFF90                         */
0x70,              /* F1               XK_KP_F1           0xFF91  PF1, KP_A, ...         */
0x71,              /* F2               XK_KP_F2           0xFF92                         */
0x72,              /* F3               XK_KP_F3           0xFF93                         */
0x73,              /* F4               XK_KP_F4           0xFF94                         */
0x24,              /* HOME             XK_KP_Home         0xFF95                         */
0x25,              /* LEFT             XK_KP_Left         0xFF96                         */
0x26,              /* UP               XK_KP_Up           0xFF97                         */
0x27,              /* RIGHT            XK_KP_Right        0xFF98                         */
0x28,              /* DOWN             XK_KP_Down         0xFF99                         */
0x21,              /* PAGE_UP          XK_KP_Prior Page_Up 0xFF9A                        */
0x22,              /* PAGE_DOWN        XK_KP_Next Page_Down 0xFF9B                       */
0x23,              /* END              XK_KP_End          0xFF9C                         */
0,                 /*                  XK_KP_Begin        0xFF9D                         */
0x9b,              /* INSERT           XK_KP_Insert       0xFF9E                         */
-0x7f,             /* DELETE           XK_KP_Delete       0xFF9F                         */

0,                 /*                                     0xFFA0                         */
0,
0,
0,
0,
0,
0,
0,
0,
0,
0x6a,              /* MULTIPLY         XK_KP_Multiply     0xFFAA                         */
0x6b,              /* ADD              XK_KP_Add          0xFFAB                         */
0x6c,              /* SEPARATER        XK_KP_Separator    0xFFAC  separator, often comma */
0x6d,              /* SUBTRACT         XK_KP_Subtract     0xFFAD                         */
0x6e,              /* DECIMAL          XK_KP_Decimal      0xFFAE                         */
0x6f,              /* DIVIDE           XK_KP_Divide       0xFFAF                         */

0x60,              /* NUMPAD0          XK_KP_0            0xFFB0                         */
0x61,              /* NUMPAD1          XK_KP_1            0xFFB1                         */
0x62,              /* NUMPAD2          XK_KP_2            0xFFB2                         */
0x63,              /* NUMPAD3          XK_KP_3            0xFFB3                         */
0x64,              /* NUMPAD4          XK_KP_4            0xFFB4                         */
0x65,              /* NUMPAD5          XK_KP_5            0xFFB5                         */
0x66,              /* NUMPAD6          XK_KP_6            0xFFB6                         */
0x67,              /* NUMPAD7          XK_KP_7            0xFFB7                         */
0x68,              /* NUMPAD8          XK_KP_8            0xFFB8                         */
0x69,              /* NUMPAD9          XK_KP_9            0xFFB9                         */
0,
0,
0,
0x3d,              /* EQUALS           XK_KP_Equal        0xFFBD  equals                 */
0x70,              /* F1               XK_F1              0xFFBE                         */
0x71,              /* F2               XK_F2              0xFFBF                         */

0x72,              /* F3               XK_F3              0xFFC0                         */
0x73,              /* F4               XK_F4              0xFFC1                         */
0x74,              /* F5               XK_F5              0xFFC2                         */
0x75,              /* F6               XK_F6              0xFFC3                         */
0x76,              /* F7               XK_F7              0xFFC4                         */
0x77,              /* F8               XK_F8              0xFFC5                         */
0x78,              /* F9               XK_F9              0xFFC6                         */
0x79,              /* F10              XK_F10             0xFFC7                         */
0x7a,              /* F11              XK_F11 L1          0xFFC8                         */
0x7b,              /* F12              XK_F12             0xFFC9                         */
0,
0,
0,
0,
0,
0,

0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,                       /* 0xFFD0 */
0,                 /*                                     0cFFE0 */
0x10,              /* SHIFT            XK_Shift_L         0xFFE1                        */
0x10,              /* SHIFT            XK_Shift_R         0xFFE2                        */
0x11,              /* CONTROL          XK_Control_L       0xFFE3                        */
0x11,              /* CONTROL          XK_Control_R       0xFFE4                        */
0x14,              /* CAPS_LOCK        XK_Caps_Lock       0xFFE5                        */
0,                 /*                  XK_Shift_Lock      0xFFE6                        */
0x9d,              /* META             XK_Meta_L          0xFFE7                        */
0x9d,              /* META             XK_Meta_R          0xFFE8                        */
0x12,              /* ALT              XK_Alt_L           0xFFE9                        */
0x12,              /* ALT              XK_Alt_R           0xFFEA                        */
0,                 /*                  XK_Super_L         0xFFEB                        */
0,                 /*                  XK_Super_R         0xFFEC                        */
0,                 /*                  XK_Hyper_L         0xFFED                        */
0,                 /*                  XK_Hyper_R         0xFFEE                        */
0,

0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,                         /* 0xFFF0 */
-0x7f              /* DELETE           XK_Delete          0xFFFF  Delete, rubout         */
};



signed char LKeyCode [256] = {

0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,                         /* 0x000 */
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,                         /* 0x010 */

0x20,              /* SPACE            XK_Space             0x020                        */
0x00,              /*                  XK_exclam            0x021                        */
0x00,              /*                  XK_quotedbl          0x022                        */
0x00,              /*                  XK_numbersign        0x023                        */
0x00,              /*                  XK_dollar            0x024                        */
0x00,              /*                  XK_percent           0x025                        */
0x00,              /*                  XK_ampersand         0x026                        */
0xc0,              /* BACK_QUOTE       XK_apostrophe        0x027                        */
0x00,              /*                  XK_parenleft         0x028                        */
0x00,              /*                  XK_parenright        0x029                        */
0x00,              /*                  XK_asterisk          0x02a                        */
0x00,              /*                  XK_plus              0x02b                        */
0x2c,              /* COMMA            XK_comma             0x02c                        */
0x00,              /*                  XK_minus             0x02d                        */
0x2e,              /* PERIOD           XK_period            0x02e                        */
0x2f,              /* SLASH            XK_slash             0x02f                        */

0x30,              /* 0                XK_0                 0x030                        */
0x31,              /* 1                XK_1                 0x031                        */
0x32,              /* 2                XK_2                 0x032                        */
0x33,              /* 3                XK_3                 0x033                        */
0x34,              /* 4                XK_4                 0x034                        */
0x35,              /* 5                XK_5                 0x035                        */
0x36,              /* 6                XK_6                 0x036                        */
0x37,              /* 7                XK_7                 0x037                        */
0x38,              /* 8                XK_8                 0x038                        */
0x39,              /* 9                XK_9                 0x039                        */
0x00,              /*                  XK_colon             0x03a                        */
0x3b,              /* SEMICOLON        XK_semicolon         0x03b                        */
0x00,              /*                  XK_less              0x03c                        */
0x3d,              /* EQUALS           XK_equal             0x03d                        */
0x00,              /*                  XK_greater           0x03e                        */
0x00,              /*                  XK_question          0x03f                        */

0x00,              /*                  XK_at                0x040                        */
0x41,              /* A                XK_A                 0x041                        */
0x42,              /* B                XK_B                 0x042                        */
0x43,              /* C                XK_C                 0x043                        */
0x44,              /* D                XK_D                 0x044                        */
0x45,              /* E                XK_E                 0x045                        */
0x46,              /* F                XK_F                 0x046                        */
0x47,              /* G                XK_G                 0x047                        */
0x48,              /* H                XK_H                 0x048                        */
0x49,              /* I                XK_I                 0x049                        */
0x4a,              /* J                XK_J                 0x04a                        */
0x4b,              /* K                XK_K                 0x04b                        */
0x4c,              /* L                XK_L                 0x04c                        */
0x4d,              /* M                XK_M                 0x04d                        */
0x4e,              /* N                XK_N                 0x04e                        */
0x4f,              /* O                XK_O                 0x04f                        */

0x50,              /* P                XK_P                 0x050                        */
0x51,              /* Q                XK_Q                 0x051                        */
0x52,              /* R                XK_R                 0x052                        */
0x53,              /* S                XK_S                 0x053                        */
0x54,              /* T                XK_T                 0x054                        */
0x55,              /* U                XK_U                 0x055                        */
0x56,              /* V                XK_V                 0x056                        */
0x57,              /* W                XK_W                 0x057                        */
0x58,              /* X                XK_X                 0x058                        */
0x59,              /* Y                XK_Y                 0x059                        */
0x5a,              /* Z                XK_Z                 0x05a                        */
0x5b,              /* OPEN_BRACKET     XK_bracketleft       0x05b                        */
0x5c,              /* BACK_SLASH       XK_backslash         0x05c                        */
0x5d,              /* CLOSE_BRACKET    XK_bracketright      0x05d                        */
0x00,              /*                  XK_asciicircum       0x05e                        */
0x00,              /*                  XK_underscore        0x05f                        */

0xde,              /* QUOTE            XK_grave             0x060                        */
0x41,              /* A                XK_a                 0x061                        */
0x42,              /* B                XK_b                 0x062                        */
0x43,              /* C                XK_c                 0x063                        */
0x44,              /* D                XK_d                 0x064                        */
0x45,              /* E                XK_e                 0x065                        */
0x46,              /* F                XK_f                 0x066                        */
0x47,              /* G                XK_g                 0x067                        */
0x48,              /* H                XK_h                 0x068                        */
0x49,              /* I                XK_i                 0x069                        */
0x4a,              /* J                XK_j                 0x06a                        */
0x4b,              /* K                XK_k                 0x06b                        */
0x4c,              /* L                XK_l                 0x06c                        */
0x4d,              /* M                XK_m                 0x06d                        */
0x4e,              /* N                XK_n                 0x06e                        */
0x4f,              /* O                XK_o                 0x06f                        */

0x50,              /* P                XK_p                 0x070                        */
0x51,              /* Q                XK_q                 0x071                        */
0x52,              /* R                XK_r                 0x072                        */
0x53,              /* S                XK_s                 0x073                        */
0x54,              /* T                XK_t                 0x074                        */
0x55,              /* U                XK_u                 0x075                        */
0x56,              /* V                XK_v                 0x076                        */
0x57,              /* W                XK_w                 0x077                        */
0x58,              /* X                XK_x                 0x078                        */
0x59,              /* Y                XK_y                 0x079                        */
0x5a,              /* Z                XK_z                 0x07a                        */
0x00,              /*                  XK_braceleft         0x07b                        */
0x00,              /*                  XK_bar               0x07c                        */
0x00,              /*                  XK_braceright        0x07d                        */
0x00,              /*                  XK_asciitilde        0x07e                        */
0x00,              /*                                       0x07f                        */

0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,                         /* 0x080 */
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,                         /* 0x090 */

0x00,              /*                  XK_nobreakspace      0x0a0                        */
0x00,              /*                  XK_exclamdown        0x0a1                        */
0x00,              /*                  XK_cent              0x0a2                        */
0x00,              /*                  XK_sterling          0x0a3                        */
0x00,              /*                  XK_currency          0x0a4                        */
0x00,              /*                  XK_yen               0x0a5                        */
0x00,              /*                  XK_brokenbar         0x0a6                        */
0x00,              /*                  XK_section           0x0a7                        */
0x00,              /*                  XK_diaeresis         0x0a8                        */
0x00,              /*                  XK_copyright         0x0a9                        */
0x00,              /*                  XK_ordfeminine       0x0aa                        */
0x00,              /*                  XK_guillemotleft     0x0ab                        */
0x00,              /*                  XK_notsign           0x0ac                        */
0x00,              /*                  XK_hyphen            0x0ad                        */
0x00,              /*                  XK_registered        0x0ae                        */
0x00,              /*                  XK_macron            0x0af                        */

0x00,              /*                  XK_degree            0x0b0                        */
0x00,              /*                  XK_plusminus         0x0b1                        */
0x00,              /*                  XK_twosuperior       0x0b2                        */
0x00,              /*                  XK_threesuperior     0x0b3                        */
0x00,              /*                  XK_acute             0x0b4                        */
0x00,              /*                  XK_mu                0x0b5                        */
0x00,              /*                  XK_paragraph         0x0b6                        */
0x00,              /*                  XK_periodcentered    0x0b7                        */
0x00,              /*                  XK_cedilla           0x0b8                        */
0x00,              /*                  XK_onesuperior       0x0b9                        */
0x00,              /*                  XK_masculine         0x0ba                        */
0x00,              /*                  XK_guillemotright    0x0bb                        */
0x00,              /*                  XK_onequarter        0x0bc                        */
0x00,              /*                  XK_onehalf           0x0bd                        */
0x00,              /*                  XK_threequarters     0x0be                        */
0x00,              /*                  XK_questiondown      0x0bf                        */

0x00,              /*                  XK_Agrave            0x0c0                        */
0x00,              /*                  XK_Aacute            0x0c1                        */
0x00,              /*                  XK_Acircumflex       0x0c2                        */
0x00,              /*                  XK_Atilde            0x0c3                        */
0x00,              /*                  XK_Adiaeresis        0x0c4                        */
0x00,              /*                  XK_Aring             0x0c5                        */
0x00,              /*                  XK_AE                0x0c6                        */
0x00,              /*                  XK_Ccedilla          0x0c7                        */
0x00,              /*                  XK_Egrave            0x0c8                        */
0x00,              /*                  XK_Eacute            0x0c9                        */
0x00,              /*                  XK_Ecircumflex       0x0ca                        */
0x00,              /*                  XK_Ediaeresis        0x0cb                        */
0x00,              /*                  XK_Igrave            0x0cc                        */
0x00,              /*                  XK_Iacute            0x0cd                        */
0x00,              /*                  XK_Icircumflex       0x0ce                        */
0x00,              /*                  XK_Idiaeresis        0x0cf                        */

0x00,              /*                  XK_ETH               0x0d0                        */
0x00,              /*                  XK_Ntilde            0x0d1                        */
0x00,              /*                  XK_Ograve            0x0d2                        */
0x00,              /*                  XK_Oacute            0x0d3                        */
0x00,              /*                  XK_Ocircumflex       0x0d4                        */
0x00,              /*                  XK_Otilde            0x0d5                        */
0x00,              /*                  XK_Odiaeresis        0x0d6                        */
0x00,              /*                  XK_multiply          0x0d7                        */
0x00,              /*                  XK_Ooblique          0x0d8                        */
0x00,              /*                  XK_Ugrave            0x0d9                        */
0x00,              /*                  XK_Uacute            0x0da                        */
0x00,              /*                  XK_Ucircumflex       0x0db                        */
0x00,              /*                  XK_Udiaeresis        0x0dc                        */
0x00,              /*                  XK_Yacute            0x0dd                        */
0x00,              /*                  XK_THORN             0x0de                        */
0x00,              /*                  XK_ssharp            0x0df                        */

0x00,              /*                  XK_agrave            0x0e0                        */
0x00,              /*                  XK_aacute            0x0e1                        */
0x00,              /*                  XK_acircumflex       0x0e2                        */
0x00,              /*                  XK_atilde            0x0e3                        */
0x00,              /*                  XK_adiaeresis        0x0e4                        */
0x00,              /*                  XK_aring             0x0e5                        */
0x00,              /*                  XK_ae                0x0e6                        */
0x00,              /*                  XK_ccedilla          0x0e7                        */
0x00,              /*                  XK_egrave            0x0e8                        */
0x00,              /*                  XK_eacute            0x0e9                        */
0x00,              /*                  XK_ecircumflex       0x0ea                        */
0x00,              /*                  XK_ediaeresis        0x0eb                        */
0x00,              /*                  XK_igrave            0x0ec                        */
0x00,              /*                  XK_iacute            0x0ed                        */
0x00,              /*                  XK_icircumflex       0x0ee                        */
0x00,              /*                  XK_idiaeresis        0x0ef                        */

0x00,              /*                  XK_eth               0x0f0                        */
0x00,              /*                  XK_ntilde            0x0f1                        */
0x00,              /*                  XK_ograve            0x0f2                        */
0x00,              /*                  XK_oacute            0x0f3                        */
0x00,              /*                  XK_ocircumflex       0x0f4                        */
0x00,              /*                  XK_otilde            0x0f5                        */
0x00,              /*                  XK_odiaeresis        0x0f6                        */
0x00,              /*                  XK_division          0x0f7                        */
0x00,              /*                  XK_oslash            0x0f8                        */
0x00,              /*                  XK_ugrave            0x0f9                        */
0x00,              /*                  XK_uacute            0x0fa                        */
0x00,              /*                  XK_ucircumflex       0x0fb                        */
0x00,              /*                  XK_udiaeresis        0x0fc                        */
0x00,              /*                  XK_yacute            0x0fd                        */
0x00,              /*                  XK_thorn             0x0fe                        */
0x00               /*                  XK_ydiaeresis        0x0ff                        */
};

#endif





