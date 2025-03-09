#include "Application.h"

#include "Log.h"
#include "ImGuiLayer.h"

#include "Asserts.h"
#include "RHI.h"
#include "Platform.h"

FApplication *GApplication = nullptr;

void FApplication::AddLayer(IApplicationLayer *Layer) {
    LayerStack.Push(Layer);
}

void FApplication::Shutdown() {
    Renderer.ShutdownImGui();
    LayerStack.Clear();

    Renderer.Shutdown();

    Window->Shutdown();

    RHIShutdown();

    if (GApplication == this) {
        GApplication = nullptr;
    }
}

bool FApplication::Init(const FApplicationSpec &Spec) {
    if (!GApplication) {
        GApplication = this;
    }

    Name = Spec.AppName;

    if (!RHIInit()) {
        SK_LOG_ERROR("Failed to initialize RHI");
        return false;
    }

    FWindowConfig Cfg = {
        Spec.WindowWidth,
        Spec.WindowHeight,
        Spec.WindowTitle
    };

    Window = IWindow::Create();
    if (!Window->Init(Cfg)) {
        SK_LOG_ERROR("Failed to create window");
        return false;
    }

    Window->SetResizeEventFn([this](const FWindowResizeEvent &Event) {
        this->OnWindowResize(Event);
    });

    Window->SetCloseEventFn([this]() {
        this->OnWindowClose();
    });

    Window->SetKeyEventFn([this](const FKeyEvent &Event) {
        this->OnKeyEvent(Event);
    });

    Window->SetMouseButtonEventFn([this](const FMouseButtonEvent &Event) {
        this->OnMouseButtonEvent(Event);
    });

    Window->SetMouseMoveEventFn([this](const FMouseMoveEvent &Event) {
        this->OnMouseMoveEvent(Event);
    });

    Input = TCreateRef<FInput>(Window);

    if (!Renderer.Init(Window)) {
        SK_LOG_ERROR("Failed to initialize renderer");
        return false;
    }

    ImGuiLayer = new FImGuiLayer();

    Renderer.InitImGui();
    Renderer.SetPostProxy(ImGuiLayer);

    AddLayer(ImGuiLayer);

    Running = true;

    return true;
}

void FApplication::Run() {
    while (Running) {
        PlatformPollEvents();

        for (IApplicationLayer *Layer : LayerStack) {
            Layer->Update();
        }

        Renderer.ImGuiNewFrame();
        ImGuiLayer->BeginFrame();
        {
            for (IApplicationLayer *Layer : LayerStack) {
                Layer->OnImGuiRender();
            }
        }
        ImGuiLayer->EndFrame();

        Renderer.Render();
    }
}

void FApplication::OnWindowResize(const FWindowResizeEvent &Event) {
    for (IApplicationLayer *Layer : LayerStack) {
        Layer->OnWindowResize(Event);
    }
}

void FApplication::OnWindowClose() {
    Close();
}

void FApplication::OnKeyEvent(const FKeyEvent &Event) {
    Input->SetKey(Event.Key, Event.State);
}

void FApplication::OnMouseButtonEvent(const FMouseButtonEvent &Event) {
    Input->SetMouseButton(Event.Button, Event.State);
}

void FApplication::OnMouseMoveEvent(const FMouseMoveEvent &Event) {
    Input->SetMousePos(Event.X, Event.Y);
}

void FApplication::Close() {
    Running = false;
}

void AppRun() {
    GApplication->Run();
}

void AppAddLayer(IApplicationLayer *layer) {
    GApplication->AddLayer(layer);
}

void AppClose() {
    GApplication->Close();
}

const char *AppGetName() {
    return GApplication->GetName();
}

const TRef<IWindow> AppGetWindow() {
    return GApplication->GetWindow();
}

FRenderer &AppGetRenderer() {
    return GApplication->GetRenderer();
}

const TRef<FInput> AppGetInput() {
    return GApplication->GetInput();
}
