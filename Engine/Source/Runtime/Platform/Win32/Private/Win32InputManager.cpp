#include <windows.h>

#include "Win32InputManager.h"
#include "Types.h"

namespace SK
{
    bool Win32InputManager::KeyDown(i32 key)
    {
        return GetAsyncKeyState(key);
    }

    MousePosition Win32InputManager::GetMousePosition()
    {
        POINT pos = {};
        GetCursorPos(&pos);
        return { pos.x, pos.y };
    }
}

