project "OpenFBX"
	kind "StaticLib"
	language "C++"
	
	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")
	
	files
	{
		"OpenFBX/src/miniz.c",
		"OpenFBX/src/miniz.h",
		"OpenFBX/src/ofbx.cpp",
		"OpenFBX/src/ofbx.h",
	}
	
	filter "system:windows"
		systemversion "latest"
		staticruntime "Off"
		
	filter "configurations:Debug"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		runtime "Release"
		optimize "on"