project "StakEngine"
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"
	staticruntime "off"

	targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

	pchheader "stkpch.h"
	pchsource "src/stkpch.cpp"

	files
	{
		"src/**.h",
		"src/**.cpp",
		"external/stb_image/**.h",
		"external/stb_image/**.cpp",
		"external/glm/glm/**.hpp",
		"external/glm/glm/**.inl",

		"external/ImGuizmo/ImGuizmo.h",
		"external/ImGuizmo/ImGuizmo.cpp"
	}

	defines
	{
		"_CRT_SECURE_NO_WARNINGS",
		"GLFW_INCLUDE_NONE"
	}

	includedirs
	{
		"src",
		"vendor/spdlog/include",
		"%{IncludeDir.GLFW}",
		"%{IncludeDir.Glad}",
		"%{IncludeDir.ImGui}",
		"%{IncludeDir.glm}",
		"%{IncludeDir.stb_image}",
		"%{IncludeDir.ImGuizmo}",
	}

	links
	{
		"GLFW",
		"Glad",
		"ImGui",
		"opengl32.lib",
	}

	filter "files:vendor/ImGuizmo/**.cpp"
	flags { "NoPCH" }

	filter "system:windows"
		systemversion "latest"
        defines
        {
            "STK_PLATFORM_WINDOWS"
        }

	filter "configurations:Debug"
		defines "STK_DEBUG"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		defines "STK_RELEASE"
		runtime "Release"
		optimize "on"

	filter "configurations:Dist"
		defines "STK_DIST"
		runtime "Release"
		optimize "on"