#pragma once

#include "IRHIContext.h"
#include "FVulkanRHIDevice.h"

class VulkanRHIGraphicsContext final : public IRHIGraphicsContext {
public:
  VulkanRHIGraphicsContext(TRef<IRHIDevice> Device);
  ~VulkanRHIGraphicsContext();

  void Begin();
  void End();

  void SetRenderPass(TRef<IRHIRenderPass> RenderPass);
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

class VulkanRHIComputeContext final : public IRHIComputeContext {
public:
  VulkanRHIComputeContext(TRef<IRHIDevice> Device);
  ~VulkanRHIComputeContext();
};

class VulkanRHIUploadContext final : public IRHIUploadContext {
public:
  VulkanRHIUploadContext(TRef<IRHIDevice> Device);
  ~VulkanRHIUploadContext();
};
