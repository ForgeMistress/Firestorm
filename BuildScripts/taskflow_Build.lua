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
project("taskflow")
language("C++")
cppdialect("C++17")
kind("StaticLib")

targetdir(ENGINE_BIN_OUTPUT_DIR)

includedirs({
    THIRD_PARTY_SRC_DIR,
    THIRD_PARTY_SRC_DIR.."/taskflow"
})

files({
    THIRD_PARTY_SRC_DIR.."/taskflow/taskflow/**.h",
    THIRD_PARTY_SRC_DIR.."/taskflow/taskflow/**.hpp",
})
