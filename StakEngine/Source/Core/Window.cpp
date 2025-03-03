#include "Window.h"

#if defined(SK_WINDOWS) or defined(SK_LINUX)
#include "GLFWWindow.h"
#endif

#include "Log.h"

TScope<IWindow> IWindow::Create(const FWindowConfig &Cfg) {
#if defined(SK_WINDOWS) or defined(SK_LINUX)
    return TCreateScope<FGLFWWindow>(Cfg);
#else
    SK_LOG_CRITICAL("Window Platform Not Supported");
    return NULL;
#endif
}
