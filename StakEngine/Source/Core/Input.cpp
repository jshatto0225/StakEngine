#include "Input.h"

#if defined(SK_WINDOWS) or defined(SK_LINUX)
#include "../Platform/GLFW/GLFWInput.h"
#endif

#include "Log.h"

namespace Stak {

Scope<InputManager> InputManager::Create(Ref<Window> window) {
#if defined(SK_WINDOWS) or defined(SK_LINUX)
  return CreateScope<GLFWInputManager>(window);
#else
  SK_LOG_CRITICAL("Invalid Input Platform");
  return NULL;
#endif
}

} // namespace Stak