------------------------------------------------------------------------------------------------------------------------
--
--  ElflordPP/premake5.lua
--
--  This file is part of the Elflord Engine.
--
------------------------------------------------------------------------------------------------------------------------
--  Copyright (c) 2018 Miki Ryan
------------------------------------------------------------------------------------------------------------------------
workspace("ElflordPP")
configurations({
    "Debug",
    "Release",
    "ReleaseDbg",
    "Final"
})
location("Premake")
includedirs({
    "Source/Include"
})

include("Source")
