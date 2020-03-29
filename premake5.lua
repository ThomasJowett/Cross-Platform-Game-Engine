workspace "Cross Platform Game Engine"
	architecture "x86_64"
	startproject "ExampleGame"

	configurations
	{
		"Debug",
		"Release",
		"Distribution"
	}
	
	flags		
	{		
		"MultiProcessorCompile"		
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}"

group "Dependencies"
	include "Engine/vendor/GLFW"
	include "Engine/vendor/GLAD"
	include "Engine/vendor/imgui"
	
group ""

project "Engine"
	location "Engine"
	kind "StaticLib"
	language "C++"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	pchheader "stdafx.h"
	pchsource "Engine/src/stdafx.cpp"

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp",
		"%{prj.name}/vendor/stb/**.h",
		"%{prj.name}/vendor/stb/**.cpp"
	}

	includedirs
	{
		"%{prj.name}/src",
		"%{prj.name}/vendor",
		"%{prj.name}/vendor/GLFW/include",
		"%{prj.name}/vendor/GLAD/include",
		"%{prj.name}/vendor/imgui",
		"%{prj.name}/vendor/stb",
		"%{prj.name}/vendor/spdlog/include"
	}
	
	links
	{
		"GLFW",
		"GLAD",
		"ImGui"
	}

	filter "system:windows"
		cppdialect "C++17"
		staticruntime "Off"
		systemversion "latest"

		defines
		{
			"__WINDOWS__",
			"GLFW_INCLUDE_NONE"
		}
		
		links
		{
			"opengl32.lib"
		}
		
		excludes
		{
			"%{prj.name}/src/Platform/Android**.h",
			"%{prj.name}/src/Platform/Android**.cpp",
			"%{prj.name}/src/Platform/iOS**.h",
			"%{prj.name}/src/Platform/iOS**.cpp",
			"%{prj.name}/src/Platform/Linux**.h",
			"%{prj.name}/src/Platform/Linux**.cpp",
			"%{prj.name}/src/Platform/Mac OS**.h",
			"%{prj.name}/src/Platform/Mac OS**.cpp"
		}
	
	filter "system:linux"
		pic "on"
		cppdialect "C++17"
		staticruntime "Off"
		systemversion "latest"
		
		links
		{
			"xrandr",
			"xi",
			"GL",
			"X11"
		}
		
		excludes
		{
			"%{prj.name}/src/Platform/Android**.h",
			"%{prj.name}/src/Platform/Android**.cpp",
			"%{prj.name}/src/Platform/DirectX**.h",
			"%{prj.name}/src/Platform/DirectX**.cpp",
			"%{prj.name}/src/Platform/iOS**.h",
			"%{prj.name}/src/Platform/iOS**.cpp",
			"%{prj.name}/src/Platform/Mac OS**.h",
			"%{prj.name}/src/Platform/Mac OS**.cpp"
		}
		
		defines
		{
			"__linux__"
		}
		

	filter "configurations:Debug"
		defines 
		{
			"DEBUG",
			"ENABLE_ASSERTS"
		}
		
		runtime "Debug"
		symbols "On"

	filter "configurations:Release"
		defines "RELEASE"
		runtime "Release"
		optimize "On"
		symbols "Off"

	filter "configurations:Distribution"
		defines "DIST"
		runtime "Release"
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
		"ExampleGame/src",
		"Engine/src",
		"Engine/vendor",
		"Engine/vendor/spdlog/include"
	}
	
	links
	{
		"Engine"
	}

	filter "system:windows"
		cppdialect "C++17"
		staticruntime "Off"
		systemversion "latest"

		defines
		{
			"__WINDOWS__"
		}
		
	filter "system:linux"
		links
		{
			"GLFW",
			"GLAD",
			"ImGui",
			"Xrandr",
			"Xi",
			"GL",
			"X11",
			"dl",
			"pthread",
			"stdc++fs"
		}
		
		defines
		{
			"__linux__"
		}

	filter "configurations:Debug"
		defines "DEBUG"
		runtime "Debug"
		symbols "On"

	filter "configurations:Release"
		defines "RELEASE"
		runtime "Release"
		optimize "On"
		symbols "Off"

	filter "configurations:Distribution"
		defines "DIST"
		runtime "Release"
		optimize "On"