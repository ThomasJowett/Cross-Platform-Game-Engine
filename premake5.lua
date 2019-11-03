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

project "Engine"
	location "Engine"
	kind "SharedLib"
	language "C++"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	pchheader "stdafx.h"
	pchsource "stdafx.cpp"

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp"
	}

	includedirs
	{
		"Vendor/math"
	}

	filter "system:windows"
		cppdialect "C++17"
		staticruntime "On"
		systemversion "latest"

		defines
		{
			"JPT_PLATFORM_WINDOWS",
			"JPT_BUILD_DLL"
		}

		postbuildcommands
		{
			("{COPY} %{cfg.buildtarget.relpath} ../bin/" .. outputdir .. "/ExampleGame")
		}

	filter "configurations:Debug"
		defines "JPT_DEBUG"
		symbols "On"

	filter "configurations:Release"
		defines "JPT_RELEASE"
		optimize "On"

	filter "configurations:Distribution"
		defines "JPT_DIST"
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
			"JPT_PLATFORM_WINDOWS"
		}

	filter "configurations:Debug"
		defines "JPT_DEBUG"
		symbols "On"

	filter "configurations:Release"
		defines "JPT_RELEASE"
		optimize "On"

	filter "configurations:Distribution"
		defines "JPT_DIST"
		optimize "On"