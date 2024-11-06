#pragma once

#include <GLFW/glfw3.h>

#include "Input.h"

namespace Stak {

class GLFWInput : public Input {
public:
  GLFWInput(Ref<Window> window);

  MousePosData getMousePosImpl() const;
  bool keyDownImpl(KeyCode key) const;
  bool mouseButtonDownImpl(MouseCode button) const;

private:
  Ref<Window> m_Window;
};

} // namespace Stak
