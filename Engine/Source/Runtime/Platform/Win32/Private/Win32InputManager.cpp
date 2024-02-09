#include <windows.h>

#include "Types.h"
#include "Win32InputManager.h"

namespace sk {
bool Win32InputManager::key_down(i32 key) {
  return GetAsyncKeyState(key);
}

MousePosition Win32InputManager::get_mouse_position() {
  POINT pos = {};
  GetCursorPos(&pos);
  return { pos.x, pos.y };
}
} // namespace SK
