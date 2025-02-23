#include "RHI.h"

#include "Asserts.h"
#include "RHICommandList.h"

#ifdef SK_VULKAN
#include "VulkanRHI.h"
using Platform_Rhi = Vulkan_Rhi;
#endif

Rhi::Rhi() {
    ASSERT(!instance);

    instance = this;
}

Rhi::~Rhi() {
    instance = NULL;
}

static Rhi *rhi;

void Rhi::init() {
    rhi = new Platform_Rhi();
}

void Rhi::shutdown() {
    ASSERT(rhi);
    delete rhi;
}


void Rhi::submit(Rhi_Command_List &command_list) {
    command_list.Execute();
}
