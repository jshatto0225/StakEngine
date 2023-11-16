#include "Window.h"

#include "Event.h"
#include "Application.h"
#include "Log.h"

#ifdef WIN32
#include "Win32Window.h"
#endif

Scope<Window> MakeWindow(const std::string& name, i32 x, i32 y, i32 width, i32 height)
{
#ifdef WIN32
    return MakeScope<Win32Window>(name, x, y, width, height);
#endif
}

u64 Window::CurrentId = 0;

Window::Window()
{
    if (CurrentId > UINT_MAX)
    {
        SK_CORE_INFO("Warning: Used significant number of uniqe windows. Id Overflow is not checked.");
    }
    id = ++CurrentId;
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

    Application::HandleEvent(event);
}