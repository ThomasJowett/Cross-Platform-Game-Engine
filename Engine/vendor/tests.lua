group "Tests"
project "Box2D_Testbed"
kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	includedirs
	{
		"box2d/include",
		"box2d/testbed"
	}

	externalincludedirs
	{
		"GLFW/include",
		"box2d/extern",
		"box2d/extern/imgui",
		"box2d/extern/glad/include"
	}

	files
	{
		"box2d/testbed/**.h",
		"box2d/testbed/**.cpp",
		"box2d/extern/imgui/**.h",
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

	filter "system:macosx"
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

group "Tests/GLFW"
project "Boing Example"
	kind "ConsoleApp"
	language "C"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

		externalincludedirs
	{
		"GLFW/deps",
		"GLFW/include"
	}

	files
	{
		"GLFW/examples/boing.c"
	}

	links
	{
		"GLFW"
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
			"Cocoa.framework"
		}

project "Gears Example"
	kind "ConsoleApp"
	language "C"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")
	externalincludedirs
	{
		"GLFW/deps",
		"GLFW/include"
	}

	files
	{
		"GLFW/examples/gears.c"
	}

	links
	{
		"GLFW"
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
			"Cocoa.framework"
		}

project "Heightmap Example"
	kind "ConsoleApp"
	language "C"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")
	externalincludedirs
	{
		"GLFW/deps",
		"GLFW/include"
	}

	files
	{
		"GLFW/examples/Heightmap.c"
	}

	links
	{
		"GLFW"
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
			"Cocoa.framework"
		}

project "Offscreen Example"
	kind "ConsoleApp"
	language "C"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")
	externalincludedirs
	{
		"GLFW/deps",
		"GLFW/include"
	}

	links
	{
		"GLFW"
	}

	files
	{
		"GLFW/examples/offscreen.c"
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
			"Cocoa.framework"
		}

project "Offscreen Example"
	kind "ConsoleApp"
	language "C"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")
	externalincludedirs
	{
		"GLFW/deps",
		"GLFW/include"
	}

	links
	{
		"GLFW"
	}
	
	files
	{
		"GLFW/examples/offscreen.c"
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
			"Cocoa.framework"
		}

project "Particles Example"
	kind "ConsoleApp"
	language "C"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")
	externalincludedirs
	{
		"GLFW/deps",
		"GLFW/include"
	}

	links
	{
		"GLFW"
	}

	files
	{
		"GLFW/examples/particles.c",
		"GLFW/deps/getopt.c",
		"GLFW/deps/tinycthread.c"
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
			"Cocoa.framework"
		}

project "Sharing Example"
	kind "ConsoleApp"
	language "C"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")
	externalincludedirs
	{
		"GLFW/deps",
		"GLFW/include"
	}
	
		files
		{
			"GLFW/examples/sharing.c"
		}

	links
	{
		"GLFW"
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
			"Cocoa.framework"
		}

project "Split View Example"
	kind "ConsoleApp"
	language "C"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")
	externalincludedirs
	{
		"GLFW/deps",
		"GLFW/include"
	}
	
	files
	{
		"GLFW/examples/splitview.c"
	}

	links
	{
		"GLFW"
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
			"Cocoa.framework"
		}

project "Triangle OpenGL"
	kind "ConsoleApp"
	language "C"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")
	externalincludedirs
	{
		"GLFW/deps",
		"GLFW/include"
	}
	
	files
	{
		"GLFW/examples/triangle-opengl.c"
	}

	links
	{
		"GLFW"
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
			"Cocoa.framework"
		}

project "Wave Example"
	kind "ConsoleApp"
	language "C"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")
	externalincludedirs
	{
		"GLFW/deps",
		"GLFW/include"
	}
	
	files
	{
		"GLFW/examples/wave.c"
	}

	links
	{
		"GLFW"
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
			"Cocoa.framework"
		}
	
project "Windows Example"
	kind "ConsoleApp"
	language "C"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")
	externalincludedirs
	{
		"GLFW/deps",
		"GLFW/include"
	}
	
	files
	{
		"GLFW/examples/windows.c"
	}

	links
	{
		"GLFW"
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
			"Cocoa.framework"
		}

project "Triangle Vulkan"
	kind "ConsoleApp"
	language "C"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")
	externalincludedirs
	{
		"GLFW/deps",
		"GLFW/include"
	}

	files
	{
		"GLFW/tests/triangle-vulkan.c"
	}

	links
		{
			"GLFW"
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
			"Cocoa.framework"
		}

project "Allocator Test"
	kind "ConsoleApp"
	language "C"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")
	externalincludedirs
	{
		"GLFW/deps",
		"GLFW/include"
	}
	
	files
	{
		"GLFW/tests/allocator.c"
	}

	links
	{
		"GLFW"
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
			"Cocoa.framework"
		}

project "Clipboard Test"
	kind "ConsoleApp"
	language "C"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")
	externalincludedirs
	{
		"GLFW/deps",
		"GLFW/include"
	}
	
	files
	{
		"GLFW/tests/clipboard.c",
		"GLFW/deps/getopt.c"
	}

	links
	{
		"GLFW"
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
			"Cocoa.framework"
		}

project "Cursor Test"
	kind "ConsoleApp"
	language "C"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")
	externalincludedirs
	{
		"GLFW/deps",
		"GLFW/include"
	}
	
	files
	{
		"GLFW/tests/cursor.c"
	}

	links
	{
		"GLFW"
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
			"Cocoa.framework"
		}

project "Empty Test"
	kind "ConsoleApp"
	language "C"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")
	externalincludedirs
	{
		"GLFW/deps",
		"GLFW/include"
	}
	
	files
	{
			"GLFW/tests/empty.c",
			"GLFW/deps/getopt.c",
			"GLFW/deps/tinycthread.c"
	}

	links
	{
		"GLFW"
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
			"Cocoa.framework"
		}

project "Events Test"
	kind "ConsoleApp"
	language "C"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")
	externalincludedirs
	{
		"GLFW/deps",
		"GLFW/include"
	}

	
	files
	{
		"GLFW/tests/events.c",
		"GLFW/deps/getopt.c"
	}

	links
	{
		"GLFW"
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
			"Cocoa.framework"
		}

project "Gamma Test"
	kind "ConsoleApp"
	language "C"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")
	externalincludedirs
	{
		"GLFW/deps",
		"GLFW/include"
	}
	
	files
	{
		"GLFW/tests/gamma.c"
	}

	links
	{
		"GLFW"
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
			"Cocoa.framework"
		}

project "Info"
	kind "ConsoleApp"
	language "C"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")
	externalincludedirs
	{
		"GLFW/deps",
		"GLFW/include"
	}
	
	files
	{
		"GLFW/tests/glfwinfo.c",
		"GLFW/deps/getopt.c"
	}

	links
	{
		"GLFW"
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
			"Cocoa.framework"
		}

project "Icon Test"
	kind "ConsoleApp"
	language "C"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")
	externalincludedirs
	{
		"GLFW/deps",
		"GLFW/include"
	}
	
	files
	{
		"GLFW/tests/icon.c"
	}

	links
	{
		"GLFW"
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
			"Cocoa.framework"
		}

project "Iconify Test"
	kind "ConsoleApp"
	language "C"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")
	externalincludedirs
	{
		"GLFW/deps",
		"GLFW/include"
	}
	
	files
	{
		"GLFW/tests/iconify.c",
		"GLFW/deps/getopt.c"
	}

	links
	{
		"GLFW"
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
			"Cocoa.framework"
		}

project "Input Lag Test"
	kind "ConsoleApp"
	language "C"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")
	externalincludedirs
	{
		"GLFW/deps",
		"GLFW/include"
	}
	
	files
	{
		"GLFW/tests/inputlag.c",
		"GLFW/deps/getopt.c"
	}

	links
	{
		"GLFW"
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
			"Cocoa.framework"
		}

project "Joysticks Test"
	kind "ConsoleApp"
	language "C"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")
	externalincludedirs
	{
		"GLFW/deps",
		"GLFW/include"
	}
	
	files
	{
		"GLFW/tests/joysticks.c"
	}

	links
	{
		"GLFW"
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
			"Cocoa.framework"
		}

project "Monitors Test"
	kind "ConsoleApp"
	language "C"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")
	externalincludedirs
	{
		"GLFW/deps",
		"GLFW/include"
	}
	
	files
	{
		"GLFW/tests/monitors.c",
		"GLFW/deps/getopt.c"
	}

	links
	{
		"GLFW"
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
			"Cocoa.framework"
		}

project "MSAA Test"
	kind "ConsoleApp"
	language "C"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")
	externalincludedirs
	{
		"GLFW/deps",
		"GLFW/include"
	}
	
	files
	{
		"GLFW/tests/msaa.c",
		"GLFW/deps/getopt.c"
	}

	links
	{
		"GLFW"
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
			"Cocoa.framework"
		}

project "Reopen Test"
	kind "ConsoleApp"
	language "C"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")
	externalincludedirs
	{
		"GLFW/deps",
		"GLFW/include"
	}
	
	files
	{
		"GLFW/tests/reopen.c"
	}

	links
	{
		"GLFW"
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
			"Cocoa.framework"
		}

project "Tearing Test"
	kind "ConsoleApp"
	language "C"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")
	externalincludedirs
	{
		"GLFW/deps",
		"GLFW/include"
	}
	
	files
	{
		"GLFW/tests/tearing.c"
	}

	links
	{
		"GLFW"
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
			"Cocoa.framework"
		}

project "Threads Test"
	kind "ConsoleApp"
	language "C"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")
	externalincludedirs
	{
		"GLFW/deps",
		"GLFW/include"
	}
	
	files
	{
		"GLFW/tests/threads.c",
		"GLFW/deps/tinycthread.c"
	}

	links
	{
		"GLFW"
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
			"Cocoa.framework"
		}

project "Timeout Test"
	kind "ConsoleApp"
	language "C"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")
	externalincludedirs
	{
		"GLFW/deps",
		"GLFW/include"
	}
	
	files
	{
		"GLFW/tests/timeout.c"
	}

	links
	{
		"GLFW"
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
			"Cocoa.framework"
		}

project "Title Test"
	kind "ConsoleApp"
	language "C"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")
	externalincludedirs
	{
		"GLFW/deps",
		"GLFW/include"
	}
	
	files
	{
		"GLFW/tests/title.c"
	}

	links
	{
		"GLFW"
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
			"Cocoa.framework"
		}

project "Window Test"
	kind "ConsoleApp"
	language "C"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")
	externalincludedirs
	{
		"GLFW/deps",
		"GLFW/include"
	}
	
	files
	{
		"GLFW/tests/window.c"
	}

	links
	{
		"GLFW"
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
			"Cocoa.framework"
		}