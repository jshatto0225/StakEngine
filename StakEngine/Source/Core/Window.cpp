#include "Window.h"

#if defined(SK_WINDOWS) or defined(SK_LINUX)
#include "../Platform/GLFW/GLFWWindow.h"
#endif

#include "Log.h"

namespace Stak {

Scope<Window> Window::Create(const WindowConfig &cfg) {
#if defined(SK_WINDOWS) or defined(SK_LINUX)
  return CreateScope<GLFWWindow>(cfg);
#else
  SK_LOG_CRITICAL("Window Platform Not Supported")
  return NULL;
#endif
}

} // namepsace Stak