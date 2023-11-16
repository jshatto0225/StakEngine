#include "Window.h"

#include "Win32Window.h"
#include "Event.h"
#include "Application.h"

std::unique_ptr<Window> MakeWindow(const std::string& name, i32 x, i32 y, i32 width, i32 height)
{
#ifdef WIN32
    return std::make_unique<Win32Window>(name, x, y, width, height);
#endif
}

u64 Window::CurrentId = 0;

Window::Window()
{
    if (CurrentId > UINT_MAX)
    {
        printf("Warning: Used significant number of uniqe windows. Id Overflow is not checked.");
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
        // TODO: Maby pass new dims and pos
        event.data = GetId();
        event.size = sizeof(u64);
    }
    }

    Application::HandleEvent(event);
}