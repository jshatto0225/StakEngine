#ifdef WIN32

#include <windows.h>

#include "Input.h"
#include "Types.h"

bool Input::KeyDown(i32 key)
{
    return GetAsyncKeyState(key);
}

MousePosition Input::GetMousePosition()
{
    POINT pos = {};
    GetCursorPos(&pos);
    return { pos.x, pos.y };
}

#endif