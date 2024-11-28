#include "RHI.h"

#include "Asserts.h"
#include "RHICommandList.h"

#ifdef SK_VULKAN
#include "VulkanRHI.h"
using FPlatformRHI = FVulkanRHI;
#endif

FRHI::FRHI() {
  ASSERT(!sInstance);

  sInstance = this;
}

FRHI::~FRHI() {
  sInstance = NULL;
}

static FRHI *gRHI;

void FRHI::Init() {
  gRHI = new FPlatformRHI();
}

void FRHI::Shutdown() {
  ASSERT(gRHI);
  delete gRHI;
}


void FRHI::Submit(FRHICommandList &CommandList) {
  CommandList.Execute();
}
