#include "GLFWInput.h"

#include <GLFW/glfw3.h>

namespace Stak {

GLFWInput::GLFWInput(Ref<Window> window) : m_Window(window) {}

Input::MousePosData GLFWInput::getMousePosImpl() const {
  double x;
  double y;
  glfwGetCursorPos(static_cast<GLFWwindow *>(m_Window->getNativeHandle()), &x, &y);
  return { static_cast<i32>(x), static_cast<i32>(y) };
}

bool GLFWInput::keyDownImpl(KeyCode key) const {

  return glfwGetKey(static_cast<GLFWwindow *>(m_Window->getNativeHandle()), static_cast<i16>(key));
}

bool GLFWInput::mouseButtonDownImpl(MouseCode button) const {
  return glfwGetMouseButton(static_cast<GLFWwindow *>(m_Window->getNativeHandle()), static_cast<i8>(button));
}

} // namespace Stak
