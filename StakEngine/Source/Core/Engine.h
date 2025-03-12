#pragma once

#include <vector>
#include <string>

#include "Events.h"
#include "Renderer.h"
#include "Input.h"
#include "ImGuiRenderer.h"

struct FEngine;

struct FApplication {
    bool (*Init)          (FEngine *, FApplication *);
    void (*Shutdown)      (FEngine *, FApplication *);
    void (*OnImGuiRender) (FEngine *, FApplication *);
    void (*Update)        (FEngine *, FApplication *);
    void (*OnEvent)       (FEngine *, FEvent *);

    void *UserData;
};

struct FEngineSpecification {
    const char *WindowTitle;
    const char *AppName;
    FSInt32 WindowWidth;
    FSInt32 WindowHeight;
    bool RenderToOffscreenBuffer;
};

struct FEngine {
    FApplication *App;
    FWindow Window;
    FRenderer Renderer;
    FInput Input;
    FBool Running;
    FImGuiRenderer ImGuiRenderer;
    const char *Name;
};

bool EngineInit(FEngine *Engine, FEngineSpecification *Spec, FApplication *AppImpl);
void EngineShutdown(FEngine *Engine);

void EngineRun(FEngine *Engine);
void EngineClose(FEngine *Engine);

void EngineOnEvent(FEngine *Engine, FEvent *Event);