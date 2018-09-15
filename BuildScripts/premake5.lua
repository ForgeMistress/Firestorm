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
    "Debug", 
    "Release",
    "Final",
})

platforms({
    "Win32",
    "Win64"
})

location("Projects")

filter("platforms:Win32")
    architecture("x86")
filter("platforms:Win64")
    architecture("x86_64")

filter("action:vs*")
    defines({ "FIRE_VISUALSTUDIO" })
clearFilters()

filter("configurations:Debug")
    runtime("Debug")
    defines({"FIRE_DEBUG"});
filter("configurations:Release")
    runtime("Release")
    defines({"FIRE_RELEASE"})
filter("configurations:Final")
    runtime("Release");
    defines({"FIRE_FINAL"});
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
