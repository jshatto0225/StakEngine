#pragma once

#include <GLFW/glfw3.h>

#include "Input.h"

namespace Stak {

class GLFWInput : public Input {
public:
  GLFWInput(Ref<Window> window);
  ~GLFWInput();

  MousePosData getMousePosImpl() const;
  bool keyDownImpl(KeyCode key) const;
  bool mouseButtonDownImpl(MouseCode button) const;

private:
  Ref<Window> mWindow;
};

} // namespace Stak
