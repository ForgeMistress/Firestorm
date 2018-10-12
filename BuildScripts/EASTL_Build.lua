------------------------------------------------------------------------------------------------------------------------
--
--  ElflordPP/JSONBuild.lua
--
--  Running the build process for jsoncpp.
--
------------------------------------------------------------------------------------------------------------------------
--  Copyright (c) 2018 Miki Ryan
------------------------------------------------------------------------------------------------------------------------
print("--", "Generating project for EASTL.")


group("ThirdParty/EASTL")
project("EABase")
language("C++")
cppdialect("C++17")
kind("StaticLib")

targetdir(ENGINE_BIN_OUTPUT_DIR)
defines({
    "_CHAR16T"
})

group("ThirdParty/EASTL")
project("EASTL")
language("C++")
cppdialect("C++17")
kind("StaticLib")

defines({
    "_CHAR16T",
    "_CRT_SECURE_NO_WARNINGS",
    "_SCL_SECURE_NO_WARNINGS",
    "EASTL_OPENSOURCE=1"
})

targetdir(ENGINE_BIN_OUTPUT_DIR)

includedirs({
    THIRD_PARTY_SRC_DIR.."/EASTL",
    THIRD_PARTY_SRC_DIR.."/EASTL/Packages/EABase/include/Common",
    THIRD_PARTY_SRC_DIR.."/EASTL/Packages/EAStdC/include",
    THIRD_PARTY_SRC_DIR.."/EASTL/Packages/EAAssert/include",
    THIRD_PARTY_SRC_DIR.."/EASTD/Packages/EABase/include/Common/EABase/**.h",
    THIRD_PARTY_SRC_DIR.."/EASTD/Packages/EABase/include/Common/EABase/config/**.h"
})

files({
    THIRD_PARTY_SRC_DIR.."/EASTL/EASTL/**.h",
    THIRD_PARTY_SRC_DIR.."/EASTL/EASTL/**.cpp",
    THIRD_PARTY_SRC_DIR.."/EASTL/EASTL/**.c",
    THIRD_PARTY_SRC_DIR.."/EASTL/Packages/EAStdC/**.cpp",
    THIRD_PARTY_SRC_DIR.."/EASTL/Packages/EAStdC/include/**.h",
    THIRD_PARTY_SRC_DIR.."/EASTL/Packages/EAAssert/include/**.h",
})

clearFilters()
