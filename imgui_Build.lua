------------------------------------------------------------------------------------------------------------------------
--
--  imgui_Build.lua
--
--  Running the build process for imgui.
--
------------------------------------------------------------------------------------------------------------------------
--  Copyright (c) 2018 Miki Ryan
------------------------------------------------------------------------------------------------------------------------
print("--", "Generating project for imgui.")

group("ThirdParty")
project("imgui")
language("C++")
cppdialect("C++17")
kind("StaticLib")

targetdir(ENGINE_LIB_OUTPUT_DIR)

includedirs({
    "ThirdParty/imgui/src"
})

files({
    "ThirdParty/imgui/*.h",
    "ThirdParty/imgui/*.cpp"
})
