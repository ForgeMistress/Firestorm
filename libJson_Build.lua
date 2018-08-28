------------------------------------------------------------------------------------------------------------------------
--
--  ElflordPP/JSONBuild.lua
--
--  Running the build process for jsoncpp.
--
------------------------------------------------------------------------------------------------------------------------
--  Copyright (c) 2018 Miki Ryan
------------------------------------------------------------------------------------------------------------------------
configureEngineLib("libJson")

includedirs({
    "ThirdParty",
    "ThirdParty/json",
    "ThirdParty/rttr/src"
})

addDependencies({
    "libCore",
    "libMirror"
})

links("jsoncpp")
