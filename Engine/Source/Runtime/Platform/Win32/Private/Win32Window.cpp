#include "Win32Window.h"
#include "Application.h"
#include "Event.h"

LRESULT CALLBACK Win32WindowCallback(HWND handle, u32 msg, WPARAM wParam, LPARAM lParam)
{
    Win32Window* currentWindow = (Win32Window*)GetWindowLongPtrA(handle, GWLP_USERDATA);
    if (currentWindow)
    {
        switch (msg)
        {
        case WM_CLOSE:
        {
            currentWindow->Close();
            return 0;
        } break;
        case WM_DESTROY:
        {
            currentWindow->GenerateEvent(WINDOW_CLOSE);
            return 0;
        } break;
        case WM_SIZE:
        {
            RECT rect = {};
            GetWindowRect((HWND)currentWindow->GetHandle(), &rect);
            currentWindow->SetSizeAndPos(rect.left,
                                         rect.top,
                                         rect.right - rect.left,
                                         rect.bottom - rect.top);
            return 0;
        } break;
        case WM_MOVE:
        {
            RECT rect = {};
            GetWindowRect((HWND)currentWindow->GetHandle(), &rect);
            currentWindow->SetSizeAndPos(rect.left,
                                         rect.top,
                                         rect.right - rect.left,
                                         rect.bottom - rect.top);
            return 0;
        } break;
        }
    }

    return DefWindowProc(handle, msg, wParam, lParam);
}

Win32Window::Win32Window(const std::string& name, i32 x, i32 y, i32 width, i32 height)
{
    this->name = name;
    this->x = x;
    this->y = y;
    this->width = width;
    this->height = height;

    WNDCLASSEXA windowClass = {};
    windowClass.cbSize = sizeof(WNDCLASSEXA);
    windowClass.lpszClassName = this->name.c_str();
    windowClass.hInstance = this->instance;
    windowClass.hIcon = LoadIcon(nullptr, IDI_WINLOGO);
    windowClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
    windowClass.lpfnWndProc = Win32WindowCallback;
    windowClass.cbClsExtra = sizeof(Window*);

    RegisterClassExA(&windowClass);

    this->handle= CreateWindowExA(0,            // dwExStyle
        this->name.c_str(),                     // lpClassName
        this->name.c_str(),                     // lpWindowName
        WS_OVERLAPPEDWINDOW | WS_VISIBLE,       // dwStyle
        this->x,                                // X
        this->y,                                // Y
        this->width,                            // nWidth
        this->height,                           // nHeight
        nullptr,                                // hWndParent
        nullptr,                                // hMenu
        this->instance,                         // hInstance
        nullptr);                               // lpParam
    SetWindowLongPtrA(this->handle, GWLP_USERDATA, (LONG_PTR)this);
}

Win32Window::~Win32Window()
{
    Close();
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
    this->x = x;
    this->y = y;
    this->width = width;
    this->height = height;
    SetWindowPos(handle, 0, x, y, width, height, 0);
    GenerateEvent(WINDOW_RECT_CHANGED);
}

void Win32Window::Close()
{
    DestroyWindow(handle);
    UnregisterClassA(name.c_str(), instance);
}
