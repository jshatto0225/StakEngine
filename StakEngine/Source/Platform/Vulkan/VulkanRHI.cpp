#include "VulkanRHI.h"

#include "FLog.h"
#include "FApplication.h"
#include "VulkanRHICommandContext.h"

#include <set>

#ifdef SK_GLFW
#include "FGLFWWindow.h"
#endif

#define CHECK_VK_ERR(Err, Message) \
if (Err != VK_SUCCESS) {           \
  SK_LOG_ERROR(Message);           \
}

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

VkResult CreateDebugMessenger(VkInstance Instance, const VkDebugUtilsMessengerCreateInfoEXT *Info, const VkAllocationCallbacks *Allocator, VkDebugUtilsMessengerEXT *Messenger) {
  auto Func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(Instance, "vkCreateDebugUtilsMessengerEXT");
  if (Func != NULL) {
    return Func(Instance, Info, Allocator, Messenger);
  }
  else {
    SK_LOG_ERROR("Extension not present");
    return VK_ERROR_EXTENSION_NOT_PRESENT;
  }
}

void DestroyDebugMessenger(VkInstance Instance, VkDebugUtilsMessengerEXT Messenger, const VkAllocationCallbacks *Allocator) {
  auto Func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(Instance, "vkDestroyDebugUtilsMessengerEXT");
  if (Func != NULL) {
    Func(Instance, Messenger, Allocator);
  }
}

FVulkanRHI::VulkanQueueFamilies FVulkanRHI::FindQueueFamilies(VkPhysicalDevice Device) {
  VulkanQueueFamilies Indices;

  FUInt32 QueueFamilyCount = 0;
  vkGetPhysicalDeviceQueueFamilyProperties(Device, &QueueFamilyCount, NULL);
  std::vector<VkQueueFamilyProperties> QueueFamilies(QueueFamilyCount);
  vkGetPhysicalDeviceQueueFamilyProperties(Device, &QueueFamilyCount, QueueFamilies.data());

  for (FUInt32 QueueFamilyIndex = 0; QueueFamilyIndex < QueueFamilyCount; QueueFamilyIndex++) {
    if (QueueFamilies[QueueFamilyIndex].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
      Indices.GraphicsFamily = QueueFamilyIndex;
    }

    VkBool32 PresentSupport = VK_FALSE;
    vkGetPhysicalDeviceSurfaceSupportKHR(Device, QueueFamilyIndex, mSurface, &PresentSupport);
    if (PresentSupport) {
      Indices.PresentFamily = QueueFamilyIndex;
    }
    if (Indices.PresentFamily.has_value() && Indices.GraphicsFamily.has_value()) {
      break;
    }
  }

  return Indices;
}

FVulkanRHI::VulkanSwapchainSupport FVulkanRHI::GetSwapchainSupport(VkPhysicalDevice Device) {
  VulkanSwapchainSupport Support = {};

  vkGetPhysicalDeviceSurfaceCapabilitiesKHR(Device, mSurface, &Support.Capabilities);

  FUInt32 FormatCount = 0;
  vkGetPhysicalDeviceSurfaceFormatsKHR(Device, mSurface, &FormatCount, NULL);
  Support.Formats.resize(FormatCount);

  vkGetPhysicalDeviceSurfaceFormatsKHR(Device, mSurface, &FormatCount, Support.Formats.data());

  FUInt32 PresentModeCount = 0;
  vkGetPhysicalDeviceSurfacePresentModesKHR(Device, mSurface, &PresentModeCount, NULL);
  Support.PresentModes.resize(PresentModeCount);

  vkGetPhysicalDeviceSurfacePresentModesKHR(Device, mSurface, &PresentModeCount, Support.PresentModes.data());

  return Support;
}

FVulkanRHI::FVulkanRHI() {
  mCommandContext = new FVulkanRHICommandContext();

  // INSTANCE
  FBool ExtensionsSupported = true;
  FUInt32 LayerCount = 0;
  vkEnumerateInstanceLayerProperties(&LayerCount, NULL);

  std::vector<VkLayerProperties> AvailableLayers(LayerCount);
  vkEnumerateInstanceLayerProperties(&LayerCount, AvailableLayers.data());

  for (FUInt32 ValidationLayerIndex = 0; ValidationLayerIndex < VALIDATION_LAYERS.size(); ValidationLayerIndex++) {
    FBool LayerFound = false;

    for (FUInt32 LayerIndex = 0; LayerIndex < LayerCount; LayerIndex++) {
      if (strcmp(VALIDATION_LAYERS[ValidationLayerIndex], AvailableLayers[LayerIndex].layerName)) {
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
  AppInfo.pApplicationName = FApplication::Get().GetName();
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

  CHECK_VK_ERR(vkCreateInstance(&InstanceInfo, NULL, &mInstance), "Failed to create vulkan instance");
  CHECK_VK_ERR(CreateDebugMessenger(mInstance, &DebugMessengerInfo, NULL, &mDebugMessenger), "Failed to create vulkan debug messenger");

  // SURFACE
#ifdef SK_GLFW
  TRef<FGLFWWindow> glfwWindow = std::static_pointer_cast<FGLFWWindow>(FApplication::Get().GetWindow());
  glfwCreateWindowSurface(mInstance, glfwWindow->GetGlfwWindow(), NULL, &mSurface);
#endif

  // DEVICE
  FUInt32 DeviceCount = 0;
  vkEnumeratePhysicalDevices(mInstance, &DeviceCount, NULL);
  std::vector<VkPhysicalDevice> Devices(DeviceCount);
  vkEnumeratePhysicalDevices(mInstance, &DeviceCount, Devices.data());

  FBool DeviceFound = false;
  for (FUInt32 DeviceIndex = 0; DeviceIndex < DeviceCount; DeviceIndex++) {
    FBool IsDeviceSuitable = false;
    VulkanQueueFamilies Indices = FindQueueFamilies(Devices[DeviceIndex]);

    FUInt32 DeviceExtensionCount = 0;
    vkEnumerateDeviceExtensionProperties(Devices[DeviceIndex], NULL, &DeviceExtensionCount, NULL);
    std::vector<VkExtensionProperties> AvailableExtensions(DeviceExtensionCount);

    vkEnumerateDeviceExtensionProperties(Devices[DeviceIndex], NULL, &DeviceExtensionCount, AvailableExtensions.data());

    FBool ExtensionsSupported = false;

    for (FUInt32 DeviceExtensionIndex = 0; DeviceExtensionIndex < DeviceExtensionCount; DeviceExtensionIndex++) {
      if (std::strcmp(AvailableExtensions[DeviceExtensionIndex].extensionName, VK_KHR_SWAPCHAIN_EXTENSION_NAME)) {
        ExtensionsSupported = true;
        break;
      }
    }

    FBool SwapchainAdequate = false;
    if (ExtensionsSupported) {
      VulkanSwapchainSupport Support = GetSwapchainSupport(Devices[DeviceIndex]);
      SwapchainAdequate = Support.PresentModes.size() > 0 && Support.Formats.size() > 0;
    }

    VkPhysicalDeviceFeatures SupportedFeatures;
    vkGetPhysicalDeviceFeatures(Devices[DeviceIndex], &SupportedFeatures);

    IsDeviceSuitable = ExtensionsSupported && SwapchainAdequate && Indices.GraphicsFamily.has_value() && Indices.PresentFamily.has_value() && SupportedFeatures.samplerAnisotropy;

    if (IsDeviceSuitable) {
      mGPU = Devices[DeviceIndex];
      DeviceFound = true;
      break;
    }
  }

  if (!DeviceFound) {
    SK_LOG_ERROR("Failed to find suitable physical device");
  }

  VulkanQueueFamilies Indices = FindQueueFamilies(mGPU);

  std::set<FUInt32> UniqueFamilies = {
    Indices.GraphicsFamily.value(),
    Indices.PresentFamily.value()
  };

  std::vector<VkDeviceQueueCreateInfo> QueueInfos;
  QueueInfos.reserve(UniqueFamilies.size());

  FFloat QueuePriority = 1.0f;
  for (FUInt32 QueueFamily : UniqueFamilies) {
    VkDeviceQueueCreateInfo queueInfo = {};
    queueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueInfo.queueFamilyIndex = QueueFamily;
    queueInfo.queueCount = 1;
    queueInfo.pQueuePriorities = &QueuePriority;
    QueueInfos.push_back(queueInfo);
  }

  VkPhysicalDeviceFeatures DeviceFeatures = {};
  DeviceFeatures.samplerAnisotropy = VK_TRUE;

  VkDeviceCreateInfo DeviceInfo = {};
  DeviceInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

  VkPhysicalDeviceDynamicRenderingFeatures DynamicRenderingFeatures = {};
  DynamicRenderingFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_FEATURES;
  DynamicRenderingFeatures.dynamicRendering = VK_TRUE;
  DeviceInfo.pNext = &DynamicRenderingFeatures;

  VkPhysicalDeviceMultiviewFeatures MultiviewFeatures = {};
  MultiviewFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MULTIVIEW_FEATURES;
  MultiviewFeatures.multiview = VK_TRUE;

  DynamicRenderingFeatures.pNext = &MultiviewFeatures;

  VkPhysicalDeviceSynchronization2Features Sync2Features = {};
  Sync2Features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SYNCHRONIZATION_2_FEATURES;
  Sync2Features.synchronization2 = VK_TRUE;

  MultiviewFeatures.pNext = &Sync2Features;

  DeviceInfo.queueCreateInfoCount = static_cast<FUInt32>(QueueInfos.size());
  DeviceInfo.pQueueCreateInfos = QueueInfos.data();
  DeviceInfo.pEnabledFeatures = &DeviceFeatures;
  DeviceInfo.enabledExtensionCount = static_cast<FUInt32>(DEVICE_EXTENSIONS.size());
  DeviceInfo.ppEnabledExtensionNames = DEVICE_EXTENSIONS.data();

  if (ENABLE_VALIDATION_LAYERS) {
    DeviceInfo.enabledLayerCount = static_cast<FUInt32>(VALIDATION_LAYERS.size());
    DeviceInfo.ppEnabledLayerNames = VALIDATION_LAYERS.data();
  }

  CHECK_VK_ERR(vkCreateDevice(mGPU, &DeviceInfo, NULL, &mDevice), "Failed to create vulkan device");

  vkGetDeviceQueue(mDevice, Indices.GraphicsFamily.value(), 0, &mGraphicsQueue);
  vkGetDeviceQueue(mDevice, Indices.PresentFamily.value(), 0, &mPresentQueue);

  mGraphicsQueueIndex = Indices.GraphicsFamily.value();
  mPresentQueueIndex = Indices.PresentFamily.value();

  // SWAPCHAIN
  CreateSwapchain();
  CreateImageViews();

  // SYNC
  CreateSyncObjects();

  // IMGUI
  VkDescriptorPoolSize PoolSizes[] =
  {
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

  VkDescriptorPoolCreateInfo PoolInfo = {};
  PoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
  PoolInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
  PoolInfo.maxSets = 1000;
  PoolInfo.poolSizeCount = static_cast<FUInt32>(std::size(PoolSizes));
  PoolInfo.pPoolSizes = PoolSizes;

  CHECK_VK_ERR(vkCreateDescriptorPool(mDevice, &PoolInfo, NULL, &mImGuiPool), "Failed to create imgui descriptor pool");

  // COMMAND BUFFERS
  VkCommandPoolCreateInfo CommandPoolInfo = {};

  CommandPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
  CommandPoolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
  CommandPoolInfo.queueFamilyIndex = mGraphicsQueueIndex;

  CHECK_VK_ERR(vkCreateCommandPool(mDevice, &CommandPoolInfo, NULL, &mCommandPool), "Failed to create vulkan command pool");

  VkCommandBufferAllocateInfo AllocInfo = {};
  AllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  AllocInfo.commandPool = mCommandPool;
  AllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  AllocInfo.commandBufferCount = MAX_FRAMES_IN_FLIGHT;

  mCommandBuffers.resize(MAX_FRAMES_IN_FLIGHT);
  CHECK_VK_ERR(vkAllocateCommandBuffers(mDevice, &AllocInfo, mCommandBuffers.data()), "Failed to allocate vulkan command buffers");
}

FVulkanRHI::~FVulkanRHI() {
  delete mCommandContext;

  for (VkSemaphore Semaphore : mImageAvailableSemaphores) {
    vkDestroySemaphore(mDevice, Semaphore, NULL);
  }

  for (VkSemaphore Semaphore : mRenderFinishedSemaphores) {
    vkDestroySemaphore(mDevice, Semaphore, NULL);
  }

  for (VkFence Fence : mInFlightFences) {
    vkDestroyFence(mDevice, Fence, NULL);
  }

  vkFreeCommandBuffers(mDevice, mCommandPool, static_cast<FUInt32>(mCommandBuffers.size()), mCommandBuffers.data());
  vkDestroyCommandPool(mDevice, mCommandPool, NULL);

  vkDestroyDescriptorPool(mDevice, mImGuiPool, NULL);

  for (FUInt32 ImageIndex = 0; ImageIndex < mSwapchainImages.size(); ImageIndex++) {
    vkDestroyImageView(mDevice, mSwapchainImageViews[ImageIndex], NULL);
  }

  vkDestroySwapchainKHR(mDevice, mSwapchain, NULL);

  vkDestroyDevice(mDevice, NULL);

  vkDestroySurfaceKHR(mInstance, mSurface, NULL);

  DestroyDebugMessenger(mInstance, mDebugMessenger, NULL);

  vkDestroyInstance(mInstance, NULL);
}

void FVulkanRHI::CreateSyncObjects() {
  VkSemaphoreCreateInfo SemaphoreInfo = {};
  SemaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

  VkFenceCreateInfo FenceInfo = {};
  FenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
  FenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

  mRenderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
  mImageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
  mInFlightFences.resize(MAX_FRAMES_IN_FLIGHT);

  for (FUInt32 i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
    CHECK_VK_ERR(vkCreateSemaphore(mDevice, &SemaphoreInfo, NULL, &mImageAvailableSemaphores[i]), "Failed to create vulkan semaphore");

    CHECK_VK_ERR(vkCreateSemaphore(mDevice, &SemaphoreInfo, NULL, &mRenderFinishedSemaphores[i]), "Failed to create vulkan semaphore");

    CHECK_VK_ERR(vkCreateFence(mDevice, &FenceInfo, NULL, &mInFlightFences[i]), "Failed to create vulkan fence");
  }
}

void FVulkanRHI::RecreateSwapchain() {
  // TODO
  auto [Width, Height] = FApplication::Get().GetWindow()->GetSize();
  
  while (Width == 0 || Height == 0) {
    FWindowSizeData data = FApplication::Get().GetWindow()->GetSize();
    Width = data.Width;
    Height = data.Height;
  }
  
  WaitForGPUIdle();
  
  for (FUInt32 ImageIndex = 0; ImageIndex < mSwapchainImages.size(); ImageIndex++) {
    vkDestroyImageView(mDevice, mSwapchainImageViews[ImageIndex], NULL);
  }
  
  vkDestroySwapchainKHR(mDevice, mSwapchain, NULL);
  
  CreateSwapchain();
  CreateImageViews();
}

void FVulkanRHI::CreateSwapchain() {
  VulkanSwapchainSupport Support = GetSwapchainSupport(mGPU);
    
  VkSurfaceFormatKHR Format = {};
  for (FUInt32 FormatIndex = 0; FormatIndex < Support.Formats.size(); FormatIndex++) {
    if (		
      Support.Formats[FormatIndex].format == VK_FORMAT_B8G8R8A8_SRGB &&
      Support.Formats[FormatIndex].colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR
    ) {
      Format = Support.Formats[FormatIndex];
      break;
    }
  }	
    
  VkPresentModeKHR PresentMode = {};
  for (FUInt32 PresentModeIndex = 0; PresentModeIndex < Support.PresentModes.size(); PresentModeIndex++) {
    if (Support.PresentModes[PresentModeIndex] == VK_PRESENT_MODE_MAILBOX_KHR) {
      PresentMode = Support.PresentModes[PresentModeIndex];
      break;
    }
  }
    
  if (Support.Capabilities.currentExtent.width != UINT32_MAX) {
    mSwapchainExtent = Support.Capabilities.currentExtent;
  }
  else {
    auto [Width, Height] = FApplication::Get().GetWindow()->GetSize();
        
    VkExtent2D Extent = { 
      static_cast<FUInt32>(Width), 
      static_cast<FUInt32>(Height) 
    };
        
    Extent.width = std::clamp(Extent.width, Support.Capabilities.minImageExtent.width, Support.Capabilities.maxImageExtent.width);
    Extent.height = std::clamp(Extent.height, Support.Capabilities.minImageExtent.height, Support.Capabilities.maxImageExtent.height);
        
    mSwapchainExtent = Extent;
  }
    
  FUInt32 ImageCount = Support.Capabilities.maxImageCount + 1;
    
  if (Support.Capabilities.maxImageCount > 0 && ImageCount > Support.Capabilities.maxImageCount) {
    ImageCount = Support.Capabilities.maxImageCount;
  }
    
  VkSwapchainCreateInfoKHR SwapchainInfo = {};
  SwapchainInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
  SwapchainInfo.surface = mSurface;
  SwapchainInfo.minImageCount = ImageCount;
  SwapchainInfo.imageFormat = Format.format;
  SwapchainInfo.imageColorSpace = Format.colorSpace;
  SwapchainInfo.imageExtent = mSwapchainExtent;
  SwapchainInfo.imageArrayLayers = 1;
  SwapchainInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    
  mSwapchainImageFormat = Format;
    
  VulkanQueueFamilies Indices = FindQueueFamilies(mGPU);
    
  FUInt32 queueFamilyIndices[] = {
    Indices.GraphicsFamily.value(),
    Indices.PresentFamily.value()
  };
    
  if (Indices.GraphicsFamily.value() != Indices.PresentFamily.value()) {
    SwapchainInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
    SwapchainInfo.queueFamilyIndexCount = 2;
    SwapchainInfo.pQueueFamilyIndices = queueFamilyIndices;
  }
  else {
    SwapchainInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
  }
    
  SwapchainInfo.preTransform = Support.Capabilities.currentTransform;
  SwapchainInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
  SwapchainInfo.presentMode = PresentMode;
  SwapchainInfo.clipped = VK_TRUE;
  SwapchainInfo.oldSwapchain = VK_NULL_HANDLE;
  CHECK_VK_ERR(vkCreateSwapchainKHR(mDevice, &SwapchainInfo, NULL, &mSwapchain), "Failed to create swapchian");
    
  mSwapchainImages.resize(ImageCount);
  vkGetSwapchainImagesKHR(mDevice, mSwapchain, &ImageCount, mSwapchainImages.data());
}

VkImageView FVulkanRHI::CreateImageView(VkImage Image, VkFormat Format) {
  VkImageViewCreateInfo ViewInfo = {};
  ViewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
  ViewInfo.image = Image;
  ViewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
  ViewInfo.format = Format;
  ViewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  ViewInfo.subresourceRange.baseMipLevel = 0;
  ViewInfo.subresourceRange.levelCount = 1;
  ViewInfo.subresourceRange.baseArrayLayer = 0;
  ViewInfo.subresourceRange.layerCount = 1;
    
  VkImageView ImageView;
  CHECK_VK_ERR(vkCreateImageView(mDevice, &ViewInfo, NULL, &ImageView), "Failed to create image view");
    
  return ImageView;
}

void FVulkanRHI::CreateImageViews() {
  mSwapchainImageViews.resize(mSwapchainImages.size());
    
  for (FUInt32 ImageViewIndex = 0; ImageViewIndex < mSwapchainImages.size(); ImageViewIndex++) {
    mSwapchainImageViews[ImageViewIndex] = CreateImageView(mSwapchainImages[ImageViewIndex], mSwapchainImageFormat.format);
  }
}

FRHISamplerStateRef FVulkanRHI::CreateSamplerState(const FRHISamplerStateInitializer &Initializer) {
  return NULL;
}

FRHIRasterizerStateRef FVulkanRHI::CreateRasterizerState(const FRHIRasterizerStateInitializer &Initializer) {
  return NULL;
}

FRHIDepthStencilStateRef FVulkanRHI::CreateDepthStencilState(const FRHIDepthStencilStateInitializer &Initializer) {
  return NULL;
}

FRHIBlendStateRef FVulkanRHI::CreateBlendState(const FRHIBlendStateInitializer &Initializer) {
  return NULL;
}

FRHIVertexDeclarationRef FVulkanRHI::CreateVertexDeclaration(const FRHIVertexDeclarationElementList &Elements) {
  return NULL;
}

FRHIPixelShaderRef FVulkanRHI::CreatePixelShader() {
  return NULL;
}

FRHIVertexShaderRef FVulkanRHI::CreateVertexShader() {
  return NULL;
}

FRHIGeometryShaderRef FVulkanRHI::CreateGeometryShader() {
  return NULL;
}

FRHIBufferRef FVulkanRHI::CreateBuffer(const FRHIBufferDescription &Description, ERHIAccess Access) {
  return NULL;
}

FRHIUniformBufferRef FVulkanRHI::CreateUniformBuffer(const void *InitialContents, const FRHIUniformBufferLayout &Layout, ERHIUniformBufferUsage Usage) {
  return NULL;
}

FRHIStagingBufferRef FVulkanRHI::CreateStagingBuffer() {
  return NULL;
}

FRHITextureRef FVulkanRHI::CreateTexture(const FRHITextureDescription &Description) {
  return NULL;
}

FRHIGraphicsPipelineStateRef FVulkanRHI::CreateGraphicsPipelineState(const FRHIGraphicsPipelineStateInitializer &Initializer) {
  return NULL;
}

FRHIShaderResourceViewRef FVulkanRHI::CreateShaderResourceView(FRHIViewableResourceRef Resource, const FRHIViewDescription &Description) {
  return NULL;
}

FRHIUnorderedAccessViewRef FVulkanRHI::CreateUnorderedAccessView(FRHIViewableResourceRef Resource, const FRHIViewDescription &Description) {
  return NULL;
}

void FVulkanRHI::InitImGui() {
  ImGui_ImplVulkan_InitInfo InitInfo = {};
  InitInfo.Allocator = NULL;
  InitInfo.CheckVkResultFn = [](VkResult Err) {
    CHECK_VK_ERR(Err, "ImGui Vulkan Error");
  };
  InitInfo.DescriptorPool = mImGuiPool;
  InitInfo.Instance = mInstance;
  InitInfo.Device = mDevice;
  InitInfo.ImageCount = static_cast<FUInt32>(mSwapchainImages.size());
  InitInfo.MinImageCount = static_cast<FUInt32>(mSwapchainImages.size());
  InitInfo.MinAllocationSize = 1024 * 1024;
  InitInfo.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
  InitInfo.PhysicalDevice = mGPU;
  InitInfo.PipelineCache = NULL;
  VkPipelineRenderingCreateInfo PipelineInfo = {};
  PipelineInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO;
  PipelineInfo.viewMask = 0x01;
  PipelineInfo.colorAttachmentCount = 1;
  VkFormat Formats[] = { mSwapchainImageFormat.format };
  PipelineInfo.pColorAttachmentFormats = Formats;
  PipelineInfo.depthAttachmentFormat = VK_FORMAT_UNDEFINED;
  PipelineInfo.stencilAttachmentFormat = VK_FORMAT_UNDEFINED;
  InitInfo.PipelineRenderingCreateInfo = PipelineInfo;
  InitInfo.Queue = mGraphicsQueue;
  InitInfo.QueueFamily = mGraphicsQueueIndex;
  InitInfo.RenderPass = VK_NULL_HANDLE;
  InitInfo.Subpass = 0;
  InitInfo.UseDynamicRendering = true;

  ImGui_ImplVulkan_Init(&InitInfo);
}

void FVulkanRHI::ShutdownImGui() {
  WaitForGPUIdle();
  ImGui_ImplVulkan_Shutdown();
}

void FVulkanRHI::ImGuiNewFrame() {
  ImGui_ImplVulkan_NewFrame();
}

void FVulkanRHI::SubmitImGuiDrawData(ImDrawData *DrawData) {
  ImGui_ImplVulkan_RenderDrawData(DrawData, mCommandBuffers[mCurrentFrame]);
}

void FVulkanRHI::WaitForGPUIdle() {
  vkDeviceWaitIdle(mDevice);
}

void FVulkanRHI::BeginDrawing() {
  vkWaitForFences(mDevice, 1, &mInFlightFences[mCurrentFrame], VK_TRUE, UINT64_MAX);
  
  VkResult Err = vkAcquireNextImageKHR(
    mDevice,
    mSwapchain,
    UINT64_MAX,
    mImageAvailableSemaphores[mCurrentFrame], 
    VK_NULL_HANDLE, 
    &mImageIndex
  );
  
  if (Err == VK_ERROR_OUT_OF_DATE_KHR) {
    RecreateSwapchain();
    BeginDrawing();
  }
  else if (Err != VK_SUCCESS && Err != VK_SUBOPTIMAL_KHR) {
    SK_LOG_ERROR("Failed to get next swapchain image");
  }
  
  vkResetFences(mDevice, 1, &mInFlightFences[mCurrentFrame]);
  
  vkResetCommandBuffer(mCommandBuffers[mCurrentFrame], 0);
  
  VkCommandBufferBeginInfo BeginInfo = {};
  BeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  CHECK_VK_ERR(vkBeginCommandBuffer(mCommandBuffers[mCurrentFrame], &BeginInfo), "Failed to begin command buffer");
  
  VkImageMemoryBarrier ImageBarrier = {};
  ImageBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
  ImageBarrier.srcAccessMask = 0;
  ImageBarrier.dstAccessMask = 0;
  ImageBarrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  ImageBarrier.newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
  ImageBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
  ImageBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
  ImageBarrier.image = mSwapchainImages[mImageIndex];
  ImageBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  ImageBarrier.subresourceRange.baseMipLevel = 0;
  ImageBarrier.subresourceRange.levelCount = 1;
  ImageBarrier.subresourceRange.baseArrayLayer = 0;
  ImageBarrier.subresourceRange.layerCount = 1;
  
  vkCmdPipelineBarrier(
    mCommandBuffers[mCurrentFrame],
    VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
    VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
    0, 
    0,
    NULL,
    0,
    NULL,
    1,
    &ImageBarrier
  );

  VkRenderingAttachmentInfo AttachmentInfo = {};
  AttachmentInfo.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
  AttachmentInfo.clearValue.color.float32[0] = 1.0f;
  AttachmentInfo.clearValue.color.float32[1] = 0.0f;
  AttachmentInfo.clearValue.color.float32[2] = 1.0f;
  AttachmentInfo.clearValue.color.float32[3] = 1.0f;
  AttachmentInfo.imageView = mSwapchainImageViews[mImageIndex];
  AttachmentInfo.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
  AttachmentInfo.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
  AttachmentInfo.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

  VkRenderingInfo RenderingInfo = {};
  RenderingInfo.sType = VK_STRUCTURE_TYPE_RENDERING_INFO;
  RenderingInfo.colorAttachmentCount = 1;
  RenderingInfo.pColorAttachments = &AttachmentInfo;
  RenderingInfo.viewMask = 0x01;
  RenderingInfo.layerCount = 1;
  RenderingInfo.renderArea.extent = { 0, 0 };
  RenderingInfo.renderArea.extent = mSwapchainExtent;
  vkCmdBeginRendering(mCommandBuffers[mCurrentFrame], &RenderingInfo);
}

void FVulkanRHI::EndDrawing() {
  vkCmdEndRendering(mCommandBuffers[mCurrentFrame]);

  VkImageMemoryBarrier ImageBarrier = {};
  ImageBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
  ImageBarrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
  ImageBarrier.dstAccessMask = 0;
  ImageBarrier.oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
  ImageBarrier.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
  ImageBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
  ImageBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
  ImageBarrier.image = mSwapchainImages[mImageIndex];
  ImageBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  ImageBarrier.subresourceRange.baseMipLevel = 0;
  ImageBarrier.subresourceRange.levelCount = 1;
  ImageBarrier.subresourceRange.baseArrayLayer = 0;
  ImageBarrier.subresourceRange.layerCount = 1;
  
  vkCmdPipelineBarrier(
    mCommandBuffers[mCurrentFrame], 
    VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, 
    VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, 
    0, 
    0, 
    NULL,
    0,
    NULL,
    1,
    &ImageBarrier
  );
  
  CHECK_VK_ERR(vkEndCommandBuffer(mCommandBuffers[mCurrentFrame]), "Failed to end command buffer");
  
  VkSubmitInfo SubmitInfo = {};
  SubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
  VkPipelineStageFlags WaitStages[] = {
    VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
  };
  SubmitInfo.waitSemaphoreCount = 1;
  SubmitInfo.pWaitSemaphores = &mImageAvailableSemaphores[mCurrentFrame];
  SubmitInfo.pWaitDstStageMask = WaitStages;
  SubmitInfo.signalSemaphoreCount = 1;
  SubmitInfo.pSignalSemaphores = &mRenderFinishedSemaphores[mCurrentFrame];
  SubmitInfo.commandBufferCount = 1;
  SubmitInfo.pCommandBuffers = &mCommandBuffers[mCurrentFrame];
  
  CHECK_VK_ERR(vkQueueSubmit(mGraphicsQueue, 1, &SubmitInfo, mInFlightFences[mCurrentFrame]), "Failed to submit to graphics queue");

  VkPresentInfoKHR PresentInfo = {};
  PresentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
  PresentInfo.swapchainCount = 1;
  PresentInfo.pSwapchains = &mSwapchain;
  PresentInfo.waitSemaphoreCount = 1;
  PresentInfo.pWaitSemaphores = &mRenderFinishedSemaphores[mCurrentFrame];
  PresentInfo.pImageIndices = &mImageIndex;
  VkResult Err = vkQueuePresentKHR(mPresentQueue, &PresentInfo);	
    
  if (Err == VK_ERROR_OUT_OF_DATE_KHR || Err == VK_SUBOPTIMAL_KHR || mFramebufferResized) {
    mFramebufferResized = false;
    RecreateSwapchain();
  }
  else if (Err != VK_SUCCESS) {
    SK_LOG_ERROR("Failed to present");
  }
  
  mCurrentFrame = (mCurrentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}


IRHICommandContext *FVulkanRHI::GetCommandContext() {
  return mCommandContext;
}