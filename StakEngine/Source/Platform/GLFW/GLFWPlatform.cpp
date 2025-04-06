#include <imgui.h>
#include <backends/imgui_impl_glfw.h>

#include <glfw/glfw3.h>

#include "RHI.h"
#include "Window.h"
#include "Log.h"

static bool glfw_initialized = false;

bool platform_init() {
    if (!glfwInit()) {
        return false;
    }

    glfw_initialized = true;

    return true;
}

void platform_shutdown() {
    assert(glfw_initialized);

    glfwTerminate();
    glfw_initialized = false;
}

bool platform_open_window(Window *window, const Window_Config *cfg) {
    assert(glfw_initialized);
    assert(window);

    if (window->open) {
        SK_LOG_WARN("Window already open");
        return true;
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    window->width   = cfg->width;
    window->height  = cfg->height;
    window->title   = cfg->title;

    window->event_function = cfg->event_function;

    auto handle = glfwCreateWindow(cfg->width, cfg->height, cfg->title, nullptr, nullptr);

    if (!handle) {
        SK_LOG_ERROR("Failed to create glfw window");
        return false;
    }

    window->platform_handle = (Handle)handle;

    glfwSetWindowUserPointer(handle, static_cast<void *>(window));
    glfwGetFramebufferSize(handle, &window->framebuffer_width, &window->framebuffer_height);

    glfwSetFramebufferSizeCallback(handle, [](GLFWwindow *window, s32 width, s32 height) {
        auto *data = static_cast<Window *>(glfwGetWindowUserPointer(window));

        data->framebuffer_width = width;
        data->framebuffer_height = height;

        rhi.notify_swapchain_of_resize(data->swapchain);
    });

    glfwSetWindowSizeCallback(handle, [](GLFWwindow *window, s32 width, s32 height) {
        auto *data = static_cast<Window *>(glfwGetWindowUserPointer(window));

        data->width = width;
        data->height = height;

        if (!data->event_function) return;

        Event e = {};
        e.type = Event_Type::WINDOW_RESIZE;
        e.wre.width = width;
        e.wre.height = height;
        data->event_function(&e);
    });

    glfwSetWindowCloseCallback(handle, [](GLFWwindow *window) {
        auto *data = static_cast<Window *>(glfwGetWindowUserPointer(window));

        if (!data->event_function) return;

        Event e = {};
        e.type = Event_Type::WINDOW_CLOSE;
        data->event_function(&e);
    });

    glfwSetKeyCallback(handle, [](GLFWwindow *window, s32 key, s32 Scancode, s32 action, s32 Mods) {
        auto *data = static_cast<Window *>(glfwGetWindowUserPointer(window));

        if (!data->event_function) return;

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

        Event e = {};
        e.type = Event_Type::KEY;
        e.ke.key = static_cast<Key_Code>(key);
        e.ke.state = state;
        data->event_function(&e);
    });

    glfwSetMouseButtonCallback(handle, [](GLFWwindow *window, s32 button, s32 action, s32 Mods) {
        auto *data = static_cast<Window *>(glfwGetWindowUserPointer(window));

        if (!data->event_function) return;

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

        Event e = {};
        e.type = Event_Type::MOUSE_BUTTON;
        e.mbe.button = static_cast<Mouse_Code>(button);
        e.mbe.state = state;
        data->event_function(&e);
    });

    glfwSetCursorPosCallback(handle, [](GLFWwindow *window, double x, double y) {
        auto *data = static_cast<Window *>(glfwGetWindowUserPointer(window));

        if (!data->event_function) return;

        Event e = {};
        e.type = Event_Type::MOUSE_MOVE;
        e.mme.x = static_cast<f32>(x);
        e.mme.y = static_cast<f32>(y);
        data->event_function(&e);
    });

    window->swapchain = rhi.create_swapchain(window);
    if (!window->swapchain) {
        SK_LOG_ERROR("Failed to create viewport for window");
        return false;
    }

    window->open = true;

    return true;
}

void platform_close_window(Window *window) {
    assert(window);

    if (!window->open) {
        SK_LOG_WARN("Window already closed");
        return;
    }

    auto glfw = (GLFWwindow *)window->platform_handle;
    assert(glfw);

    rhi.destroy_swapchain(&window->swapchain);

    glfwDestroyWindow(glfw);

    window->platform_handle = 0;

    window->open = false;
}

bool platform_init_imgui(const Window *window) {
    assert(window);

    if (!window->open) {
        SK_LOG_ERROR("Failed to initialize imgui, window was not open");
        return false;
    }

    auto glfw = reinterpret_cast<GLFWwindow *>(window->platform_handle);
    assert(glfw);

    if (!ImGui_ImplGlfw_InitForVulkan(glfw, true)) {
        SK_LOG_ERROR("Failed to initialize imgui");
        return false;
    }

    return true;
}

void platform_imgui_new_frame() {
    ImGui_ImplGlfw_NewFrame();
}

void platform_shutdown_imgui() {
    ImGui_ImplGlfw_Shutdown();
}

void platform_process_messages() {
    glfwPollEvents();
}

void platform_enable_raw_input(const Window *window) {
    assert(window);

    if (!window->open) {
        SK_LOG_WARN("Cannot enable raw input, window is not open");
        return;
    }

    auto glfw = reinterpret_cast<GLFWwindow *>(window->platform_handle);
    assert(glfw);

    glfwSetInputMode(glfw, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
}

void platform_disable_raw_input(const Window *window) {
    assert(window);

    if (!window->open) {
        SK_LOG_WARN("Cannot disable raw input, window is not open");
        return;
    }

    auto glfw = reinterpret_cast<GLFWwindow *>(window->platform_handle);
    assert(glfw);

    glfwSetInputMode(glfw, GLFW_RAW_MOUSE_MOTION, GLFW_FALSE);
}

void platform_set_cursor_visibility(const Window *window, Cursor_Visibility visibility) {
    assert(window);

    if (!window->open) {
        SK_LOG_WARN("Cannot disable raw input, window is not open");
        return;
    }

    auto glfw = reinterpret_cast<GLFWwindow *>(window->platform_handle);
    assert(glfw);

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

    glfwSetInputMode(glfw, GLFW_CURSOR, glfw_visibility);
}
