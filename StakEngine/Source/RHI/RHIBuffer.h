#pragma once

#include "RHIResource.h"
#include <vector>

#include "Log.h"

struct FRHIBufferElement {
    FUInt32 Location = 0;
    FUInt32 Binding = 0;
    ERHIFormat Format = ERHIFormat::UNDEFINED;
    FUInt32 Offset = 0;
};

struct FRHIBufferLayout {
    FRHIBufferLayout() = default;

    FRHIBufferLayout(const std::vector<FRHIBufferElement> &Elements) {
        for (auto &Element : Elements) {
            if (Element.Offset > Stride) {
                Stride = Element.Offset;
            }

            switch (Element.Format) {
            case ERHIFormat::B8G8R8A8_SRGB:
                Stride += 4;
                break;
            default:
                SK_LOG_ERROR("Unsupported buffer element format");
                break;
            }
        }
    }

    std::vector<FRHIBufferElement> Elements;
    FUInt32 Stride = 0;
};

enum class ERHIBufferType {
    VERTEX,
    INDEX
};

struct FRHIBufferDescription {
    ERHIBufferType Type = {};
    FRHIBufferLayout Layout;
    FUInt32 ElementCount = {};
    void *InitialContents = nullptr; // NOTE: Optional
    FUInt32 InitialContentsSize = 0; // NOTE: Optional
    bool UseStagingBuffer = false;
};

struct IRHIBuffer : public IRHIResource {
    virtual bool Init(FRHIBufferDescription *Description) = 0;

    void Shutdown() override = 0;
    
    virtual bool SendToGPU() = 0;

    FUInt32 ElementCount = 0;
    FRHIBufferLayout Layout;
    ERHIBufferType BufferType = ERHIBufferType::VERTEX;
    void *MappedData = nullptr;
    FUInt32 Size = 0;
};
