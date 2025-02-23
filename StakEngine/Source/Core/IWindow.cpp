#include "IWindow.h"

#if defined(SK_WINDOWS) or defined(SK_LINUX)
#include "FGLFWWindow.h"
#endif

#include "FLog.h"

Scope<Window> Window::create(const Window_Config &Cfg) {
#if defined(SK_WINDOWS) or defined(SK_LINUX)
    return create_scope<Glfw_Window>(Cfg);
#else
    SK_LOG_CRITICAL("Window Platform Not Supported")
        return nullptr;
#endif
}
