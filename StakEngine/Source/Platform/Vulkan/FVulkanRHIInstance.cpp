#include "FVulkanRHIInstance.h"

#include "FLog.h"
#include "FVulkanRHIDevice.h"
#include "FVulkanRHIContext.h"

#ifdef SK_GLFW
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#endif

#include <imgui.h>
#include <backends/imgui_impl_vulkan.h>

VKAPI_ATTR VkBool32 VKAPI_CALL VkMessenger(VkDebugUtilsMessageSeverityFlagBitsEXT severity, VkDebugUtilsMessageTypeFlagsEXT type, const VkDebugUtilsMessengerCallbackDataEXT *callback_data, void *user_data) {
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

VkResult FVulkanRHIInstance::CreateDebugMessenger(VkInstance Instance, const VkDebugUtilsMessengerCreateInfoEXT *Info, const VkAllocationCallbacks *Allocator, VkDebugUtilsMessengerEXT *Messenger) {
  auto Func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(Instance, "vkCreateDebugUtilsMessengerEXT");
  if (Func != NULL) {
    return Func(Instance, Info, Allocator, Messenger);
  }
  else {
    SK_LOG_ERROR("Extension not present");
    return VK_ERROR_EXTENSION_NOT_PRESENT;
  }
}

void FVulkanRHIInstance::DestroyDebugMessenger(VkInstance Instance, VkDebugUtilsMessengerEXT Messenger, const VkAllocationCallbacks *Allocator) {
  auto Func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(Instance, "vkDestroyDebugUtilsMessengerEXT");
  if (Func != NULL) {
    Func(Instance, Messenger, Allocator);
  }
}

FVulkanRHIInstance::FVulkanRHIInstance(const std::string &AppName) {
  FBool ExtensionsSupported = true;
  FUInt32 LayerCount = 0;
  vkEnumerateInstanceLayerProperties(&LayerCount, NULL);

  std::vector<VkLayerProperties> AvailableLayers(LayerCount);
  vkEnumerateInstanceLayerProperties(&LayerCount, AvailableLayers.data());

  for (FUInt32 ValidationLayerIndex = 0; ValidationLayerIndex < VALIDATION_LAYERS.size(); ValidationLayerIndex++) {
    FBool LayerFound = false;

    for (FUInt32 j = 0; j < LayerCount; j++) {
      if (strcmp(VALIDATION_LAYERS[ValidationLayerIndex], AvailableLayers[j].layerName)) {
        LayerFound = true;
      }
    }

    if (!LayerFound) {
      ExtensionsSupported = false;
      break;
    }
  }

  if (ENABLE_VALIDATION_LAYERS && !ExtensionsSupported) {
    SK_LOG_ERROR("Validation layers not supported");
  }

  VkApplicationInfo AppInfo = {};
  AppInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  AppInfo.pApplicationName = AppName.c_str();
  AppInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
  AppInfo.pEngineName = "StakEngine";
  AppInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
  AppInfo.apiVersion = VK_API_VERSION_1_3;

  VkInstanceCreateInfo InstanceInfo = {};
  InstanceInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  InstanceInfo.pApplicationInfo = &AppInfo;

  VkDebugUtilsMessengerCreateInfoEXT DebugMessengerInfo = {};

  if (ENABLE_VALIDATION_LAYERS) {
    InstanceInfo.enabledLayerCount = static_cast<FUInt32>(VALIDATION_LAYERS.size());
    InstanceInfo.ppEnabledLayerNames = VALIDATION_LAYERS.data();

    DebugMessengerInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    DebugMessengerInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    DebugMessengerInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    DebugMessengerInfo.pfnUserCallback = VkMessenger;

    InstanceInfo.pNext = &DebugMessengerInfo;
  }

#ifdef SK_GLFW
  FUInt32 GlfwExtensionCount = 0;
  const char **GlfwExtensions = NULL;
  GlfwExtensions = glfwGetRequiredInstanceExtensions(&GlfwExtensionCount);

  std::vector<const char *> Extensions(GlfwExtensions, GlfwExtensions + GlfwExtensionCount);
#else
  std::vector<const char *> extensions;
#endif

  if (ENABLE_VALIDATION_LAYERS) {
    Extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
  }

  InstanceInfo.enabledExtensionCount = static_cast<FUInt32>(Extensions.size());
  InstanceInfo.ppEnabledExtensionNames = Extensions.data();

  VkResult Err = vkCreateInstance(&InstanceInfo, NULL, &mInstance);

  if (Err != VK_SUCCESS) {
    SK_LOG_ERROR("Failed to create vulkan instance");
    return;
  }

  Err = CreateDebugMessenger(mInstance, &DebugMessengerInfo, NULL, &mDebugMessenger);
  if (Err != VK_SUCCESS) {
    SK_LOG_ERROR("Failed to create vulkan debug messenger");
  }
}

FVulkanRHIInstance::~FVulkanRHIInstance() {
  DestroyDebugMessenger(mInstance, mDebugMessenger, NULL);
  vkDestroyInstance(mInstance, NULL);
}

void FVulkanRHIInstance::InitImGui(TRef<IRHIDevice> Device) {
  TRef<FVulkanRHIDevice> VulkanDevice = std::static_pointer_cast<FVulkanRHIDevice>(Device);

  ImGui_ImplVulkan_InitInfo InitInfo = {};
  InitInfo.Allocator = NULL;
  InitInfo.CheckVkResultFn = [](VkResult Err) {
    if (Err != VK_SUCCESS) {
      SK_LOG_ERROR("ImGui Vulkan Error");
    }
  };
  InitInfo.DescriptorPool = VulkanDevice->GetImGuiVkDescriptorPool();
  InitInfo.Instance = mInstance;
  InitInfo.Device = VulkanDevice->GetVkDevice();
  InitInfo.ImageCount = VulkanDevice->GetSwapchainImageCount();
  InitInfo.MinImageCount = VulkanDevice->GetSwapchainImageCount();
  InitInfo.MinAllocationSize = 1024 * 1024;
  InitInfo.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
  InitInfo.PhysicalDevice = VulkanDevice->GetVkPhysicalDevice();
  InitInfo.PipelineCache = NULL;
  VkPipelineRenderingCreateInfo PipelineInfo = {};
  PipelineInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO;
  PipelineInfo.viewMask = 0x01; // Render to attachment 1
  PipelineInfo.colorAttachmentCount = 1;
  VkFormat Formats[] = { VulkanDevice->GetVkSwapchainImageFormat() };
  PipelineInfo.pColorAttachmentFormats = Formats;
  PipelineInfo.depthAttachmentFormat = VK_FORMAT_UNDEFINED;
  PipelineInfo.stencilAttachmentFormat = VK_FORMAT_UNDEFINED;
  InitInfo.PipelineRenderingCreateInfo = PipelineInfo;
  InitInfo.Queue = VulkanDevice->GetVkGraphicsQueue();
  InitInfo.QueueFamily = VulkanDevice->GetVkGraphicsQueueFamilyIndex();
  InitInfo.RenderPass = VK_NULL_HANDLE;
  InitInfo.Subpass = 0;
  InitInfo.UseDynamicRendering = true;

  ImGui_ImplVulkan_Init(&InitInfo);
}

void FVulkanRHIInstance::ShutdownImGui(TRef<IRHIDevice> Device) {
  TRef<FVulkanRHIDevice> VulkanDevice = std::static_pointer_cast<FVulkanRHIDevice>(Device);

  ImGui_ImplVulkan_Shutdown();
  VulkanDevice->FreeImGuiVkDescriptorPool();
}

void FVulkanRHIInstance::ImGuiNewFrame() {
  ImGui_ImplVulkan_NewFrame();
}

void FVulkanRHIInstance::SubmitImGuiDrawData(ImDrawData *DrawData, TRef<IRHIGraphicsContext> Context) {
  TRef<FVulkanRHIGraphicsContext> VulkanContext = std::static_pointer_cast<FVulkanRHIGraphicsContext>(Context);
  ImGui_ImplVulkan_RenderDrawData(DrawData, VulkanContext->GetVkCommandBuffer());
}
