#include "Window.h"

#if defined(SK_WINDOWS) or defined(SK_LINUX)
#include "GLFWWindow.h"
#endif

#include "Log.h"

TRef<IWindow> IWindow::Create() {
#if defined(SK_WINDOWS) or defined(SK_LINUX)
    return TCreateRef<FGLFWWindow>();
#else
    SK_LOG_CRITICAL("Window Platform Not Supported");
    return NULL;
#endif
}
