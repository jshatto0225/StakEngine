#include "FVulkanRHIDevice.h"

#ifdef SK_GLFW
#include <GLFW/glfw3.h>
#include "FGLFWWindow.h"
#endif

#include "FLog.h"
#include "FVulkanRHIInstance.h"

FVulkanRHIDevice::FVulkanRHIDevice(TRef<FVulkanRHIInstance> Instance, TRef<IWindow> Window) {
  mInstance = Instance;
  mWindow = Window;

  // SURFACE
#ifdef SK_GLFW
  TRef<FGLFWWindow> glfwWindow = std::static_pointer_cast<FGLFWWindow>(Window);
  glfwCreateWindowSurface(mInstance->GetInstance(), glfwWindow->mNativeHandle, NULL, &mSurface);
#endif
  
  // DEVICE
  FUInt32 DeviceCount = 0;
  vkEnumeratePhysicalDevices(mInstance->GetInstance(), &DeviceCount, NULL);
  std::vector<VkPhysicalDevice> Devices(DeviceCount);
  vkEnumeratePhysicalDevices(mInstance->GetInstance(), &DeviceCount, Devices.data());

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

  std::vector<VkDeviceQueueCreateInfo> QueueInfos;
  std::set<FUInt32> UniqueFamilies = {
    Indices.GraphicsFamily.value(),
    Indices.PresentFamily.value()
  };
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
}

FVulkanRHIDevice::~FVulkanRHIDevice() {
  vkDeviceWaitIdle(mDevice);

  vkDestroyCommandPool(mDevice, mCommandPool, NULL);

  for (FUInt32 ImageViewIndex = 0; ImageViewIndex < mSwapchainImages.size(); ImageViewIndex++) {
    vkDestroyImageView(mDevice, mSwapchainImageViews[ImageViewIndex], NULL);
  }

  vkDestroySwapchainKHR(mDevice, mSwapchain, NULL);
  vkDestroySurfaceKHR(mInstance->GetInstance(), mSurface, NULL);
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

    VkExtent2D Extent = { static_cast<FUInt32>(Width), static_cast<FUInt32>(Height) };

    Extent.width = std::clamp(Extent.width, Support.Capabilities.minImageExtent.width, Support.Capabilities.maxImageExtent.width);
    Extent.height = std::clamp(Extent.height, Support.Capabilities.minImageExtent.height, Support.Capabilities.maxImageExtent.height);

    mExtent = Extent;
  }

  FUInt32 ImageCount = Support.Capabilities.maxImageCount + 1;

  if (
    Support.Capabilities.maxImageCount > 0 &&
    ImageCount > Support.Capabilities.maxImageCount
  ) {
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
