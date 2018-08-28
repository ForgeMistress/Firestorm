------------------------------------------------------------------------------------------------------------------------
--
--  ElflordPP/Angelscript_Build.lua
--
--  Running the build process for Angelscript.
--
------------------------------------------------------------------------------------------------------------------------
--  Copyright (c) 2018 Miki Ryan
------------------------------------------------------------------------------------------------------------------------
print("------------------------------------------------------------------------------------------------------------------------")
print("--", "Generating project for Angelscript.")
print("------------------------------------------------------------------------------------------------------------------------")

group("ThirdParty")
project("angelscript")
language("C++")
cppdialect("C++17")
kind("StaticLib")

targetdir(ENGINE_LIB_OUTPUT_DIR)

includedirs({
    "ThirdParty/angelscript/sdk/angelscript/include",
    "ThirdParty/angelscript/sdk/angelscript/source"
})

files({
    "ThirdParty/angelscript/sdk/angelscript/source/**.h",
    "ThirdParty/angelscript/sdk/angelscript/source/**.cpp"
})

