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

targetdir(ENGINE_LIB_OUTPUT_DIR)

includedirs({
    "ThirdParty/rttr/src"
})

files({
    "ThirdParty/rttr/src/rttr/**.h",
    "ThirdParty/rttr/src/rttr/**.cpp"
})
