workspace "stak_engine"
	architecture "x86_64"
	startproject "stak_editor"

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

include "stak_engine"

include "stak_editor"
