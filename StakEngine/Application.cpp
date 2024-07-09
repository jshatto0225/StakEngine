#include "Application.h"

#include "Renderer.h"
#include "Window.h"
#include "Asserts.h"

namespace Application
{

/*********************
 * Private Interface *
 *********************/

struct application
{
    spec Spec;
    window *Window;
    bool Running;
    layer_stack LayerStack;
};

static application App;

/********************
 * Public Interface *
 ********************/

void
AddLayerToStack(layer_init Init, layer_shutdown Shutdown, layer_update Update, layer_on_event OnEvent)
{
    ASSERT(App.LayerStack.Size <= MAX_LAYERS);

    App.LayerStack.Layers[App.LayerStack.Size].Init = Init;
    App.LayerStack.Layers[App.LayerStack.Size].Shutdown = Shutdown;
    App.LayerStack.Layers[App.LayerStack.Size].Update = Update;
    App.LayerStack.Layers[App.LayerStack.Size].OnEvent = OnEvent;
    App.LayerStack.Size++;
}

void
Init(const spec *Spec)
{
    ASSERT(!App.Running);

    window_config Cfg =
    {
        Spec->WindowX,
        Spec->WindowY,
        Spec->WindowWidth,
        Spec->WindowHeight,
        Spec->WindowTitle
    };
    App.Window = CreateWindow(&Cfg);
    SetWindowEventFn(App.Window, OnEvent);

    Renderer::Init(App.Window);

    App.Running = true;

    for (u32 i = 0; i < App.LayerStack.Size; i++)
    {
        App.LayerStack.Layers[i].Init();
    }
}

void
Run()
{
    while (App.Running)
    {
        for (u32 i = 0; i < App.LayerStack.Size; i++)
        {
            App.LayerStack.Layers[i].Update();
        }

        UpdateWindow(App.Window);

        Renderer::DrawFrame();
    }

    Renderer::WaitForDevice();
}

void
OnEvent(const event *Event)
{
    if (App.Running)
    {
        for (u32 i = 0; i < App.LayerStack.Size; i++)
        {
            App.LayerStack.Layers[i].OnEvent(Event);
        }

        switch (Event->Type)
        {
        case WINDOW_CLOSE:
            App.Running = false;
            break;

        case WINDOW_RESIZED:
            Renderer::SetViewport(0, 0, Event->WinResizeEvent.Width, Event->WinResizeEvent.Height);
            break;

        default:
            break;
        }
    }
}

void
Shutdown()
{
    if (!App.Running)
    {
        return;
    }

    App.Running = false;

    for (u32 i = 0; i < App.LayerStack.Size; i++)
    {
        App.LayerStack.Layers[i].Shutdown();
    }

    Renderer::Shutdown();

    DestroyWindow(&App.Window);
}

void
RequestShutdown()
{
    SendWindowCloseRequest(App.Window);
}

} // namespace Application
