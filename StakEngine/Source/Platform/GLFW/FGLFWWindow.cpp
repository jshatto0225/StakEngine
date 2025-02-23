#include "FGLFWWindow.h"

#include <imgui.h>
#include <backends/imgui_impl_glfw.h>

bool Glfw_Window::glfw_initialized = false;

Glfw_Window::Glfw_Window(const Window_Config &cfg) {
    data = { nullptr, nullptr, nullptr, nullptr, nullptr, 0, 0, cfg.width, cfg.height, cfg.title };

    if (!glfw_initialized) {
        if (!glfwInit()) {
            return;
        }
        glfw_initialized = true;
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    handle = glfwCreateWindow(cfg.width, cfg.height, cfg.title, nullptr, nullptr);
    glfwSetWindowUserPointer(handle, static_cast<void *>(&data));

    glfwSetWindowSizeCallback(handle, [](GLFWwindow *window, s32 width, s32 height) {
        auto *data = static_cast<Window_Data *>(glfwGetWindowUserPointer(window));
        
        if (!data->resize_proc) {
            return;
        }

        data->width = width;
        data->height = height;
        Window_Resize_Event e(width, height);
        data->resize_proc(e);
    });

  glfwSetWindowCloseCallback(handle, [](GLFWwindow *window) {
      auto *data = static_cast<Window_Data *>(glfwGetWindowUserPointer(window));
      
      if (!data->close_proc) {
          return;
      }

      data->close_proc();
  });

  glfwSetKeyCallback(handle, [](GLFWwindow *window, s32 key, s32 scancode, s32 action, s32 mods) {
      auto *data = static_cast<Window_Data *>(glfwGetWindowUserPointer(window));

      if (!data->key_proc) {
          return;
      }

      Input_State state;
      switch (action) {
      case GLFW_PRESS:
          state = Input_State::DOWN;
          break;
      case GLFW_RELEASE:
          state = Input_State::UP;
          break;
      default:
          return;
      }

      Key_Event e(static_cast<Key_Code>(key), state);
      data->key_proc(e);
  });

  glfwSetMouseButtonCallback(handle, [](GLFWwindow *window, s32 button, s32 action, s32 mods) {
      auto *data = static_cast<Window_Data *>(glfwGetWindowUserPointer(window));
      
      if (!data->mouse_button_proc) {
          return;
      }

      Input_State state;
      switch (action) {
      case GLFW_PRESS:
          state = Input_State::DOWN;
          break;
      case GLFW_RELEASE:
          state = Input_State::UP;
          break;
      default:
          return;
      }
      
      Mouse_Button_Event e(static_cast<Mouse_Code>(button), state);
      data->mouse_button_proc(e);
  });

  glfwSetCursorPosCallback(handle, [](GLFWwindow *window, double x, double y) {
      auto *data = static_cast<Window_Data *>(glfwGetWindowUserPointer(window));
      
      if (!data->mouse_move_proc) {
          return;
      }

      Mouse_Move_Event e(static_cast<f32>(x), static_cast<f32>(y));
      data->mouse_move_proc(e);
  });
}

Glfw_Window::~Glfw_Window() {
    glfwDestroyWindow(handle);
    handle = NULL;
}

void Glfw_Window::init_imgui() {
    ImGui_ImplGlfw_InitForVulkan(handle, true);
}

void Glfw_Window::shutdown_imgui() {
    ImGui_ImplGlfw_Shutdown();
}

void Glfw_Window::imgui_new_frame() {
    ImGui_ImplGlfw_NewFrame();
}

void Glfw_Window::update() {
    glfwPollEvents();
}

void Glfw_Window::get_size(s32* width, s32* height) {
    *width = data.width;
    *height = data.height;
}

void Glfw_Window::get_pos(s32* x, s32* y) {
    *x = data.x;
    *y = data.y;
}

void Glfw_Window::set_resize_proc(const Resize_Proc &func) {
    data.resize_proc = func;
}

void Glfw_Window::SetCloseEventFn(const Close_Proc &func) {
    data.close_proc = func;
}

void Glfw_Window::set_key_proc(const Key_Proc &func) {
    data.key_proc = func;
}

void Glfw_Window::set_mouse_button_proc(const Mouse_Button_Proc &func) {
    data.mouse_button_proc = func;
}

void Glfw_Window::set_mouse_move_proc(const Mouse_Move_Proc &func) {
    data.mouse_move_proc = func;
}
