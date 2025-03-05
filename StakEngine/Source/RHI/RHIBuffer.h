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
            default:
                SK_LOG_ERROR("Unsupported buffer element format");
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
    void *InitialContents = {}; // NOTE: Optional
    FUInt32 InitialContentsSize = {}; // NOTE: Optional
    bool UseStagingBuffer = {};
};

class IRHIBuffer : public IRHIResource {
public:
    virtual ~IRHIBuffer() = default;
    
    virtual void SetData(void *Data, FUInt32 DataSize) = 0;
    virtual void *GetMappedBuffer() = 0;

    virtual FUInt32 GetElementCount() = 0;

    virtual FRHIBufferLayout GetLayout() = 0;
    
    void Shutdown() override = 0;
    
    inline ERHIResourceType GetType() override { return ERHIResourceType::BUFFER; }

    virtual ERHIBufferType GetBufferType() = 0;
};
