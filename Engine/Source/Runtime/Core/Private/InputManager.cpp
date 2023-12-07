#include "InputManager.h"

#ifdef WIN32
#include "Win32InputManager.h"
#endif

Unique<InputManager> MakeInputManager()
{
    #ifdef WIN32
    return MakeUnique<Win32InputManager>();
    #endif
}