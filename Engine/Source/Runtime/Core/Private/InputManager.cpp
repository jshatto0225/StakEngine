#include "InputManager.h"

#ifdef WIN32
#include "Win32InputManager.h"
#endif

Unique<InputManager> makeInputManager()
{
    #ifdef WIN32
    return MakeUnique<Win32InputManager>();
    #endif
}