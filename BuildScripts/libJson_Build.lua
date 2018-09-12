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
    THIRD_PARTY_SRC_DIR,
    THIRD_PARTY_SRC_DIR.."/json",
    THIRD_PARTY_SRC_DIR.."/rttr/src"
})

addDependencies({
    "libCore",
    "libMirror"
})

links("jsoncpp")
