------------------------------------------------------------------------------------------------------------------------
--
--  ElflordPP/libExistence_Build.lua
--
--  Running the build process for libExistence.
--
------------------------------------------------------------------------------------------------------------------------
--  Copyright (c) 2018 Miki Ryan
------------------------------------------------------------------------------------------------------------------------
configureEngineLib("libExistence")

includedirs({
    "ThirdParty",
    "ThirdParty/rttr/src",
})

addDependencies({
    "libCore",
    "libMirror",
    "libIO",
    "libJson"
})

dependson({ "rttr", "jsoncpp" })
