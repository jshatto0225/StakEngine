#pragma once

#include "Platform.h"

#ifdef SK_WINDOWS

#include <Windows.h>

typedef BOOL(WINAPI *DESTROYWINDOWPROC)(HWND);
const DESTROYWINDOWPROC Win32DestroyWindow = DestroyWindow;
#undef DestroyWindow

typedef BOOL(WINAPI *SETWINDOWPOSPROC)(HWND, HWND, int, int, int, int, UINT);
const SETWINDOWPOSPROC Win32SetWindowPos = SetWindowPos;
#undef SetWindowPos

// NOTE: Use CreateWidowExA
#undef CreateWindow
// NOTE: Use CreateFileA
#undef CreateFile

#include "Types.h"
#include "Event.h"
#include "Window.h"

struct window
{
    HWND Handle;
    i32 X;
    i32 Y;
    i32 Width;
    i32 Height;
    const char *Title;
    EventFn EventFunc;
    void *Parent;
};

LRESULT CALLBACK WindowMessageCallback(HWND, UINT, WPARAM, LPARAM);

namespace Platform
{

constexpr const char *DEFAULT_WNDCLASS_NAME = "SK_DEFAULT_WNDCLASS";

struct win32
{
    HINSTANCE Instance;
    ATOM DefaultWindowClass;
    bool Initialized;
};

extern win32 Win32;

} // namespace Platform

#endif // #ifdef SK_WIN32
