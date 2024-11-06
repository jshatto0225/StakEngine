#include "Window.h"

#if defined(SK_WINDOWS) or defined(SK_LINUX)
#include "GLFWWindow.h"
#endif

#include "Log.h"

namespace Stak {

Scope<Window> Window::create(const WindowConfig &cfg) {
#if defined(SK_WINDOWS) or defined(SK_LINUX)
  return createScope<GLFWWindow>(cfg);
#else
  SK_LOG_CRITICAL("Window Platform Not Supported")
  return NULL;
#endif
}

} // namepsace Stak
