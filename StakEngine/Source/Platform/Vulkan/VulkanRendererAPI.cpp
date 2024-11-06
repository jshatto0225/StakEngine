#include "VulkanRendererAPI.h"

#include <vector>

#include "Log.h"

#ifdef SK_DEBUG
const bool ENABLE_VALIDATION_LAYERS = true;
#else
const bool ENABLE_VALIDATION_LAYERS = false;
#endif

const std::vector<const char *> VALIDATION_LAYERS = {
  "VK_LAYER_KHRONOS_validation",
};

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

namespace Stak {

VkResult VulkanRendererAPI::createDebugMessenger(
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

VulkanRendererAPI::VulkanRendererAPI(Ref<Window> window, std::string appName) {
  m_Window = window;

  createInstance(appName);
  createDevice();
  createSurface();
}

VulkanRendererAPI::~VulkanRendererAPI() {

}

void VulkanRendererAPI::createInstance(std::string appName) {
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
  appInfo.pApplicationName = appName.c_str();
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

  std::vector<const char *> extensions = {
        VK_KHR_SURFACE_EXTENSION_NAME,
#ifdef SK_DEBUG
        VK_EXT_DEBUG_UTILS_EXTENSION_NAME,
#endif
  };

  instanceInfo.enabledExtensionCount = extensions.size();
  instanceInfo.ppEnabledExtensionNames = extensions.data();

  VkResult r = vkCreateInstance(&instanceInfo, NULL, &m_Instance);

  if (r != VK_SUCCESS) {
    SK_LOG_ERROR("Failed to create vulkan instance");
    return;
  }

  r = createDebugMessenger(m_Instance, &debugMessengerInfo, NULL, &m_DebugMessenger);
  if (r != VK_SUCCESS) {
    SK_LOG_ERROR("Failed to create vulkan debug messenger");
  }
}

void VulkanRendererAPI::createDevice() {

}

void VulkanRendererAPI::createSurface() {

}

void VulkanRendererAPI::createImGuiRenderPass() {

}

void VulkanRendererAPI::createImGuiPipelineCache() {

}

void VulkanRendererAPI::createImGuiDescriptorPool() {

}

void VulkanRendererAPI::createImGuiAllocator() {

}

} // namespace Stak
