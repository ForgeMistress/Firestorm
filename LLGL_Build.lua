------------------------------------------------------------------------------------------------------------------------
--
--  ElflordPP/JSONBuild.lua
--
--  Running the build process for jsoncpp.
--
------------------------------------------------------------------------------------------------------------------------
--  Copyright (c) 2018 Miki Ryan
------------------------------------------------------------------------------------------------------------------------
print("------------------------------------------------------------------------------------------------------------------------")
print("--", "Generating project for JSONCPP.")
print("------------------------------------------------------------------------------------------------------------------------")

group("ThirdParty")
project("LLGL")
language("C++")
cppdialect("C++17")
kind("StaticLib")

targetdir(ENGINE_LIB_OUTPUT_DIR)

includedirs({"ThirdParty/LLGL/include"})

files({
    "ThirdParty/LLGL/source/Core/**.h",
    "ThirdParty/LLGL/source/Core/**.cpp",

	"ThirdParty/LLGL/source/Platform/*.h",
	"ThirdParty/LLGL/source/Platform/*.cpp"
})

if os.ishost("windows") then
	files({
		"ThirdParty/LLGL/source/Platform/Win32/**.h",
		"ThirdParty/LLGL/source/Platform/Win32/**.cpp"
	})
elseif os.ishost("unix") then
end
