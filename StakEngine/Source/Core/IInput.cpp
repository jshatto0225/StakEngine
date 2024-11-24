#include "IInput.h"

#if defined(SK_WINDOWS) or defined(SK_LINUX)
#include "FGLFWInput.h"
#endif

#include "FLog.h"


TRef<const IInput> IInput::Create(TRef<IWindow> Window) {
#if defined(SK_WINDOWS) or defined(SK_LINUX)
  return TCreateScope<FGLFWInput>(Window);
#else
  SK_LOG_CRITICAL("Invalid Input Platform");
  return NULL;
#endif
}
