#include "FApplication.h"

#include "FLog.h"
#include "FImGuiLayer.h"

#include "Asserts.h"
#include "RHI.h"

void FApplication::AddLayer(IApplicationLayer *Layer) {
  mLayerStack.Push(Layer);
}

FApplication::~FApplication() {
  mLayerStack.Clear();

  mRenderer = NULL;
  FRHI::Shutdown();
  mInput = NULL;
  mWindow = NULL;


  sInstance = NULL;
}

FApplication::FApplication(const FApplicationSpec &Spec) {
  ASSERT(!sInstance);

  sInstance = this;

  FWindowConfig Cfg = {
    Spec.WindowWidth,
    Spec.WindowHeight,
    Spec.WindowTitle
  };
  mWindow = IWindow::Create(Cfg);
  mWindow->SetResizeEventFn(
    [this](const FWindowResizeEvent &Event) { 
      this->OnWindowResize(Event); 
    }
  );
  mWindow->SetCloseEventFn(
    [this]() { 
      this->OnWindowClose(); 
    }
  );
  mWindow->SetKeyEventFn(
    [this](const FKeyEvent &Event) {
      this->OnKeyEvent(Event);
    }
  );
  mWindow->SetMouseButtonEventFn(
    [this](const FMouseButtonEvent &Event) {
      this->OnMouseButtonEvent(Event);
    }
  );
  mWindow->SetMouseMoveEventFn(
    [this](const FMouseMoveEvent &Event) {
      this->OnMouseMoveEvent(Event);
    }
  );

  mInput = TCreateRef<FInput>();

  FRHI::Init();

  mRenderer = TCreateRef<FRenderer>();

  mImGuiLayer = new FImGuiLayer();

  AddLayer(mImGuiLayer);

  mRunning = true;
}

void FApplication::Run() {
  while (mRunning) {
    for (IApplicationLayer *Layer : mLayerStack) {
      Layer->Update();
    }

    mImGuiLayer->BeginFrame();
    {
      for (IApplicationLayer *Layer : mLayerStack) {
        Layer->OnImGuiRender();
      }
    }
    mImGuiLayer->EndFrame();

    mRenderer->Render();

    mWindow->Update();
  }
}

void FApplication::OnWindowResize(const FWindowResizeEvent &Event) {
  FRHI::Get().FramebufferResized();
  for (IApplicationLayer *Layer : mLayerStack) {
    Layer->OnWindowResize(Event);
  }
}

void FApplication::OnWindowClose() {
  Close();
}

void FApplication::OnKeyEvent(const FKeyEvent &Event) {
  mInput->SetKey(Event.Key, Event.State);
}

void FApplication::OnMouseButtonEvent(const FMouseButtonEvent &Event) {
  mInput->SetMouseButton(Event.Button, Event.State);
}

void FApplication::OnMouseMoveEvent(const FMouseMoveEvent &Event) {
  mInput->SetMousePos(Event.X, Event.Y);
}

void FApplication::Close() {
  mRunning = false;
}
