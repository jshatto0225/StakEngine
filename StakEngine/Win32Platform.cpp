#include "Platform.h"

#ifdef SK_WINDOWS

#include "Window.h"
#include "Log.h"
#include "Asserts.h"
#include "Input.h"
#include "Image.h"
#include "File.h"
#include "Win32Platform.h"

namespace Platform 
{

/*********************
 * Private Interface *
 *********************/

win32 Win32;

/********************
 * Public Interface *
 ********************/

void
Init()
{
    ASSERT(!Win32.Initialized);
    
    Win32.Instance = GetModuleHandleW(NULL);
    
    WNDCLASSEXA WindowClass = {};
    WindowClass.cbSize = sizeof(WNDCLASSEXA);
    WindowClass.lpszClassName = DEFAULT_WNDCLASS_NAME;
    WindowClass.hInstance = Win32.Instance;
    WindowClass.hIcon = LoadIconW(NULL, (LPCWSTR)IDI_WINLOGO);
    WindowClass.hCursor = LoadCursorW(NULL, (LPCWSTR)IDC_ARROW);
    WindowClass.lpfnWndProc = WindowMessageCallback;
    WindowClass.cbClsExtra = sizeof(window *);
    Win32.DefaultWindowClass = RegisterClassExA(&WindowClass);
    
    SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
    
    Win32.Initialized = true;
}

void
Shutdown()
{
    ASSERT(Win32.Initialized);

    Win32.Initialized = false;
    UnregisterClassA(DEFAULT_WNDCLASS_NAME, Win32.Instance);
}

} // namespace Win32

#endif // #ifdef SK_WIN32
