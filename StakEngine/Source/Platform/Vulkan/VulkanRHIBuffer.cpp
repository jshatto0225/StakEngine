#include "VulkanRHIBuffer.h"

#include "Log.h"

namespace Stak {

VulkanRHIBuffer::VulkanRHIBuffer(VkDevice device, VkPhysicalDevice physicalDevice, const RHIBufferDescription &desc) {
  mSize = desc.elements.size() * desc.count;
  mDevice = device;
  mStride = 0;
  mUsage = desc.usage;
  for (RHIBufferElement element : desc.elements) {
    mStride += getSizeOfRHIFormat(element.format);
  }

  VkBufferUsageFlags usage = 0;
  VkMemoryPropertyFlags flags = 0;

  switch (desc.usage) {
  case ERHIBufferUsage::VERTEX_DEVICE:
    usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
    flags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
    break;
  case ERHIBufferUsage::VERTEX_SHARED:
    usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
    flags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
    break;
  case ERHIBufferUsage::INDEX_DEVICE:
    usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
    flags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
    break;
  case ERHIBufferUsage::INDEX_SHARED:
    usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
    flags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
    break;
  case ERHIBufferUsage::UNIFORM_DEVICE:
    usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
    flags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
    break;
  case ERHIBufferUsage::UNIFORM_SHARED:
    usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
    flags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
    break;
  case ERHIBufferUsage::STAGING:
    usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    flags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
    break;
  }

  VkBufferCreateInfo bufferInfo = {};
  bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  bufferInfo.size = mSize;
  bufferInfo.usage = usage;
  bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

  VkResult r = vkCreateBuffer(
    mDevice,
    &bufferInfo,
    NULL,
    &mDeviceBuffer
  );
  if (r != VK_SUCCESS) {
    SK_LOG_ERROR("Failed to create buffer");
  }

  VkMemoryRequirements memReqs = {};
  vkGetBufferMemoryRequirements(
    mDevice,
    mDeviceBuffer,
    &memReqs
  );

  VkMemoryAllocateInfo allocInfo = {};
  allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
  allocInfo.allocationSize = memReqs.size;

  allocInfo.memoryTypeIndex = findMemoryType(
    memReqs.memoryTypeBits,
    flags
  );

  r = vkAllocateMemory(
    mDevice,
    &allocInfo,
    NULL,
    &mDeviceMemory
  );
  if (r != VK_SUCCESS) {
    SK_LOG_ERROR("Failed to allocate memory");
  }
  vkBindBufferMemory(
    mDevice,
    mDeviceBuffer,
    mDeviceMemory,
    0
  );

  if (
    mUsage == ERHIBufferUsage::VERTEX_SHARED ||
    mUsage == ERHIBufferUsage::INDEX_SHARED ||
    mUsage == ERHIBufferUsage::STAGING ||
    mUsage == ERHIBufferUsage::UNIFORM_SHARED
  ) {
    vkMapMemory(mDevice, mDeviceMemory, 0, mSize, 0, &mMappedMemory);
  }
}

u32 VulkanRHIBuffer::findMemoryType(u32 filter, VkMemoryPropertyFlags flags) {
  VkPhysicalDeviceMemoryProperties memProps;
  vkGetPhysicalDeviceMemoryProperties(
    mPhysicalDevice,
    &memProps
  );

  for (u32 i = 0; i < memProps.memoryTypeCount; i++) {
    if ((filter & (1 << i)) && ((memProps.memoryTypes[i].propertyFlags & flags) == flags)) {
      return i;
    }
  }

  SK_LOG_ERROR("Failed to get memory type");
  return 0;
}

VulkanRHIBuffer::~VulkanRHIBuffer() {
  vkDestroyBuffer(mDevice, mDeviceBuffer, NULL);
  vkFreeMemory(mDevice, mDeviceMemory, NULL);
}

void VulkanRHIBuffer::setData(u32 size, void *data) {
  // cant set data of a device local buffer
  if (
    mUsage == ERHIBufferUsage::VERTEX_SHARED ||
    mUsage == ERHIBufferUsage::INDEX_SHARED ||
    mUsage == ERHIBufferUsage::STAGING ||
    mUsage == ERHIBufferUsage::UNIFORM_SHARED
  ) {
    memcpy(mMappedMemory, data, size);
  }
}

ERHIBufferUsage VulkanRHIBuffer::getUsage() {
  return mUsage;
}

u32 VulkanRHIBuffer::getStride() {
  return mStride;
}

u32 VulkanRHIBuffer::getSize() {
  return mSize;
}

u32 VulkanRHIBuffer::getNumElements() {
  return mNumElements;
}

} // namespace Stak