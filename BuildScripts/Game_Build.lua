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

addDependencies(ENGINE_GAME_LIBS)
links(ENGINE_GAME_LIBS)

links({
    "rttr",
    "jsoncpp",
    "angelscript",
    "LLGL",
    "physfs",
    "EASTL"
})

if _OPTIONS["gfxapi"] == "OpenGL" then
    dependson("LLGL_OpenGL")
end
