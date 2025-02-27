#include "glfw.hpp"

#include "window.hpp"
#include "platform.hpp"

#include "utils.hpp"
#include "log.hpp"
#include "rhi.hpp"

#include <vulkan/vulkan.h>

static s32 window_count = 0;

Array<const char *> get_vulkan_extensions() {
    u32 count;
    const char** glfw_extensions = glfwGetRequiredInstanceExtensions(&count);
    Array<const char *> extensions(glfw_extensions, glfw_extensions + count);
    return extensions;
}

void poll_events() {
	glfwPollEvents();
}

Ref<Window> create_window(const Window_Config &config, const Window_Callbacks &callbacks) {
    return create_ref<Glfw_Window>(config, callbacks);
}

Glfw_Window::Glfw_Window(const Window_Config &config, const Window_Callbacks &callbacks) {
    if (window_count == 0) {
        if (!glfwInit()) return;
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    data.title = config.title;
    data.width = config.width;
    data.height = config.height;
    data.x = config.x;
    data.y = config.y;
    data.callbacks = callbacks;

    handle = glfwCreateWindow(config.width, config.height, config.title.c_str(), nullptr, nullptr);
    if (handle == nullptr) {
        SK_LOG_ERROR("Failed to create glfw window");
        if (window_count == 0)
            glfwTerminate();
        return;
    }

    window_count++;

    glfwSetWindowUserPointer(handle, &data);

    glfwSetWindowCloseCallback(handle, [](GLFWwindow *window) {
        auto data = (Window_Data *)glfwGetWindowUserPointer(window);

        if (data->callbacks.close)
            data->callbacks.close();
    });

    glfwSetWindowSizeCallback(handle, [](GLFWwindow *window, s32 width, s32 height) {
        auto data = (Window_Data *)glfwGetWindowUserPointer(window);

        data->width = width;
        data->height = height;

        data->device->notify_window_resize();
        
        if (!data->callbacks.resize) return;
        
        Window_Resize_Event e = {
            width, 
            height
        };
        data->callbacks.resize(e);
    });

    glfwSetKeyCallback(handle, [](GLFWwindow* window, s32 key, s32 scancode, s32 action, s32 mods) {
        auto data = (Window_Data *)glfwGetWindowUserPointer(window);

        if (!data->callbacks.key) return;

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

        Key_Event e = {
            (Key_Code)key,
            state
        };
        data->callbacks.key(e);
    });

    glfwSetMouseButtonCallback(handle, [](GLFWwindow* window, s32 button, s32 action, s32 mods) {
        auto data = (Window_Data *)glfwGetWindowUserPointer(window);

        if (!data->callbacks.mouse_button) return;

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

        Mouse_Button_Event e = {
            (Mouse_Code)button,
            state
        };
        data->callbacks.mouse_button(e);
    });

    glfwSetCursorPosCallback(handle, [](GLFWwindow* window, double x, double y) {
        auto data = (Window_Data *)glfwGetWindowUserPointer(window);

        if (!data->callbacks.mouse_move) return;

        Mouse_Move_Event e = {
            (f32)x,
            (f32)y
        };
        data->callbacks.mouse_move(e);
    });
}

Glfw_Window::~Glfw_Window() {
    data.device->manual_shutdown();
        
    glfwDestroyWindow(handle);
        
    window_count--;
    if (window_count == 0)
        glfwTerminate();
}

void Glfw_Window::get_size(s32 *width, s32 *height) {
    *width = data.width;
    *height = data.height;
}
    
void Glfw_Window::enable_raw_input() {
    glfwSetInputMode(handle, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
}

void Glfw_Window::disable_raw_input() {
    glfwSetInputMode(handle, GLFW_RAW_MOUSE_MOTION, GLFW_FALSE);
}

void Glfw_Window::hide_cursor() {
    glfwSetInputMode(handle, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void Glfw_Window::show_cursor() {
    glfwSetInputMode(handle, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}

void Glfw_Window::get_vulkan_surface(void *instance, void *surface) {
    glfwCreateWindowSurface(*(VkInstance *)instance, handle, nullptr, (VkSurfaceKHR *)surface);
}

Ref<Rhi_Device> Glfw_Window::get_device() {
    if (!data.device)
        data.device = create_rhi_device(this);
    return data.device;
}
