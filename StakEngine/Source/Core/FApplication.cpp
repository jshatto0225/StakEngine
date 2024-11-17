#include "FApplication.h"

#include "FLog.h"
#include "FImGuiLayer.h"

#include "IRHIInstance.h"
#include "IRHIDevice.h"

void FApplication::AddLayer(IApplicationLayer *Layer) {
  mLayerStack.Push(Layer);
}

FApplication::~FApplication() {
  mLayerStack.Clear();

  mInput = NULL;
  mImGuiLayer = NULL;
  mRenderer = NULL;
  mWindow = NULL;
}

FApplication::FApplication(const FApplicationSpec &Spec) {
  FWindowConfig Cfg = {
    Spec.WindowWidth,
    Spec.WindowHeight,
    Spec.WindowTitle
  };
  mWindow = IWindow::Create(Cfg);
  mWindow->SetResizeEventFn([this](FWindowResizeEvent &Event) { return this->OnWindowResize(Event); });
  mWindow->SetCloseEventFn([this]() { return this->OnWindowClose(); });

  mInput = IInput::Create(mWindow);

  mRHIInstance = IRHIInstance::Create("");
  mRHIDevice = IRHIDevice::Create(mRHIInstance, mWindow);

  mImGuiLayer = new FImGuiLayer(mRHIDevice, mWindow);

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
      for (IApplicationLayer *layer : mLayerStack) {
        layer->OnImGuiRender();
      }
    }
    mImGuiLayer->EndFrame();

    mWindow->Update();
  }
}

void FApplication::OnWindowResize(FWindowResizeEvent &Event) {
  for (IApplicationLayer *Layer : mLayerStack) {
    Layer->OnWindowResize(Event);
  }
}

void FApplication::OnWindowClose() {
  Close();
}

void FApplication::Close() {
  mRunning = false;
}
