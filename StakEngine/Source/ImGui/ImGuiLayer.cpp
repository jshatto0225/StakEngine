#include "ImguiLayer.h"

#include <imgui.h>
#include <backends/imgui_impl_vulkan.h>
#include <backends/imgui_impl_glfw.h>

#if defined(SK_WINDOWS) or defined(SK_LINUX)
#include "VulkanRendererAPI.h"
#endif

#include "Renderer.h"
#include "Log.h"
#include "RendererAPI.h"

namespace Stak {

ImGuiLayer::ImGuiLayer(Ref<Window> window) {
  ImGui::CreateContext();
  ImGuiIO &io = ImGui::GetIO();
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

  ImGui::StyleColorsDark();
#if defined(SK_WINDOWS) or defined(SK_LINUX)
  ImGui_ImplGlfw_InitForVulkan(static_cast<GLFWwindow *>(window->GetNativeHandle()), true);

  Ref<VulkanRendererAPI> vulkanRenderer = std::dynamic_pointer_cast<VulkanRendererAPI>(Renderer::GetAPI());

  ImGui_ImplVulkan_InitInfo initInfo = {};

  initInfo.Instance = vulkanRenderer->GetImGuiInstance();
  initInfo.PhysicalDevice = vulkanRenderer->GetImGuiPhysicalDevice();
  initInfo.Device = vulkanRenderer->GetImGuiDevice();
  initInfo.QueueFamily = vulkanRenderer->GetImGuiGraphicsQueueFamily();
  initInfo.Queue = vulkanRenderer->GetImGuiGraphicsQueue();
  initInfo.PipelineCache = vulkanRenderer->GetImGuiPipelineCache();
  initInfo.DescriptorPool = vulkanRenderer->GetImGuiDescriptorPool();
  initInfo.RenderPass = vulkanRenderer->GetImGuiRenderPass();
  initInfo.Subpass = 0;
  initInfo.MinImageCount = vulkanRenderer->GetImGuiMinImageCount();
  initInfo.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
  initInfo.Allocator = vulkanRenderer->GetImGuiAllocator();
  initInfo.CheckVkResultFn = [](VkResult err) {
    SK_LOG_ERROR("Vulkan ImGui Error: {0}", static_cast<u32>(err));
    };

  ImGui_ImplVulkan_Init(&initInfo);
#endif
}

ImGuiLayer::~ImGuiLayer() {

}

void ImGuiLayer::OnAttach() {

}

void ImGuiLayer::OnDetach() {

}

void ImGuiLayer::Update() {

}

void ImGuiLayer::OnEvent(Event &event) {

}

} // namespace Stak