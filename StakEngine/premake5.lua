project "StakEngine"
    kind "StaticLib"
    language "C++"
    cppdialect "C++20"
    staticruntime "off"

    targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
    objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

    files {
        "Source/**.h",
        "Source/**.cpp",
        "StakEngine.h"
    }

    includedirs {
        "Source",
        "Source/Core",
        "Source/Platform/GLFW",
        "Source/Platform/Vulkan",
        "Source/Renderer",
        "Source/RHI",
        "Source/ImGui",
    }

    externalincludedirs {
        "External/imgui",
        "External/spdlog/include",
        "External/glfw/include",
    }

    defines {    
        "SK_GLFW"
    }

    links {
        "ImGui",
        "GLFW",
    }

    filter "system:windows"
        systemversion "latest"
        defines {
            "SK_WINDOWS",
            "SK_VULKAN",
        }
        
        links { 
            "$(VULKAN_SDK)/lib/vulkan-1.lib",
        }
        
        includedirs {
            "$(VULKAN_SDK)/include"
        }

    filter "configurations:Debug"
        runtime "Debug"
        defines {
            "SK_DEBUG"
        }
        symbols "on"

    filter "configurations:Release"
        runtime "Release"
        optimize "on"

include "External/glfw.lua"
include "External/imgui.lua"