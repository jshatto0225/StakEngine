project "StakEditor"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++20"
	staticruntime "off"

	targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

	files {
		"Source/**.h",
		"Source/**.cpp",
	}

	includedirs {
		"../StakEngine",
		"../StakEngine/External/spdlog/include",
		"../StakEngine/Source",
		"../StakEngine/Source/Core",
		"../StakEngine/Source/Platform/GLFW",
		"../StakEngine/Source/Platform/Vulkan",
		"../StakEngine/Source/Renderer",
		"../StakEngine/Source/ImGui",
		"../StakEngine/External/imgui",
		"../StakEngine/External/spdlog/include",
		"../StakEngine/External/glfw/include",
		"../StakEngine/$(VULKAN_SDK)/include"
	}

  links {
    "StakEngine"
  }

	filter "system:windows"
		systemversion "latest"

	filter "configurations:Debug"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		runtime "Release"
		optimize "on"
