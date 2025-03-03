#include "RHITexture.h"

FRHITexture::FRHITexture(TRef<IRHITexture> Texture) {
    Impl = Texture;
}

void FRHITexture::Shutdown() {
    Impl->Shutdown();
    Impl = nullptr;
}

bool FRHITexture::IsBackbuffer() {
    return std::static_pointer_cast<IRHITexture>(Impl)->IsBackbuffer();
}

FRHIRenderArea FRHITexture::GetRenderArea() const {
    return std::static_pointer_cast<IRHITexture>(Impl)->GetRenderArea();
}
