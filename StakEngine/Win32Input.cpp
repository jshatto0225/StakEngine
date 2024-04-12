#include "Win32Platform.h"

#ifdef SK_WINDOWS

#include "Input.h"


///////////////////////
// Private Interface //
///////////////////////

const i32 KeyMap[K_LAST + 1] =
{
    VK_LBUTTON,
    VK_RBUTTON,
    VK_MBUTTON,
    VK_SPACE,
    VK_OEM_7,
    VK_OEM_COMMA,
    VK_OEM_MINUS,
    VK_OEM_PERIOD,
    VK_OEM_2,
    0x30,
    0x31,
    0x32,
    0x33,
    0x34,
    0x35,
    0x36,
    0x37,
    0x38,
    0x39,
    VK_OEM_1,
    VK_OEM_PLUS,
    0x41,
    0x42,
    0x43,
    0x44,
    0x45,
    0x46,
    0x47,
    0x48,
    0x49,
    0x4A,
    0x4B,
    0x4C,
    0x4D,
    0x4E,
    0x4F,
    0x50,
    0x51,
    0x52,
    0x53,
    0x54,
    0x55,
    0x56,
    0x57,
    0x58,
    0x59,
    0x5A,
    VK_OEM_4,
    VK_OEM_4,
    VK_OEM_6,
    VK_OEM_3,
    VK_ESCAPE,
    VK_RETURN,
    VK_TAB,
    VK_BACK,
    VK_INSERT,
    VK_DELETE,
    VK_RIGHT,
    VK_LEFT,
    VK_DOWN,
    VK_UP,
    VK_PRIOR,
    VK_NEXT,
    VK_HOME,
    VK_END,
    VK_CAPITAL,
    VK_SCROLL,
    VK_NUMLOCK,
    VK_SNAPSHOT,
    VK_PAUSE,
    VK_F1,
    VK_F2,
    VK_F3,
    VK_F4,
    VK_F5,
    VK_F6,
    VK_F7,
    VK_F8,
    VK_F9,
    VK_F10,
    VK_F11,
    VK_F12,
    VK_F13,
    VK_F14,
    VK_F15,
    VK_F16,
    VK_F17,
    VK_F18,
    VK_F19,
    VK_F20,
    VK_F21,
    VK_F22,
    VK_F23,
    VK_F24,
    VK_NUMPAD0,
    VK_NUMPAD1,
    VK_NUMPAD2,
    VK_NUMPAD3,
    VK_NUMPAD4,
    VK_NUMPAD5,
    VK_NUMPAD6,
    VK_NUMPAD7,
    VK_NUMPAD8,
    VK_NUMPAD9,
    VK_DECIMAL,
    VK_DIVIDE,
    VK_MULTIPLY,
    VK_SUBTRACT,
    VK_ADD,
    VK_RETURN,
    VK_RETURN, // NOTE: On Windows the key at this position does not exist
    VK_LSHIFT,
    VK_LCONTROL,
    VK_LMENU,
    VK_LWIN,
    VK_RSHIFT,
    VK_RCONTROL,
    VK_RMENU,
    VK_RWIN,
    VK_MENU
};

//////////////////////
// Public Interface //
//////////////////////

bool
GetKeyDown(key Key)
{
    return GetAsyncKeyState(KeyMap[Key]);
}

vec2
GetMousePos()
{
    POINT Pos = {};
    GetCursorPos(&Pos);
    return vec2{ (f32)Pos.x, (f32)Pos.y };
}

#endif
