#pragma once

#include "IRHIContext.h"
#include "VulkanRHIPipeline.h"
#include "VulkanbRHIBuffer.h"
#include "VulkanRHITexture.h"

namespace Stak {

class VulkanRHIGraphicsContext : public IRHIGraphicsContext {
public:
  VulkanRHIGraphicsContext(VkDevice device);
  ~VulkanRHIGraphicsContext();

  void begin();
  void end();
  void resourceBarrier(ResourceBarrierDescription &barrierDesc);

  void setPipeline(Ref<IRHIPipeline> pipeline);
  void setVertexBuffer(Ref<IRHIBuffer> buffer);
  void setIndexBuffer(Ref<IRHIBuffer> buffer);
  void draw();

  inline ERHIContextType getType() { return ERHIContextType::GRAPHICS; }

  inline std::vector<VkFence> getFences() { return mFences; }

public:
  VkDescriptorPool getDescriptorPool() { return mPipeline->getDescriptorPool(); }
  VkRenderPass getRenderPass() { return mPipeline->getRenderPass(); }

private:
  Ref<VulkanRHIPipeline> mPipeline;
  Ref<VulkanRHIBuffer> mVertexBuffer;
  Ref<VulkanRHIBuffer> mIndexBuffer;

private:
  std::vector<VkFence> mFences;
};

class VulkanRHIComputeContext : public IRHIContext {
public:
  VulkanRHIComputeContext();
  ~VulkanRHIComputeContext();

  void setPipeline(Ref<IRHIPipeline> pipeline);
  void dispatch();

  inline ERHIContextType getType() { return ERHIContextType::COMPUTE; }

  inline std::vector<VkFence> getFences() { return mFences; }

private:
  std::vector<VkFence> mFences;
};

class VulkanRHIUploadContext : public IRHIContext {
public:
  VulkanRHIUploadContext();
  ~VulkanRHIUploadContext();

  void uploadBuffer(Ref<IRHIBuffer> buffer, void *data);
  void uploadTexture(Ref<IRHITexture> texture, void *data);

  inline ERHIContextType getType() { return ERHIContextType::UPLOAD; }

  inline std::vector<VkFence> getFences() { return mFences; }

private:
  std::vector<VkFence> mFences;
};

} // namespace Stak