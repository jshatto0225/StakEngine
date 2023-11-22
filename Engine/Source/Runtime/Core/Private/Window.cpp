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
        SK_CORE_INFO("Warning: Used significant number of uniqe windows. Id Overflow is not checked.");
    }
    mId = ++sCurrentId;
}

void Window::GenerateEvent(EventType e)
{
    Event event;
    event.type = e;
    switch (e)
    {
    case WINDOW_CLOSE:
    {
        event.data = GetId();
        event.size = sizeof(u64);
    }
    case WINDOW_RECT_CHANGED:
    {
        // TODO: Maybe pass new dims and pos
        event.data = GetId();
        event.size = sizeof(u64);
    }
    }

    Application::OnEvent(event);
}
