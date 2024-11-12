#include "GLFWInput.h"

#include <GLFW/glfw3.h>

namespace Stak {

GLFWInput::GLFWInput(Ref<Window> window) : mWindow(window) {}

GLFWInput::~GLFWInput() {
  mWindow = NULL;
}

Input::MousePosData GLFWInput::getMousePosImpl() const {
  double x;
  double y;
  glfwGetCursorPos(reinterpret_cast<GLFWwindow *>(mWindow->getNativeHandle()), &x, &y);
  return { static_cast<i32>(x), static_cast<i32>(y) };
}

bool GLFWInput::keyDownImpl(KeyCode key) const {

  return glfwGetKey(reinterpret_cast<GLFWwindow *>(mWindow->getNativeHandle()), static_cast<i16>(key));
}

bool GLFWInput::mouseButtonDownImpl(MouseCode button) const {
  return glfwGetMouseButton(reinterpret_cast<GLFWwindow *>(mWindow->getNativeHandle()), static_cast<i8>(button));
}

} // namespace Stak
