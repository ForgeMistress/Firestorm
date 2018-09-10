------------------------------------------------------------------------------------------------------------------------
--
--  ElflordPP/physfs_Build.lua
--
--  Running the build process for physfs.
--
------------------------------------------------------------------------------------------------------------------------
--  Copyright (c) 2018 Miki Ryan
------------------------------------------------------------------------------------------------------------------------
print("--", "Generating project for physfs.")

group("ThirdParty")
project("physfs")
language("C++")
cppdialect("C++17")
kind("StaticLib")

targetdir(ENGINE_BIN_OUTPUT_DIR)

includedirs({
    "ThirdParty"
})

files({
    "ThirdParty/physfs/*.h",
    "ThirdParty/physfs/*.c",
    "ThirdParty/physfs/*.m",
    "ThirdParty/physfs/*.cpp",
})

