#include "Window.h"

#include "Event.h"
#include "Application.h"
#include "Log.h"

u64 Window::sCurrentId = 0;

Window::Window()
{
    if (sCurrentId > UINT_MAX)
    {
        SK_CORE_WARN("Used significant number of unique windows. Id Overflow is not checked.");
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
