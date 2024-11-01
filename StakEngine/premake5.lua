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
		"External/spdlog/include",
		"External/glfw/include",
		"$(VULKAN_SDK)/include"
	}

	filter "system:windows"
		systemversion "latest"
		defines {
			"SK_WINDOWS"
		}
		links { 
			"$(VULKAN_SDK)/lib/vulkan-1.lib",
			"GLFW"
		}

	filter "configurations:Debug"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		runtime "Release"
		optimize "on"

include "External/glfw.lua"