#pragma once

#include "Types.h"

struct MousePosition
{
    i32 x;
    i32 y;
};

class Input
{
public:
    static bool KeyDown(i32 key);
    static MousePosition GetMousePosition();
    inline static i32 GetMouseX() { return GetMousePosition().x; }
    inline static i32 GetMouseY() { return GetMousePosition().y; }
};

// NOTE: Windows Key Codes
#ifdef WIN32
#define MOUSE_LEFT 0x01
#define MOUSE_RIGHT 0x02
#define CANCEL 0x03
#define MOUSE_MIDDLE 0x04
#define MOUSE_X1 0x05
#define MOUSE_X2 0x06
#define KEY_BACKSPACE 0x08
#define KEY_TAB 0x09
#define KEY_ENTER 0x0D
#define KEY_SHIFT 0x10
#define KEY_CONTROL 0x11
#define KEY_ALT 0x12
#define KEY_PAUSE 0x13
#define KEY_CAPSLOCK 0x14
#define KEY_KANA 0x15
#define KEY_HANGUL 0x15
#define KEY_IMEON 0x16
#define KEY_JUNJA 0x17
#define KEY_FINAL 0x18
#define KEY_HANJA 0x19
#define KEY_KANJI 0x19
#define KEY_IMEOFF 0x1A
#define KEY_ESCAPE 0x1B
#define KEY_CONVERT 0x1C
#define KEY_NONCONVERT 0x1D
#define KEY_ACCEPT 0x1E
#define KEY_MODECHANGE 0x1F
#define KEY_SPACE 0x20
#define KEY_PAGEUP 0x21
#define KEY_PAGEDOWN 0x22
#define KEY_END 0x23
#define KEY_HOME 0x24
#define KEY_LEFTARROW 0x25
#define KEY_UPARROW 0x26
#define KEY_RIGHTARROW 0x27
#define KEY_DOWNARROW 0x28
#define KEY_SELECT 0x29
#define KEY_PRINT 0x2A
#define KEY_PRINTSCREEN 0x2C
#define KEY_INSERT 0x2D
#define KEY_DELETE 0x2E
#define KEY_HELP 0x2F
#define KEY_0 0x30
#define KEY_1 0x31
#define KEY_2 0x32
#define KEY_3 0x33
#define KEY_4 0x34
#define KEY_5 0x35
#define KEY_6 0x36
#define KEY_7 0x37
#define KEY_8 0x38
#define KEY_9 0x39
#define KEY_A 0x41
#define KEY_B 0x42
#define KEY_C 0x43
#define KEY_D 0x44
#define KEY_E 0x45
#define KEY_F 0x46
#define KEY_G 0x47
#define KEY_H 0x48
#define KEY_I 0x49
#define KEY_J 0x4A
#define KEY_K 0x4B
#define KEY_L 0x4C
#define KEY_M 0x4D
#define KEY_N 0x4E
#define KEY_O 0x4F
#define KEY_P 0x50
#define KEY_Q 0x51
#define KEY_R 0x52
#define KEY_S 0x53
#define KEY_T 0x54
#define KEY_U 0x55
#define KEY_V 0x56
#define KEY_W 0x57
#define KEY_X 0x58
#define KEY_Y 0x59
#define KEY_Z 0x5A
#define KEY_LWINDOWS 0x5B
#define KEY_RWINDOWS 0x5C
#define KEY_APPLICATIONS 0x5D
#define KEY_SLEEP 0x5F
#define KEY_NUMPAD0 0x60
#define KEY_NUMPAD1 0x61
#define KEY_NUMPAD2 0x62
#define KEY_NUMPAD3 0x63
#define KEY_NUMPAD4 0x65
#define KEY_NUMPAD5 0x66
#define KEY_NUMPAD6 0x67
#define KEY_NUMPAD8 0x68
#define KEY_NUMPAD9 0x69
#define KEY_MULTIPLY 0x6A
#define KEY_ADD 0x6B
#define KEY_SEPARATOR 0x6C
#define KEY_SUBTRACT 0x6D
#define KEY_DECIMAL 0x6E
#define KEY_DIVIDE 0x6F
#define KEY_F1 0x70
#define KEY_F2 0x71
#define KEY_F3 0x72
#define KEY_F4 0x73
#define KEY_F5 0x74
#define KEY_F6 0x75
#define KEY_F7 0x76
#define KEY_F8 0x77
#define KEY_F9 0x78
#define KEY_F10 0x79
#define KEY_F11 0x7A
#define KEY_F12 0x7B
#define KEY_F13 0x7C
#define KEY_F14 0x7D
#define KEY_F15 0x7E
#define KEY_F16 0x7F
#define KEY_F17 0x80
#define KEY_F18 0x81
#define KEY_F19 0x82
#define KEY_F20 0x83
#define KEY_F21 0x84
#define KEY_F22 0x85
#define KEY_F23 0x86
#define KEY_F34 0x87
#define KEY_NUMLOCK 0x90
#define KEY_SCROLLLOCK 0x91
#define KEY_LSHIFT 0xA0
#define KEY_RSHIFT 0xA1
#define KEY_LCONTROL 0xA2
#define KEY_RCONTROL 0xA3
#define KEY_LMENU 0xA4
#define KEY_RMENU 0xA5
#define KEY_BROWSERBACK 0xA6
#define KEY_BROWSER_FORWARD 0xA7
#define KEY_BROWSERREFRESH 0xA8
#define KEY_BROWSERSTOP 0xA9
#define KEY_BROWSERSEARCH 0xAA
#define KEY_BROWSERFAVORITES 0xAB
#define KEY_BROSERHOME 0xAC
#define KEY_VOLUMEMUTE 0xAD
#define KEY_VOLUMEDOWN 0xAE
#define KEY_VOLUMEUP 0xAF
#define KEY_MEDIANEXTTRACK 0xB0
#define KEY_MEDIAPREVTRACK 0xB1
#define KEY_MEDIASTOP 0xB2
#define KEY_MEDIAPLAYPAUSE 0xB3
#define KEY_LAUNCHMAIL 0xB4
#define KEY_MEDIASELECT 0xB5
#define KEY_LAUNCHAPP1 0xB6
#define KEY_LAUNCHAPP2 0xB7
#define KEY_SEMICOLON 0xBA
#define KEY_PLUS 0xBB
#define KEY_COMMA 0xBC
#define KEY_MINUS 0xBD
#define KEY_PERIOD 0xBE
#define KEY_FSLASH 0xBF
#define KEY_TILDE 0xC0
#define KEY_LBRACKET 0xDB
#define KEY_BSLASH 0xDC
#define KEY_RBRACKET 0xDD
#define KEY_QUOTE 0xDE
#define KEY_OEM8 0xDF
#define KEY_OEM102 0xE2 // TODO: possibly <>
#define KEY_PROCESS 0xE5
#define KEY_PACKET 0xE7
#define KEY_ATTN 0xF6
#define KEY_CRSEL 0xF7
#define KEY_EXSEL 0xF8
#define KEY_ERASEEOF 0xF9
#define KEY_PLAY 0xFA
#define KEY_ZOOM 0xFB
#define KEY_PA1 0xFD
#define KEY_CLEAR 0xFE
#endif
