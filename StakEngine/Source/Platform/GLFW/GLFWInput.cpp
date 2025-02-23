#include "GLFWInput.h"

#include "FApplication.h"
#include "FGLFWWindow.h"

#include <GLFW/glfw3.h>


void glfw_enable_raw_input() {
    Ref<Glfw_Window> window = std::static_pointer_cast<Glfw_Window>(App::get().get_window());
    glfwSetInputMode(window->get_glfw_window(), GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
}

void glfw_disable_raw_input() {
    Ref<Glfw_Window> window = std::static_pointer_cast<Glfw_Window>(App::get().get_window());
    glfwSetInputMode(window->get_glfw_window(), GLFW_RAW_MOUSE_MOTION, GLFW_FALSE);
}

void glfw_set_cursor_visibility(Cursor_Visibility visibility) {
    Ref<Glfw_Window> window = std::static_pointer_cast<Glfw_Window>(App::get().get_window());

    s32 glfw_visibility;
    switch (visibility) {
    case Cursor_Visibility::NORMAL:
        glfw_visibility = GLFW_CURSOR_NORMAL;
        break;
    case Cursor_Visibility::HIDDEN:
        glfw_visibility = GLFW_CURSOR_HIDDEN;
        break;
    case Cursor_Visibility::DISABLED:
        glfw_visibility = GLFW_CURSOR_DISABLED;
        break;
    default:
        return;
    }

    glfwSetInputMode(window->get_glfw_window(), GLFW_CURSOR, glfw_visibility);
}
