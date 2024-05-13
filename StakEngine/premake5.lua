project "StakEngine"
    kind "StaticLib"
    language "C++"
    cppdialect "C++20"
    staticruntime "off"

    targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
    objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

    files
    {
        "**.h",
        "**.cpp",
    }

    defines 
    { 
        "PROJECT_DIR=\"" .. PROJECT_DIR .. "\"" 
    }

    links
    {
        "opengl32",
    }

    filter "system:windows"
        systemversion "latest"

        libdirs
        {
            "./External/Vulkan/Lib/"
        }

        links
        {
            "user32",
            "gdi32",
            "vulkan-1"
        }

        includedirs
        {
            "External/Vulkan/Include/"
        }

    filter "configurations:Debug"
        runtime "Debug"
        symbols "on"

    filter "configurations:Release"
        runtime "Release"
        optimize "on"
