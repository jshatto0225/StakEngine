#pragma once

#include <GLFW/glfw3.h>

#include <string>

#include "Window.h"
#include "Event.h"

namespace Stak {

class GLFWWindow : public Window {
public:
  GLFWWindow(const WindowConfig &cfg);
  ~GLFWWindow();

  void update();

  WindowSizeData getWindowSize();
  WindowPosData getWindowPos();

  inline void *getNativeHandle() const { return static_cast<void *>(m_NativeHandle); }

  void setEventFn(const EventFn &func);

private:
  struct WindowData {
    EventFn eventFunction;
    i32 x;
    i32 y;
    i32 width;
    i32 height;
    std::string title;
  };

  WindowData m_Data;
  GLFWwindow *m_NativeHandle;

  static bool s_GLFWInitialized;
};

} // namespace Stak
