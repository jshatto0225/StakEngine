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
		"External/spdlog/include",
		"External/glfw/include"
	}

	links { "GLFW" }

	filter "system:windows"
		systemversion "latest"
		defines {
			"SK_WINDOWS"
		}

	filter "configurations:Debug"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		runtime "Release"
		optimize "on"

include "External/glfw.lua"