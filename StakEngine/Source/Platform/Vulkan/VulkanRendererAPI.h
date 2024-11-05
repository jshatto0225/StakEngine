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

  void WaitForDevice();

public:
  inline VkInstance GetImGuiInstance() { return m_Instance; }
  inline VkPhysicalDevice GetImGuiPhysicalDevice() { return m_PhysicalDevice; }
  inline VkDevice GetImGuiDevice() { return m_Device; }
  inline u32 GetImGuiGraphicsQueueFamily() { return m_GraphicsQueueFamily; }
  inline VkQueue GetImGuiGraphicsQueue() { return m_GraphicsQueue; }
  inline VkPipelineCache GetImGuiPipelineCache() { return m_ImGuiPipelineCache; }
  inline VkDescriptorPool GetImGuiDescriptorPool() { return m_ImGuiDescriptorPool; }
  inline VkRenderPass GetImGuiRenderPass() { return m_ImGuiRenderPass; }
  inline u32 GetImGuiMinImageCount() { return m_ImGuiMinImageCount; }
  inline const VkAllocationCallbacks *GetImGuiAllocator() { return m_ImGuiAllocator; }

private:
  void CreateInstance(std::string appName);
  void CreateDevice();
  void CreateSurface();

  static VkResult CreateDebugMessenger(
    VkInstance instance, 
    const VkDebugUtilsMessengerCreateInfoEXT *info, 
    const VkAllocationCallbacks *allocator, 
    VkDebugUtilsMessengerEXT *messenger
  );

private:
  void CreateImGuiRenderPass();
  void CreateImGuiPipelineCache();
  void CreateImGuiDescriptorPool();
  void CreateImGuiAllocator();

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