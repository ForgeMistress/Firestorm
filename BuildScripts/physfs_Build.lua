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
    THIRD_PARTY_SRC_DIR
})

files({
    THIRD_PARTY_SRC_DIR.."/physfs/*.h",
    THIRD_PARTY_SRC_DIR.."/physfs/*.c",
    THIRD_PARTY_SRC_DIR.."/physfs/*.m",
    THIRD_PARTY_SRC_DIR.."/physfs/*.cpp",
})
