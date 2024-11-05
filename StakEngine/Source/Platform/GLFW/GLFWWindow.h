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

  void Update();

  WindowSizeData GetWindowSize();
  WindowPosData GetWindowPos();

  inline void *GetNativeHandle() const { return static_cast<void *>(m_NativeHandle); }

  void SetEventFn(const EventFn &func);

private:
  struct WindowData {
    EventFn EventFunction;
    i32 X;
    i32 Y;
    i32 Width;
    i32 Height;
    std::string Title;
  };

  WindowData m_Data;
  GLFWwindow *m_NativeHandle;

  static bool s_GLFWInitialized;
};

} // namespace Stak