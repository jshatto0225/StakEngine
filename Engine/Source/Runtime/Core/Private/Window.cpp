#include "Window.h"

#include "Event.h"
#include "Application.h"
#include "Log.h"

#ifdef WIN32
#include "Win32Window.h"
#endif

Unique<Window> MakeWindow(const std::string& name, i32 x, i32 y, i32 width, i32 height)
{
    #ifdef WIN32
    return MakeUnique<Win32Window>(name, x, y, width, height);
    #endif

    SK_CORE_CRITICAL("Invalid Window Platform, Returning NULL");
    return nullptr;
}

u64 Window::sCurrentId = 0;

Window::Window()
{
    if (sCurrentId > UINT_MAX)
    {
        SK_CORE_INFO("Warning: Used significant number of unique windows. Id Overflow is not checked.");
    }
    mId = ++sCurrentId;
}

void Window::GenerateEvent(EventType e)
{
    switch (e)
    {
    case WINDOW_CLOSE:
    {
        WindowCloseEvent event(GetId());
        Application::OnEvent(event);
    }
    case WINDOW_RECT_CHANGED:
    {
        WindowRectChangedEvent event(GetId(), mWidth, mHeight, mX, mY);
        Application::OnEvent(event);
    }
    }
}
