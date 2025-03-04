#include "RHIDescriptorSetLayout.h"

FRHIDescriptorSetLayout::FRHIDescriptorSetLayout(TRef<IRHIDescriptorSetLayout> Layout) {
    Impl = Layout;
}

TRef<IRHIDescriptorSetLayout> FRHIDescriptorSetLayout::GetImpl() const {
    return Impl;
}

void FRHIDescriptorSetLayout::Shutdown() {
    Impl->Shutdown();
}
