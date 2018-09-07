------------------------------------------------------------------------------------------------------------------------
--
--  ElflordPP/JSONBuild.lua
--
--  Running the build process for jsoncpp.
--
------------------------------------------------------------------------------------------------------------------------
--  Copyright (c) 2018 Miki Ryan
------------------------------------------------------------------------------------------------------------------------
print("--", "Generating project for JSONCPP.")

group("ThirdParty")
project("jsoncpp")
language("C++")
cppdialect("C++17")
kind("StaticLib")

targetdir(ENGINE_LIB_OUTPUT_DIR)

includedirs({"ThirdParty"})

files({
    "ThirdParty/json/**.h",
    "ThirdParty/json/**.cpp"
})

filter("action:vs*")
    disablewarnings({
        "4005"
    })
clearFilters()
