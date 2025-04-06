#include "RHI.h"

#ifdef SK_VULKAN
#include "VulkanRHI.h"
#endif

Rhi rhi;

bool rhi_init(Rhi_Backend backend) {
    switch (backend) {
    case Rhi_Backend::VULKAN:
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

static std::unordered_map<Rhi_Resource_State, std::unordered_map<Rhi_Resource_State, Rhi_Transition_Type>> transition_types = {
    {
        Rhi_Resource_State::UNDEFINED,
        {
            { Rhi_Resource_State::RENDER_TARGET,   Rhi_Transition_Type::IMAGE   },
            { Rhi_Resource_State::PRESENT,         Rhi_Transition_Type::IMAGE   },
            { Rhi_Resource_State::UNDEFINED,       Rhi_Transition_Type::INVALID },
            { Rhi_Resource_State::SHADER_RESOURCE, Rhi_Transition_Type::IMAGE   },
        }
    },
    {
        Rhi_Resource_State::RENDER_TARGET,
        {
            { Rhi_Resource_State::RENDER_TARGET,   Rhi_Transition_Type::INVALID },
            { Rhi_Resource_State::PRESENT,         Rhi_Transition_Type::IMAGE   },
            { Rhi_Resource_State::UNDEFINED,       Rhi_Transition_Type::INVALID },
            { Rhi_Resource_State::SHADER_RESOURCE, Rhi_Transition_Type::IMAGE   },
        }
    },
    {
        Rhi_Resource_State::PRESENT,
        {
            { Rhi_Resource_State::RENDER_TARGET,   Rhi_Transition_Type::IMAGE   },
            { Rhi_Resource_State::PRESENT,         Rhi_Transition_Type::INVALID },
            { Rhi_Resource_State::UNDEFINED,       Rhi_Transition_Type::INVALID },
            { Rhi_Resource_State::SHADER_RESOURCE, Rhi_Transition_Type::IMAGE   },
        }
    },
    {
        Rhi_Resource_State::SHADER_RESOURCE,
        {
            { Rhi_Resource_State::RENDER_TARGET,   Rhi_Transition_Type::IMAGE   },
            { Rhi_Resource_State::PRESENT,         Rhi_Transition_Type::IMAGE   },
            { Rhi_Resource_State::UNDEFINED,       Rhi_Transition_Type::INVALID },
            { Rhi_Resource_State::SHADER_RESOURCE, Rhi_Transition_Type::INVALID },
        }
    },
};

Rhi_Transition_Type rhi_get_transition_type(Rhi_Resource_State before, Rhi_Resource_State after) {
    return transition_types[before][after];
}
