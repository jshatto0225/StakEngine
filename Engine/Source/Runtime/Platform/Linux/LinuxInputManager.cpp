#include "LinuxInputManager.h"

namespace sk {
bool LinuxInputManager::key_down(i32 key) { return false; }

MousePosition LinuxInputManager::get_mouse_position() { return {0, 0}; }
} // namespace SK
