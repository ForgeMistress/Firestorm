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

--filter("configurations:*32"); architecture("x86")
filter("configurations:*64")
    architecture("x86_64")

--filter(

filter("action:vs*")
defines({ "ELF_PLATFORM_WINDOWS", "ELF_VISUALSTUDIO" })

filter("action:xcode*")
defines({ "ELF_PLATFORM_OSX", "ELF_XCODE" })

filter("action:gmake*")
defines({ "ELF_PLATFORM_UNIX" })
--defines({ "ELF_PLATFORM_ANDROID" })
--defines({ "ELF_PLATFORM_IOS"     })

filter("configurations:Debug32");
    libdirs({ "Bin/x86/Debug" })
filter("configurations:Release32 or Final32")
    libdirs({ "Bin/x86/Release" })

filter("configurations:Debug64")
    libdirs({ "Bin/x64/Debug" })
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

------------------------------------------------------------------------------------------------------------------------
--  FIRST PARTY LIBRARIES
------------------------------------------------------------------------------------------------------------------------
include("libCore_Build")
include("libExistence_Build")
include("libHarnessed_Build")
include("libIO_Build")
include("libJson_Build")
include("libMath_Build")
include("libMirror_Build")
include("libScript_Build")
include("libSerial_Build")

include("Game_Build")
