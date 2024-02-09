#pragma once

#include "InputManager.h"

namespace sk {
class LinuxInputManager : public InputManager {
public:
  bool key_down(i32 key) override;
  MousePosition get_mouse_position() override;
};
}
