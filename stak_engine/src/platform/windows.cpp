#include "platform.hpp"

#include "utils/asserts.hpp"

#include <vulkan/vulkan.h>
#include <glfw/glfw3.h>

static s32 window_count = 0;

struct Window_Data {
	std::string title = "";
	s32 width = 0;
	s32 height = 0;
	s32 x = 0;
	s32 y = 0;
	Platform::Window_Callbacks callbacks = {};
};

struct Platform::Window {
	GLFWwindow* handle = nullptr;
	Window_Data data = {};
};

void Platform::get_window_size(Window *window, s32 *width, s32 *height) {
    *width = window->data.width;
    *height = window->data.height;
}

namespace Platform {

std::vector<const char *> get_vulkan_extensions() {
    u32 count;
    const char** glfw_extensions = glfwGetRequiredInstanceExtensions(&count);
    std::vector<const char *> extensions(glfw_extensions, glfw_extensions + count);
    return extensions;
}

void get_vulkan_surface(Window *window, VkInstance instance, VkSurfaceKHR *surface) {
    if (window == nullptr || surface == nullptr) {
        return;
    }
    
    glfwCreateWindowSurface(instance, window->handle, nullptr, surface);
}

}

Platform::Window* Platform::create_window(Window_Config* config, Window_Callbacks* callbacks) {
	if (window_count == 0) {
		if (!glfwInit()) {
			return nullptr;
		}
	}

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

	auto window = new Window;
	window->data.title = config->title;
	window->data.width = config->width;
	window->data.height = config->height;
	window->data.x = config->x;
	window->data.y = config->y;
	window->data.callbacks = *callbacks;

	window->handle = glfwCreateWindow(config->width, config->height, config->title.c_str(), nullptr, nullptr);
	if (window->handle == nullptr) {
		if (window_count == 0) {
			glfwTerminate();
		}
		delete window;
		return nullptr;
	}

	window_count++;

	glfwSetWindowUserPointer(window->handle, &window->data);

	glfwSetWindowCloseCallback(window->handle, [](GLFWwindow *window) {
		auto data = (Window_Data *)glfwGetWindowUserPointer(window);

		if (data->callbacks.close != nullptr) {
			data->callbacks.close();
		}
	});

	glfwSetWindowSizeCallback(window->handle, [](GLFWwindow *window, s32 width, s32 height) {
		auto data = (Window_Data*)glfwGetWindowUserPointer(window);

		if (data->callbacks.resize != nullptr) {
			Window_Resize_Event e = {
				width, 
				height
			};
			data->callbacks.resize(&e);
		}
	});

	glfwSetKeyCallback(window->handle, [](GLFWwindow* window, s32 key, s32 scancode, s32 action, s32 mods) {
		auto* data = static_cast<Window_Data*>(glfwGetWindowUserPointer(window));

		if (!data->callbacks.key) {
			return;
		}

		Input::Input_State state;
		switch (action) {
		case GLFW_PRESS:
			state = Input::Input_State::DOWN;
			break;
		case GLFW_RELEASE:
			state = Input::Input_State::UP;
			break;
		default:
			return;
		}

		Key_Event e = {
			static_cast<Input::Key_Code>(key),
			state
		};
		data->callbacks.key(&e);
	});

	glfwSetMouseButtonCallback(window->handle, [](GLFWwindow* window, s32 button, s32 action, s32 mods) {
		auto* data = static_cast<Window_Data*>(glfwGetWindowUserPointer(window));

		if (!data->callbacks.mouse_button) {
			return;
		}

		Input::Input_State state;
		switch (action) {
		case GLFW_PRESS:
			state = Input::Input_State::DOWN;
			break;
		case GLFW_RELEASE:
			state = Input::Input_State::UP;
			break;
		default:
			return;
		}

		Mouse_Button_Event e = {
			static_cast<Input::Mouse_Code>(button),
			state
		};
		data->callbacks.mouse_button(&e);
    });

	glfwSetCursorPosCallback(window->handle, [](GLFWwindow* window, double x, double y) {
		auto* data = static_cast<Window_Data*>(glfwGetWindowUserPointer(window));

		if (!data->callbacks.mouse_move) {
			return;
		}

		Mouse_Move_Event e = {
			static_cast<f32>(x),
			static_cast<f32>(y)
		};
		data->callbacks.mouse_move(&e);
	});

	return window;
}

void Platform::destroy_window(Window **window) {
	if (window == nullptr || *window) {
		return;
	}

	glfwDestroyWindow((*window)->handle);

    delete *window;
    *window = nullptr;

	window_count--;
	if (window_count == 0) {
		glfwTerminate();
	}
}

void Platform::poll_events() {
	glfwPollEvents();
}

void Platform::enable_raw_input(Window *window) {
    glfwSetInputMode(window->handle, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
}

void Platform::disable_raw_input(Window *window) {
    glfwSetInputMode(window->handle, GLFW_RAW_MOUSE_MOTION, GLFW_FALSE);
}

void Platform::hide_cursor(Window *window) {
    glfwSetInputMode(window->handle, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void Platform::show_cursor(Window *window) {
    glfwSetInputMode(window->handle, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}

