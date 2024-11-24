#pragma once

#include "FVulkanRHIDevice.h"
#include "IRHIContext.h"

class FVulkanRHIContext : virtual public IRHIContext {
public:
  virtual ~FVulkanRHIContext() = default;

  virtual VkCommandBuffer GetVkCommandBuffer() const = 0;
};

class FVulkanRHIGraphicsContext final : public IRHIGraphicsContext, public FVulkanRHIContext {
public:
  FVulkanRHIGraphicsContext(TRef<FVulkanRHIDevice> Device);
  ~FVulkanRHIGraphicsContext();

  void Begin() override;
  void End() override;

  void SetPipeline(TRef<IRHIPipeline> Pipeline) override;
  void SetIndexBuffer(TRef<IRHIBuffer> Buffer) override;
  void SetVertexBuffers(std::vector<TRef<IRHIBuffer>> Buffers) override;
  void SetStreamOutputTargets(std::vector<TRef<IRHITexture>> Textures) override;
  void SetRenderTargets(std::vector<TRef<IRHITexture>> Textures, const FRHIRect &RenderArea) override;
  void UnsetRenderTargets() override;
  void SetDescriptorSet(TRef<IRHIDescritporSet> Set) override;
  void SetViewports(const std::vector<FRHIViewport> &Viewports) override;
  void SetScissors(const std::vector<FRHIRect> &Scissors) override;
  void SetBlendConstants(std::array<FFloat, 4> Constants) override;
  void SetDepthStencilReferenceValue(FUInt32 Val) override;
  void SetTopology(ERHITopology Topology) override;

  void ResourceBarrier(const FRHIResourceBarrierDescription &Description) override;

  void DrawIndexed(FUInt32 FirstIndex, FUInt32 IndexCount, FUInt32 FirstInstance, FUInt32 InstanceCount, FSInt32 VertexOffset) override;
  void DrawInstanced(FUInt32 FirstVertex, FUInt32 VertexCount, FUInt32 FirstInstance, FUInt32 InstanceCount) override;

  inline ERHIContextType GetType() const override { return ERHIContextType::GRAPHICS; }

public:
  inline VkCommandBuffer GetVkCommandBuffer() const override { return mCommandBuffers[mDevice->GetCurrentFrameIndex()]; }

private:
  TRef<FVulkanRHIDevice> mDevice;

private:
  VkCommandPool mCommandPool = VK_NULL_HANDLE;
  std::vector<VkCommandBuffer> mCommandBuffers = {};
  FUInt64 mVerticesToDraw = 0;
};

class FVulkanRHIComputeContext final : public IRHIComputeContext, public FVulkanRHIContext {
public:
  FVulkanRHIComputeContext(TRef<IRHIDevice> Device);
  ~FVulkanRHIComputeContext();
};

class FVulkanRHIUploadContext final : public IRHIUploadContext, public FVulkanRHIContext {
public:
  FVulkanRHIUploadContext(TRef<IRHIDevice> Device);
  ~FVulkanRHIUploadContext();
};
