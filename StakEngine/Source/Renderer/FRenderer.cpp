#include "FRenderer.h"

//#include "IRHIInstance.h"
//#include "IRHIDevice.h"
//#include "IRHIContext.h"
//#include "IRHIResource.h"
#include "IWindow.h"
#include "RHI.h"
#include "FApplication.h"

FRenderer::FRenderer() {
  ASSERT(!sInstance);
  sInstance = this;
}

FRenderer::~FRenderer() {

}

void FRenderer::Render() {
  /*
  IRHIGraphicsContext &GraphicsContext = FRHI::Get().GetGraphicsContext();

  TRef<IRHITexture> SwapchainTexture = FRHI::Get().GetSwapchainTexture();

  GraphicsContext.Begin();
  {
    FRHIResourceBarrierDescription RenderTargetTransition = {};
    RenderTargetTransition.Transitions.emplace_back(SwapchainTexture, ERHIResourceUsage::RESOURCE_STATE_RENDER_TARGET);
    GraphicsContext.ResourceBarrier(RenderTargetTransition);

    GraphicsContext.SetRenderTargets({ SwapchainTexture }, { 0, 0, SwapchainTexture->GetWidth(), SwapchainTexture->GetHeight() });
    {
      for (FRenderProxy *Proxy : mRenderProxies) {
        Proxy->Render(GraphicsContext);
      }
      for (FRenderProxy *Proxy : mPostRenderProxies) {
        Proxy->Render(GraphicsContext);
      }
    }
    GraphicsContext.UnsetRenderTargets();

    FRHIResourceBarrierDescription PresentTransition = {};
    PresentTransition.Transitions.emplace_back(SwapchainTexture, ERHIResourceUsage::RESOURCE_STATE_PRESENT);
    GraphicsContext.ResourceBarrier(PresentTransition);
  }
  GraphicsContext.End();

  IRHIWorkRecipt *Recipt = FRHI::Get().SubmitWork(GraphicsContext);
  FRHI::Get().WaitOnWork(Recipt);
  FRHI::Get().Present();
  */

  mCommandList.BeginDrawing();
  {
    for (FRenderProxy *Proxy : mRenderProxies) {
      Proxy->Render(mCommandList);
    }
    for (FRenderProxy *Proxy : mPostRenderProxies) {
      Proxy->Render(mCommandList);
    }
  }
  mCommandList.EndDrawing();

  FRHI::Get().Submit(mCommandList);
}

void FRenderer::InitImGui() {
  FApplication::Get().GetWindow()->InitImGui();
  FRHI::Get().InitImGui();
}

void FRenderer::ImGuiNewFrame() {
  FApplication::Get().GetWindow()->ImGuiNewFrame();
  FRHI::Get().ImGuiNewFrame();
}

void FRenderer::ShutdownImGui() {
  FRHI::Get().ShutdownImGui();
  FApplication::Get().GetWindow()->ShutdownImGui();
}

void FRenderer::AddProxy(FRenderProxy *Proxy) {
  mRenderProxies.push_back(Proxy);
}

void FRenderer::RemoveProxy(FRenderProxy *Proxy) {
  auto It = std::find(mRenderProxies.begin(), mRenderProxies.end(), Proxy);
  if (It != mRenderProxies.end()) {
    mRenderProxies.erase(It);
  }
}

void FRenderer::AddPostProxy(FRenderProxy *Proxy) {
  mPostRenderProxies.push_back(Proxy);
}

void FRenderer::RemovePostProxy(FRenderProxy *Proxy) {
  auto It = std::find(mPostRenderProxies.begin(), mPostRenderProxies.end(), Proxy);
  if (It != mPostRenderProxies.end()) {
    mPostRenderProxies.erase(It);
  }
}

FRenderProxy::FRenderProxy() {
  FRenderer::Get().AddProxy(this);
}

FRenderProxy::~FRenderProxy() {
  FRenderer::Get().RemoveProxy(this);
}