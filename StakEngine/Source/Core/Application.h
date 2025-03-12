#pragma once

#include <vector>
#include <string>

#include "Events.h"
#include "Renderer.h"
#include "Input.h"
#include "ImGuiRenderer.h"

struct FEngineSpecification {
    const char *WindowTitle;
    const char *AppName;
    FSInt32 WindowWidth;
    FSInt32 WindowHeight;
    bool RenderToOffscreenBuffer;
};

struct FEngine;

struct FApplication {
    bool (*Init)(FEngine *, FApplication *App);
    void (*Shutdown)(FEngine *, FApplication *App);
    void (*OnImGuiRender)(FEngine *, FApplication *App);
    void (*Update)(FEngine *, FApplication *App);
    void (*OnEvent)(FEngine *, FEvent *);

    void *UserData;
};

struct FEngine {
    bool Init(FEngineSpecification *Spec, FApplication *AppImpl);
    void Shutdown();

    void Run();
    void Close();
  
    void OnEvent(FEvent *Event);

    FApplication *App;
    FWindow Window;
    FRenderer Renderer;
    FInput Input;
    FBool Running;
    FImGuiRenderer ImGuiRenderer;
    const char *Name;
};
