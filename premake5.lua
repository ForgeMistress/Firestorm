------------------------------------------------------------------------------------------------------------------------
--
--  ElflordPP/premake5.lua
--
--  This file is part of the Elflord Engine.
--
------------------------------------------------------------------------------------------------------------------------
--  Copyright (c) 2018 Miki Ryan
------------------------------------------------------------------------------------------------------------------------

------------------------------------------------------------------------------------------------------------------------
--  GLOBAL SETUP
------------------------------------------------------------------------------------------------------------------------
include("GlobalConfig")
include("precore")

-- GLOBAL BUILD CONFIGURATION
workspace("*")

-- All projects will have the following build targets available to them.
configurations({
    "Debug32", "Release32", "Final32",
    "Debug64", "Release64", "Final64"
})

location("Premake")

filter("configurations:Debug32 or Release32 or Final32")
    architecture("x86")
clearFilters()

filter("configurations:Debug64 or Release64 or Final64")
    architecture("x86_64")
clearFilters()

filter("action:vs*")
    defines({ "ELF_VISUALSTUDIO" })
clearFilters()


if os.ishost("windows") then
    defines({ "ELF_PLATFORM_WINDOWS" })
end

filter("action:xcode*")
    defines({ "ELF_XCODE" })
clearFilters()

if os.ishost("macosx") then
    defines({ "ELF_PLATFORM_OSX" })
end

filter("action:gmake*")
    defines({ "ELF_GMAKE", "ELF_PLATFORM_UNIX" })
clearFilters()
if os.ishost("linux") then
    defines({ "ELF_PLATFORM_UNIX" })
end

filter("configurations:Debug32")
    libdirs({ "Bin/x86/Debug" })
clearFilters()
filter("configurations:Release32 or Final32")
    libdirs({ "Bin/x86/Release" })
clearFilters()

filter("configurations:Debug64")
    libdirs({ "Bin/x64/Debug" })
clearFilters()

filter("configurations:Release64 or Final64");
    libdirs({ "Bin/x64/Release" })
clearFilters()


------------------------------------------------------------------------------------------------------------------------
--  WORKSPACE DEFINITION
------------------------------------------------------------------------------------------------------------------------
workspace("ElflordPP")

------------------------------------------------------------------------------------------------------------------------
--  THIRD PARTY LIBRARIES
------------------------------------------------------------------------------------------------------------------------
include("JsonCPP_Build")
include("RTTR_Build")
include("Angelscript_Build")
include("GLFW_Build")
include("physfs_Build")

------------------------------------------------------------------------------------------------------------------------
--  FIRST PARTY LIBRARIES
------------------------------------------------------------------------------------------------------------------------
include("libApp_Build")
include("libCore_Build")
include("libExistence_Build")
include("libHarnessed_Build")
include("libIO_Build")
include("libJson_Build")
include("libMath_Build")
include("libMirror_Build")
include("libScene_Build")
include("libScript_Build")
include("libSerial_Build")

include("Game_Build")
