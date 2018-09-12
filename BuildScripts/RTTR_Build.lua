------------------------------------------------------------------------------------------------------------------------
--
--  ElflordPP/JSONBuild.lua
--
--  Running the build process for jsoncpp.
--
------------------------------------------------------------------------------------------------------------------------
--  Copyright (c) 2018 Miki Ryan
------------------------------------------------------------------------------------------------------------------------
print("--", "Generating project for rttr.")

group("ThirdParty")
project("rttr")
language("C++")
cppdialect("C++17")
kind("StaticLib")

targetdir(ENGINE_BIN_OUTPUT_DIR)

includedirs({
    THIRD_PARTY_SRC_DIR.."/rttr/src"
})

files({
    THIRD_PARTY_SRC_DIR.."/rttr/src/rttr/**.h",
    THIRD_PARTY_SRC_DIR.."/rttr/src/rttr/**.cpp"
})