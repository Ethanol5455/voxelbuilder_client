project "enet"
    kind "StaticLib"
    language "C"
    staticruntime "on"

    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

    files
    {
        "*.c"
    }
		
    includedirs
    {
        "include"
    }

    flags
	{
		"MultiProcessorCompile"
	}

    filter "system:windows"
		systemversion "latest"
        links
        {
            "winmm.lib",
            "ws2_32.lib"
        }
    
    filter "configurations:Debug"
        defines
        {
            "DEBUG"
        }
        runtime "Debug"
        symbols "on"
    
    filter "configurations:Release"
        defines
        {
            "NDEBUG"
        }
        runtime "Release"
        optimize "on"