project "GLAD"
	kind "StaticLib"
	language "C"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"include/GLAD/glad.h",
		"include/KHR/khrplatform.h",
		"src/glad.c"
	}
	
	includedirs
	{
		"include"
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
