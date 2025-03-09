#pragma once

#include <vector>
#include <string>

#include "ApplicationLayer.h"
#include "Events.h"
#include "Renderer.h"
#include "Input.h"
#include "ImGuiLayer.h"

struct FApplicationSpec {
    const char *WindowTitle;
    const char* AppName;
    FSInt32 WindowWidth;
    FSInt32 WindowHeight;
};

class FApplication {
public:
    bool Init(const FApplicationSpec &Spec);
    void Shutdown();

    void Run();
    void AddLayer(IApplicationLayer *layer);
    void Close();
  
    inline const char *GetName() const { return Name; }
    inline TRef<IWindow> GetWindow() { return Window; }
    inline FRenderer &GetRenderer() { return Renderer; }
    inline TRef<FInput> GetInput() { return Input; }

private:
    void OnWindowResize(const FWindowResizeEvent &Event);
    void OnKeyEvent(const FKeyEvent &Event);
    void OnMouseButtonEvent(const FMouseButtonEvent &Event);
    void OnMouseMoveEvent(const FMouseMoveEvent &Event);

private:
    TRef<IWindow> Window = nullptr;
    FRenderer Renderer = {};
    TRef<FInput> Input = nullptr;
    FLayerStack LayerStack = {};
    FBool Running = false;
    FImGuiLayer *ImGuiLayer = nullptr;
    const char *Name;
};

void AppRun();
void AppAddLayer(IApplicationLayer* Layer);
void AppClose();

const char* AppGetName();
const TRef<IWindow> AppGetWindow();
FRenderer &AppGetRenderer();
const TRef<FInput> AppGetInput();
