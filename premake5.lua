workspace "StakEngine"
	architecture "x86_64"
	startproject "StakEditor"

	configurations
	{
		"Debug",
		"Release",
	}

	flags
	{
		"MultiProcessorCompile"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

include "StakEngine"

include "StakEditor"
