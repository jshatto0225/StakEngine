#include "FImguiLayer.h"

#include <imgui.h>

#include "FLog.h"

FImGuiLayer::FImGuiLayer(TRef<IRHIDevice> Device, TRef<IWindow> Window) {
  mWindow = Window;
  mDevice = Device;

  mGraphicsContext = IRHIGraphicsContext::Create(Device);

  FRHIRenderPassAttachmentDescription ColorAttachment = {};
  // TODO: Need to add ABGR and BGRA formats :(
  ColorAttachment.Format = Device->GetSwapchainImageFormat();
  ColorAttachment.Samples = ERHISampleCount::ONE;
  ColorAttachment.LoadOp = ERHILoadOp::CLEAR;
  ColorAttachment.StoreOp = ERHIStoreOp::STORE;
  ColorAttachment.StencilLoadOp = ERHILoadOp::DISCARD;
  ColorAttachment.StencilStoreOp = ERHIStoreOp::DISCARD;
  ColorAttachment.InitialState = ERHIImageState::UNDEFINED;
  ColorAttachment.FinalState = ERHIImageState::PRESENT;

  FRHIAttachmentReference ColorAttachmentReference = {};
  ColorAttachmentReference.Attachment = ColorAttachment;
  ColorAttachmentReference.State = ERHIImageState::COLOR_ATTACHMENT;

  FRHISubpassDescription Subpass = {};
  Subpass.PipelineBindPoint = ERHIPipelineBindPoint::GRAPHICS;
  Subpass.ColorAttachmentIndices = { 0 };

  FRHISubpassDependency Dependency = {};
  Dependency.SourceSubpass = EXTERNAL_SUBPASS;
  Dependency.DestinationSubpass = 0;
  Dependency.SourceStage = ERHIPipelineStage::FRAMEBUFFER_WRITE;
  Dependency.SourceAccess = ERHIAccess::DONT_CARE;
  Dependency.DestinationStage = ERHIPipelineStage::FRAMEBUFFER_WRITE;
  Dependency.DestinationAccess = ERHIAccess::COLOR_ATTACHMENT_WRITE;

  FRHIRenderPassDescription RenderPassDescription = {};
  RenderPassDescription.Attachments = { ColorAttachmentReference };
  RenderPassDescription.Dependencies = { Dependency };
  RenderPassDescription.Subpasses = { Subpass };

  mRenderPass = IRHIRenderPass::Create(mDevice, RenderPassDescription);

  mSwapchainFramebuffers.resize(mDevice->GetSwapchainImageCount());

  for (FUInt32 SwapchainTextureIndex = 0; SwapchainTextureIndex < mDevice->GetSwapchainImageCount(); SwapchainTextureIndex++) {
    FRHITextureDescription TextureDescription = {};
    TextureDescription.IsSwapchainImage = true;
    TextureDescription.SwapchainTextureIndex = SwapchainTextureIndex;
    // Does not matter
    TextureDescription.Width = 0;
    TextureDescription.Height = 0;
    TextureDescription.Layers = 0;

    std::vector<TRef<IRHITexture>> Textures = { IRHITexture::Create(mDevice, TextureDescription) };

    mSwapchainFramebuffers[SwapchainTextureIndex] = IRHIFramebuffer::Create(mDevice, Textures, mRenderPass);
  }

  FRHISyncObjectDescription SemaphoreDescription = {};
  SemaphoreDescription.Type = ERHISyncronizationType::GPU_ONLY;
  mImageAvailableSemaphore = IRHISyncObject::Create(mDevice, SemaphoreDescription);

  ImGui::CreateContext();
  ImGuiIO &io = ImGui::GetIO();
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
  ImGui::StyleColorsDark();

  mDevice->InitImGui(mWindow, mRenderPass);
}

FImGuiLayer::~FImGuiLayer() {
  mDevice->ShutdownImGui();
  mWindow->ShutdownImGui();
  ImGui::DestroyContext();
}

void FImGuiLayer::BeginFrame() {
  mDevice->ImGuiNewFrame();
  mWindow->ImGuiNewFrame();
  ImGui::NewFrame();
}

void FImGuiLayer::EndFrame() {

  ImGui::Render();
  ImDrawData *DrawData = ImGui::GetDrawData();
  const FBool IsMinimized = (DrawData->DisplaySize.x <= 0.0f || DrawData->DisplaySize.y <= 0.0f);
  if (!IsMinimized) {
    FRHIRenderArea RenderArea = { 0, 0, DrawData->DisplaySize.x, DrawData->DisplaySize.y };
    FUInt32 CurrentImage = mDevice->GetNextSwapchainImageIndex();
    mGraphicsContext->Begin();
    {
      mGraphicsContext->SetRenderPass(mRenderPass, mSwapchainFramebuffers[CurrentImage], RenderArea);
      mGraphicsContext->ImGuiRenderDrawData(DrawData);
    }
    mGraphicsContext->End();
  }

  mCurrentFrame = (mCurrentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}

void FImGuiLayer::OnWindowResize(const FWindowResizeEvent &Event) {

}
