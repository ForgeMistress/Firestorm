------------------------------------------------------------------------------------------------------------------------
--
--  ElflordPP/premake5.lua
--
--  This file is part of the Elflord Engine.
--
------------------------------------------------------------------------------------------------------------------------
--  Copyright (c) 2018 Miki Ryan
------------------------------------------------------------------------------------------------------------------------
workspace("*")
configurations({
    "Debug32", "Release32", "Final32",
    "Debug64", "Release64", "Final64"
})

location("Premake")

includedirs({ "Source/Include" })


filter("configurations:*32")
    architecture("x86")

filter("configurations:*64")
    architecture("x86_64")

filter("configurations:Debug32")
    libdirs({ "Bin/x86/Debug" })
filter("configurations:Release32")
    libdirs({ "Bin/x86/Release" })
filter("configurations:Final32")
    libdirs({ "Bin/x86/Release" })

filter("configurations:Debug64")
    libdirs({ "Bin/x64/Debug" })
filter("configurations:Release64")
    libdirs({ "Bin/x64/Release" })
filter("configurations:Final64")
    libdirs({ "Bin/x64/Release" })

workspace("ElflordPP")
 
include("Source")
