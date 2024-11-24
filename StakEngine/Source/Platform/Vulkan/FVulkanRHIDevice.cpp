#include "FVulkanRHIDevice.h"

#ifdef SK_GLFW
#include <GLFW/glfw3.h>
#include <backends/imgui_impl_glfw.h>
#include "FGLFWWindow.h"
#endif

#include <imgui.h>
#include <backends/imgui_impl_vulkan.h>

#include "FLog.h"
#include "FVulkanRHIInstance.h"
#include "FVulkanRHIContext.h"

FVulkanRHIDevice::FVulkanRHIDevice(TRef<FVulkanRHIInstance> Instance, TRef<IWindow> Window, FUInt32 MaxFramesInFlight) {
  mInstance = Instance;
  mWindow = Window;
  mMaxFramesInFlight = MaxFramesInFlight;

  // SURFACE
#ifdef SK_GLFW
  TRef<FGLFWWindow> glfwWindow = std::static_pointer_cast<FGLFWWindow>(Window);
  glfwCreateWindowSurface(mInstance->GetVkInstance(), glfwWindow->GetGlfwWindow(), NULL, &mSurface);
#endif
  
  // DEVICE
  FUInt32 DeviceCount = 0;
  vkEnumeratePhysicalDevices(mInstance->GetVkInstance(), &DeviceCount, NULL);
  std::vector<VkPhysicalDevice> Devices(DeviceCount);
  vkEnumeratePhysicalDevices(mInstance->GetVkInstance(), &DeviceCount, Devices.data());

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
      mPhysicalDevice = Devices[DeviceIndex];
      DeviceFound = true;
      break;
    }
  }

  if (!DeviceFound) {
    SK_LOG_ERROR("Failed to find suitable physical device");
  }

  VulkanQueueFamilies Indices = FindQueueFamilies(mPhysicalDevice);

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

  if (vkCreateDevice(mPhysicalDevice, &DeviceInfo, NULL, &mDevice) != VK_SUCCESS) {
    SK_LOG_ERROR("Failed to create vulkan device");
  }

  vkGetDeviceQueue(mDevice, Indices.GraphicsFamily.value(), 0, &mGraphicsQueue);
  vkGetDeviceQueue( mDevice, Indices.PresentFamily.value(), 0, &mPresentQueue);

  mGraphicsQueueFamily = Indices.GraphicsFamily.value();

  CreateSwapchain();
  CreateImageViews();
  CreateSyncObjects();

  vkAcquireNextImageKHR(mDevice, mSwapchain, UINT32_MAX, mImageAvailableSemaphores[mCurrentFrame], VK_NULL_HANDLE, &mSwapchainImageIndex);

  // ImGui Descriptor Pool
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
  PoolInfo.poolSizeCount = std::size(PoolSizes);
  PoolInfo.pPoolSizes = PoolSizes;

  if (vkCreateDescriptorPool(mDevice, &PoolInfo, NULL, &mImGuiPool) != VK_SUCCESS) {
    SK_LOG_ERROR("Failed to create imgu descriptor pool");
  }
}

void FVulkanRHIDevice::CreateSyncObjects() {
  VkSemaphoreCreateInfo SemaphoreInfo = {};
  SemaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

  VkFenceCreateInfo FenceInfo = {};
  FenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
  FenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

  mRenderFinishedSemaphores.resize(mMaxFramesInFlight);
  mImageAvailableSemaphores.resize(mMaxFramesInFlight);
  mFrameInFlightFences.resize(mMaxFramesInFlight);

  for (FUInt32 i = 0; i < mMaxFramesInFlight; i++) {
    VkResult r = vkCreateSemaphore(mDevice, &SemaphoreInfo, NULL, &mImageAvailableSemaphores[i]);
    if (r != VK_SUCCESS) {
      SK_LOG_ERROR("Failed to create semaphore");
    }

    r = vkCreateSemaphore(mDevice, &SemaphoreInfo, NULL, &mRenderFinishedSemaphores[i]);
    if (r != VK_SUCCESS) {
      SK_LOG_ERROR("Failed to create semaphore");
    }

    r = vkCreateFence(mDevice, &FenceInfo, NULL, &mFrameInFlightFences[i]);
    if (r != VK_SUCCESS) {
      SK_LOG_ERROR("Failed to create semaphore");
    }
  }
}

FVulkanRHIDevice::~FVulkanRHIDevice() {
  vkDeviceWaitIdle(mDevice);

  for (VkSemaphore Semaphore : mImageAvailableSemaphores) {
    vkDestroySemaphore(mDevice, Semaphore, NULL);
  }
  for (VkSemaphore Semaphore : mRenderFinishedSemaphores) {
    vkDestroySemaphore(mDevice, Semaphore, NULL);
  }
  for (VkFence Fence : mFrameInFlightFences) {
    vkDestroyFence(mDevice, Fence, NULL);
  }

  vkDestroyCommandPool(mDevice, mCommandPool, NULL);

  for (FUInt32 ImageViewIndex = 0; ImageViewIndex < mSwapchainImages.size(); ImageViewIndex++) {
    vkDestroyImageView(mDevice, mSwapchainImageViews[ImageViewIndex], NULL);
  }

  vkDestroySwapchainKHR(mDevice, mSwapchain, NULL);
  vkDestroySurfaceKHR(mInstance->GetVkInstance(), mSurface, NULL);
  vkDestroyDevice(mDevice, NULL);
}

void FVulkanRHIDevice::CreateSwapchain() {
  VulkanSwapchainSupport Support = GetSwapchainSupport(mPhysicalDevice);

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
    mExtent = Support.Capabilities.currentExtent;
  }
  else {
    auto [Width, Height] = mWindow->GetSize();

    VkExtent2D Extent = { 
      static_cast<FUInt32>(Width), 
      static_cast<FUInt32>(Height) 
    };

    Extent.width = std::clamp(Extent.width, Support.Capabilities.minImageExtent.width, Support.Capabilities.maxImageExtent.width);
    Extent.height = std::clamp(Extent.height, Support.Capabilities.minImageExtent.height, Support.Capabilities.maxImageExtent.height);

    mExtent = Extent;
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
  SwapchainInfo.imageExtent = mExtent;
  SwapchainInfo.imageArrayLayers = 1;
  SwapchainInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

  mSwapchainImageFormat = Format;

  VulkanQueueFamilies Indices = FindQueueFamilies(mPhysicalDevice);

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
  VkResult Err = vkCreateSwapchainKHR(mDevice, &SwapchainInfo, NULL, &mSwapchain);

  if (Err != VK_SUCCESS) {
    SK_LOG_ERROR("Failed to create swapchain\n");
    return;
  }

  mSwapchainImages.resize(ImageCount);
  vkGetSwapchainImagesKHR(mDevice, mSwapchain, &ImageCount, mSwapchainImages.data());
}

VkImageView FVulkanRHIDevice::CreateImageView(VkImage Image, VkFormat Format) {
  VkImageViewCreateInfo ViewInfo = { };
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
  VkResult r = vkCreateImageView(mDevice, &ViewInfo, NULL, &ImageView);
  if (r != VK_SUCCESS) {
    SK_LOG_ERROR("failed to create image view!\n");
  }

  return ImageView;
}

void FVulkanRHIDevice::CreateImageViews() {
  mSwapchainImageViews.resize(mSwapchainImages.size());

  for (FUInt32 ImageViewIndex = 0; ImageViewIndex < mSwapchainImages.size(); ImageViewIndex++) {
    mSwapchainImageViews[ImageViewIndex] =
      CreateImageView(mSwapchainImages[ImageViewIndex], mSwapchainImageFormat.format);
  }
}

FVulkanRHIDevice::VulkanQueueFamilies FVulkanRHIDevice::FindQueueFamilies(VkPhysicalDevice Device) {
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

FVulkanRHIDevice::VulkanSwapchainSupport FVulkanRHIDevice::GetSwapchainSupport(VkPhysicalDevice Device) {
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

void FVulkanRHIDevice::RecreateSwapchain() {
  auto [Width, Height] = mWindow->GetSize();

  while (Width == 0 || Height == 0) {
    FWindowSizeData data = mWindow->GetSize();
    Width = data.Width;
    Height = data.Height;
  }

  vkDeviceWaitIdle(mDevice);

  for (FUInt32 ImageIndex = 0; ImageIndex < mSwapchainImages.size(); ImageIndex++) {
    vkDestroyImageView(mDevice, mSwapchainImageViews[ImageIndex], NULL);
  }

  vkDestroySwapchainKHR(mDevice, mSwapchain, NULL);

  CreateSwapchain();
  CreateImageViews();
}

TRef<IRHIWorkRecipt> FVulkanRHIDevice::SubmitWork(TRef<IRHIContext> Context) {
  TRef<FVulkanRHIContext> VulkanContext = std::dynamic_pointer_cast<FVulkanRHIContext>(Context);

  VkSubmitInfo SubmitInfo = {};
  SubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
  SubmitInfo.commandBufferCount = 1;
  VkCommandBuffer Buffers[] = { VulkanContext->GetVkCommandBuffer() };
  SubmitInfo.pCommandBuffers = Buffers;

  if (Context->GetType() == ERHIContextType::GRAPHICS) {
    VkPipelineStageFlags WaitStages[] = {
          VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
    };
    SubmitInfo.pWaitDstStageMask = WaitStages;

    SubmitInfo.signalSemaphoreCount = 1;
    SubmitInfo.pSignalSemaphores = &mRenderFinishedSemaphores[mCurrentFrame];

    SubmitInfo.waitSemaphoreCount = 1;
    SubmitInfo.pWaitSemaphores = &mImageAvailableSemaphores[mCurrentFrame];

    // TODO: Check if fence was not ready and print error
    vkResetFences(mDevice, 1, &mFrameInFlightFences[mCurrentFrame]);

    vkQueueSubmit(mGraphicsQueue, 1, &SubmitInfo, mFrameInFlightFences[mCurrentFrame]);
  }
  else  {
    SK_LOG_ERROR("Unsupported context type");
  }

  return TCreateRef<FVulkanRHIWorkRecipt>(mFrameInFlightFences[mCurrentFrame]);
}

void FVulkanRHIDevice::WaitOnWork(TRef<IRHIWorkRecipt> Recipt) {
  TRef<FVulkanRHIWorkRecipt> VulkanRecipt = std::static_pointer_cast<FVulkanRHIWorkRecipt>(Recipt);

  VkFence Fences[] = { VulkanRecipt->GetFence() };

  vkWaitForFences(mDevice, 1, Fences, VK_TRUE, UINT32_MAX);
}

void FVulkanRHIDevice::Present() {
  VkPresentInfoKHR PresentInfo = {};
  PresentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
  PresentInfo.swapchainCount = 1;
  PresentInfo.pSwapchains = &mSwapchain;
  PresentInfo.pImageIndices = &mSwapchainImageIndex;
  PresentInfo.waitSemaphoreCount = 1;
  PresentInfo.pWaitSemaphores = &mRenderFinishedSemaphores[mCurrentFrame];

  vkQueuePresentKHR(mPresentQueue, &PresentInfo);

  mCurrentFrame = (mCurrentFrame + 1) % mMaxFramesInFlight;

  VkResult Err = vkAcquireNextImageKHR(
    mDevice,
    mSwapchain,
    UINT64_MAX,
    mImageAvailableSemaphores[mCurrentFrame],
    VK_NULL_HANDLE,
    &mSwapchainImageIndex
  );

  if (Err != VK_SUCCESS) {
    SK_LOG_ERROR("Failed to acquire next swapchin image");
  }
}

void FVulkanRHIDevice::FreeImGuiVkDescriptorPool() {
  vkDestroyDescriptorPool(mDevice, mImGuiPool, NULL);
}
