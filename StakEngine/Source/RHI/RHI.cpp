#include "RHI.h"

#include "Asserts.h"

#include "RHIDevice.h"

#ifdef SK_VULKAN
#include "VulkanRHI.h"
using FPlatformRHI = FVulkanRHI;
#endif

static IRHI *GRHI;

void RHIInit() {
    GRHI = new FPlatformRHI();
}

void RHIShutdown() {
    if (GRHI) {
        delete GRHI;
        GRHI = nullptr;
    }
}

TRef<IRHIDevice> RHICreateDevice(TRef<IWindow> Window) {
    assert(GRHI);
    return GRHI->CreateDevice(Window);
}

TRef<IRHIDevice> RHICreateDeviceImpl(TRef<IWindow> Window) {
    assert(GRHI);
    return GRHI->CreateDevice(Window);
}