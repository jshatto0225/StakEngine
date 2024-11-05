#include "GLFWInput.h"

#include <GLFW/glfw3.h>

namespace Stak {

GLFWInput::GLFWInput(Ref<Window> window) : m_Window(window) {}

Input::MousePosData GLFWInput::GetMousePosImpl() const {
  double x;
  double y;
  glfwGetCursorPos(static_cast<GLFWwindow *>(m_Window->GetNativeHandle()), &x, &y);
  return { static_cast<i32>(x), static_cast<i32>(y) };
}

bool GLFWInput::KeyDownImpl(KeyCode key) const {
  
  return glfwGetKey(static_cast<GLFWwindow *>(m_Window->GetNativeHandle()), static_cast<i16>(key));
}

bool GLFWInput::MouseButtonDownImpl(MouseCode button) const {
  return glfwGetMouseButton(static_cast<GLFWwindow *>(m_Window->GetNativeHandle()), static_cast<i8>(button));
}

} // namespace Stak