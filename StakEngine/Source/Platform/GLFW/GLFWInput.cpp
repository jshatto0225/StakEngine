#include "GLFWInput.h"

#include "FApplication.h"
#include "FGLFWWindow.h"

#include <GLFW/glfw3.h>


void GLFWEnableRawInput() {
  TRef<FGLFWWindow> Window = std::static_pointer_cast<FGLFWWindow>(FApplication::Get().GetWindow());
  glfwSetInputMode(Window->GetGlfwWindow(), GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
}

void GLFWDisableRawInput() {
  TRef<FGLFWWindow> Window = std::static_pointer_cast<FGLFWWindow>(FApplication::Get().GetWindow());
  glfwSetInputMode(Window->GetGlfwWindow(), GLFW_RAW_MOUSE_MOTION, GLFW_FALSE);
}

void GLFWSetCursorVisibility(ECursorVisibility Visibility) {
  TRef<FGLFWWindow> Window = std::static_pointer_cast<FGLFWWindow>(FApplication::Get().GetWindow());

  FSInt32 GlfwVisibility;
  switch (Visibility) {
  case ECursorVisibility::NORMAL:
    GlfwVisibility = GLFW_CURSOR_NORMAL;
    break;
  case ECursorVisibility::HIDDEN:
    GlfwVisibility = GLFW_CURSOR_HIDDEN;
    break;
  case ECursorVisibility::DISABLED:
    GlfwVisibility = GLFW_CURSOR_DISABLED;
    break;
  default:
    return;
  }

  glfwSetInputMode(Window->GetGlfwWindow(), GLFW_CURSOR, GlfwVisibility);
}
