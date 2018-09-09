------------------------------------------------------------------------------------------------------------------------
--
--  ElflordPP/JSONBuild.lua
--
--  Running the build process for jsoncpp.
--
------------------------------------------------------------------------------------------------------------------------
--  Copyright (c) 2018 Miki Ryan
------------------------------------------------------------------------------------------------------------------------
configureEngineLib("libApp")

includedirs({
    "ThirdParty",
    "ThirdParty/glfw/include",
    "ThirdParty/rttr/src",
    "ThirdParty/LLGL/include"
})

addDependencies(ENGINE_GAME_LIBS)

links({
    "LLGL"
})
if _OPTIONS["gfxapi"] == "OpenGL" then
    links({"glfw", "OpenGL32"})
end
