newoption
{
	trigger = "arch",
	value = "arch",
	description = "Choose system architecture",
	allowed = 
	{
		{"x86_64", "x64"},
		{"arm64", "arm64"}
	}
}

newoption
{
	trigger = "tests",
	description = "Add the test projects to the solution"
}

Arch = "x86_64"
if _OPTIONS["arch"] then
	Arch = _OPTIONS["arch"]
end

workspace "Cross Platform Game Engine"
	if Arch == "arm64" then
		architecture "arm64"
	else
		architecture "x86_64"
	end

	startproject "Editor"

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

group "Dependencies/Engine"
	include "Engine/vendor"
	if _OPTIONS["tests"] then
		require("Engine/vendor/tests")
	end
group "Dependencies/Editor"
	include "Editor/vendor"
	
group ""

project "Engine"
	location "%{prj.name}"
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	pchheader "stdafx.h"
	pchsource "%{prj.name}/src/stdafx.cpp"

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp",
		"%{prj.name}/vendor/stb/**.h",
		"%{prj.name}/vendor/stb/**.cpp",
		"%{prj.name}/vendor/simpleini/**.h",
		"%{prj.name}/vendor/EnTT/entt.hpp"
	}

	includedirs
	{
		"%{prj.name}/src",
		"%{prj.name}/vendor",
		"%{prj.name}/vendor/GLFW/include",
		"%{prj.name}/vendor/GLAD/include",
		"%{prj.name}/vendor/imgui",
		"%{prj.name}/vendor/stb",
		"%{prj.name}/vendor/spdlog/include",
		"%{prj.name}/vendor/simpleini",
		"%{prj.name}/vendor/EnTT",
		"%{prj.name}/vendor/cereal/include",
		"%{prj.name}/vendor/box2d/include",
		"%{prj.name}/vendor/lua",
		"%{prj.name}/vendor/msdf-atlas-gen/msdf-atlas-gen",
		"%{prj.name}/vendor/msdf-atlas-gen/msdfgen"
	}

	externalincludedirs
	{
		"%{prj.name}/vendor",
		"%{prj.name}/vendor/GLFW/include",
		"%{prj.name}/vendor/GLAD/include",
		"%{prj.name}/vendor/lua",
		"%{prj.name}/vendor/spdlog/include",
		"%{prj.name}/vendor/msdf-atlas-gen/msdf-atlas-gen",
		"%{prj.name}/vendor/msdf-atlas-gen/msdfgen",
		"%{prj.name}/vendor/box2d/include"
	}
	
	links
	{
		"GLFW",
		"GLAD",
		"ImGui",
		"TinyXml2",
		"Box2D",
		"SPIRV-Cross",
		"lua",
		"msdf-atlas-gen",
		"freetype"
	}
	
	defines
	{
		"GLFW_INCLUDE_NONE",
		"_CRT_SECURE_NO_WARNINGS"
	}

	filter "system:windows"
		staticruntime "Off"
		systemversion "latest"

		defines
		{
			"__WINDOWS__"
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

		buildoptions
		{
			"/bigobj"
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
			"%{prj.name}/src/Platform/Mac OS**.cpp",
			"%{prj.name}/src/ImGui/ImGuiBuildDirectX11.cpp"
		}
		
		defines
		{
			"__linux__"
		}
		
	filter "system:macosx"
		excludes
		{
			"%{prj.name}/src/Platform/DirectX**.h",
			"%{prj.name}/src/Platform/DirectX**.cpp",
			"%{prj.name}/src/ImGui/ImGuiBuildDirectX11.cpp"
		}

		buildoptions {"-F /Library/Frameworks", "-F ~/Library/Frameworks"}
		linkoptions {"-F /Library/Frameworks", "-F ~/Library/Frameworks"}
		
		linkoptions
		{
			"-framework OpenGL",
			"-framework Cocoa",
			"-framework IOKit"
		}

		links
		{
			"Cocoa.framework"
		}

		filter 'files:vendor/**.cpp'
			flags  { 'NoPCH' }
		filter 'files:vendor/**.c'
			flags  { 'NoPCH' }
		filter 'files:src/vendor/**.m'
			flags  { 'NoPCH' }
		filter 'files:src/vendor/**.mm'
			flags  { 'NoPCH' }

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
		
project "Editor"
	location "%{prj.name}"
	kind "ConsoleApp"
	cppdialect "C++17"

	language "C++"
	icon "%{prj.name}/Icon.ico"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp",
		"%{prj.name}/vendor/IconFont/**.h",
		"%{prj.name}/Icon.ico"
	}

	includedirs
	{
		"%{prj.name}/src",
		"%{prj.name}/vendor",
		"%{prj.name}/vendor/IconFont",
		"%{prj.name}/vendor/OpenFBX/src",
		"%{prj.name}/vendor/json",
		"%{prj.name}/vendor/tinygltf",
		"Engine/src",
		"Engine/vendor",
		"Engine/vendor/spdlog/include",
		"Engine/vendor/cereal/include",
		"Engine/vendor/box2d/include",
		"Engine/vendor/lua",
		"Engine/vendor/stb"
	}

	externalincludedirs
	{
		"Engine/vendor",
		"Engine/vendor/spdlog/include",
		"Engine/vendor/cereal/include",
		"Engine/vendor/box2d/include",
		"Engine/vendor/lua",
		"Engine/vendor/stb"
	}
	
	links
	{
		"Engine",
		"OpenFBX",
		"lua"
	}

	defines
	{
		"ENABLE_ASSERTS",
		"_CRT_SECURE_NO_WARNINGS"
	}

	postbuildcommands
	{
		"{COPY} data/ ../bin/" .. outputdir .. "/%{prj.name}/data",
		"{COPY} imgui.ini ../bin/" .. outputdir .. "/%{prj.name}"
	}

	dependson 
	{
		"Runtime"
	}

	filter "system:windows"
		staticruntime "Off"
		systemversion "latest"

		defines
		{
			"__WINDOWS__"
		}

		files
		{
			"%{prj.name}/resource.h",
			"%{prj.name}/Editor.rc"
		}
		
	filter "system:linux"
		links
		{
			"GLFW",
			"GLAD",
			"ImGui",
			"TinyXml2",
			"Box2D",
			"Xrandr",
			"Xi",
			"GL",
			"X11",
			"dl",
			"pthread",
			"stdc++fs",
			"msdf-atlas-gen",
			"msdfgen",
			"freetype"
		}
		
		defines
		{
			"__linux__"
		}

		excludes
		{
			"%{prj.name}/Editor.rc",
			"%{prj.name}/resource.h"
		}

	filter "system:macosx"
	linkoptions
		{
			"-framework OpenGL",
			"-framework Cocoa",
			"-framework IOKit"
		}
		
		links
		{
			"Cocoa.framework",
			"IOKit.framework"
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

project "Runtime"
	location "Runtime"
	kind "ConsoleApp"
	cppdialect "C++17"
	language "C++"

	targetdir("bin/" .. outputdir .. "/Editor/runtime")
	objdir("bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp"
	}

	includedirs
	{
		"{prj.name}/src",
		"Engine/src",
		"Engine/vendor",
		"Engine/vendor/spdlog/include",
		"Engine/vendor/cereal/include",
		"Engine/vendor/box2d/include",
		"Engine/vendor/lua"
	}

	externalincludedirs
	{
		"Engine/vendor",
		"Engine/vendor/spdlog/include",
		"Engine/vendor/cereal/include",
		"Engine/vendor/box2d/include",
		"Engine/vendor/lua",
		"Engine/vendor/stb"
	}

	links
	{
		"Engine",
		"lua"
	}

	filter "system:windows"
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
			"TinyXml2",
			"Box2D",
			"Xrandr",
			"Xi",
			"GL",
			"X11",
			"dl",
			"pthread",
			"stdc++fs",
			"msdf-atlas-gen",
			"msdfgen",
			"freetype"
		}
		
		defines
		{
			"__linux__"
		}

	filter "system:macosx"
		links
		{
			"Cocoa.framework",
			"IOKit.framework"
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

project "EditorWpf"
	location "%{prj.name}"
	kind "WindowedApp"
	language "C#"
	flags {"WPF"}
	namespace "Editor"
	icon "Editor/Icon.ico"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"%{prj.name}/**.cs",
		"%{prj.name}/**.xaml"
	}

	dependson
	{
		"EngineDLL"
	}

	links
	{
		"Microsoft.Csharp",
    	"PresentationCore",
    	"PresentationFramework",
    	"System",
    	"System.Core",
    	"System.Data",
    	"System.Data.DataSetExtensions",
    	"System.Net.Http",
    	"System.Xaml",
    	"System.Xml",
    	"System.Xml.Linq",
		"WindowsBase"
	}

project "EngineDLL"
	location "%{prj.name}"
	kind "SharedLib"
	language "C++"
	cppdialect "C++17"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	pchheader "pch.h"
	pchsource "%{prj.name}/src/pch.cpp"

	files
	{
		"%{prj.name}/src/**.cpp",
		"%{prj.name}/src/**.h"
	}

	links
	{
		"Engine",
		"lua"
	}

	includedirs
	{
		"Engine/src",
		"Engine/vendor",
		"Engine/vendor/spdlog/include",
		"Engine/vendor/cereal/include",
		"Engine/vendor/lua",
		"Engine/vendor/box2d/include"
	}

	postbuildcommands
	{
		("{COPY} %{cfg.buildtarget.relpath} \"../bin/" .. outputdir .. "/EditorWPF/\"")
	}
	
	filter "system:windows"
		defines
		{
			"ENGINEDLL_EXPORTS",
			"__WINDOWS__"
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