#include "RHI.h"

#ifdef SK_VULKAN
#include "VulkanRHI.h"
#endif

FRHI GRHI;

bool RHIInit(ERHIBackend Backend) {
    switch (Backend) {
    case ERHIBackend::VULKAN:
        GRHI = VulkanCreateRHI();
        break;
    default:
        SK_LOG_ERROR("Unsupported RHI backend");
        return false;
    }

    return GRHI.Init();
}

void RHIShutdown() {
    GRHI.Shutdown();
    GRHI = {};
}

static std::unordered_map<ERHIResourceState, std::unordered_map<ERHIResourceState, ERHITransitionType>> TransitionTypes = {
    {
        ERHIResourceState::UNDEFINED,
        {
            { ERHIResourceState::RENDER_TARGET,   ERHITransitionType::IMAGE   },
            { ERHIResourceState::PRESENT,         ERHITransitionType::IMAGE   },
            { ERHIResourceState::UNDEFINED,       ERHITransitionType::INVALID },
            { ERHIResourceState::SHADER_RESOURCE, ERHITransitionType::IMAGE   },
        }
    },
    {
        ERHIResourceState::RENDER_TARGET,
        {
            { ERHIResourceState::RENDER_TARGET,   ERHITransitionType::INVALID },
            { ERHIResourceState::PRESENT,         ERHITransitionType::IMAGE   },
            { ERHIResourceState::UNDEFINED,       ERHITransitionType::INVALID },
            { ERHIResourceState::SHADER_RESOURCE, ERHITransitionType::IMAGE   },
        }
    },
    {
        ERHIResourceState::PRESENT,
        {
            { ERHIResourceState::RENDER_TARGET,   ERHITransitionType::IMAGE   },
            { ERHIResourceState::PRESENT,         ERHITransitionType::INVALID },
            { ERHIResourceState::UNDEFINED,       ERHITransitionType::INVALID },
            { ERHIResourceState::SHADER_RESOURCE, ERHITransitionType::IMAGE   },
        }
    },
    {
        ERHIResourceState::SHADER_RESOURCE,
        {
            { ERHIResourceState::RENDER_TARGET,   ERHITransitionType::IMAGE   },
            { ERHIResourceState::PRESENT,         ERHITransitionType::IMAGE   },
            { ERHIResourceState::UNDEFINED,       ERHITransitionType::INVALID },
            { ERHIResourceState::SHADER_RESOURCE, ERHITransitionType::INVALID },
        }
    },
};

ERHITransitionType RHIGetTransitionType(ERHIResourceState Before, ERHIResourceState After) {
    return TransitionTypes[Before][After];
}
