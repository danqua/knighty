workspace "Knighty"
	architecture "x86_64"
	flags {
		"MultiProcessorCompile"
	}
	configurations {
		"Debug",
		"Release"
	}
	debugdir "data"
	targetdir (path.join("bin", "%{cfg.buildcfg}"))
	objdir (path.join("bin", "obj", "%{cfg.buildcfg}"))
	
project "Knighty"
	kind "ConsoleApp"
	language "C"
	cdialect "C99"
	includedirs {
		"code",
		path.join("extern", "SDL2-2.0.18", "include"),
		path.join("extern", "stb"),
		path.join("extern", "json")
	}
	libdirs {
		path.join("extern", "SDL2-2.0.18", "lib", "x64")
	}
	files {
		"code/**.h",
		"code/**.c"
	}
	links {
		"SDL2",
		"SDL2main"
	}
	filter "configurations:Debug"
		defines { "DEBUG" }
		symbols "On"

	filter "configurations:Release"
		defines { "NDEBUG" }
		optimize "Full"