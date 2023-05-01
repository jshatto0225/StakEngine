project "TestApp"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	staticruntime "off"

	targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"src/**.h",
		"src/**.cpp"
	}

	includedirs
	{
		"%{wks.location}/StakEngine/external/spdlog/include",
		"%{wks.location}/StakEngine/src",
		"%{wks.location}/StakEngine/external",
		"%{IncludeDir.glm}"
	}

	links
	{
		"StakEngine"
	}

	filter "system:windows"
		systemversion "latest"
		defines "STK_PLATFORM_WINDOWS"

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