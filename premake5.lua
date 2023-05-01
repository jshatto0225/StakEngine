include "Dependencies.lua"

workspace "StakEngine"
	architecture "x86_64"
	startproject "TestApp"

	configurations
	{
		"Debug",
		"Release",
		"Dist"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

group "Dependencies"
	include "StakEngine/external/Box2D"
	include "StakEngine/external/GLFW"
	include "StakEngine/external/Glad"
	include "StakEngine/external/imgui"
group ""

group "Core"
	include "StakEngine"
group ""

group "Misc"
	include "TestApp"
group ""