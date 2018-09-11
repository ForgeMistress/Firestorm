------------------------------------------------------------------------------------------------------------------------
--
--  Game_Build.lua
--
--  Running the build process for Game.
--
------------------------------------------------------------------------------------------------------------------------
--  Copyright (c) 2018 Miki Ryan
------------------------------------------------------------------------------------------------------------------------
configureGameLib("Firestorm")
configureGame("Firestorm")

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
    "GLFW",
    "LLGL"
})

if _OPTIONS["gfxapi"] == "OpenGL" then
    dependson("LLGL_OpenGL")
end
