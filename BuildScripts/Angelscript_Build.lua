------------------------------------------------------------------------------------------------------------------------
--
--  ElflordPP/Angelscript_Build.lua
--
--  Running the build process for Angelscript.
--
------------------------------------------------------------------------------------------------------------------------
--  Copyright (c) 2018 Miki Ryan
------------------------------------------------------------------------------------------------------------------------
print("--", "Generating project for Angelscript.")

group("ThirdParty")
project("angelscript")
language("C++")
cppdialect("C++17")
kind("StaticLib")

targetdir(ENGINE_LIB_OUTPUT_DIR)

includedirs({
    THIRD_PARTY_SRC_DIR.."/angelscript/sdk/angelscript/include",
    THIRD_PARTY_SRC_DIR.."/angelscript/sdk/angelscript/source"
})

files({
    THIRD_PARTY_SRC_DIR.."/angelscript/sdk/angelscript/source/**.h",
    THIRD_PARTY_SRC_DIR.."/angelscript/sdk/angelscript/source/**.cpp"
})
