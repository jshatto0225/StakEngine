#include "Win32Platform.h"

#ifdef SK_WINDOWS

#include "Window.h"

#include "Platform.h"
#include "Log.h"

///////////////////////
// Private Interface //
///////////////////////

const UINT TIMER_INTERVAL = 10;
UINT_PTR TimerId;

LRESULT CALLBACK
Win32MessageCallback(HWND Window, UINT Msg, WPARAM WParam, LPARAM LParam)
{
    window *Win = (window *)GetWindowLongPtrA(Window, GWLP_USERDATA);
    if (Win)
    {
        switch (Msg)
        {
        case WM_ENTERSIZEMOVE:
        {
            TimerId = SetTimer(Window, 1, TIMER_INTERVAL, NULL);
            return 0;
        };
        case WM_TIMER:
        {
            RedrawWindow(Window, NULL, NULL, RDW_INVALIDATE);
            return 0;
        }
        case WM_EXITSIZEMOVE:
        {
            KillTimer(Window, TimerId);
            return 0;
        }
        case WM_CLOSE:
        {
            if (Win->EventFunc)
            {
                event Event = {};
                Event.Type = WINDOW_CLOSE;
                Event.WinCloseEvent.Window = Win;
                Win->EventFunc(Win->Parent, &Event);
            }
            return 0;
        }
        case WM_SIZE:
        {
            RECT Rect = {};
            GetClientRect(Window, &Rect);

            Win->Width = Rect.right - Rect.left;
            Win->Height = Rect.bottom - Rect.top;

            if (Win->EventFunc)
            {
                event Event;
                Event.Type = WINDOW_RESIZED;
                Event.WinResizeEvent.Window = Win;
                Event.WinResizeEvent.Width = Rect.right - Rect.left;
                Event.WinResizeEvent.Height = Rect.bottom - Rect.top;
                Win->EventFunc(Win->Parent, &Event);
            }
            RedrawWindow(Window, NULL, NULL, RDW_INVALIDATE);
            return 0;
        }
        case WM_MOVE:
        {
            RECT Rect = {};
            GetClientRect(Window, &Rect);
            Win->X = Rect.left;
            Win->Y = Rect.top;

            if (Win->EventFunc)
            {
                event Event;
                Event.Type = WINDOW_MOVED;
                Event.WinMoveEvent.Window = Win;
                Event.WinMoveEvent.X = Rect.left;
                Event.WinMoveEvent.Y = Rect.top;
                Win->EventFunc(Win->Parent, &Event);
            }
            RedrawWindow(Window, NULL, NULL, RDW_INVALIDATE);
            return 0;
        }
        case WM_PAINT:
        {
            PAINTSTRUCT PaintStruct;
            HDC DeviceContext = BeginPaint(Window, &PaintStruct);
            if (Win->EventFunc)
            {
                event Event;
                Event.Type = WINDOW_PAINT;
                Event.WinPaintEvent.Window = Win;
                Win->EventFunc(Win->Parent, &Event);
            }
            EndPaint(Window, &PaintStruct);
            return 0;
        }
        }
    }
    return DefWindowProcA(Window, Msg, WParam, LParam);
}

//////////////////////
// Public Interface //
//////////////////////

u64 WindowCount = 0;

window *
CreateWindow(const window_config *Config)
{
    if (WindowCount == 0)
    {
        PlatformInit();
    }
    WindowCount++;

    window *Window = (window *)malloc(sizeof(window));

    if (!Window)
    {
        LogCoreError("Failed to allocate memory for window");
        return NULL;
    }

    Window->Parent = Config->Parent;
    Window->X = Config->X;
    Window->Y = Config->Y;
    Window->Width = Config->Width;
    Window->Height = Config->Height;
    Window->Title = Config->Title;

    Window->Handle = CreateWindowExA(0,
                                     WIN32_DEFAULT_WNDCLASS_NAME,
                                     Window->Title,
                                     WS_OVERLAPPEDWINDOW | WS_VISIBLE,
                                     Window->X,
                                     Window->Y,
                                     Window->Width,
                                     Window->Height,
                                     NULL,
                                     NULL,
                                     Platform.Instance,
                                     NULL);
    SetWindowLongPtrA(Window->Handle, GWLP_USERDATA, (LONG_PTR)Window);
    Window->Context = CreateContext(Window);

    return Window;
}

void
DestroyWindow(window **Window)
{
    if (*Window)
    {
        DestroyContext(&(*Window)->Context);

        Win32DestroyWindow((*Window)->Handle);
        WindowCount--;
        if (WindowCount == 0)
        {
            PlatformShutdown();
        }

        free(*Window);

        *Window = NULL;
    }
}

void
SetWindowPos(window *Window, i32 X, i32 Y)
{
    Win32SetWindowPos(Window->Handle, NULL, X, Y, Window->Width, Window->Height, 0);
    Window->X = X;
    Window->Y = Y;
}

void
SetWindowSize(window *Window, i32 Width, i32 Height)
{
    Win32SetWindowPos(Window->Handle, NULL, Window->X, Window->Y, Width, Height, 0);
    Window->Width = Width;
    Window->Height = Height;
}

window_pos_data
GetWindowPos(window *Window)
{
    return { Window->X, Window->Y };
}

window_size_data
GetWindowSize(window *Window)
{
    return { Window->Width, Window->Height };
}

void
MakeWindowCurrent(const window *Window)
{
    MakeContextCurrent(Window->Context);
}

void
UpdateWindow(const window *Window)
{
    RedrawWindow(Window->Handle, NULL, NULL, RDW_INVALIDATE);
    MSG Msg = {};
    while (PeekMessageA(&Msg, NULL, 0, 0, PM_REMOVE))
    {
        TranslateMessage(&Msg);
        DispatchMessageA(&Msg);
    }
}

void
SwapWindowBuffers(window *Window)
{
    SwapContextBuffers(Window->Context);
}

void
SetWindowEventFn(window *Window, EventFn Func)
{
    Window->EventFunc = Func;
}

#endif
