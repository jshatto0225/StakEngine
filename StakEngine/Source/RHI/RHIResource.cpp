#include "RHIResource.h"

#include <unordered_map>

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
