#include "IWindow.h"

#if defined(SK_WINDOWS) or defined(SK_LINUX)
#include "FGLFWWindow.h"
#endif

#include "FLog.h"

TScope<IWindow> IWindow::Create(const FWindowConfig &Cfg) {
#if defined(SK_WINDOWS) or defined(SK_LINUX)
  return TCreateScope<FGLFWWindow>(Cfg);
#else
  SK_LOG_CRITICAL("Window Platform Not Supported")
  return NULL;
#endif
}
