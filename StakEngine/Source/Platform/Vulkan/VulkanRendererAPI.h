#pragma once

#ifdef SK_WINDOWS
#define VK_USE_PLATFORM_GLFW_KHR
#endif

#include <vulkan/vulkan.h>

#include <string>

#include "RendererAPI.h"

#define MAX_FRAMES_IN_FLIGHT 2

namespace Stak {

class VulkanRendererAPI : public RendererAPI {
public:
  VulkanRendererAPI(Ref<Window> window, std::string appName);
  ~VulkanRendererAPI();

public:
  inline VkInstance getImGuiInstance() { return m_Instance; }
  inline VkPhysicalDevice getImGuiPhysicalDevice() { return m_PhysicalDevice; }
  inline VkDevice getImGuiDevice() { return m_Device; }
  inline u32 getImGuiGraphicsQueueFamily() { return m_GraphicsQueueFamily; }
  inline VkQueue getImGuiGraphicsQueue() { return m_GraphicsQueue; }
  inline VkPipelineCache getImGuiPipelineCache() { return m_ImGuiPipelineCache; }
  inline VkDescriptorPool getImGuiDescriptorPool() { return m_ImGuiDescriptorPool; }
  inline VkRenderPass getImGuiRenderPass() { return m_ImGuiRenderPass; }
  inline u32 getImGuiMinImageCount() { return m_ImGuiMinImageCount; }
  inline const VkAllocationCallbacks *getImGuiAllocator() { return m_ImGuiAllocator; }

private:
  void createInstance(std::string appName);
  void createDevice();
  void createSurface();

  static VkResult createDebugMessenger(
    VkInstance instance,
    const VkDebugUtilsMessengerCreateInfoEXT *info,
    const VkAllocationCallbacks *allocator,
    VkDebugUtilsMessengerEXT *messenger
  );

private:
  void createImGuiRenderPass();
  void createImGuiPipelineCache();
  void createImGuiDescriptorPool();
  void createImGuiAllocator();

private:
  Ref<Window> m_Window;

  VkInstance m_Instance;
  VkDebugUtilsMessengerEXT m_DebugMessenger;
  VkSurfaceKHR m_Surface;
  VkPhysicalDevice m_PhysicalDevice;
  VkDevice m_Device;
  u32 m_GraphicsQueueFamily;
  VkQueue m_GraphicsQueue;
  VkQueue m_PresentQueue;

private:
  VkPipelineCache m_ImGuiPipelineCache;
  VkDescriptorPool m_ImGuiDescriptorPool;
  VkRenderPass m_ImGuiRenderPass;
  u32 m_ImGuiMinImageCount;
  VkAllocationCallbacks *m_ImGuiAllocator;
};

} // namespace Stak
