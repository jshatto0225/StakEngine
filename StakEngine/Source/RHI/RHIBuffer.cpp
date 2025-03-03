#include "RHIBuffer.h"

FRHIBufferLayout::FRHIBufferLayout(const std::vector<FRHIBufferElement> &Elements) : Elements(Elements) {
    Stride = 0;
    for (auto &Element : Elements) {
        if (Element.Offset > Stride) {
            Stride = Element.Offset;
        }
        
        // TODO:
        switch (Element.Format) {
        case ERHIFormat::UNDEFINED:
            break;
        }
    }
}

FRHIBuffer::FRHIBuffer(TRef<IRHIBuffer> Buffer) {
    Impl = Buffer;
}

void FRHIBuffer::SetData(void *Data, FUInt32 DataSize) {
    std::static_pointer_cast<IRHIBuffer>(Impl)->SetData(Data, DataSize);
}

void *FRHIBuffer::GetMappedBuffer() {
    return std::static_pointer_cast<IRHIBuffer>(Impl)->GetMappedBuffer();
}

void FRHIBuffer::Shutdown() {
    Impl->Shutdown();
}

ERHIBufferType FRHIBuffer::GetBufferType() {
    return std::static_pointer_cast<IRHIBuffer>(Impl)->GetBufferType();
}

FUInt32 FRHIBuffer::GetElementCount() {
    return std::static_pointer_cast<IRHIBuffer>(Impl)->GetElementCount();
}
    
FRHIBufferLayout FRHIBuffer::GetLayout() {
    return std::static_pointer_cast<IRHIBuffer>(Impl)->GetLayout();
}
