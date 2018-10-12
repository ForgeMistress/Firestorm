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


location("Projects")

function build(lib)
    include(lib.."_Build")
end

------------------------------------------------------------------------------------------------------------------------
--  WORKSPACE DEFINITION
------------------------------------------------------------------------------------------------------------------------
workspace("Firestorm")

-- All projects will have the following build targets available to them.
configurations({
    "Debug",
    "Release",
    "Final",
})

platforms({
    "Windows"
})

filter({"platforms:*"})
    architecture("x86_64")
clearFilters()

filter("action:vs*")
    defines({ "FIRE_VISUALSTUDIO" })
clearFilters()

filter("configurations:Debug")
    runtime("Debug")
    defines({"FIRE_DEBUG"});
clearFilters()
filter("configurations:Release")
    runtime("Release")
    defines({"FIRE_RELEASE"})
clearFilters()
filter("configurations:Final")
    runtime("Release");
    defines({"FIRE_FINAL"});
clearFilters()

------------------------------------------------------------------------------------------------------------------------
--  THIRD PARTY LIBRARIES
------------------------------------------------------------------------------------------------------------------------
build("EASTL")
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
build("libMath")
build("libMirror")
build("libScene")
build("libScript")
build("libSerial")
build("libUI")

build("Game")

configureUnitTestApplication()
