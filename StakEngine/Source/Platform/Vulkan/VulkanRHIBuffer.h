#pragma once

#include "IRHIBuffer.h"
#include "VulkanRHIDevice.h"
#include "RHICore.h"

#include <vulkan/vulkan.h>

namespace Stak {

class VulkanRHIBuffer : public IRHIBuffer {
public:
  VulkanRHIBuffer(VkDevice device, VkPhysicalDevice physicalDevice, const RHIBufferDescription &desc);
  ~VulkanRHIBuffer();

  u32 getStride();
  u32 getSize();
  u32 getNumElements();
  void setData(u32 size, void *data);
  ERHIBufferUsage getUsage();

public:
  u32 findMemoryType(u32 filter, VkMemoryPropertyFlags flags);
  VkBuffer getBuffer() { return mDeviceBuffer; }

private:
  u32 mStride;
  u32 mSize;
  u32 mNumElements;

private:
  VkDevice mDevice;
  VkPhysicalDevice mPhysicalDevice;
  VkBuffer mDeviceBuffer;
  VkDeviceMemory mDeviceMemory;
  ERHIBufferUsage mUsage;
  void *mMappedMemory;
};

} // namespace Stak