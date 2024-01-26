#include "Win32Window.h"

#include "Application.h"
#include "Event.h"
#include "Log.h"

// The window is handeling its own lifetime which conflicts with the window manager haneling of its lifetime
LRESULT CALLBACK Win32WindowCallback(HWND windowHandle, u32 msg, WPARAM wParam, LPARAM lParam)
{
    Win32Window* win32Window = (Win32Window*)GetWindowLongPtrA(windowHandle, GWLP_USERDATA);
    if (win32Window)
    {
        switch(msg)
        {
        case WM_CLOSE:
        {
            win32Window->GenerateEvent(WINDOW_CLOSE);
            return 0;
        } break;
        case WM_DESTROY:
        {
            SK_CORE_TRACE("WM_DESTROY message recieved");
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
    if(mOpen) {
        Close();
    }
}

void Win32Window::Update()
{
    if(mOpen)
    {
        MSG msg = {};
        while(PeekMessageA(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessageA(&msg);
        }
    }
}

void Win32Window::SetSizeAndPos(i32 x, i32 y, i32 width, i32 height)
{
    if(mOpen)
    {
        mX = x;
        mY = y;
        mWidth = width;
        mHeight = height;
        SetWindowPos(mWindowHandle, 0, x, y, width, height, 0);
        GenerateEvent(WINDOW_RECT_CHANGED);
    }
}

void Win32Window::SetX(i32 x)
{
    SetSizeAndPos(x, mY, mWidth, mHeight);
}

void Win32Window::SetY(i32 y)
{
    SetSizeAndPos(mX, y, mWidth, mHeight);
}

void Win32Window::SetWidth(i32 width)
{
    SetSizeAndPos(mX, mY, width, mHeight);
}

void Win32Window::SetHeight(i32 height)
{
    SetSizeAndPos(mX, mY, mWidth, height);
}

i32 Win32Window::GetWidth()
{
    return mWidth;
}

i32 Win32Window::GetHeight()
{
    return mHeight;
}

i32 Win32Window::GetX()
{
    return mX;
}

i32 Win32Window::GetY()
{
    return mY;
}

f32 Win32Window::GetAspect()
{
    return (f32)mWidth / (f32)mHeight;
}

// Wierd bug sometimes fails here sometimes in WindowManager::Update();
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

void* Win32Window::GetHandle()
{
    return (void*)mWindowHandle;
}

bool Win32Window::IsOpen()
{
    return mOpen;
}
