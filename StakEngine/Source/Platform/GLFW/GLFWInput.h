#pragma once

#include <GLFW/glfw3.h>

#include "../../Core/Input.h"

namespace Stak {

class GLFWInputManager : public InputManager {
public:
  GLFWInputManager(Ref<Window> window);
  ~GLFWInputManager();

  MousePosData GetMousePos() const;
  i32 GetMouseX() const;
  i32 GetMouseY() const;

  bool KeyDown(KeyCode key) const;
  bool MouseButtonDown(MouseCode button) const;

private:
  GLFWwindow *m_Window;
};

} // namespace Stak