------------------------------------------------------------------------------------------------------------------------
--
--  ElflordPP/libSerial_Build.lua
--
--  Running the build process for libSerial.
--
------------------------------------------------------------------------------------------------------------------------
--  Copyright (c) 2018 Miki Ryan
------------------------------------------------------------------------------------------------------------------------
configureGame("Elflord")

addDependencies({
    "libCore",
    "libMirror",
    "libMath",
    "libIO",
    "libJson",
    "libExistence"
})

links({ 
    "rttr", 
    "jsoncpp", 
    "angelscript",
    "GLFW"
})