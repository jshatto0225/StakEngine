#include "FVulkanRHIRenderPass.h"

#include "FLog.h"

#include "VulkanRHICore.h"
#include "IRHIResource.h"

FVulkanRHIRenderPass::FVulkanRHIRenderPass(TRef<FVulkanRHIDevice> Device, TRef<FVulkanRHIFramebuffer> InitialFramebuffer, const FRHIRenderPassDescription &Description) {
  mDescription = Description;
  mFramebuffer = InitialFramebuffer;
  mDevice = Device;

  std::vector<VkAttachmentDescription> VkAttachments;
  std::vector<VkAttachmentReference> VkAttachmentReferences;
  std::vector<VkSubpassDescription> VkSubpasses;
  std::vector<VkSubpassDependency> VkDependencies;

  for (FUInt32 AttachmentIndex = 0; AttachmentIndex < Description.Attachments.size(); AttachmentIndex++) {

    FRHIAttachmentReference AttachmentRef = Description.Attachments[AttachmentIndex];
    FRHIRenderPassAttachmentDescription *Attachment = AttachmentRef.Attachment;

    VkAttachmentDescription VkAttachment = {};
    VkAttachment.flags = 0; // TODO

    VkAttachment.format = VulkanRHIGetVkFormat(Attachment->Format);
    VkAttachment.samples = VulkanRHIGetVkSampleCount(Attachment->Samples);
    VkAttachment.loadOp = VulkanRHIGetVkLoadOp(Attachment->LoadOp);
    VkAttachment.storeOp = VulkanRHIGetVkStoreOp(Attachment->StoreOp);
    VkAttachment.stencilLoadOp = VulkanRHIGetVkLoadOp(Attachment->StencilLoadOp);
    VkAttachment.stencilStoreOp = VulkanRHIGetVkStoreOp(Attachment->StencilStoreOp);
    VkAttachment.initialLayout = VulkanRHIGetVkImageLayout(Attachment->InitialState);
    VkAttachment.finalLayout = VulkanRHIGetVkImageLayout(Attachment->FinalState);

    VkAttachments.push_back(VkAttachment);

    VkAttachmentReference VkAttachmentRef = {};
    VkAttachmentRef.attachment = AttachmentIndex;
    VkAttachmentRef.layout = VulkanRHIGetVkImageLayout(AttachmentRef.State);

    VkAttachmentReferences.push_back(VkAttachmentRef);
  }

  for (FUInt32 SubpassIndex = 0; SubpassIndex < Description.Subpasses.size(); SubpassIndex++) {
    FRHISubpassDescription Subpass = Description.Subpasses[SubpassIndex];

    VkSubpassDescription VkSubpass = {};
    VkSubpass.flags = 0; // TODO

    VkSubpass.pipelineBindPoint = VulkanRHIGetVkPipelineBindPoint(Subpass.PipelineBindPoint);

    std::vector<VkAttachmentReference> ColorAttachmentRefs;
    std::vector<VkAttachmentReference> InputAttachmentRefs;
    std::vector<VkAttachmentReference> ResolveAttachmentRefs;
    std::vector<VkAttachmentReference> PreserveAttachmentRefs;

    for (FUInt32 AttachmentIndex : Subpass.InputAttachmentIndices) {
      InputAttachmentRefs.push_back(VkAttachmentReferences[AttachmentIndex]);
    }

    for (FUInt32 AttachmentIndex : Subpass.ColorAttachmentIndices) {
      ColorAttachmentRefs.push_back(VkAttachmentReferences[AttachmentIndex]);
    }

    for (FUInt32 AttachmentIndex : Subpass.ResolveAttachmentIndices) {
      ResolveAttachmentRefs.push_back(VkAttachmentReferences[AttachmentIndex]);
    }

    VkSubpass.inputAttachmentCount = InputAttachmentRefs.size();
    VkSubpass.pColorAttachments = InputAttachmentRefs.data();

    VkSubpass.colorAttachmentCount = ColorAttachmentRefs.size();
    VkSubpass.pColorAttachments = ColorAttachmentRefs.data();

    VkSubpass.pColorAttachments = ResolveAttachmentRefs.data();

    VkSubpass.preserveAttachmentCount = PreserveAttachmentRefs.size();
    VkSubpass.pColorAttachments = PreserveAttachmentRefs.data();

    VkSubpass.pDepthStencilAttachment = &VkAttachmentReferences[Subpass.DepthStencilAttachmentIndex];

    VkSubpasses.push_back(VkSubpass);
  }

  for (FUInt32 DependencyIndex = 0; DependencyIndex < Description.Dependencies.size(); DependencyIndex++) {
    FRHISubpassDependency Dependency = Description.Dependencies[DependencyIndex];

    VkSubpassDependency VkDependency = {};
    VkDependency.srcSubpass = Dependency.SourceSubpass;
    VkDependency.dstSubpass = Dependency.DestinationSubpass;
    VkDependency.srcStageMask = VulkanRHIGetVkPipelineStage(Dependency.SourceStage);
    VkDependency.dstStageMask = VulkanRHIGetVkPipelineStage(Dependency.DestinationStage);
    VkDependency.srcAccessMask = VulkanRHIGetVkAccessMask(Dependency.SourceAccess);
    VkDependency.dstAccessMask = VulkanRHIGetVkAccessMask(Dependency.DestinationAccess);
    VkDependency.dependencyFlags = 0; // TODO

    VkDependencies.push_back(VkDependency);
  }

  VkRenderPassCreateInfo RenderPassInfo = {};
  RenderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
  RenderPassInfo.attachmentCount = VkAttachments.size();
  RenderPassInfo.pAttachments = VkAttachments.data();
  RenderPassInfo.subpassCount = VkSubpasses.size();
  RenderPassInfo.pSubpasses = VkSubpasses.data();
  RenderPassInfo.dependencyCount = VkDependencies.size();
  RenderPassInfo.pDependencies = VkDependencies.data();

  VkResult Err = vkCreateRenderPass(mDevice->GetVkDevice(), &RenderPassInfo, NULL, &mRenderPass);

  if (Err != VK_SUCCESS) {
    SK_LOG_ERROR("Failed to create render pass");
  }
}

FVulkanRHIRenderPass::~FVulkanRHIRenderPass() {
  vkDestroyRenderPass(mDevice->GetVkDevice(), mRenderPass, NULL);
}

void FVulkanRHIRenderPass::SetFramebuffer(TRef<IRHIFramebuffer> Framebuffer) {
  mFramebuffer = std::static_pointer_cast<FVulkanRHIFramebuffer>(Framebuffer);
}
