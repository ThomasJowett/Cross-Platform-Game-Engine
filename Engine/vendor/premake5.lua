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
		"GLFW/src/context.c",
		"GLFW/src/init.c",
		"GLFW/src/input.c",
		"GLFW/src/monitor.c",
		"GLFW/src/vulkan.c",
		"GLFW/src/window.c",
		"GLFW/src/platform.h",
		"GLFW/src/platform.c",
		"GLFW/src/mappings.h",
		"GLFW/src/osmesa_context.c",
		"GLFW/src/null_platform.h",
		"GLFW/src/null_init.c",
		"GLFW/src/null_joystick.h",
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
			"GLFW/src/linux_joystick.h",
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
			"GLFW/src/egl_context.c"
		}

		defines 
		{ 
			"_GLFW_WIN32",
			"_CRT_SECURE_NO_WARNINGS"
		}

	filter "system:macosx"
		defines
		{
			"_GLFW_COCOA",
			"_GLFW_USE_RETINA"
		}

		files 
		{
			"GLFW/src/cocoa_platform.h",
			"GLFW/src/cocoa_joystick.h",
			"GLFW/src/nsgl_context.m",
			"GLFW/src/cocoa_init.m",
			"GLFW/src/cocoa_joystick.m",
			"GLFW/src/cocoa_monitor.m",
			"GLFW/src/cocoa_window.m",
			"GLFW/src/cocoa_time.h",
			"GLFW/src/cocoa_time.c",
			"GLFW/src/nsgl_context.m",
			"GLFW/src/posix_thread.h",
			"GLFW/src/posix_thread.c",
			"GLFW/src/posix_module.c",
			"GLFW/src/glx_context.c",
			"GLFW/src/egl_context.c"
		}

		links
		{
			"Cocoa.framework",
        	"IOKit.framework",
			"CoreFoundation.framework",
			"CoreVideo.framework"
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

	externalincludedirs
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

project "freetype"
	kind "StaticLib"
	language "C"
    staticruntime "off"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"freetype/include/ft2build.h",
		"freetype/include/freetype/*.h",
		"freetype/include/freetype/config/*.h",
		"freetype/include/freetype/internal/*.h",

		"freetype/src/autofit/autofit.c",
		"freetype/src/base/ftbase.c",
		"freetype/src/base/ftbbox.c",
		"freetype/src/base/ftbdf.c",
		"freetype/src/base/ftbitmap.c",
		"freetype/src/base/ftcid.c",
		"freetype/src/base/ftdebug.c",
		"freetype/src/base/ftfstype.c",
		"freetype/src/base/ftgasp.c",
		"freetype/src/base/ftglyph.c",
		"freetype/src/base/ftgxval.c",
		"freetype/src/base/ftinit.c",
		"freetype/src/base/ftmm.c",
		"freetype/src/base/ftotval.c",
		"freetype/src/base/ftpatent.c",
		"freetype/src/base/ftpfr.c",
		"freetype/src/base/ftstroke.c",
		"freetype/src/base/ftsynth.c",
		"freetype/src/base/ftsystem.c",
		"freetype/src/base/fttype1.c",
		"freetype/src/base/ftwinfnt.c",
		"freetype/src/bdf/bdf.c",
		"freetype/src/bzip2/ftbzip2.c",
		"freetype/src/cache/ftcache.c",
		"freetype/src/cff/cff.c",
		"freetype/src/cid/type1cid.c",
		"freetype/src/gzip/ftgzip.c",
		"freetype/src/lzw/ftlzw.c",
		"freetype/src/pcf/pcf.c",
		"freetype/src/pfr/pfr.c",
		"freetype/src/psaux/psaux.c",
		"freetype/src/pshinter/pshinter.c",
		"freetype/src/psnames/psnames.c",
		"freetype/src/raster/raster.c",
		"freetype/src/sdf/sdf.c",
		"freetype/src/sfnt/sfnt.c",
		"freetype/src/smooth/smooth.c",
		"freetype/src/truetype/truetype.c",
		"freetype/src/type1/type1.c",
		"freetype/src/type42/type42.c",
		"freetype/src/winfonts/winfnt.c"
	}

	includedirs
	{
		"freetype/include"
	}

	defines
	{
		"FT2_BUILD_LIBRARY",
		"_CRT_SECURE_NO_WARNINGS",
		"_CRT_NONSTDC_NO_WARNINGS",
	}

	filter "system:windows"
		systemversion "latest"

	filter "configurations:Debug"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		runtime "Release"
		optimize "on"

	filter "configurations:Dist"
		runtime "Release"
		optimize "on"
		symbols "off"

project "msdfgen"
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"
	staticruntime "off"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"msdf-atlas-gen/msdfgen/core/**.h",
		"msdf-atlas-gen/msdfgen/core/**.hpp",
		"msdf-atlas-gen/msdfgen/core/**.cpp",
		"msdf-atlas-gen/msdfgen/ext/**.h",
		"msdf-atlas-gen/msdfgen/ext/**.hpp",
		"msdf-atlas-gen/msdfgen/ext/**.cpp",
		"msdf-atlas-gen/msdfgen/lib/**.cpp",
		"msdf-atlas-gen/msdfgen/include/**.h"
	}

	includedirs
	{
		"msdf-atlas-gen/msdfgen/include"
	}

	externalincludedirs
	{
		"freetype/include"
	}

	defines
	{
		"MSDFGEN_USE_CPP11"
	}

	links
	{
		"freetype"
	}

	defines
	{
	}

	filter "system:windows"
		systemversion "latest"

	filter "configurations:Debug"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		runtime "Release"
		optimize "on"

	filter "configurations:Dist"
		runtime "Release"
		optimize "on"
		symbols "off"

project "msdf-atlas-gen"
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"
	staticruntime "off"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"msdf-atlas-gen/msdf-atlas-gen/**.h",
		"msdf-atlas-gen/msdf-atlas-gen/**.hpp",
		"msdf-atlas-gen/msdf-atlas-gen/**.cpp"
	}

	includedirs
	{
		"msdf-atlas-gen/msdf-atlas-gen",
		"msdf-atlas-gen/msdfgen",
		"msdf-atlas-gen/msdfgen/include",
		"msdf-atlas-gen/artery-font-format"
	}

	defines
	{
		"_CRT_SECURE_NO_WARNINGS"
	}

	links
	{
		"msdfgen"
	}

	filter "system:windows"
		systemversion "latest"

	filter "configurations:Debug"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		runtime "Release"
		optimize "on"

	filter "configurations:Dist"
		runtime "Release"
		optimize "on"
		symbols "off"