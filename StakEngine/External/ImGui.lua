project "ImGui"
	kind "StaticLib"
	language "C++"
	architecture "x64"

	targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

	files {
		"Imgui/*.cpp",
		"Imgui/*.h"
	}

	includedirs {
			"ImGui"
	}
    
	filter "system:linux"
		pic "On"

		systemversion "latest"
		staticruntime "On"

		files {
			"Imgui/backends/imgui_impl_vulkan.h",
			"Imgui/backends/imgui_impl_vulkan.cpp"
		}

		includedirs {
			"$(VULKAN_SDK)/include"
		}

	filter "system:windows"
		systemversion "latest"
		staticruntime "On"

		files {
			"Imgui/backends/imgui_impl_vulkan.h",
			"Imgui/backends/imgui_impl_vulkan.cpp"
		}

		includedirs {
			"$(VULKAN_SDK)/include"
		}

		defines {
			"_CRT_SECURE_NO_WARNINGS"
		}

	filter "configurations:Debug"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		runtime "Release"
		optimize "on"