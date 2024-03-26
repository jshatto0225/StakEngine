#include "Application.h"

#include "Log.h"
#include "Platform.h"
#include "Renderer.h"
#include "Window.h"

void AddLayerToStack(layer_stack *Stack,
                     void *(*Init)(),
                     void(*Shutdown)(void **),
                     void(*Update)(void *),
                     void(*OnEvent)(void *, void *, const event *)) {
  if (Stack->Size == Stack->MAX_LAYERS) {
    LogCoreError("Max number of layers reached");
    return;
  }
  
  Stack->Layers[Stack->Size].Init = Init;
  Stack->Layers[Stack->Size].Shutdown = Shutdown;
  Stack->Layers[Stack->Size].Update = Update;
  Stack->Layers[Stack->Size].OnEvent = OnEvent;
  Stack->Size++;
}

application *ApplicationInit(const application_spec *Spec) {
  application *App = (application *)malloc(sizeof(application));

  if (!App) {
    LogCoreError("Failed to allocate space for application");
    return NULL;
  }

  window_config Cfg = {
    Spec->WindowX,
    Spec->WindowY,
    Spec->WindowWidth,
    Spec->WindowHeight,
    Spec->WindowTitle,
    App
  };
  App->Window = CreateWindow(&Cfg);
  SetWindowEventFn(App->Window, ApplicationOnEvent);

  RendererInit(App->Window);

  App->Layers = Spec->Layers;

  App->Running = true;
  LogCoreInfo("Application Initialized");
  
  for (u32 i = 0; i < App->Layers.Size; i++) {
    App->Layers.LayerData[i] = App->Layers.Layers[i].Init();
  }

  return App;
}

void ApplicationRun(application *App) {
  while (App->Running) {
    for (u32 i = 0; i < App->Layers.Size; i++) {
      App->Layers.Layers[i].Update(App->Layers.LayerData[i]);
    }
    UpdateWindow(App->Window);
  }
}

void ApplicationOnEvent(void *Parent, const event *Event) {
  application *App = (application *)Parent;
  if (App->Running) {
    for (u32 i = 0; i < App->Layers.Size; i++) {
      App->Layers.Layers[i].OnEvent(Parent, App->Layers.LayerData[i], Event);
    }
    switch (Event->Type) {
    case WINDOW_CLOSE:
      LogCoreTrace("Window Closed");
      App->Running = false;
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

void ApplicationShutdown(application **App) {
  if (*App) {
    LogCoreTrace("Application Shutdown");

    if ((*App)->Layers.Layers != NULL) {
      for (u32 i = 0; i < (*App)->Layers.Size; i++) {
        (*App)->Layers.Layers[i].Shutdown(&(*App)->Layers.LayerData[i]);
      }
    }
  
    RendererShutdown();

    DestroyWindow(&(*App)->Window);
  
    free(*App);
    *App = NULL;
  }
}
