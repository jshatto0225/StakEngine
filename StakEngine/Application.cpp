#include "Application.h"

#include "Log.h"
#include "Renderer.h"
#include "Window.h"

/*********************
 * Private Interface *
 *********************/

struct application
{
    application_spec Spec;
    window *Window;
    bool Running;
    layer_stack LayerStack;
};

application App;

/********************
 * Public Interface *
 ********************/

void
AddLayerToStack(layer_init Init, layer_shutdown Shutdown, layer_update Update, layer_on_event OnEvent)
{
    if (App.LayerStack.Size == MAX_LAYERS)
    {
        LogCoreError("Max number of layers reached");
        return;
    }

    App.LayerStack.Layers[App.LayerStack.Size].Init = Init;
    App.LayerStack.Layers[App.LayerStack.Size].Shutdown = Shutdown;
    App.LayerStack.Layers[App.LayerStack.Size].Update = Update;
    App.LayerStack.Layers[App.LayerStack.Size].OnEvent = OnEvent;
    App.LayerStack.Size++;
}

void
ApplicationInit(const application_spec *Spec)
{
    window_config Cfg =
    {
        Spec->WindowX,
        Spec->WindowY,
        Spec->WindowWidth,
        Spec->WindowHeight,
        Spec->WindowTitle
    };
    App.Window = CreateWindow(&Cfg);
    SetWindowEventFn(App.Window, ApplicationOnEvent);

    RendererInit(App.Window);

    App.Running = true;
    LogCoreInfo("Application Initialized");

    for (u32 i = 0; i < App.LayerStack.Size; i++)
    {
        App.LayerStack.Layers[i].Init();
    }
}

void
ApplicationRun()
{
    while (App.Running)
    {
        for (u32 i = 0; i < App.LayerStack.Size; i++)
        {
            App.LayerStack.Layers[i].Update();
        }
        UpdateWindow(App.Window);
    }
}

void
ApplicationOnEvent(const event *Event)
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
            LogCoreTrace("Window Closed");
            App.Running = false;
            break;

        case WINDOW_RESIZED:
            RendererOnWindowResize(Event->WinResizeEvent.Width, Event->WinResizeEvent.Height);
            LogCoreTrace("Viewport: 0, 0, %d, %d",
                         Event->WinResizeEvent.Width,
                         Event->WinResizeEvent.Height);
            break;
        default:
            break;
        }
    }
}

void
ApplicationShutdown()
{
    LogCoreTrace("Application Shutdown");

    for (u32 i = 0; i < App.LayerStack.Size; i++)
    {
        App.LayerStack.Layers[i].Shutdown();
    }

    RendererShutdown();

    DestroyWindow(&App.Window);
}
