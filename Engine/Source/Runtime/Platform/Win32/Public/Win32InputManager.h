#pragma once

#include "InputManager.h"

namespace sk {
class Win32InputManager : public InputManager {
public:
  bool key_down(i32 key) override;
  MousePosition get_mouse_position() override;
};
} // namespace SK
