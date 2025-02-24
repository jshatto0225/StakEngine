project "stak_editor"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++20"
	staticruntime "off"

	targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

	files {
		"src/**.h",
		"src/**.cpp",
	}

	includedirs {
		"src/",
		"../stak_engine/vendor/spdlog/include",
		"../stak_engine/src",
		"../stak_engine/vendor/imgui",
		"../stak_engine/vendor/spdlog/include",
		"../stak_engine/vendor/glfw/include"
	}

  links {
    "stak_engine"
  }

	filter "system:windows"
		systemversion "latest"
		includedirs {
			"$(VULKAN_SDK)/include"
		}

	filter "configurations:Debug"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		runtime "Release"
		optimize "on"
