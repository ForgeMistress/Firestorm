------------------------------------------------------------------------------------------------------------------------
--
--  ElflordPP/precore.lua
--
--  This file is part of the Elflord Engine. This should be included before anything in premake5.lua is called.
--
------------------------------------------------------------------------------------------------------------------------
--  Copyright (c) 2018 Miki Ryan
------------------------------------------------------------------------------------------------------------------------


Constants = {
    RootDir = os.getcwd();
}

local function processargs(name, args)
    -- additional build configuration.
    if type(args) == 'table' then
        if type(args.depends) == 'table' then
            links(args.depends)
        end

        if type(args.incdirs) == 'table' then
            includedirs(args.incdirs)
        end

        if type(args.post) == 'function' then
            args.post()
        end
    end
end

function staticlib(libname, args)
    filter({})

    local libsrcpath = "Source/Libraries/"..libname
    local libincpath = "Source/Include/"..libname

    group("Libraries")

    project(libname)
    language("C++")
    kind("StaticLib")
    targetdir("Build/Libraries/%{cfg.buildcfg}")

    files({
        libsrcpath.."/**.h",   -- all header files
        libsrcpath.."/**.cpp", -- all source files

        libincpath..".h",      -- public headers in Source/Include
        libincpath.."/**.h"    -- all other public headers in Source/Include/{libname}
    })


    -- required for precompiled headers.
    includedirs({ 
        libsrcpath,
        "ThirdParty/rttr/src",
        "ThirdParty/rttr/build/src",
    })

    filter("action:vs*")
        pchheader("stdafx.h")
        pchsource(libsrcpath.."/stdafx.cpp")

    filter("action:not vs*")
        pchheader("stdafx.h")

    filter({})

    processargs(libname, args)
end

function unittest(testname, args)
    filter({})

    local tstsrcpath = "Source/Tests/"..testname
    local libincpath = "Source/Include"

    group("UnitTests")
    project(testname.."_Test")

    language("C++")
    kind("ConsoleApp")

    targetdir("Build/Tests/%{cfg.buildcfg}")

    --libdirs({ "Build/Libraries/%{cfg.buildtarget}" })

    files({
        tstsrcpath.."/**.h",
        tstsrcpath.."/**.cpp"
    })
    includedirs({ 
        libincpath,
        "ThirdParty/rttr/src",
        "ThirdParty/rttr/build/src",
    })
    links({ 
        'libHarnessed', 
        'libCore'
    })

    processargs(testname, args)
end



-- GLOBAL BUILD CONFIGURATION
workspace("*")

-- All projects will have the following build targets available to them.
configurations({
    "Debug32", "Release32", "Final32",
    "Debug64", "Release64", "Final64"
})

location("Premake")

includedirs({ "Source/Include" })

filter("configurations:*32"); architecture("x86")
filter("configurations:*64"); architecture("x86_64")

filter("configurations:Debug32");              libdirs({ "Bin/x86/Debug"   })
filter("configurations:Release32 or Final32"); libdirs({ "Bin/x86/Release" })

filter("configurations:Debug64");              libdirs({ "Bin/x64/Debug"   })
filter("configurations:Release64 or Final64"); libdirs({ "Bin/x64/Release" })

