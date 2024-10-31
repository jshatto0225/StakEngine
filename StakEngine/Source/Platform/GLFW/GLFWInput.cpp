#include "GLFWInput.h"

#include <GLFW/glfw3.h>

namespace Stak {

GLFWInputManager::GLFWInputManager(Ref<Window> window) {
  m_Window = static_cast<GLFWwindow *>(window->GetNativeHandle());
}

GLFWInputManager::~GLFWInputManager() {
  m_Window = NULL;
}

InputManager::MousePosData GLFWInputManager::GetMousePos() const {
  double x;
  double y;
  glfwGetCursorPos(m_Window, &x, &y);
  return { static_cast<i32>(x), static_cast<i32>(y) };
}

i32 GLFWInputManager::GetMouseX() const {
  return GetMousePos().X;
}

i32 GLFWInputManager::GetMouseY() const {
  return GetMousePos().Y;
}

bool GLFWInputManager::KeyDown(KeyCode key) const {
  
  return glfwGetKey(m_Window, static_cast<i16>(key));
}

bool GLFWInputManager::MouseButtonDown(MouseCode button) const {
  return glfwGetMouseButton(m_Window, static_cast<i8>(button));
}

} // namespace Stak