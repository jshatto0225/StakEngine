project "stak_engine"
    kind "StaticLib"
    language "C++"
    cppdialect "C++20"
    staticruntime "off"

    targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
    objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

    files {
        "src/*.hpp",
        "src/*.cpp",
    }

    includedirs {
        "src",
        "vendor/imgui",
        "vendor/spdlog/include",
    }

    links {
        "imgui"
    }

    filter "system:windows"
        systemversion "latest"
        files {
            "src/glfw/glfw.cpp",
            "src/vulkan/vulkan.cpp",
            "src/vulkan/vulkan.hpp"
        }
        defines {
            "SK_WINDOWS"
        }
        links {
            "$(VULKAN_SDK)/lib/vulkan-1.lib",
            "glfw"
        }
        includedirs {
            "vendor/glfw/include",
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

include "vendor/glfw.lua"
include "vendor/imgui.lua"
