workspace "Cross Platform Game Engine"
	architecture "x64"
	startproject "ExampleGame"

	configurations
	{
		"Debug",
		"Release",
		"Distribution"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}"

include "Engine/Vendor/GLFW"
include "Engine/Vendor/GLAD"
include "Engine/Vendor/imgui"

project "Engine"
	location "Engine"
	kind "SharedLib"
	language "C++"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	pchheader "stdafx.h"
	pchsource "Engine/src/stdafx.cpp"

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp"
	}

	includedirs
	{
		"%{prj.name}/src",
		"%{prj.name}/Vendor",
		"%{prj.name}/Vendor/GLFW/include",
		"%{prj.name}/Vendor/GLAD/include",
		"%{prj.name}/Vendor/imgui"
	}
	
	links
	{
		"GLFW",
		"GLAD",
		"ImGui",
		"opengl32.lib"
	}

	filter "system:windows"
		cppdialect "C++17"
		staticruntime "Off"
		systemversion "latest"

		defines
		{
			"__WINDOWS__",
			"BUILD_DLL",
			"GLFW_INCLUDE_NONE"
		}
		
		postbuildcommands
		{
			("{COPY} %{cfg.buildtarget.relpath} ../bin/" .. outputdir .. "/ExampleGame")
		}

	filter "configurations:Debug"
		defines 
		{
			"DEBUG",
			"ENABLE_ASSERTS"
		}
		
		symbols "On"

	filter "configurations:Release"
		defines "RELEASE"
		optimize "On"

	filter "configurations:Distribution"
		defines "DIST"
		optimize "On"

project "ExampleGame"
	location "ExampleGame"
	kind "ConsoleApp"

	language "C++"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp"
	}

	includedirs
	{
		"Engine/src"
	}
	
	links
	{
		"Engine"
	}

	filter "system:windows"
		cppdialect "C++17"
		staticruntime "On"
		systemversion "latest"

		defines
		{
			"__WINDOWS__"
		}

		postbuildcommands
		{
			("{COPY} ../bin/" .. outputdir .. "/Engine/Engine.dll ../bin/" .. outputdir .. "/%{prj.name}")
		}

	filter "configurations:Debug"
		defines "DEBUG"
		symbols "On"

	filter "configurations:Release"
		defines "RELEASE"
		optimize "On"

	filter "configurations:Distribution"
		defines "DIST"
		optimize "On"