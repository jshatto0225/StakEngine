#include "VulkanRHI.h"

#include "VulkanRHIDevice.h"
#include "VulkanRHIContext.h"
#include "Log.h"

#if defined(SK_GLFW)
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#endif

#include <imgui.h>
#include <backends/imgui_impl_vulkan.h>

namespace Stak {

VKAPI_ATTR VkBool32 VKAPI_CALL vkMessenger(
  VkDebugUtilsMessageSeverityFlagBitsEXT severity,
  VkDebugUtilsMessageTypeFlagsEXT type,
  const VkDebugUtilsMessengerCallbackDataEXT *callback_data,
  void *user_data
) {
  switch (severity) {
  case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
    SK_LOG_ERROR(callback_data->pMessage);
    break;
  case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
    SK_LOG_INFO(callback_data->pMessage);
    break;
  case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
    SK_LOG_INFO(callback_data->pMessage);
    break;
  case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
    SK_LOG_WARN(callback_data->pMessage);
    break;
  default:
    break;
  }

  return VK_FALSE;
}

VkResult VulkanRHI::createDebugMessenger(
  VkInstance instance,
  const VkDebugUtilsMessengerCreateInfoEXT *info,
  const VkAllocationCallbacks *allocator,
  VkDebugUtilsMessengerEXT *messenger
) {
  PFN_vkCreateDebugUtilsMessengerEXT func =
    (PFN_vkCreateDebugUtilsMessengerEXT)
    vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
  if (func != NULL) {
    return func(instance, info, allocator, messenger);
  }
  else {
    SK_LOG_ERROR("Extension not present");
    return VK_ERROR_EXTENSION_NOT_PRESENT;
  }
}

void VulkanRHI::init() {
  bool extensionsSupported = true;
  u32 layerCount;
  vkEnumerateInstanceLayerProperties(&layerCount, NULL);

  std::vector<VkLayerProperties> availableLayers(layerCount);
  vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

  for (u32 i = 0; i < VALIDATION_LAYERS.size(); i++) {
    bool layerFound = false;

    for (u32 j = 0; j < layerCount; j++) {
      if (strcmp(VALIDATION_LAYERS[i], availableLayers[j].layerName)) {
        layerFound = true;
      }
    }

    if (!layerFound) {
      extensionsSupported = false;
      break;
    }
  }

  if (ENABLE_VALIDATION_LAYERS && !extensionsSupported) {
    SK_LOG_ERROR("Validation layers not supported");
  }

  VkApplicationInfo appInfo = {};
  appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  appInfo.pApplicationName = ""; // TODO
  appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
  appInfo.pEngineName = "StakEngine";
  appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
  appInfo.apiVersion = VK_API_VERSION_1_0;

  VkInstanceCreateInfo instanceInfo = {};
  instanceInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  instanceInfo.pApplicationInfo = &appInfo;

  VkDebugUtilsMessengerCreateInfoEXT debugMessengerInfo = {};

  if (ENABLE_VALIDATION_LAYERS) {
    instanceInfo.enabledLayerCount = static_cast<u32>(VALIDATION_LAYERS.size());
    instanceInfo.ppEnabledLayerNames = VALIDATION_LAYERS.data();

    debugMessengerInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    debugMessengerInfo.messageSeverity =
      VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
      VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
      VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    debugMessengerInfo.messageType =
      VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
      VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
      VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    debugMessengerInfo.pfnUserCallback = vkMessenger;

    instanceInfo.pNext = &debugMessengerInfo;
  }

#ifdef SK_GLFW
  u32 glfwExtensionCount = 0;
  const char **glfwExtensions;
  glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

  std::vector<const char *> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);
#else
  std::vector<const char *> extensions;
#endif

  if (ENABLE_VALIDATION_LAYERS) {
    extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
  }

  instanceInfo.enabledExtensionCount = extensions.size();
  instanceInfo.ppEnabledExtensionNames = extensions.data();

  VkResult r = vkCreateInstance(&instanceInfo, NULL, &mInstance);

  if (r != VK_SUCCESS) {
    SK_LOG_ERROR("Failed to create vulkan instance");
    return;
  }

  r = createDebugMessenger(mInstance, &debugMessengerInfo, NULL, &mDebugMessenger);
  if (r != VK_SUCCESS) {
    SK_LOG_ERROR("Failed to create vulkan debug messenger");
  }
}

void VulkanRHI::shutdown() {
  vkDestroyDebugUtilsMessengerEXT(mInstance, mDebugMessenger, NULL);
  vkDestroyInstance(mInstance, NULL);
}

Ref<IRHIDevice> VulkanRHI::createDevice(Ref<Window> window) {
  return createRef<VulkanRHIDevice>(mInstance, window);
}

void VulkanRHI::initImGui(Ref<IRHIDevice> device, Ref<IRHIGraphicsContext> context) {
  Ref<VulkanRHIDevice> vulkanDevice = std::static_pointer_cast<VulkanRHIDevice>(device);
  Ref<VulkanRHIGraphicsContext> vulkanContext = std::static_pointer_cast<VulkanRHIGraphicsContext>(context);

  ImGui_ImplVulkan_InitInfo initInfo = {};

  initInfo.Instance = mInstance;

  initInfo.PhysicalDevice = vulkanDevice->getPhysicalDevice();
  initInfo.Device = vulkanDevice->getDevice();
  initInfo.QueueFamily = vulkanDevice->getGraphicsQueueFamily();
  initInfo.Queue = vulkanDevice->getGraphicsQueue();

  initInfo.DescriptorPool = vulkanContext->getDescriptorPool();
  initInfo.RenderPass = vulkanContext->getRenderPass();

  initInfo.MinImageCount = vulkanDevice->getImageCount();
  initInfo.ImageCount = vulkanDevice->getImageCount();

  ImGui_ImplVulkan_Init(&initInfo);
}

void VulkanRHI::imGuiNewFrame() {
  ImGui_ImplVulkan_NewFrame();
}

} // namespace Stak