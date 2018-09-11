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
include("Platform")

-- GLOBAL BUILD CONFIGURATION
workspace("*")

include("Options")

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
    defines({ "FIRE_VISUALSTUDIO" })
clearFilters()

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

filter("configurations:Debug*")
    runtime("Debug")
filter("configurations:Release* or Final*")
    runtime("Release")
clearFilters()

function build(lib)
    include(lib.."_Build")
end

------------------------------------------------------------------------------------------------------------------------
--  WORKSPACE DEFINITION
------------------------------------------------------------------------------------------------------------------------
workspace("Firestorm")

------------------------------------------------------------------------------------------------------------------------
--  THIRD PARTY LIBRARIES
------------------------------------------------------------------------------------------------------------------------
build("JsonCPP")
build("RTTR")
build("Angelscript")
build("GLFW")
build("physfs")
build("LLGL")
build("imgui")

------------------------------------------------------------------------------------------------------------------------
--  FIRST PARTY LIBRARIES
------------------------------------------------------------------------------------------------------------------------
build("libApp")
build("libCore")
build("libExistence")
build("libHarnessed")
build("libIO")
build("libJson")
build("libMath")
build("libMirror")
build("libScene")
build("libScript")
build("libSerial")
build("libUI")

build("Game")

configureUnitTestApplication()
