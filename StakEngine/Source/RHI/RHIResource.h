#pragma once

#include "Types.h"
#include "RHIForward.h"

enum class ERHIResourceState {
    UNDEFINED,
//    GENERAL,
    RENDER_TARGET,
//    DEPTH_STENCIL,
//    INDEX_BUFFER,
//    VERTEX_BUFFER,
//    UNORDERED_ACCESS,
//    SHADER_RESOURCE,
//    COPY_SOURCE,
//    COPY_DEST,
    PRESENT,
};

enum class ERHITransitionType {
    INVALID,
    IMAGE,
    BUFFER,
};

struct FRHITransitionBarrier {
    TRef<IRHIResource> Resource;
    ERHIResourceState StateBefore;
    ERHIResourceState StateAfter;
    FUInt32 Subresource;
};

struct FRHIUAVBarrier {
    TRef<IRHIResource> Resource;
};

struct FRHIAliasingBarrier {
    TRef<IRHIResource> ResourceBefore;
    TRef<IRHIResource> ResourceAfter;
};

enum class ERHIBarrierType {
    TRANSITION,
//    UAV,
//    ALIASING
};

enum class ERHIFormat {
    UNDEFINED,
    B8G8R8A8_SRGB,
};

struct FRHIResourceBarrier {
    ERHIBarrierType Type;

    union {
        FRHITransitionBarrier TransitionBarrier;
        FRHIUAVBarrier UAVBarrier;
        FRHIAliasingBarrier AliasingBarrier;
    };

    ~FRHIResourceBarrier() {}
};

enum class ERHIResourceType {
    TEXTURE,
    BUFFER,
};

struct FRHIRenderArea {
    FUInt32 LayerCount;
    FUInt32 X;
    FUInt32 Y;
    FUInt32 Width;
    FUInt32 Height;
};

class IRHIResource {
public:
    virtual ~IRHIResource() = default;

    virtual ERHIResourceType GetType() = 0;

    virtual void Shutdown() = 0;
};

ERHITransitionType RHIGetTransitionType(ERHIResourceState Before, ERHIResourceState After);
