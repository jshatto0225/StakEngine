#include "Application.h"

#include "Log.h"
#include "Asserts.h"
#include "RHI.h"
#include "Platform.h"

void FEngine::Shutdown() {
    App->Shutdown(this, App);

    ImGuiRenderer.Shutdown();

    Window->Shutdown();

    Renderer.Shutdown();

    RHIShutdown();
}

bool FEngine::Init(FEngineSpecification *Spec, FApplication *AppImpl) {
    App = AppImpl;

    Name = Spec->AppName;

    if (!RHIInit()) {
        SK_LOG_ERROR("Failed to initialize RHI");
        return false;
    }

    FWindowConfig Cfg = {
        Spec->WindowWidth,
        Spec->WindowHeight,
        Spec->WindowTitle
    };

    Window = IWindow::Create();
    if (!Window->Init(Cfg)) {
        SK_LOG_ERROR("Failed to create window");
        return false;
    }

    Window->Data.EventFn = ([this](FEvent *Event) {
        this->OnEvent(Event);
    });

    Input = TCreateRef<FInput>(Window);

    if (!Renderer.Init(Window, Spec->RenderToOffscreenBuffer)) {
        SK_LOG_ERROR("Failed to initialize renderer");
        return false;
    }

    ImGuiRenderer.Init(&Renderer);

    Running = true;

    if (App) App->Init(this, App);

    return true;
}

void FEngine::Run() {
    while (Running) {
        PlatformPollEvents();

        if (App->Update) App->Update(this, App);

        ImGuiRenderer.BeginFrame();
        if (App->OnImGuiRender) App->OnImGuiRender(this, App);
        ImGuiRenderer.EndFrame();

        if (!Renderer.Render()) {
            SK_LOG_ERROR("Failed to render");
        }
    }
}

void FEngine::OnEvent(FEvent *Event) {
    switch (Event->Type) {
    case EEventType::WINDOW_CLOSE:
        Running = false;
        break;
    default:
        break;
    }

    if (App->OnEvent) App->OnEvent(this, Event);
}

void FEngine::Close() {
    Running = false;
}