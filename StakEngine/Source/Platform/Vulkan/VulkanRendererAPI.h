#pragma once

#include <vulkan/vulkan.h>

#include "../../Renderer/RendererAPI.h"

#define MAX_FRAMES_IN_FLIGHT 2

namespace Stak {

class VulkanRendererAPI : public RendererAPI {
public:
  VulkanRendererAPI(Ref<Window> window);
  ~VulkanRendererAPI();

  void WaitForDevice();

private:
  Ref<Window> m_Window;

  VkInstance m_Instance;
  VkDebugUtilsMessengerEXT m_DebugMessenger;
  VkSurfaceKHR m_Surface;
  VkPhysicalDevice m_PhysicalDevice;
  VkDevice m_LogicalDevice;
  VkQueue m_GraphicsQueue;
  VkQueue m_PresentQueue;
  VkExtent2D m_Extent;
  VkSwapchainKHR m_Swapchain;
  std::vector<VkImage> m_SwapchainImages;
  VkFormat m_SwapchainImageFormat;
  u32 m_SwapchainImageCount;
  std::vector<VkImageView> m_SwapchainImageViews;
  VkCommandPool m_CommandPool;
  VkCommandBuffer m_CommandBuffers[MAX_FRAMES_IN_FLIGHT];
  VkSemaphore m_ImageAvailableSemaphores[MAX_FRAMES_IN_FLIGHT];
  VkSemaphore m_RenderFinishedSemaphores[MAX_FRAMES_IN_FLIGHT];
  VkFence m_InFlightFences[MAX_FRAMES_IN_FLIGHT];
  std::vector<VkFramebuffer> m_Framebuffers;
  u32 CurrentFrame;
  bool m_FramebufferResized;
  VkRenderPass m_RenderPass;
  VkPipeline m_GraphicsPipeline;
  VkPipelineLayout m_PipelineLayout;
  VkBuffer m_VertexBuffer;
  VkDeviceMemory m_VertexBufferMemory;
  VkBuffer m_VwertexStagingBuffer;
  VkDeviceMemory m_VertexStagingBufferMemory;
  VkBuffer m_IndexBuffer;
  VkDeviceMemory m_IndexBufferMemory;
  VkBuffer m_IndexStagingBuffer;
  VkDeviceMemory m_IndexStagingBufferMemory;
  VkDescriptorSetLayout m_DescriptorSetLayout;
  VkBuffer m_UniformBuffers[MAX_FRAMES_IN_FLIGHT];
  VkDeviceMemory m_UniformBuffersMemory[MAX_FRAMES_IN_FLIGHT];
  void *m_UniformBuffersMapped[MAX_FRAMES_IN_FLIGHT];
  VkDescriptorPool m_DescriptorPool;
  VkDescriptorSet m_DescriptorSets[MAX_FRAMES_IN_FLIGHT];
};

} // namespace Stak