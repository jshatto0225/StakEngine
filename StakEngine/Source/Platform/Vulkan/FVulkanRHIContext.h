#pragma once

#include "IRHIContext.h"
#include "FVulkanRHIDevice.h"

class FVulkanRHIGraphicsContext final : public IRHIGraphicsContext {
public:
  FVulkanRHIGraphicsContext(TRef<FVulkanRHIDevice> Device);
  ~FVulkanRHIGraphicsContext();

  void Begin();
  void End();

  void SetRenderPass(TRef<IRHIRenderPass> RenderPass, FRHIRenderArea RenderArea);
  void SetPipeline(TRef<IRHIPipeline> Pipeline);
  void SetVertexBuffer(TRef<IRHIBuffer> Buffer);
  void SetIndexBuffer(TRef<IRHIBuffer> Buffer);
  void SetViewport(FSInt32 X, FSInt32 Y, FSInt32 Width, FSInt32 Height);
  void Draw();

private:
  TRef<FVulkanRHIDevice> mDevice;

private:
  VkCommandPool mCommandPool;
  VkCommandBuffer mCommandBuffer;
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
