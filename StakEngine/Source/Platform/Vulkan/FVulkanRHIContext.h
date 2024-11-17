#pragma once

#include "IRHIContext.h"
#include "FVulkanRHIDevice.h"

class FVulkanRHIGraphicsContext final : public IRHIGraphicsContext {
public:
  FVulkanRHIGraphicsContext(TRef<FVulkanRHIDevice> Device, FUInt32 MaxFramesInFlight);
  ~FVulkanRHIGraphicsContext();

  void Begin();
  void End();

  void SetPipeline(TRef<IRHIPipeline> Pipeline);
  void SetIndexBuffer(TRef<IRHIBuffer> Buffer);
  void SetVertexBuffers(std::vector<TRef<IRHIBuffer>> Buffers);
  void SetStreamOutputTargets(std::vector<TRef<IRHITexture>> Textures);
  void SetRenderTargets(std::vector<TRef<IRHITexture>> Textures, const FRHIRect &RenderArea);
  void SetDescriptorSet(TRef<IRHIDescritporSet> Set);
  void SetViewports(const std::vector<FRHIRect> &Viewports);
  void SetScissors(const std::vector<FRHIRect> &Scissors);
  void SetBlendConstants(std::array<FFloat, 4> Constants);
  void SetDepthStencilReferenceValue(FUInt32 Val);
  void SetTopology(ERHITopology Topology);

  void ResourceBarrier(const FRHIResourceBarrierDescription &Description);

  void Draw();

private:
  TRef<FVulkanRHIDevice> mDevice;

private:
  VkCommandPool mCommandPool;
  std::vector<VkCommandBuffer> mCommandBuffers;
  FBool mIsRendering;
};

class FVulkanRHIComputeContext final : public IRHIComputeContext {
public:
  FVulkanRHIComputeContext(TRef<IRHIDevice> Device);
  ~FVulkanRHIComputeContext();
};

class FVulkanRHIUploadContext final : public IRHIUploadContext {
public:
  FVulkanRHIUploadContext(TRef<IRHIDevice> Device);
  ~FVulkanRHIUploadContext();
};
