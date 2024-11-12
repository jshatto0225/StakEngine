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

  WindowSizeData getSize();
  WindowPosData getPos();

  void initImGui();
  void imGuiNewFrame();

  inline OpaqueHandle getNativeHandle() const { return reinterpret_cast<OpaqueHandle>(mNativeHandle); }

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

  WindowData mData;
  GLFWwindow *mNativeHandle;

  static bool sGLFWInitialized;
};

} // namespace Stak
