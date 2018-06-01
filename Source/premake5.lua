------------------------------------------------------------------------------------------------------------------------
--
--  ElflordPP/Source/premake5.lua
--
--  This file is part of the Elflord Engine.
--
------------------------------------------------------------------------------------------------------------------------
--  Copyright (c) 2018 Miki Ryan
------------------------------------------------------------------------------------------------------------------------
function staticlib(libname, lnks)
    project(libname)
        language("C++")
        kind("StaticLib")
        targetdir("../../../Build/%{cfg.buildcfg}")
        files({
            "**.h",
            "**.cpp",
            "../../Include/"..libname..".h",
            "../../Include/"..libname.."/**.h"
        })
        pchheader("stdafx.h")
        pchsource("stdafx.cpp")

        includedirs({ "." })
        if lnks then
            links(lnks)
            --links({ "libCore" })
        end
end

include('Libraries/libCore')
include('Libraries/libExistence')
include('Libraries/libHarnessed')
include('Libraries/libMirror')

function unittest(libName, lnk)
    project(libName.."_Test")
        language("C++")
        kind("ConsoleApp")
        targetdir("../Build/Tests/%{cfg.buildcfg}")
        files({
            "Tests/"..libName.."/**.h",
            "Tests/"..libName.."/**.cpp"
        })
        includedirs({"."})
        links({'libHarnessed', 'libCore'})
        if lnk then
            links(lnk or { "libCore" })
        end
end
unittest('libHarnessed')
unittest('libExistence')

