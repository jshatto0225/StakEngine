#include "RHI.h"

#ifdef SK_VULKAN
#include "VulkanRHI.h"
#endif

RHI rhi;

bool rhi_init(RHIBackend backend) {
    switch (backend) {
    case RHIBackend::Vulkan:
        rhi = vulkan_create_rhi();
        break;
    default:
        SK_LOG_ERROR("Unsupported RHI backend");
        return false;
    }

    return rhi.init();
}

void rhi_shutdown() {
    rhi.shutdown();
    rhi = {};
}

static std::unordered_map<RHIResourceState, std::unordered_map<RHIResourceState, RHITransitionType>> transition_types = {
    {
        RHIResourceState::Undefined,
        {
            { RHIResourceState::RenderTarget,   RHITransitionType::IMAGE   },
            { RHIResourceState::Present,         RHITransitionType::IMAGE   },
            { RHIResourceState::Undefined,       RHITransitionType::INVALID },
            { RHIResourceState::ShaderResource, RHITransitionType::IMAGE   },
        }
    },
    {
        RHIResourceState::RenderTarget,
        {
            { RHIResourceState::RenderTarget,   RHITransitionType::INVALID },
            { RHIResourceState::Present,         RHITransitionType::IMAGE   },
            { RHIResourceState::Undefined,       RHITransitionType::INVALID },
            { RHIResourceState::ShaderResource, RHITransitionType::IMAGE   },
        }
    },
    {
        RHIResourceState::Present,
        {
            { RHIResourceState::RenderTarget,   RHITransitionType::IMAGE   },
            { RHIResourceState::Present,         RHITransitionType::INVALID },
            { RHIResourceState::Undefined,       RHITransitionType::INVALID },
            { RHIResourceState::ShaderResource, RHITransitionType::IMAGE   },
        }
    },
    {
        RHIResourceState::ShaderResource,
        {
            { RHIResourceState::RenderTarget,   RHITransitionType::IMAGE   },
            { RHIResourceState::Present,         RHITransitionType::IMAGE   },
            { RHIResourceState::Undefined,       RHITransitionType::INVALID },
            { RHIResourceState::ShaderResource, RHITransitionType::INVALID },
        }
    },
};

RHITransitionType rhi_get_transition_type(RHIResourceState before, RHIResourceState after) {
    return transition_types[before][after];
}
