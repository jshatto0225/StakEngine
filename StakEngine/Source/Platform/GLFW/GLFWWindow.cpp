#include "GLFWWindow.h"

namespace Stak {

bool GLFWWindow::s_GLFWInitialized = false;

GLFWWindow::GLFWWindow(const WindowConfig &cfg) : m_Data({ NULL, 0, 0, cfg.width, cfg.height, cfg.title }), m_NativeHandle(NULL) {
  if (!s_GLFWInitialized) {
    if (!glfwInit()) {
      return;
    }
    s_GLFWInitialized = true;
  }

  m_NativeHandle = glfwCreateWindow(cfg.width, cfg.height, cfg.title, NULL, NULL);
  glfwSetWindowUserPointer(m_NativeHandle, static_cast<void *>(&m_Data));

  glfwSetWindowSizeCallback(m_NativeHandle, [](GLFWwindow *window, i32 width, i32 height) {
    WindowData *data = static_cast<WindowData *>(glfwGetWindowUserPointer(window));
    data->width = width;
    data->height = height;
    WindowResizeEvent e(width, height);
    data->eventFunction(e);
    });

  glfwSetWindowPosCallback(m_NativeHandle, [](GLFWwindow *window, i32 x, i32 y) {
    WindowData *data = static_cast<WindowData *>(glfwGetWindowUserPointer(window));
    data->x = x;
    data->y = y;
    WindowMovedEvent e(x, y);
    data->eventFunction(e);
    });

  glfwSetWindowCloseCallback(m_NativeHandle, [](GLFWwindow *window) {
    WindowData *data = static_cast<WindowData *>(glfwGetWindowUserPointer(window));
    WindowCloseEvent e;
    data->eventFunction(e);
    });
}

GLFWWindow::~GLFWWindow() {
  glfwDestroyWindow(m_NativeHandle);
  m_NativeHandle = NULL;
}

void GLFWWindow::update() {
  glfwPollEvents();
}

WindowSizeData GLFWWindow::getWindowSize() {
  return { m_Data.width, m_Data.height };
}

WindowPosData GLFWWindow::getWindowPos() {
  return { m_Data.x, m_Data.y };
}

void GLFWWindow::setEventFn(const EventFn &func) {
  m_Data.eventFunction = func;
}

} // namespace Stak
