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

includedirs({
    "ThirdParty",
    "ThirdParty/rttr/src",
    "ThirdParty/angelscript/sdk/angelscript/include"
})

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
    "angelscript"
})