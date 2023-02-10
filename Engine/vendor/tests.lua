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

	linkoptions
	{
		"-framework OpenGL",
		"-framework Cocoa",
		"-framework IOKit"
	}

	links
	{
		"GLFW",
		"Cocoa.framework"
	}

	files
	{
		"GLFW/examples/boing.c"
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

	linkoptions
	{
		"-framework OpenGL",
		"-framework Cocoa",
		"-framework IOKit"
	}

	links
	{
		"GLFW",
		"Cocoa.framework"
	}

	files
	{
		"GLFW/examples/gears.c"
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

	linkoptions
	{
		"-framework OpenGL",
		"-framework Cocoa",
		"-framework IOKit"
	}

	links
	{
		"GLFW",
		"Cocoa.framework"
	}

	files
	{
		"GLFW/examples/Heightmap.c"
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

	linkoptions
	{
		"-framework OpenGL",
		"-framework Cocoa",
		"-framework IOKit"
	}

	links
	{
		"GLFW",
		"Cocoa.framework"
	}

	files
	{
		"GLFW/examples/offscreen.c"
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

	linkoptions
	{
		"-framework OpenGL",
		"-framework Cocoa",
		"-framework IOKit"
	}

	links
	{
		"GLFW",
		"Cocoa.framework"
	}

	files
	{
		"GLFW/examples/offscreen.c"
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

	linkoptions
	{
		"-framework OpenGL",
		"-framework Cocoa",
		"-framework IOKit"
	}

	links
	{
		"GLFW",
		"Cocoa.framework"
	}

	files
	{
		"GLFW/examples/particles.c",
		"GLFW/deps/getopt.c",
		"GLFW/deps/tinycthread.c"
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

	linkoptions
	{
		"-framework OpenGL",
		"-framework Cocoa",
		"-framework IOKit"
	}

	links
	{
		"GLFW",
		"Cocoa.framework"
	}

	files
	{
		"GLFW/examples/sharing.c"
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

	linkoptions
	{
		"-framework OpenGL",
		"-framework Cocoa",
		"-framework IOKit"
	}

	links
	{
		"GLFW",
		"Cocoa.framework"
	}

	files
	{
		"GLFW/examples/splitview.c"
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

	linkoptions
	{
		"-framework OpenGL",
		"-framework Cocoa",
		"-framework IOKit"
	}

	links
	{
		"GLFW",
		"Cocoa.framework"
	}

	files
	{
		"GLFW/examples/triangle-opengl.c"
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

	linkoptions
	{
		"-framework OpenGL",
		"-framework Cocoa",
		"-framework IOKit"
	}

	links
	{
		"GLFW",
		"Cocoa.framework"
	}

	files
	{
		"GLFW/examples/wave.c"
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

	linkoptions
	{
		"-framework OpenGL",
		"-framework Cocoa",
		"-framework IOKit"
	}

	links
	{
		"GLFW",
		"Cocoa.framework"
	}

	files
	{
		"GLFW/examples/wave.c"
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

	linkoptions
	{
		"-framework OpenGL",
		"-framework Cocoa",
		"-framework IOKit"
	}

	links
	{
		"GLFW",
		"Cocoa.framework"
	}

	files
	{
		"GLFW/examples/windows.c"
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

	linkoptions
	{
		"-framework OpenGL",
		"-framework Cocoa",
		"-framework IOKit"
	}

	links
	{
		"GLFW",
		"Cocoa.framework"
	}

	files
	{
		"GLFW/tests/triangle-vulkan.c"
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

	linkoptions
	{
		"-framework OpenGL",
		"-framework Cocoa",
		"-framework IOKit"
	}

	links
	{
		"GLFW",
		"Cocoa.framework"
	}

	files
	{
		"GLFW/tests/allocator.c"
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

	linkoptions
	{
		"-framework OpenGL",
		"-framework Cocoa",
		"-framework IOKit"
	}

	links
	{
		"GLFW",
		"Cocoa.framework"
	}

	files
	{
		"GLFW/tests/clipboard.c",
		"GLFW/deps/getopt.c"
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

	linkoptions
	{
		"-framework OpenGL",
		"-framework Cocoa",
		"-framework IOKit"
	}

	links
	{
		"GLFW",
		"Cocoa.framework"
	}

	files
	{
		"GLFW/tests/cursor.c"
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

	linkoptions
	{
		"-framework OpenGL",
		"-framework Cocoa",
		"-framework IOKit"
	}

	links
	{
		"GLFW",
		"Cocoa.framework"
	}

	files
	{
		"GLFW/tests/empty.c",
		"GLFW/deps/getopt.c",
		"GLFW/deps/tinycthread.c"
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

	linkoptions
	{
		"-framework OpenGL",
		"-framework Cocoa",
		"-framework IOKit"
	}

	links
	{
		"GLFW",
		"Cocoa.framework"
	}

	files
	{
		"GLFW/tests/events.c",
		"GLFW/deps/getopt.c"
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

	linkoptions
	{
		"-framework OpenGL",
		"-framework Cocoa",
		"-framework IOKit"
	}

	links
	{
		"GLFW",
		"Cocoa.framework"
	}

	files
	{
		"GLFW/tests/gamma.c"
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

	linkoptions
	{
		"-framework OpenGL",
		"-framework Cocoa",
		"-framework IOKit"
	}

	links
	{
		"GLFW",
		"Cocoa.framework"
	}

	files
	{
		"GLFW/tests/glfwinfo.c",
		"GLFW/deps/getopt.c"
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

	linkoptions
	{
		"-framework OpenGL",
		"-framework Cocoa",
		"-framework IOKit"
	}

	links
	{
		"GLFW",
		"Cocoa.framework"
	}

	files
	{
		"GLFW/tests/icon.c"
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

	linkoptions
	{
		"-framework OpenGL",
		"-framework Cocoa",
		"-framework IOKit"
	}

	links
	{
		"GLFW",
		"Cocoa.framework"
	}

	files
	{
		"GLFW/tests/iconify.c",
		"GLFW/deps/getopt.c"
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

	linkoptions
	{
		"-framework OpenGL",
		"-framework Cocoa",
		"-framework IOKit"
	}

	links
	{
		"GLFW",
		"Cocoa.framework"
	}

	files
	{
		"GLFW/tests/inputlag.c",
		"GLFW/deps/getopt.c"
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

	linkoptions
	{
		"-framework OpenGL",
		"-framework Cocoa",
		"-framework IOKit"
	}

	links
	{
		"GLFW",
		"Cocoa.framework"
	}

	files
	{
		"GLFW/tests/joysticks.c"
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

	linkoptions
	{
		"-framework OpenGL",
		"-framework Cocoa",
		"-framework IOKit"
	}

	links
	{
		"GLFW",
		"Cocoa.framework"
	}

	files
	{
		"GLFW/tests/monitors.c",
		"GLFW/deps/getopt.c"
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

	linkoptions
	{
		"-framework OpenGL",
		"-framework Cocoa",
		"-framework IOKit"
	}

	links
	{
		"GLFW",
		"Cocoa.framework"
	}

	files
	{
		"GLFW/tests/msaa.c",
		"GLFW/deps/getopt.c"
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

	linkoptions
	{
		"-framework OpenGL",
		"-framework Cocoa",
		"-framework IOKit"
	}

	links
	{
		"GLFW",
		"Cocoa.framework"
	}

	files
	{
		"GLFW/tests/reopen.c"
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

	linkoptions
	{
		"-framework OpenGL",
		"-framework Cocoa",
		"-framework IOKit"
	}

	links
	{
		"GLFW",
		"Cocoa.framework"
	}

	files
	{
		"GLFW/tests/tearing.c"
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

	linkoptions
	{
		"-framework OpenGL",
		"-framework Cocoa",
		"-framework IOKit"
	}

	links
	{
		"GLFW",
		"Cocoa.framework"
	}

	files
	{
		"GLFW/tests/threads.c",
		"GLFW/deps/tinycthread.c"
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

	linkoptions
	{
		"-framework OpenGL",
		"-framework Cocoa",
		"-framework IOKit"
	}

	links
	{
		"GLFW",
		"Cocoa.framework"
	}

	files
	{
		"GLFW/tests/timeout.c"
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

	linkoptions
	{
		"-framework OpenGL",
		"-framework Cocoa",
		"-framework IOKit"
	}

	links
	{
		"GLFW",
		"Cocoa.framework"
	}

	files
	{
		"GLFW/tests/title.c"
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

	linkoptions
	{
		"-framework OpenGL",
		"-framework Cocoa",
		"-framework IOKit"
	}

	links
	{
		"GLFW",
		"Cocoa.framework"
	}

	files
	{
		"GLFW/tests/window.c"
	}