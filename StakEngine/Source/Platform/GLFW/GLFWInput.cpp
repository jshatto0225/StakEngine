#include "GLFWInput.h"

#include "Application.h"
#include "GLFWWindow.h"

#include <GLFW/glfw3.h>


void GLFWEnableRawInput(TRef<IWindow> Window) {
    TRef<FGLFWWindow> GLFWWindow = std::static_pointer_cast<FGLFWWindow>(Window);
    glfwSetInputMode(GLFWWindow->GetGLFWWindow(), GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
}

void GLFWDisableRawInput(TRef<IWindow> Window) {
    TRef<FGLFWWindow> GLFWWindow = std::static_pointer_cast<FGLFWWindow>(Window);
    glfwSetInputMode(GLFWWindow->GetGLFWWindow(), GLFW_RAW_MOUSE_MOTION, GLFW_FALSE);
}

void GLFWSetCursorVisibility(TRef<IWindow> Window, ECursorVisibility Visibility) {
    TRef<FGLFWWindow> GLFWWindow = std::static_pointer_cast<FGLFWWindow>(Window);

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

    glfwSetInputMode(GLFWWindow->GetGLFWWindow(), GLFW_CURSOR, GlfwVisibility);
}
