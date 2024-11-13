#pragma once

#include "IRHIContext.h"
#include "VulkanRHIPipeline.h"
#include "VulkanRHIBuffer.h"
#include "VulkanRHITexture.h"

namespace Stak {

class VulkanRHIGraphicsContext : public IRHIGraphicsContext {
public:
  VulkanRHIGraphicsContext(VkDevice device, u32 graphicsQueueFamily);
  ~VulkanRHIGraphicsContext();

  void begin();
  void end();
  void resourceBarrier(ResourceBarrierDescription &barrierDesc);

  void setPipeline(Ref<IRHIPipeline> pipeline);
  void setVertexBuffer(Ref<IRHIBuffer> buffer);
  void setIndexBuffer(Ref<IRHIBuffer> buffer);
  void setViewport(i32 x, i32 y, i32 width, i32 height);
  void draw();

  inline ERHIContextType getType() { return ERHIContextType::GRAPHICS; }


public:
  inline std::vector<VkFence> getFences() { return mFences; }
  inline VkCommandBuffer getCommandBuffer() { return mCommandBuffer; }

private:
  VkCommandBuffer mCommandBuffer;
  VkCommandPool mCommandPool;
  VkDevice mDevice;
  std::vector<VkFence> mFences;
};

class VulkanRHIComputeContext : public IRHIComputeContext {
public:
  VulkanRHIComputeContext();
  ~VulkanRHIComputeContext();

  void begin();
  void end();
  void resourceBarrier(ResourceBarrierDescription &barrierDesc);

  void setPipeline(Ref<IRHIPipeline> pipeline);
  void dispatch();

  inline ERHIContextType getType() { return ERHIContextType::COMPUTE; }

  inline std::vector<VkFence> getFences() { return mFences; }

private:
  std::vector<VkFence> mFences;
};

class VulkanRHIUploadContext : public IRHIUploadContext {
public:
  VulkanRHIUploadContext();
  ~VulkanRHIUploadContext();

  void begin();
  void end();
  void resourceBarrier(ResourceBarrierDescription &barrierDesc);

  void uploadBuffer(Ref<IRHIBuffer> buffer, void *data);
  void uploadTexture(Ref<IRHITexture> texture, void *data);

  inline ERHIContextType getType() { return ERHIContextType::UPLOAD; }

  inline std::vector<VkFence> getFences() { return mFences; }

private:
  std::vector<VkFence> mFences;
};

} // namespace Stak