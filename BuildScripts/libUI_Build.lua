------------------------------------------------------------------------------------------------------------------------
--
--  ElflordPP/JSONBuild.lua
--
--  Running the build process for jsoncpp.
--
------------------------------------------------------------------------------------------------------------------------
--  Copyright (c) 2018 Miki Ryan
------------------------------------------------------------------------------------------------------------------------
configureEngineLib("libUI")

includedirs({
    "ThirdParty",
    "ThirdParty/rttr/src"
})

addDependencies({
    "libCore",
    "libMirror",
    "libJson"
})

dependson({"LLGL", "angelscript"})
--[[if _OPTIONS["gfxapi"] == "OpenGL" then
    links({"OpenGL32"})
end]]
