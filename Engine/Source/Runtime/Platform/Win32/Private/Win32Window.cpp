#include "Win32Window.h"

#include "Application.h"
#include "Event.h"
#include "Log.h"

LRESULT CALLBACK Win32WindowCallback(HWND windowHandle, u32 msg, WPARAM wParam, LPARAM lParam)
{
    Win32Window* win32Window = (Win32Window*)GetWindowLongPtrA(windowHandle, GWLP_USERDATA);
    if (win32Window)
    {
        switch (msg)
        {
        case WM_CLOSE:
        {
            win32Window->Close();
            return 0;
        } break;
        case WM_DESTROY:
        {
            win32Window->GenerateEvent(WINDOW_CLOSE);
            return 0;
        } break;
        case WM_SIZE:
        {
            RECT rect = {};
            GetWindowRect((HWND)win32Window->GetHandle(), &rect);
            win32Window->SetSizeAndPos(rect.left,
                                       rect.top,
                                       rect.right - rect.left,
                                       rect.bottom - rect.top);
            return 0;
        } break;
        case WM_MOVE:
        {
            RECT rect = {};
            GetWindowRect((HWND)win32Window->GetHandle(), &rect);
            win32Window->SetSizeAndPos(rect.left,
                                       rect.top,
                                       rect.right - rect.left,
                                       rect.bottom - rect.top);
            return 0;
        } break;
        }
    }

    return DefWindowProc(windowHandle, msg, wParam, lParam);
}

Win32Window::Win32Window(const std::string& name, i32 x, i32 y, i32 width, i32 height)
{
    mName = name;
    mX = x;
    mY = y;
    mWidth = width;
    mHeight = height;
    mOpen = true;

    WNDCLASSEXA windowClass = {};
    windowClass.cbSize = sizeof(WNDCLASSEXA);
    windowClass.lpszClassName = mName.c_str();
    windowClass.hInstance = mInstanceHandle;
    windowClass.hIcon = LoadIcon(nullptr, IDI_WINLOGO);
    windowClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
    windowClass.lpfnWndProc = Win32WindowCallback;
    windowClass.cbClsExtra = sizeof(Win32Window*);

    RegisterClassExA(&windowClass);

    mWindowHandle = CreateWindowExA(0,                                      // dwExStyle
                                    mName.c_str(),                          // lpClassName
                                    mName.c_str(),                          // lpWindowName
                                    WS_OVERLAPPEDWINDOW | WS_VISIBLE,       // dwStyle
                                    mX,                                     // X
                                    mY,                                     // Y
                                    mWidth,                                 // nWidth
                                    mHeight,                                // nHeight
                                    nullptr,                                // hWndParent
                                    nullptr,                                // hMenu
                                    mInstanceHandle,                        // hInstance
                                    nullptr);                               // lpParam
    SetWindowLongPtrA(mWindowHandle, GWLP_USERDATA, (LONG_PTR)this);
}

Win32Window::~Win32Window()
{
    if (mOpen) {
        Close();
    }
}

void Win32Window::Update()
{
    MSG msg = {};
    while (PeekMessageA(&msg, nullptr, 0, 0, PM_REMOVE))
    {
        TranslateMessage(&msg);
        DispatchMessageA(&msg);
    }
}

void KillWindowManager()
{
    PostQuitMessage(0);
}

void Win32Window::SetSizeAndPos(i32 x, i32 y, i32 width, i32 height)
{
    mX = x;
    mY = y;
    mWidth = width;
    mHeight = height;
    SetWindowPos(mWindowHandle, 0, x, y, width, height, 0);
    GenerateEvent(WINDOW_RECT_CHANGED);
}

void Win32Window::Close()
{
    if(mOpen)
    {
        mOpen = false;
        SK_CORE_WARN("Window Closed");
        DestroyWindow(mWindowHandle);
        UnregisterClassA(mName.c_str(), mInstanceHandle);
    }
}
