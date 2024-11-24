#include "FRenderer.h"

#include "IRHIInstance.h"
#include "IRHIDevice.h"
#include "IRHIContext.h"
#include "IRHIResource.h"
#include "IWindow.h"

FRenderer::FRenderer(TRef<IWindow> Window) {
  mWindow = Window;

  mRHIInstance = IRHIInstance::Create("Default App Name");
  mRHIDevice = IRHIDevice::Create(mRHIInstance, mWindow);
  mImGuiRHIGraphicsContext = IRHIGraphicsContext::Create(mRHIDevice);
  FRHITextureDescription TextureDescription = {};
  TextureDescription.IsSwapchainImage = true;
  mImGuiRHISwapchainTexture = IRHITexture::Create(mRHIDevice, TextureDescription);
}

FRenderer::~FRenderer() {

}

void FRenderer::Render() {
  TRef<IRHIWorkRecipt> Recipt = mRHIDevice->SubmitWork(mImGuiRHIGraphicsContext);
  mRHIDevice->WaitOnWork(Recipt);
  mRHIDevice->Present();
}

void FRenderer::InitImGui() {
  mWindow->InitImGui();
  mRHIInstance->InitImGui(mRHIDevice);
}

void FRenderer::ImGuiNewFrame() {
  mWindow->ImGuiNewFrame();
  mRHIInstance->ImGuiNewFrame();
}

void FRenderer::SubmitImGuiDrawData(ImDrawData *DrawData) {
  mImGuiRHIGraphicsContext->Begin();
  {
    FRHIResourceBarrierDescription RenderTargetTransition = {};
    RenderTargetTransition.Transitions.emplace_back(mImGuiRHISwapchainTexture, ERHIResourceUsage::RESOURCE_STATE_RENDER_TARGET);
    mImGuiRHIGraphicsContext->ResourceBarrier(RenderTargetTransition);

    mImGuiRHIGraphicsContext->SetRenderTargets(
      { 
        mImGuiRHISwapchainTexture
      },
      { 
        0,
        0,
        mImGuiRHISwapchainTexture->GetWidth(),
        mImGuiRHISwapchainTexture->GetHeight()
      }
    );
    {
      mRHIInstance->SubmitImGuiDrawData(DrawData, mImGuiRHIGraphicsContext);
    }
    mImGuiRHIGraphicsContext->UnsetRenderTargets();

    FRHIResourceBarrierDescription PresentTransition = {};
    PresentTransition.Transitions.emplace_back(mImGuiRHISwapchainTexture , ERHIResourceUsage::RESOURCE_STATE_PRESENT);
    mImGuiRHIGraphicsContext->ResourceBarrier(PresentTransition);
  }
  mImGuiRHIGraphicsContext->End();
}

void FRenderer::ShutdownImGui() {
  mRHIInstance->ShutdownImGui(mRHIDevice);
  mWindow->ShutdownImGui();
}
