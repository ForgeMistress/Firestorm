------------------------------------------------------------------------------------------------------------------------
--
--  ElflordPP/Source/lib/libCore/premake5.lua
--
--  This file is part of the Elflord Engine.
--
------------------------------------------------------------------------------------------------------------------------
--  Copyright (c) 2018 Miki Ryan
------------------------------------------------------------------------------------------------------------------------
project("libCore")
    language("C++")
    kind("StaticLib")
    targetdir("Build/%{cfg.buildcfg}")
    files({
        "**.h",
        "**.cpp"
    })
    pchheader("stdafx.h")
    pchsource("stdafx.cpp")
    includedirs({"."})
