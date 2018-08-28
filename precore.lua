------------------------------------------------------------------------------------------------------------------------
--
--  ElflordPP/precore.lua
--
--  This file is part of the Elflord Engine. This should be included before anything in premake5.lua is called.
--
------------------------------------------------------------------------------------------------------------------------
--  Copyright (c) 2018 Miki Ryan
------------------------------------------------------------------------------------------------------------------------

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

local function configureLibraryDirectories()
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
    libdirs({"Build/%{cfg.architecture}/%{cfg.buildconfig}"})
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
        "ThirdParty/rttr/src"
    })

    configureLibraryDirectories()

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

local function init(self, key, value)
	if type(self[key]) == 'nil' then
		self[key] = value
	end
end

local environment_mt = {
	GetWorkspaceName = function(self)
		return self.__workspaceName
	end;

	SetSourcePath = function(self, sourcePath)
		self.__sourcePath = sourcePath
	end;

	AddConfigs = function(self, configs)
		if not self.__configs then
			self.__configs = {}
		end

		for _, config in pairs(configs) do
			insertUnique(self.__configs, config)
		end
	end;

	AddProject = function(self, projectName)
		init(self, "__projects", {})
		insertUnique(self.__projects, projectName)
	end;

	AddGlobalIncludeDirs = function(self, dirs)
		if not self.__globalIncludes then
			self.__globalIncludes = {}
		end
		for _, dir in pairs(dirs) do
			if os.isdir(dir) then
				insertUnique(self.__globalIncludes, dir)
			end
		end
	end;

	AddIncludeDirs = function(self, configs, dirs)
		init(self, "__includesForConfigs", {})
		init(self, "__configs", {})

		for _, config in pairs(configs) do
			if not valueExists(self.__configs, config) then
				error("could not add include directories for unknown configuration", config)
			end

			init(self.__includesForConfigs, config, {})

			for _, dir in pairs(dirs) do
				if os.isdir(dir) then
					insertUnique(self.__includesForConfigs[config], dir)
				end
			end
		end
	end;

	AddGlobalLibDirs = function(self, libDirs)
		init(self, "__globalLibDirs", {})

		for _, libDir in pairs(libDirs) do
			insertUnique(self.__globalLibDirs, libDir)
		end
	end;
	
	AddLibDirs = function(self, configs, libDirs)
		init(self, "__libDirsForConfigs", {})
		init(self, "__configs", {})
		for _, config in pairs(configs) do
			if not valueExists(self.__configs, config) then
				error("could not add lib directory for unknown configuration", config)
			end
			init(self.__libDirsForConfigs, config, {})
			for _, dir in pairs(libDirs) do
				
			end
		end
	end;

	HasUnitTests = function(self)
		self.__generateUnitTests = true
	end;

	Generate = function(self)
		clearFilters()

		workspace(self:GetWorkspaceName())
		
	end;
}
environment_mt.__index = environment_mt

function createEnvironment(workspaceName)
    local output = {
        __workspaceName = workspaceName;
        --[[StaticLibSourcePath = "Source/Libraries"; -- the directory where static lib code directories are held.
        StaticLibProjects = {}; -- a list of the names of all of the static libraries being built.
        StaticLibUnitTests = {}; -- a list of the static libs that have unit tests defined.
        Configs = {}; -- The configs for this environment.
        IncludeDirs = {};]]
    }

    -- gather the names of all of the first party static library projects.
    local libraryDirectories = os.matchdirs(librarySourcePath.."/*")
    for _, dirPath in pairs(libraryDirectories) do
		local libname = path.getname(dirPath)
        table.insert(output.StaticLibProjects, libname)
		if os.isdir(dirPath.."/Tests") then
			table.insert(output.StaticLibUnitTests, libname)
		end
    end

    return setmetatable(output, environment_mt)
end

function addConfiguration(env, configs)
    if type(configs) == "table" then
        for _, v in ipairs(configs) do
            env.Configs[v] = v
        end
    end
end

------------------------------------------------------------------------------------------------------------------------
-- Adds a third party library to the build environment.
function addThirdPartyLib(env, libName)
    workspace(env.Workspace)
end

------------------------------------------------------------------------------------------------------------------------
-- Creates the third party library projects.
function configureThirdPartyLibraries(environment)
end

------------------------------------------------------------------------------------------------------------------------
-- Creates the static library projects.
function configureStaticLibraries(env)
	
end

------------------------------------------------------------------------------------------------------------------------
-- Configures the unit test project.
function configureUnitTests(env)
	print("------------------------------------------------------------------------------------------------------------------------")
    print("--", "Defining Unit Test Application")

    clearFilters()
    -- Define the unit test application.
    workspace(env.Workspace)
    group("Unit Tests")
    project("UnitTests")
    language("C++")
    cppdialect("C++17")
    kind("Application")
    targetdir("Build/Applications/%{cfg.buildcfg}")

    -- Global variables for finding the locations of certain required files.
    local libraryPathFormatter = env.StaticLibSourcePath.."/{Project}"
    local unitTestPathFormatter = libraryPathFormatter.."/Tests"
    local sourcePattern = "{SourceDir}/**.cpp"
	local headerPattern = "{SourceDir}/**.h"

    -- first, gather the names of all of the directories in the source path by their directory names.
    for _, staticLibName in pairs(env.StaticLibUnitTests) do
        local staticLibTestDirPath = formatString(unitTestPathFormatter, { Project = staticLibName })
        if os.isdir(staticLibTestDirPath) then
            files({
                formatString(sourcePattern, {SourceDir = staticLibTestDirPath}),
                formatString(sourcePattern, {SourceDir = staticLibTestDirPath})
            })

            -- required for precompiled headers.
            includedirs({ 
                formatString(libraryPathFormatter, {Project = staticLibName})
            })
			includedirs(env.GlobalIncludeDirs)
        else
            print("could not add unit test sources for static library", staticLibName, "in directory", staticLibTestDirPath)
        end
    end

    -- first, enumerate the directories in the library source path.
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
