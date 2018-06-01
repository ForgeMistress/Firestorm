------------------------------------------------------------------------------------------------------------------------
--
--  ElflordPP/Source/premake5.lua
--
--  This file is part of the Elflord Engine.
--
------------------------------------------------------------------------------------------------------------------------
--  Copyright (c) 2018 Miki Ryan
------------------------------------------------------------------------------------------------------------------------
--[[function staticlib(libname, args)
    local libsrcpath = Constants.RootDir.."/Source/Libraries/"..libname
    local libincpath = Constants.RootDir.."/Source/Include/"..libname

    group("Libraries")

    project(libname)
    language("C++")
    kind("StaticLib")
    targetdir("../../../Build/%{cfg.buildcfg}")
    files({
        "**.h",
        "**.cpp",
        libincpath..".h",
        libincpath.."/**.h"
        --"../../Include/"..libname..".h",
        --"../../Include/"..libname.."/**.h"
    })

    pchheader(libsrcpath.."/stdafx.h")
    pchsource(libsrcpath.."/stdafx.cpp")
    includedirs({ libsrcpath })
    -- pchheader(Constants.RootDir.."/Source/Libraries/"..libname.."/stdafx.h")
    -- pchsource(Constants.RootDir.."/Source/Libraries/"..libname.."/stdafx.cpp")
    -- includedirs({ "." })
    if type(args) == 'table' then
        if type(args.links) == 'table' then
            links(args.links)
        end

        if type(args.post) == 'function' then
            args.post()
        end
    end
end]]
--include('Libraries')

function unittest(testname, lnk)
    group("UnitTests")
    project(testname.."_Test")

    language("C++")
    kind("ConsoleApp")
    targetdir(Constants.RootDir.."/Build/Tests/%{cfg.buildcfg}")
    files({
        Constants.RootDir.."/Source/Tests/"..testname.."/**.h",
        Constants.RootDir.."/Source/Tests/"..testname.."/**.cpp"
    })
    includedirs({"."})
    links({'libHarnessed', 'libCore'})
    if lnk then
        links(lnk or { "libCore" })
    end
end
include('Tests')
