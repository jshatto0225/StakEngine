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

void VulkanRHI::destroyDebugMessenger(
  VkInstance instance,
  VkDebugUtilsMessengerEXT messenger,
  const VkAllocationCallbacks *allocator
) {
  PFN_vkDestroyDebugUtilsMessengerEXT func =
    (PFN_vkDestroyDebugUtilsMessengerEXT)
    vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
  if (func != NULL) {
    func(instance, messenger, allocator);
  }
}

VulkanRHI::VulkanRHI() : mDebugMessenger(VK_NULL_HANDLE) {
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

VulkanRHI::~VulkanRHI() {
  destroyDebugMessenger(mInstance, mDebugMessenger, NULL);
  vkDestroyInstance(mInstance, NULL);
}

Ref<IRHIDevice> VulkanRHI::createDevice(Ref<Window> window) {
  return createRef<VulkanRHIDevice>(mInstance, window);
}

void VulkanRHI::initImGui(Ref<IRHIDevice> device) {
  Ref<VulkanRHIDevice> vulkanDevice = std::static_pointer_cast<VulkanRHIDevice>(device);

  ImGui_ImplVulkan_InitInfo initInfo = {};

  initInfo.Instance = mInstance;

  initInfo.PhysicalDevice = vulkanDevice->getPhysicalDevice();
  initInfo.Device = vulkanDevice->getDevice();
  initInfo.QueueFamily = vulkanDevice->getGraphicsQueueFamily();
  initInfo.Queue = vulkanDevice->getGraphicsQueue();

  VkDescriptorPoolSize poolSizes[] = {
    { VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
    { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
    { VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
    { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
    { VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
    { VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },
    { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
    { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
    { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
    { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
    { VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 }
  };

  VkDescriptorPoolCreateInfo poolInfo = {};
  poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
  poolInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
  poolInfo.maxSets = 1000;
  poolInfo.poolSizeCount = (uint32_t)std::size(poolSizes);
  poolInfo.pPoolSizes = poolSizes;

  VkDescriptorPool imguiPool = {};
  VkResult r = vkCreateDescriptorPool(vulkanDevice->getDevice(), &poolInfo, nullptr, &imguiPool);
  if (r != VK_SUCCESS) {
    SK_LOG_ERROR("Failed to create imgui descriptor pool");
  }
  initInfo.DescriptorPool = imguiPool;

  VkRenderPass imGuiRenderPass = {};

  VkAttachmentDescription colorAttachment = {};
  colorAttachment.format = vulkanDevice->getSwapchainImageFormat();
  colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
  colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
  colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
  colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

  VkAttachmentReference colorAttachmentRef = {};
  colorAttachmentRef.attachment = 0;
  colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

  VkSubpassDescription subpass = { 0 };
  subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
  subpass.colorAttachmentCount = 1;
  subpass.pColorAttachments = &colorAttachmentRef;

  VkSubpassDependency dependency = { 0 };
  dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
  dependency.dstSubpass = 0;
  dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  dependency.srcAccessMask = 0;
  dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

  VkRenderPassCreateInfo renderPassInfo = {};
  renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
  renderPassInfo.attachmentCount = 1;
  renderPassInfo.pAttachments = &colorAttachment;
  renderPassInfo.subpassCount = 1;
  renderPassInfo.pSubpasses = &subpass;
  renderPassInfo.dependencyCount = 1;
  renderPassInfo.pDependencies = &dependency;

  r = vkCreateRenderPass(
    vulkanDevice->getDevice(),
    &renderPassInfo,
    NULL,
    &imGuiRenderPass
  );

  if (r != VK_SUCCESS) {
    SK_LOG_ERROR("Failed to create imgui render pass");
  }

  initInfo.RenderPass = imGuiRenderPass;

  initInfo.MinImageCount = vulkanDevice->getImageCount();
  initInfo.ImageCount = vulkanDevice->getImageCount();

  ImGui_ImplVulkan_Init(&initInfo);

  mImGuiRenderPass = imGuiRenderPass;
  mImGuiDescriptorPool = imguiPool;
  mImGuiDevice = vulkanDevice->getDevice();
}

void VulkanRHI::shutdownImGui() {
  ImGui_ImplVulkan_Shutdown();

  vkDestroyRenderPass(mImGuiDevice, mImGuiRenderPass, NULL);
  vkDestroyDescriptorPool(mImGuiDevice, mImGuiDescriptorPool, NULL);
}

void VulkanRHI::imGuiNewFrame() {
  ImGui_ImplVulkan_NewFrame();
}

void VulkanRHI::renderImGuiDrawData(ImDrawData *data, Ref<IRHIGraphicsContext> context) {
  Ref<VulkanRHIGraphicsContext> vulkanContext = std::static_pointer_cast<VulkanRHIGraphicsContext>(context);
  ImGui_ImplVulkan_RenderDrawData(data, vulkanContext->getCommandBuffer());
}

} // namespace Stak