#pragma once

#include <GLFW/glfw3.h>

#include "Input.h"

namespace Stak {

class GLFWInput : public Input {
public:
  GLFWInput(Ref<Window> window);

  MousePosData GetMousePosImpl() const;
  bool KeyDownImpl(KeyCode key) const;
  bool MouseButtonDownImpl(MouseCode button) const;

private:
  Ref<Window> m_Window;
};

} // namespace Stak