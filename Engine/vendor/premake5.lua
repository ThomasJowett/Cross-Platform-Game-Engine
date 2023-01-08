project "GLFW"
	kind "StaticLib"
	language "C"
	cppdialect "C++17"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"GLFW/include/GLFW/glfw3.h",
		"GLFW/include/GLFW/glfw3native.h",
		"GLFW/src/glfw_config.h",
		"GLFW/src/context.c",
		"GLFW/src/init.c",
		"GLFW/src/input.c",
		"GLFW/src/monitor.c",
		"GLFW/src/vulkan.c",
		"GLFW/src/window.c",
		"GLFW/src/platform.c",
		"GLFW/src/null_init.c",
		"GLFW/src/null_joystick.c",
		"GLFW/src/null_monitor.c",
		"GLFW/src/null_window.c"
	}

	filter "system:linux"
		pic "On"

		systemversion "latest"
		staticruntime "off"

		files
		{
			"GLFW/src/x11_init.c",
			"GLFW/src/x11_monitor.c",
			"GLFW/src/x11_window.c",
			"GLFW/src/xkb_unicode.c",
			"GLFW/src/posix_poll.c",
			"GLFW/src/posix_time.c",
			"GLFW/src/posix_thread.c",
			"GLFW/src/posix_module.c",
			"GLFW/src/glx_context.c",
			"GLFW/src/egl_context.c",
			"GLFW/src/osmesa_context.c",
			"GLFW/src/linux_joystick.c"
		}

		defines
		{
			"_GLFW_X11"
		}

	filter "system:windows"
		systemversion "latest"
		staticruntime "off"

		files
		{
			"GLFW/src/win32_init.c",
			"GLFW/src/win32_joystick.c",
			"GLFW/src/win32_monitor.c",
			"GLFW/src/win32_time.c",
			"GLFW/src/win32_thread.c",
			"GLFW/src/win32_window.c",
			"GLFW/src/win32_module.c",
			"GLFW/src/wgl_context.c",
			"GLFW/src/egl_context.c",
			"GLFW/src/osmesa_context.c"
		}

		defines 
		{ 
			"_GLFW_WIN32",
			"_CRT_SECURE_NO_WARNINGS"
		}

	filter "configurations:Debug"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		runtime "Release"
		optimize "on"

	filter "configurations:Distribution"
		defines "DIST"
		runtime "Release"
		optimize "On"
		
project "ImGui"
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	defines
	{
		"_CRT_SECURE_NO_WARNINGS"
	}

	files
	{
		"imgui/imconfig.h",
		"imgui/imgui.h",
		"imgui/imgui.cpp",
		"imgui/imgui_draw.cpp",
		"imgui/imgui_internal.h",
		"imgui/imgui_widgets.cpp",
		"imgui/imgui_tables.cpp",
		"imgui/imstb_rectpack.h",
		"imgui/imstb_textedit.h",
		"imgui/imstb_truetype.h",
		"imgui/imgui_demo.cpp"
	}

	filter "system:windows"
		systemversion "latest"
		cppdialect "C++17"
		staticruntime "off"

	filter "system:linux"
		pic "On"
		systemversion "latest"
		
		staticruntime "off"

	filter "configurations:Debug"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		runtime "Release"
		optimize "on"
	
	filter "configurations:Distribution"
		defines "DIST"
		runtime "Release"
		optimize "On"
		
project "GLAD"
	kind "StaticLib"
	language "C"
	cppdialect "C++17"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"GLAD/include/GLAD/glad.h",
		"GLAD/include/KHR/khrplatform.h",
		"GLAD/src/glad.c"
	}
	
	includedirs
	{
		"GLAD/include"
	}
	
	filter "system:linux"
		pic "On"

		systemversion "latest"
		staticruntime "off"

	filter "system:windows"
		systemversion "latest"
		staticruntime "off"

	filter "configurations:Debug"
		runtime "Debug"
		symbols "On"

	filter "configurations:Release"
		runtime "Release"
		optimize "On"

	filter "configurations:Distribution"
		defines "DIST"
		runtime "Release"
		optimize "On"

project "TinyXml2"
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")
	
	files
	{
		"TinyXml2/tinyxml2.h",
		"TinyXml2/tinyxml2.cpp"
	}

	systemversion "latest"
	staticruntime "off"

	filter "configurations:Debug"
		runtime "Debug"
		symbols "On"

	filter "configurations:Release"
		runtime "Release"
		optimize "On"

	filter "configurations:Distribution"
		defines "DIST"
		runtime "Release"
		optimize "On"

project "Box2D"
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"box2d/include/box2d/**.h",
		"box2d/src/**.h",
		"box2d/src/**.cpp"
	}

	includedirs
	{
		"box2d/include/"
	}

	filter "system:windows"
		systemversion "latest"

	filter "configurations:Debug"
		runtime "Debug"
		symbols "On"

	filter "configurations:Release"
		runtime "Release"
		optimize "On"

	filter "configurations:Distribution"
		defines "DIST"
		runtime "Release"
		optimize "On"

project "SPIRV-Cross"
	kind "StaticLib"
	language "C++"
	cppdialect "C++11"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	defines
	{
		"_CRT_SECURE_NO_WARNINGS"
	}

	files
	{
		"SPIRV-Cross/spirv.h",
		"SPIRV-Cross/spirv.hpp",
		"SPIRV-Cross/spirv_cfg.hpp",
		"SPIRV-Cross/spirv_cfg.cpp",
		"SPIRV-Cross/spirv_common.hpp",
		"SPIRV-Cross/spirv_cpp.hpp",
		"SPIRV-Cross/spirv_cpp.cpp",
		"SPIRV-Cross/spirv_cross.hpp",
		"SPIRV-Cross/spirv_cross.cpp",
		"SPIRV-Cross/spirv_cross_c.hpp",
		"SPIRV-Cross/spirv_cross_c.cpp",
		"SPIRV-Cross/spirv_cross_containers.hpp",
		"SPIRV-Cross/spirv_cross_error_handling.hpp",
		"SPIRV-Cross/spirv_cross_parsed_ir.hpp",
		"SPIRV-Cross/spirv_cross_parsed_ir.cpp",
		"SPIRV-Cross/spirv_cross_util.hpp",
		"SPIRV-Cross/spirv_cross_util.cpp",
		"SPIRV-Cross/spirv_glsl.cpp",
		"SPIRV-Cross/spirv_glsl.hpp",
		"SPIRV-Cross/spirv_hlsl.hpp",
		"SPIRV-Cross/spirv_hlsl.cpp",
		"SPIRV-Cross/spirv_msl.hpp",
		"SPIRV-Cross/spirv_msl.cpp",
		"SPIRV-Cross/spirv_parser.hpp",
		"SPIRV-Cross/spirv_parser.cpp",
		"SPIRV-Cross/spirv_reflect.hpp",
		"SPIRV-Cross/spirv_reflect.cpp"
	}

	filter "system:windows"
		systemversion "latest"

	filter "configurations:Debug"
		runtime "Debug"
		symbols "On"

	filter "configurations:Release"
		runtime "Release"
		optimize "On"

	filter "configurations:Distribution"
		defines "DIST"
		runtime "Release"
		optimize "On"

project "lua"
	kind "StaticLib"
	language "C++"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	defines
	{
		"LUA_COMPAT_MATHLIB=1"
	}

	files
	{
		"lua/*.h",
		"lua/*.c"
	}

	removefiles
	{
		"lua/luac.c",
		"lua/lua.c",
		"lua/onelua.c"
	}

	filter "system:windows"
		systemversion "latest"

	filter "system:linux"
		buildoptions
		{
			"-fPIC"
		}

	filter "configurations:Debug"
		runtime "Debug"
		symbols "On"
		defines "LUA_USE_APICHECK=1"

	filter "configurations:Release"
		runtime "Release"
		optimize "On"

	filter "configurations:Distribution"
		defines "DIST"
		runtime "Release"
		optimize "On"

include "msdf-atlas-gen"

group "Tests"
project "Box2D_Testbed"
	kind "ConsoleApp"
	cppdialect "C++17"

	language "C++"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	includedirs
	{
		"box2d/include",
		"box2d/testbed",
		"box2d/extern",
		"GLFW/include",
		"box2d/extern/imgui",
		"box2d/extern/glad/include"
	}

	files
	{
		"box2d/testbed/**.h",
		"box2d/testbed/**.cpp",
		"box2d/extern/imgui/**.cpp",
		"box2d/extern/glad/src/glad.c"
	}

	links
	{
		"GLFW",
		"Box2D"
	}
	
	filter "system:windows"
		systemversion "latest"
		links
		{
			"gdi32"
		}

	filter "system:linux"
		links
		{
			"dl",
			"pthread"
		}

	filter "configurations:Debug"
		runtime "Debug"
		symbols "On"

	filter "configurations:Release"
		runtime "Release"
		optimize "On"

	filter "configurations:Distribution"
		defines "DIST"
		runtime "Release"
		optimize "On"