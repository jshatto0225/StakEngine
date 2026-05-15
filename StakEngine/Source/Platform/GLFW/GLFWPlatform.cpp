#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#define GLFW_INCLUDE_NONE
#ifdef SK_WINDOWS
#define GLFW_EXPOSE_NATIVE_WIN32
#endif
#include <glfw/glfw3.h>
#include <glfw/glfw3native.h>

#include "RHI.h"
#include "Window.h"
#include "Log.h"

static bool glfw_initialized = false;

struct GlfwWindow {
    s32 x;
    s32 y;
    s32 width;
    s32 height;
    s32 framebuffer_width;
    s32 framebuffer_height;
    const char *title;
    EventFunction event_function;

    GLFWwindow *glfw;
};

bool platform_init() {
    if (!glfwInit()) {
        SK_LOG_ERROR("platform_init glfwInit failed");
        return false;
    }

    glfw_initialized = true;

    return true;
}

void platform_shutdown() {
    glfwTerminate();
    glfw_initialized = false;
}

Window platform_create_window(const WindowConfig *cfg) {
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    auto handle = glfwCreateWindow(cfg->width, cfg->height, cfg->title, nullptr, nullptr);

    if (!handle) {
        SK_LOG_ERROR("platform_create_window glfwCreateWindow failed");
        return 0;
    }

    auto window = (GlfwWindow *) malloc(sizeof(GlfwWindow));
    if (!window) {
        SK_LOG_ERROR("platform_create_window malloc failed");
        glfwDestroyWindow(handle);
        return 0;
    }

    window->width = cfg->width;
    window->height = cfg->height;
    window->title = cfg->title;

    window->event_function = cfg->event_function;

    window->glfw = handle;

    glfwSetWindowUserPointer(handle, static_cast<void *>(window));
    glfwGetFramebufferSize(handle, &window->framebuffer_width, &window->framebuffer_height);

    glfwSetFramebufferSizeCallback(handle, [](GLFWwindow *window, s32 width, s32 height) {
        auto *data = static_cast<GlfwWindow *>(glfwGetWindowUserPointer(window));

        data->framebuffer_width = width;
        data->framebuffer_height = height;
    });

    glfwSetWindowSizeCallback(handle, [](GLFWwindow *window, s32 width, s32 height) {
        auto *data = static_cast<GlfwWindow *>(glfwGetWindowUserPointer(window));

        data->width = width;
        data->height = height;

        if (!data->event_function) return;

        Event e = {};
        e.type = EventType::WindowResize;
        e.wre.width = width;
        e.wre.height = height;
        data->event_function(&e);
    });

    glfwSetWindowCloseCallback(handle, [](GLFWwindow *window) {
        auto *data = static_cast<GlfwWindow *>(glfwGetWindowUserPointer(window));

        if (!data->event_function) return;

        Event e = {};
        e.type = EventType::WindowClose;
        data->event_function(&e);
    });

    glfwSetKeyCallback(handle, [](GLFWwindow *window, s32 key, s32 scancode, s32 action, s32 mods) {
        auto *data = static_cast<GlfwWindow *>(glfwGetWindowUserPointer(window));

        if (!data->event_function) return;

        InputState state;
        switch (action) {
        case GLFW_PRESS:
            state = InputState::Down;
            break;
        case GLFW_RELEASE:
            state = InputState::Up;
            break;
        default:
            return;
        }

        Event e = {};
        e.type = EventType::Key;
        e.ke.key = static_cast<KeyCode>(key);
        e.ke.state = state;
        data->event_function(&e);
    });

    glfwSetMouseButtonCallback(handle, [](GLFWwindow *window, s32 button, s32 action, s32 mods) {
        auto *data = static_cast<GlfwWindow *>(glfwGetWindowUserPointer(window));

        if (!data->event_function) return;

        InputState state;
        switch (action) {
        case GLFW_PRESS:
            state = InputState::Down;
            break;
        case GLFW_RELEASE:
            state = InputState::Up;
            break;
        default:
            return;
        }

        Event e = {};
        e.type = EventType::MouseButton;
        e.mbe.button = static_cast<MouseCode>(button);
        e.mbe.state = state;
        data->event_function(&e);
    });

    glfwSetCursorPosCallback(handle, [](GLFWwindow *window, double x, double y) {
        auto *data = static_cast<GlfwWindow *>(glfwGetWindowUserPointer(window));

        if (!data->event_function) return;

        Event e = {};
        e.type = EventType::MouseMove;
        e.mme.x = static_cast<f32>(x);
        e.mme.y = static_cast<f32>(y);
        data->event_function(&e);
    });

    return true;
}

void platform_destroy_window(Window win) {
    auto window = (GlfwWindow *) win;

    glfwDestroyWindow(window->glfw);

    free(window);
}

bool platform_init_imgui(Window win) {
    auto window = (GlfwWindow *) win;

    if (!ImGui_ImplGlfw_InitForVulkan(window->glfw, true)) {
        SK_LOG_ERROR("platform_init_imgui ImGui_ImplGlfw_InitForVulkan failed");
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

void platform_enable_raw_input(Window win) {
    auto window = (GlfwWindow *) win;

    glfwSetInputMode(window->glfw, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
}

void platform_disable_raw_input(Window win) {
    auto window = (GlfwWindow *) win;

    glfwSetInputMode(window->glfw, GLFW_RAW_MOUSE_MOTION, GLFW_FALSE);
}

void platform_set_cursor_visibility(Window win, CursorVisibility visibility) {
    auto window = (GlfwWindow *) win;

    s32 glfw_visibility = 0;
    switch (visibility) {
    case CursorVisibility::Normal:
        glfw_visibility = GLFW_CURSOR_NORMAL;
        break;
    case CursorVisibility::Hidden:
        glfw_visibility = GLFW_CURSOR_HIDDEN;
        break;
    case CursorVisibility::Disabled:
        glfw_visibility = GLFW_CURSOR_DISABLED;
        break;
    default:
        return;
    }

    glfwSetInputMode(window->glfw, GLFW_CURSOR, glfw_visibility);
}

u32 platform_get_window_width(Window window) {
    auto win = (GlfwWindow *) window;
    return win->width;
}

u32 platform_get_window_height(Window window) {
    auto win = (GlfwWindow *) window;
    return win->height;
}

void *platform_get_window_handle(Window window) {
    auto win = (GlfwWindow *) window;
#ifdef SK_WINDOWS
    return glfwGetWin32Window(win->glfw);
#endif
}