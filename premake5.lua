workspace "StakEngine"
    architecture "x64"
    startproject "TestApp"

    configurations
    {
        "Debug",
        "Release",
        "Dist"
    }

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

IncludeDir = {}
IncludeDir["GLFW"] = "StakEngine/external/GLFW/include"
IncludeDir["Glad"] = "StakEngine/external/Glad/include"
IncludeDir["ImGui"] = "StakEngine/external/imgui"
IncludeDir["glm"] = "StakEngine/external/glm"

include "StakEngine/external/GLFW"
include "StakEngine/external/Glad"
include "StakEngine/external/imgui"

project "StakEngine"
    location "StakEngine"
    kind "StaticLib"
    language "C++"
    cppdialect "C++17"
    staticruntime "on"

    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

    pchheader "stkpch.h"
    pchsource "StakEngine/src/stkpch.cpp"

    files
    {
        "%{prj.name}/src/**.h",
        "%{prj.name}/src/**.cpp",
        "%{prj.name}/external/glm/glm/**.hpp",
        "%{prj.name}/external/glm/glm/**.inl"
    }

    includedirs
    {
        "%{prj.name}/src",
        "%{prj.name}/external/spdlog/include",
        "{IncludeDir.GLFW}",
        "{IncludeDir.Glad}",
        "{IncludeDir.ImGui}",
        "{IncludeDir.glm}"
    }

    links
    {
        "GLFW",
        "Glad",
        "ImGui",
        "opengl32.lib"
    }

    filter "system:windows"
        systemversion "latest"

        defines
        {
            "STK_PLATFORM_WINDOWS",
            "GLFW_INCLUDE_NONE",
            "SKT_BUILD_DLL"
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

project "TestApp"
    location "TestApp"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++17"
    staticruntime "on"

    targetdir("bin/" .. outputdir .. "/%{prj.name}")
    objdir("bin-int/" .. outputdir .. "/%{prj.name}")

    files
    {
        "%{prj.name}/src/**.h",
        "%{prj.name}/src/**.cpp"
    }

    includedirs
    {
        "StakEngine/external/spdlog/include",
        "StakEngine/src",
        "StakEngine/external",
        "%{IncludeDir.glm}"
    }

    links
    {
        "StakEngine"
    }

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