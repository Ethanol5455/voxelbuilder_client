workspace "VBuilder"
	architecture "x64"
	startproject "VBuilder"

	configurations
	{
		"Debug",
		"Release"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

-- Include directories relative to root folder (solution directory)
IncludeDir = {}
IncludeDir["spdlog"]	= "vendor/spdlog/include"
IncludeDir["GLFW"] 		= "vendor/GLFW/include"
IncludeDir["Glad"] 		= "vendor/Glad/include"
IncludeDir["glm"] 		= "vendor/glm/include"
IncludeDir["stb"] 		= "vendor/stb"
IncludeDir["FastNoise"] = "vendor/FastNoise/Cpp"
IncludeDir["freetype"]  = "vendor/freetype/include"
IncludeDir["enet"]		= "vendor/enet/include"

group "Dependencies"
	include "vendor/GLFW"
	include "vendor/Glad"
	include "vendor/enet"
group ""

project "VBuilder"
	location "VBuilder"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	staticruntime "on"


	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")
	
	pchheader "vbpch.h"
	pchsource "%{prj.name}/src/vbpch.cpp"

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp",
		"%{prj.name}/res/**.glsl",
		"%{prj.name}/res/**.png",
		"vendor/glm/glm/**.hpp",
		"vendor/glm/glm/**.inl",
		"vendor/stb/**.h",
		"vendor/FastNoise/Cpp/FastNoiseLite.h"
	}

	includedirs
	{
		"%{prj.name}/src",
		"%{IncludeDir.spdlog}",
		"%{IncludeDir.GLFW}",
		"%{IncludeDir.Glad}",
		"%{IncludeDir.glm}",
		"%{IncludeDir.stb}",
		"%{IncludeDir.FastNoise}",
		"%{IncludeDir.freetype}",
		"%{IncludeDir.enet}"
	}

	links
	{
		"GLFW",
		"Glad",
		"enet"
	}
	
	flags
	{
		"MultiProcessorCompile"
	}

	filter "system:windows"
		systemversion "latest"

		defines
		{
			"VB_PLATFORM_WINDOWS",
			"VB_DEDICATED_GPU",
			"GLFW_INCLUDE_NONE"
		}

		links
		{
			"opengl32.lib",
			"vendor/freetype/freetype.lib"
		}

		filter "configurations:Release"
			optimize "on"
	
	filter "system:linux"
		systemversion "latest"

		defines
		{
			"VB_PLATFORM_LINUX",
			"GLFW_INCLUDE_NONE"
		}

		links
		{
			"X11",
            "rt",
            "m",
            "pthread",
            "dl",
            "GL",
			"freetype"
		}

		filter "configurations:Release"
			optimize "off"

	filter "configurations:Debug"
		defines "VB_DEBUG"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		defines "VB_RELEASE"
		runtime "Release"

