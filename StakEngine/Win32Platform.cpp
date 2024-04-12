#include "Platform.h"

#ifdef SK_WINDOWS

#include "Window.h"
#include "Log.h"
#include "Asserts.h"
#include "Input.h"
#include "Image.h"
#include "File.h"
#include "Win32Platform.h"

///////////////////////
// Private Interface //
///////////////////////

platform Platform;

//////////////////////
// Public Interface //
//////////////////////

void
PlatformInit()
{
    if (Platform.Initialized)
    {
        return;
    }
    
    Platform.Instance = GetModuleHandleW(NULL);
    
    WNDCLASSEXA WindowClass = {};
    WindowClass.cbSize = sizeof(WNDCLASSEXA);
    WindowClass.lpszClassName = WIN32_DEFAULT_WNDCLASS_NAME;
    WindowClass.hInstance = Platform.Instance;
    WindowClass.hIcon = LoadIconW(NULL, IDI_WINLOGO);
    WindowClass.hCursor = LoadCursorW(NULL, IDC_ARROW);
    WindowClass.lpfnWndProc = Win32MessageCallback;
    WindowClass.cbClsExtra = sizeof(window *);
    Platform.DefaultWindowClass = RegisterClassExA(&WindowClass);
    
    Platform.DummyWindow = CreateWindowExA(0,
                                           WIN32_DEFAULT_WNDCLASS_NAME,
                                           "Dummy Window",
                                           0,
                                           0,
                                           0,
                                           0,
                                           0,
                                           NULL,
                                           NULL,
                                           Platform.Instance,
                                           NULL);
    
    PlatformInitExtensions();
    
    Platform.Initialized = true;
}

void
PlatformShutdown()
{
    if (!Platform.Initialized)
    {
        return;
    }
    Platform.Initialized = false;
    UnregisterClassA(WIN32_DEFAULT_WNDCLASS_NAME, Platform.Instance);
}

bool
PlatformIsInitialized()
{
    return Platform.Initialized;
}

#endif
