------------------------------------------------------------------------------------------------------------------------
--
--  ElflordPP/libSerial_Build.lua
--
--  Running the build process for libSerial.
--
------------------------------------------------------------------------------------------------------------------------
--  Copyright (c) 2018 Miki Ryan
------------------------------------------------------------------------------------------------------------------------
configureEngineLib("libSerial")

includedirs({
    "ThirdParty",
    "ThirdParty/rttr/src",
})

addDependencies({
    "libCore",
    "libMirror",
    "libMath",
    "libIO",
    "libExistence"
})

dependson({ "rttr", "jsoncpp" })
