------------------------------------------------------------------------------------------------------------------------
--
--  ElflordPP/precore.lua
--
--  This file is part of the Elflord Engine. This should be included before anything in premake5.lua is called.
--
------------------------------------------------------------------------------------------------------------------------
--  Copyright (c) 2018 Miki Ryan
------------------------------------------------------------------------------------------------------------------------
local function clearFilters()
    filter({})
end

local function replaceDelim(str, k, v)
    return str:gsub('{'..k..'}', v)
end
local function processargs(args, stringReplacers)
    -- additional build configuration.
    if type(args) == 'table' then
        if type(args.Dependencies) == 'table' then
            links(args.Dependencies)
        end

        if type(args.IncludeDirectories) == 'table' then
            -- Replace the key {path} with the path to the project.
            for i, v in ipairs(args.IncludeDirectories) do
                local old = v
                for k, vv in pairs(stringReplacers) do
                    args.IncludeDirectories[i] = args.IncludeDirectories[i]:gsub('{'..k..'}', vv)
                    --args.IncludeDirectories[i] = replaceDelim(args.IncludeDirectories[i], k, vv)
                end
            end

            includedirs(args.IncludeDirectories)
        end

        if type(args.PostArgProcessing) == 'function' then
            args.PostArgProcessing()
        end

        if type(args.UsePCH) == 'nil' or (type(args.UsePCH) == 'boolean' and args.UsePCH) then
            includedirs({
                stringReplacers.SourcePath
            })
            filter("action:vs*")
                pchheader("stdafx.h")
                --pchsource(libsrcpath.."/"..libname.."/stdafx.cpp")
                pchsource(stringReplacers.SourcePath.."/stdafx.cpp")

            filter("action:not vs*")
                pchheader("stdafx.h")
        end
    end
end

local function printverbatem(tbl, k)
    print("--", " ", k, tbl[k], "("..type(tbl[k])..")")
end

local function printarrayverbatem(tbl, k)
    if type(tbl[k]) == 'table' then
        print("--", k)
        for i, v in ipairs(tbl[k]) do
            print("--", "  ", v)
        end
    elseif type(tbl[k]) ~= 'nil' then
        print("--", k, "INVALID! (array table required)")
    else
        print("--", k, "not found...")
    end
end

local function printexists(tbl, k)
    if type(tbl[k]) ~= 'nil' then
        print("--", k, "found!")
    else
        print("--", k, "not found...")
    end
end
local s_argsKeys = {
    { 'Dependencies',       printarrayverbatem },
    { 'IncludeDirectories', printarrayverbatem },
    { 'PostArgProcessing',  printexists        }
}
local function printargs(args)
    if args then
        assert(type(args) == 'table', 'Expected table. Got '..type(args))
        for i, v in ipairs(s_argsKeys) do
            v[2](args, v[1])
        end
    else
        print("--", "  ", "No build args.")
    end
end

function staticlib(libname, args)
    print("------------------------------------------------------------------------------------------------------------------------")
    print("--", ("Defining Static Library %q"):format(libname))

    printargs(args)

    clearFilters() 

    local libsrcpath = "Source/Libraries"

    local requiredDirectory = libsrcpath.."/"..libname
    print("--", ("Checking for staticlib directory %q"):format(requiredDirectory))
    if not os.isdir(requiredDirectory) then
        error(libname.." is not a valid staticlib setup. There is no source directory named "..requiredDirectory)
        return
    end

    --[[if not os.isdir(requiredDirectory.."/Source") then
        error(libname.." is not a valid staticlib setup. There is no directory for source code in it named "..requiredDirectory.."/Source")
        return
    end

    if not os.isdir(requiredDirectory.."/Include") then
        error(libname.." is not a valid staticlib setup. There is no directory for public headers named "..requiredDirectory.."/Include")
        return
    end]]

    -- Maybe have this check for the public header?
    --[[local requiredFile = libsrcpath.."/"..libname.."Tests.cpp"
    print("Checking for required unit test file", requiredFile)
    if not os.isfile(requiredFile) then
        error(testname.." is not a valid test. Missing file "..requiredFile)
        return
    end]]

    group("Libraries")

    project(libname)
    language("C++")
    cppdialect("C++17")
    kind("StaticLib")
    targetdir("Build/Libraries/%{cfg.buildcfg}")

    files({
        libsrcpath.."/"..libname.."/**.h",   -- all header files
        libsrcpath.."/"..libname.."/**.cpp", -- all source files
    })

    -- required for precompiled headers.
    includedirs({ 
        libsrcpath,
        libsrcpath.."/"..libname,
        "ThirdParty/rttr/src",
        "ThirdParty/rttr/build/src",
    })
    filter("configurations:Debug32")
        libdirs({ "ThirdParty/rttr/x86/lib/Debug" })
        includedirs({ "ThirdParty/rttr/x86/src" })
    filter("configurations:Release32 or Final32");
        libdirs({ "ThirdParty/rttr/x86/lib/Release" })
        includedirs({ "ThirdParty/rttr/x86/src" })

    filter("configurations:Debug64")
        libdirs({ "ThirdParty/rttr/x64/lib/Debug" })
        includedirs({ "ThirdParty/rttr/x64/src" })
        
    filter("configurations:Release64 or Final64");
        libdirs({ "ThirdParty/rttr/x64/lib/Release" })
        includedirs({ "ThirdParty/rttr/x64/src" })

    --[[filter("action:vs*")
        pchheader("stdafx.h")
        pchsource(libsrcpath.."/"..libname.."/stdafx.cpp")

    filter("action:not vs*")
        pchheader("stdafx.h")]]

    clearFilters()

    processargs(args, {
        Project       = libname;
        LibSourcePath = libsrcpath;
        SourcePath    = libsrcpath.."/"..libname;
    })

    print("------------------------------------------------------------------------------------------------------------------------")
    print()
end

function unittest(testname, args)
    print("------------------------------------------------------------------------------------------------------------------------")
    print("--", ("Defining Unit Test %q"):format(testname))
    printargs(args)

    clearFilters()

    local tstsrcpath = "Source/Tests/"..testname
    local libsrcpath = "Source/Libraries"

    local requiredDirectory = tstsrcpath
    print("--", ("Checking for required directory %q"):format(requiredDirectory))
    if not os.isdir(requiredDirectory) then
        error(testname.." is not a valid test. There is no source directory named "..requiredDirectory)
        return
    end

    local requiredFile = tstsrcpath.."/"..testname.."Tests.cpp"
    print("--", ("Checking for required unit test file %q"):format(requiredFile))
    if not os.isfile(requiredFile) then
        error(("%q is not a valid test. Missing file %q"):format(testname, requiredFile))
        return
    end

    group("UnitTests")
    project(testname.."_Test")

    language("C++")
    cppdialect("C++17")
    kind("ConsoleApp")

    targetdir("Build/Tests/%{cfg.buildcfg}")

    files({
        tstsrcpath.."/**.h",
        tstsrcpath.."/**.cpp"
    })

    includedirs({ 
        libsrcpath,
        libsrcpath.."/"..testname,
        "ThirdParty/rttr/src",
        --"thirdparty/rttr/build/src",
    })

    filter("configurations:Debug32")
        libdirs({ "ThirdParty/rttr/x86/lib/Debug" })
        includedirs({ "ThirdParty/rttr/x86/src" })
    filter("configurations:Release32 or Final32");
        libdirs({ "ThirdParty/rttr/x86/lib/Release" })
        includedirs({ "ThirdParty/rttr/x86/src" })

    filter("configurations:Debug64")
        libdirs({ "ThirdParty/rttr/x64/lib/Debug" })
        includedirs({ "ThirdParty/rttr/x64/src" })
        
    filter("configurations:Release64 or Final64");
        libdirs({ "ThirdParty/rttr/x64/lib/Release" })
        includedirs({ "ThirdParty/rttr/x64/src" })

    clearFilters()

    links({ 
        'libHarnessed', 
        'libCore'
    })

    processargs(args, {
        Project       = testname;
        LibSourcePath = libsrcpath;
        SourcePath    = "Source/Tests";
    })

    print("------------------------------------------------------------------------------------------------------------------------")
    print()
end

function application(appname, args)
    print("------------------------------------------------------------------------------------------------------------------------")
    print("--", ("Defining Application %q"):format(appname))

    printargs(args)

    clearFilters() 

    local srcpath = "Source/Applications"

    local requiredDirectory = srcpath.."/"..libname
    print("--", ("Checking for application directory %q"):format(requiredDirectory))
    if not os.isdir(requiredDirectory) then
        error(libname.." is not a valid application setup. There is no source directory named "..requiredDirectory)
        return
    end

    --[[if not os.isdir(requiredDirectory.."/Source") then
        error(libname.." is not a valid staticlib setup. There is no directory for source code in it named "..requiredDirectory.."/Source")
        return
    end

    if not os.isdir(requiredDirectory.."/Include") then
        error(libname.." is not a valid staticlib setup. There is no directory for public headers named "..requiredDirectory.."/Include")
        return
    end]]

    -- Maybe have this check for the public header?
    --[[local requiredFile = libsrcpath.."/"..libname.."Tests.cpp"
    print("Checking for required unit test file", requiredFile)
    if not os.isfile(requiredFile) then
        error(testname.." is not a valid test. Missing file "..requiredFile)
        return
    end]]

    group("Applications")

    project(appname)
    language("C++")
    cppdialect("C++17")
    kind("Application")
    targetdir("Build/Applications/%{cfg.buildcfg}")

    files({
        srcpath.."/"..appname.."/**.h",   -- all header files
        srcpath.."/"..appname.."/**.cpp", -- all source files
    })


    -- required for precompiled headers.
    includedirs({ 
        srcpath,
        srcpath.."/"..appname,
        "ThirdParty/rttr/src",
        "ThirdParty/rttr/build/src",
    })

    filter("action:vs*")
        pchheader("stdafx.h")
        pchsource(srcpath.."/"..appname.."/stdafx.cpp")

    filter("action:not vs*")
        pchheader("stdafx.h")

    filter("configurations:Debug32")
        libdirs({ "ThirdParty/rttr/x86/lib/Debug" })
        includedirs({ "ThirdParty/rttr/x86/src" })
    filter("configurations:Release32 or Final32");
        libdirs({ "ThirdParty/rttr/x86/lib/Release" })
        includedirs({ "ThirdParty/rttr/x86/src" })

    filter("configurations:Debug64")
        libdirs({ "ThirdParty/rttr/x64/lib/Debug" })
        includedirs({ "ThirdParty/rttr/x64/src" })
        
    filter("configurations:Release64 or Final64");
        libdirs({ "ThirdParty/rttr/x64/lib/Release" })
        includedirs({ "ThirdParty/rttr/x64/src" })

    clearFilters()

    args.UsePCH = false
    processargs(args, {
        Project       = appname;
        LibSourcePath = srcpath;
        SourcePath    = srcpath.."/"..appname;
    })

    print("------------------------------------------------------------------------------------------------------------------------")
    print()
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

--filter("configurations:*32"); architecture("x86")
filter("configurations:*64")
    architecture("x86_64")

--filter(

filter("action:vs*")
defines({ "ELF_PLATFORM_WINDOWS", "ELF_VISUALSTUDIO" })

filter("action:xcode*")
defines({ "ELF_PLATFORM_OSX", "ELF_XCODE" })

filter("action:gmake*")
defines({ "ELF_PLATFORM_UNIX" })
--defines({ "ELF_PLATFORM_ANDROID" })
--defines({ "ELF_PLATFORM_IOS"     })

filter("configurations:Debug32");
    --libdirs({ "Bin/x86/Debug" })
filter("configurations:Release32 or Final32")
    --libdirs({ "Bin/x86/Release" })

filter("configurations:Debug64")
    --libdirs({ "Bin/x64/Debug" })
filter("configurations:Release64 or Final64");
    --libdirs({ "Bin/x64/Release" })
clearFilters()