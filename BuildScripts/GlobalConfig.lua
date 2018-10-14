------------------------------------------------------------------------------------------------------------------------
--
--  ElflordPP/GlobalConfig.lua
--
--  This file is part of the Elflord Engine. This file includes some global utility functions to be used by all scripts.
--
------------------------------------------------------------------------------------------------------------------------
--  Copyright (c) 2018 Miki Ryan
------------------------------------------------------------------------------------------------------------------------

include("UnitTestMainTemplate")

ENGINE_LIB_SOURCE_DIR  = "../Source/Libraries"
ENGINE_APP_SOURCE_DIR  = "../Source/Applications"
ENGINE_TST_SOURCE_DIR  = "../Source/Tests"
ENGINE_BIN_OUTPUT_DIR  = "../Build/%{cfg.architecture}/%{cfg.buildcfg}"

THIRD_PARTY_SRC_DIR = "../ThirdParty"
ASSETS_DIR = "../Assets"

ENGINE_LIBS = {}
ENGINE_GAME_LIBS = {}

local DIVIDER =
"------------------------------------------------------------------------------------------------------------------------"

local WINDOWS_SDK_VERSION = "10.0.171340"

local ENGINE_IGNORES = {
    libHarnessed = true,
    libJson = true
}
local LIB_IGNORES = {
    libJson = true
}

-- gather the names of all of the first party static library projects.
do
    local libraryDirectories = os.matchdirs(ENGINE_LIB_SOURCE_DIR.."/*")
    for _, dirPath in pairs(libraryDirectories) do
        local libname = path.getname(dirPath)

        if not ENGINE_IGNORES[libname] then
            table.insert(ENGINE_GAME_LIBS, libname)
        end
        if not LIB_IGNORES[libname] then
            table.insert(ENGINE_LIBS, libname)
        end
    end
end

-- A list of all of the third party libraries that the application executable needs to see.
THIRD_PARTY_LIBS = {
    "angelscript",
    "imgui",
    "jsoncpp",
    "physfs",
    "rttr",
    "EASTL"
}

THIRD_PARTY_INCLUDE_DIRS = {
    THIRD_PARTY_SRC_DIR,
    THIRD_PARTY_SRC_DIR.."/rttr/src",
    THIRD_PARTY_SRC_DIR.."/glfw/include",
    THIRD_PARTY_SRC_DIR.."/glfw/deps",
    THIRD_PARTY_SRC_DIR.."/angelscript/sdk/angelscript/include",
    THIRD_PARTY_SRC_DIR.."/EASTL",
    THIRD_PARTY_SRC_DIR.."/EASTL/Packages/EABase/include/Common",
    THIRD_PARTY_SRC_DIR.."/EASTL/Packages/EAAssert/include",
    THIRD_PARTY_SRC_DIR.."/EASTL/Packages/EAStdC/include",
}

-- Check for all of the third party libraries that are not distributed with this.
VK_SDK_PATH = ""
VK_SDK_INCLUDES = ""
VK_SDK_LIB = "vulkan-1"
do
    print()
    print("-- Checking for required SDKs.")
    print("-- Checking for Vulkan SDK install...")
    VK_SDK_PATH = os.getenv("VK_SDK_PATH")
    if VK_SDK_PATH == nil then
        error("!! The Vulkan SDK is not installed on this machine (VK_SDK_PATH environment var is not set). "..
        "The build can not continue.\n"..
        "Please download the Vulkan SDK at https://www.lunarg.com/vulkan-sdk/")
    end
    print("++ Vulkan SDK found at "..VK_SDK_PATH..". Setting paths for build script.")
    
    VK_SDK_INCLUDES = VK_SDK_PATH.."/include"
    VK_SDK_LIBS = VK_SDK_PATH.."/Lib"
    table.insert(THIRD_PARTY_INCLUDE_DIRS, VK_SDK_INCLUDES)

    print()

    print("All required SDKs are found. Ready to rock.")
    print()
end


-- Set up common include directories for all Firestorm libraries.
function COMMON_ENGINE_INCLUDE_DIRS()
    includedirs({
        ENGINE_LIB_SOURCE_DIR,
    })
    includedirs(THIRD_PARTY_INCLUDE_DIRS)
end

-- Set up common library locations for all Firestorm libraries.
function COMMON_ENGINE_LIB_DIRS()
    libdirs({ 
        ENGINE_BIN_OUTPUT_DIR,
        VK_SDK_LIBS
    })
end

function COMMON_ENGINE_LIB_DEFINES()
    clearFilters()
    if _OPTIONS["gfxapi"] == "Vulkan" then
        defines({
            "FIRE_GFX_VULKAN"
        })
    end
end

function COMMON_ENGINE_APP_LIBS()
    links(ENGINE_LIBS)
    links(THIRD_PARTY_LIBS)

    if _OPTIONS["gfxapi"] == "Vulkan" then
        links({ 
            "glfw",
            VK_SDK_LIB
        })
    end
end

function configureEngineLib(libName)
    clearFilters()

    print("--", "Generating project for "..libName..".")

    group("Engine")
    project(libName)
    language("C++")
    cppdialect("C++17")
    kind("StaticLib")

    targetdir(ENGINE_BIN_OUTPUT_DIR)

    COMMON_ENGINE_LIB_DEFINES()
    COMMON_ENGINE_INCLUDE_DIRS()
    COMMON_ENGINE_LIB_DIRS()

    pchheader("stdafx.h")
    pchsource(ENGINE_LIB_SOURCE_DIR.."/"..libName.."/stdafx.cpp")

    files({
        ENGINE_LIB_SOURCE_DIR.."/"..libName.."/**.h",
        ENGINE_LIB_SOURCE_DIR.."/"..libName.."/**.cpp"
    })
    dependson({
        "rttr",
        "EASTL"
    })
end

function configureToolsApplication(appName, gameName)
    clearFilters()

    print("--", "Generating projects for tools application: "..appName..".")

    group("Tools")
    project(appName)
    language("C++")
    cppdialect("C++17")
    kind("ConsoleApp")

    targetdir(ENGINE_BIN_OUTPUT_DIR)

    COMMON_ENGINE_LIB_DEFINES()
    -- This is not in COMMON_INCLUDES because we don't want the core engine projects to see the game lib projects.
    includedirs({
        ENGINE_APP_SOURCE_DIR
    })
    COMMON_ENGINE_INCLUDE_DIRS()
    COMMON_ENGINE_LIB_DIRS()
    COMMON_ENGINE_APP_LIBS()

    pchheader("stdafx.h")
    pchsource(ENGINE_APP_SOURCE_DIR.."/"..gameName.."/stdafx.cpp")

    addDependencies(ENGINE_GAME_LIBS)
    links({
        "lib"..gameName,
    })
    dependson({
        "rttr",
        "EASTL"
    })

    files({
        ENGINE_APP_SOURCE_DIR.."/"..gameName.."/**.h",
        ENGINE_APP_SOURCE_DIR.."/"..gameName.."/**.cpp"
    })
    clearFilters()
    local p = path.getabsolute(ASSETS_DIR)
    print("Path To Assets:", p)
    debugargs({
        "--AssetsDir="..p,
        "--AppName="..gameName
    })
    debugdir(ENGINE_BIN_OUTPUT_DIR)
end

function configureGame(gameName)
    clearFilters()

    print("--", "Generating projects for Game: "..gameName..".")

    group("Games")
    project(gameName)
    language("C++")
    cppdialect("C++17")
    kind("ConsoleApp")

    targetdir(ENGINE_BIN_OUTPUT_DIR)

    COMMON_ENGINE_LIB_DEFINES()
    -- This is not in COMMON_INCLUDES because we don't want the core engine projects to see the game lib projects.
    includedirs({
        ENGINE_APP_SOURCE_DIR
    })
    COMMON_ENGINE_INCLUDE_DIRS()
    COMMON_ENGINE_LIB_DIRS()
    COMMON_ENGINE_APP_LIBS()

    pchheader("stdafx.h")
    pchsource(ENGINE_APP_SOURCE_DIR.."/"..gameName.."/stdafx.cpp")

    addDependencies(ENGINE_GAME_LIBS)
    links({
        "lib"..gameName
    })
    dependson({
        "rttr",
        "EASTL"
    })

    files({
        ENGINE_APP_SOURCE_DIR.."/"..gameName.."/**.h",
        ENGINE_APP_SOURCE_DIR.."/"..gameName.."/**.cpp"
    })
    clearFilters()

    local p = path.getabsolute(ASSETS_DIR)
    print("Path To Assets:", p)
    debugargs({
        "--AssetsDir="..p,
        "--AppName="..gameName
    })
    debugdir(ENGINE_BIN_OUTPUT_DIR)
end

function configureGameLib(gameName)
    clearFilters()

    print("--", "Generating projects for Game library: lib"..gameName..".")

    group("Games")
    project("lib"..gameName)
    language("C++")
    cppdialect("C++17")
    kind("StaticLib")

    targetdir(ENGINE_BIN_OUTPUT_DIR)

    COMMON_ENGINE_LIB_DEFINES()
    includedirs({
        ENGINE_APP_SOURCE_DIR
    })
    COMMON_ENGINE_INCLUDE_DIRS()
    COMMON_ENGINE_LIB_DIRS()

    dependson({
        "rttr",
        "EASTL"
    })

    pchheader("stdafx.h")
    pchsource(ENGINE_APP_SOURCE_DIR.."/lib"..gameName.."/stdafx.cpp")

    addDependencies(ENGINE_GAME_LIBS)

    files({
        ENGINE_APP_SOURCE_DIR.."/lib"..gameName.."/**.h",
        ENGINE_APP_SOURCE_DIR.."/lib"..gameName.."/**.cpp"
    })
end

function configureUnitTestApplication()
    local function hasUnitTest(libName)
        return os.isfile(ENGINE_TST_SOURCE_DIR.."/"..libName.."Tests.cpp")
    end
    -- if the engine lib has a unit test associated with it, we should
    -- pull those sources in as well.
    clearFilters()

    local FWD_DECLARES = {}
    local TEST_FUNCTIONS = {}
    local LIB_INITIALIZATIONS = {}
    local INCLUDES = {}

    for _, libName in ipairs(ENGINE_LIBS) do
        table.insert(INCLUDES, "#include <"..libName.."/"..libName..".h>")
        table.insert(LIB_INITIALIZATIONS, "    Library<::Firestorm::"..libName..">::Initialize(ac,av);")
        if hasUnitTest(libName) then
            local nameStr = libName.."PrepareHarness"
            table.insert(FWD_DECLARES, "RefPtr<TestHarness> "..nameStr.."(int ac, char** av);")
            table.insert(TEST_FUNCTIONS, "        "..nameStr.."(ac, av)")
        end
    end

    local mainCPP = UNIT_TEST_MAIN

    local includes = table.concat(INCLUDES, "\n")
    local fwdDeclareStrings = table.concat(FWD_DECLARES, "\n")
    local testFunctionStrings = table.concat(TEST_FUNCTIONS, ",\n")
    local libInitializations = table.concat(LIB_INITIALIZATIONS, "\n")

    mainCPP = formatString(mainCPP, {
        INCLUDES = includes,
        FWD_DECLARES = fwdDeclareStrings,
        TEST_FUNCTIONS = testFunctionStrings,
        LIB_INITIALIZATIONS = libInitializations
    })

    io.writefile("../Source/Tests/main.cpp", mainCPP)

    -- Now generate the unit test project.
    group("UnitTests")
    project("UnitTest")

    language("C++")
    cppdialect("C++17")
    kind("ConsoleApp")
    
    targetdir(ENGINE_BIN_OUTPUT_DIR)

    COMMON_ENGINE_LIB_DEFINES()
    includedirs({
        ENGINE_LIB_SOURCE_DIR,
    })
    COMMON_ENGINE_INCLUDE_DIRS()
    COMMON_ENGINE_LIB_DIRS()
    COMMON_ENGINE_APP_LIBS()

    files({
        ENGINE_TST_SOURCE_DIR.."/**.h",
        ENGINE_TST_SOURCE_DIR.."/**.cpp"
    })

    disablewarnings({
        "4267" -- size_t to int conversion could truncate result. encountered in 64 bit builds.
    })

    local p = path.getabsolute(ASSETS_DIR)
    print("Path To Assets:", p)
    debugargs({
        "--AssetsDir="..p,
        "--AppName=UnitTest"
    })

    debugdir(ENGINE_BIN_OUTPUT_DIR)
end


------------------------------------------------------------------------------------------------------------------------
-- Calls filter({}) to go back to the default global filter. Wrapped so that the intent is more clear.
function clearFilters()
    filter({})
end

------------------------------------------------------------------------------------------------------------------------
-- Marks another first party library as a dependency, adding the header search path as well as marking a link against
-- the built library.
function addDependencies(deps)
    local includeDirs = {}
    for _, dep in ipairs(deps) do
        table.insert(includeDirs, ENGINE_LIB_SOURCE_DIR.."/"..dep)
    end
    includedirs(includeDirs)
    dependson(deps)
end

------------------------------------------------------------------------------------------------------------------------
-- Replaces a {} delimited key with the associated value.
-- EX: replaceDelim("{foo}", "foo", "a long and winding road") will return the string "a long and winding road.
function replaceDelim(str, k, v)
    return str:gsub('{'..k..'}', tostring(v))
end

------------------------------------------------------------------------------------------------------------------------

------------------------------------------------------------------------------------------------------------------------
-- Function: formatString
--
-- Replaces items in a string delimited with {} with the keys from a table.
-- EX: formatString("{foo} = 5", { foo = 6 }) will return the string 6 = 5.
function formatString(str, kvTable)
	local outStr = str
	for k, v in pairs(kvTable) do
		outStr = replaceDelim(outStr, k, v)
	end
	return outStr
end


------------------------------------------------------------------------------------------------------------------------
-- Function: getDirItems
--
-- Retrieves all of the items in the specified directory.
function getDirItems(search)
	search = search or "*"

	local output = {}
	output.dirs = os.matchdirs(search)
	output.files = os.matchfiles(search)

	return output
end

function valueExists(tbl, val)
	for _, v in pairs(tbl) do
		if v == val then return true end
	end
	return false
end

function insertUnique(tbl, val)
	if not valueExists(tbl, val) then
		table.insert(tbl, val)
	end
end

-- Some tests on the formatter.
do
	local function printErr(expected, got)
		print("/!\\ ERROR /!\\ Expected:", expected, "Got:", got)
	end
	local testTable = {
		foo = 8,
		bar = "the future"
	}
	-- strings to test formatting paired with their expected results.
	local testStrings = {
		{ "{foo} = 5", testTable.foo.." = 5" },
		{ "{foo}/{bar}/{foo}", testTable.foo.."/"..testTable.bar.."/"..testTable.foo },
		{ "\"{foo}\"", "\""..testTable.foo.."\"" }
	}
	print(DIVIDER)
	print("-- Testing lua string formatter...")
	local numFailed = 0
	for _, testItem in ipairs(testStrings) do
		local str = testItem[1]
		local expected = testItem[2]
		str = formatString(str, testTable)
		if str ~= expected then
			printErr(expected, str)
			numFailed = numFailed + 1
		end
	end

	-- ensure that replaceDelim leaves the original string alone.
	local testString = "{key}"
	local modifiedString = replaceDelim(testString, "key", "brown foxes and all that")
	if modifiedString ~= "brown foxes and all that" then
		printErr("brown foxes and all that", modifiedString)
		numFailed = numFailed + 1
	end

	if testString ~= "{key}" then
		printErr("{key}", testString)
		numFailed = numFailed + 1
	end

    if numFailed > 0 then
		error("one or more string formatting tests failed...")
	else
		print("    all tests passed")
	end
end
