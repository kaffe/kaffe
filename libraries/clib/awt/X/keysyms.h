/**
 * keysyms.h - 
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
 * negative codes indicate control/function keys which should produce a non-zero
 * keyChar value (AKA "control-keys"), i.e. which should generate KEY_TYPED
 * events
 */

signed char FKeyCode [256] = {
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
0x0c,               /* CLEAR           XK_Clear           0xFF0B                         */
0,
-'\n',               /* ENTER           XK_Return          0xFF0D  Return, enter          */
0,
0,
0,
0,
0,
0x13,               /* PAUSE           XK_Pause           0xFF13  Pause, hold            */
0x91,               /* SCROLL_LOCK     XK_Scroll_Lock     0xFF14                         */
0,                  /*                 XK_Sys_Req         0xFF15                         */
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
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,                       /* 0xFF20 */
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,  /* 0xFF30               0..9, A..Z have 3rd byte == 0  */
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,  /* 0xFF40                 -> overlaps (Z = 0x5a)       */
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
0,                                                     /* 0xFF70 */
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
0x20,               /* SPACE           XK_KP_Space        0xFF80  space                  */
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
0,
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
-0x7f,              /* DELETE           XK_KP_Delete       0xFF9F                         */
0,                                                     /* 0xffA0 */
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
-0x7f               /* DELETE           XK_Delete          0xFFFF  Delete, rubout         */
};


#endif
