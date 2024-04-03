workspace "StakEngine"
	architecture "x86_64"
	startproject "StakEditor"

  PROJECT_DIR = path.getabsolute(".")

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

