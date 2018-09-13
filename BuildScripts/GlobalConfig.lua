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


-- gather the names of all of the first party static library projects.
do
    local libraryDirectories = os.matchdirs(ENGINE_LIB_SOURCE_DIR.."/*")
    for _, dirPath in pairs(libraryDirectories) do
        local libname = path.getname(dirPath)

        if libname ~= "libHarnessed" then
            table.insert(ENGINE_GAME_LIBS, libname)
        end
        table.insert(ENGINE_LIBS, libname)
    end
end

function configureEngineLib(libName)
    clearFilters()

    print("--", "Generating project for "..libName..".")

    group("EngineLibs")
    project(libName)
    language("C++")
    cppdialect("C++17")
    kind("StaticLib")

    targetdir(ENGINE_BIN_OUTPUT_DIR)

    includedirs({
        ENGINE_LIB_SOURCE_DIR,
        THIRD_PARTY_SRC_DIR,
        THIRD_PARTY_SRC_DIR.."/rttr/src",
        THIRD_PARTY_SRC_DIR.."/glfw/include",
        THIRD_PARTY_SRC_DIR.."/LLGL/include",
        THIRD_PARTY_SRC_DIR.."/angelscript/sdk/angelscript/include"
    })
    libdirs({ ENGINE_BIN_OUTPUT_DIR })

    pchheader("stdafx.h")
    pchsource(ENGINE_LIB_SOURCE_DIR.."/"..libName.."/stdafx.cpp")

    files({
        ENGINE_LIB_SOURCE_DIR.."/"..libName.."/**.h",
        ENGINE_LIB_SOURCE_DIR.."/"..libName.."/**.cpp"
    })
    links({"rttr"})
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

    includedirs({
        ENGINE_APP_SOURCE_DIR,
        ENGINE_LIB_SOURCE_DIR,
        THIRD_PARTY_SRC_DIR,
        THIRD_PARTY_SRC_DIR.."/rttr/src",
        THIRD_PARTY_SRC_DIR.."/glfw/deps",
        THIRD_PARTY_SRC_DIR.."/glfw/include",
        THIRD_PARTY_SRC_DIR.."/LLGL/include",
        THIRD_PARTY_SRC_DIR.."/angelscript/sdk/angelscript/include"
    })
    libdirs({ ENGINE_BIN_OUTPUT_DIR })

    pchheader("stdafx.h")
    pchsource(ENGINE_APP_SOURCE_DIR.."/"..gameName.."/stdafx.cpp")

    addDependencies(ENGINE_GAME_LIBS)
    links({
        "lib"..gameName
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

    includedirs({
        ENGINE_APP_SOURCE_DIR,
        ENGINE_LIB_SOURCE_DIR,
        THIRD_PARTY_SRC_DIR,
        THIRD_PARTY_SRC_DIR.."/rttr/src",
        THIRD_PARTY_SRC_DIR.."/glfw/deps",
        THIRD_PARTY_SRC_DIR.."/glfw/include",
        THIRD_PARTY_SRC_DIR.."/LLGL/include",
        THIRD_PARTY_SRC_DIR.."/angelscript/sdk/angelscript/include"
    })
    libdirs({ ENGINE_BIN_OUTPUT_DIR })

    pchheader("stdafx.h")
    pchsource(ENGINE_APP_SOURCE_DIR.."/"..gameName.."/stdafx.cpp")

    addDependencies(ENGINE_GAME_LIBS)
    links({
        "lib"..gameName
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

    includedirs({
        ENGINE_APP_SOURCE_DIR,
        ENGINE_LIB_SOURCE_DIR,
        THIRD_PARTY_SRC_DIR,
        THIRD_PARTY_SRC_DIR.."/rttr/src",
        THIRD_PARTY_SRC_DIR.."/glfw/deps",
        THIRD_PARTY_SRC_DIR.."/glfw/include",
        THIRD_PARTY_SRC_DIR.."/LLGL/include",
        THIRD_PARTY_SRC_DIR.."/angelscript/sdk/angelscript/include"
    })
    libdirs({
        ENGINE_BIN_OUTPUT_DIR
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
        if hasUnitTest(libName) then
            local nameStr = libName.."PrepareHarness"
            table.insert(INCLUDES, "#include <"..libName.."/"..libName..".h>")
            table.insert(FWD_DECLARES, "RefPtr<TestHarness> "..nameStr.."(int ac, char** av);")
            table.insert(TEST_FUNCTIONS, "        "..nameStr.."(ac, av)")
            table.insert(LIB_INITIALIZATIONS, "    "..libName.."::Initialize(ac,av);")
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

    includedirs({
        ENGINE_LIB_SOURCE_DIR,
        THIRD_PARTY_SRC_DIR,
        THIRD_PARTY_SRC_DIR.."/rttr/src",
        THIRD_PARTY_SRC_DIR.."/glfw/include",
        THIRD_PARTY_SRC_DIR.."/glfw/deps",
        THIRD_PARTY_SRC_DIR.."/LLGL/include",
        THIRD_PARTY_SRC_DIR.."/angelscript/sdk/angelscript/include"
    })

    files({
        ENGINE_TST_SOURCE_DIR.."/**.h",
        ENGINE_TST_SOURCE_DIR.."/**.cpp"
    })
    links(ENGINE_LIBS)
    links({
        "angelscript",
        "glfw",
        "imgui",
        "jsoncpp",
        "physfs",
        "rttr"
    })

    local p = path.getabsolute(ASSETS_DIR)
    print("Path To Assets:", p)
    debugargs({
        "--AssetsDir="..p,
        "--AppName=UnitTest"
    })

    filter("configurations:Debug64")
        libdirs({
            THIRD_PARTY_SRC_DIR.."/LLGL/build/build/Debug"
        })
        links({ "LLGLD" })

    filter("configurations:Release64")
        libdirs({
            THIRD_PARTY_SRC_DIR.."/LLGL/build/build/Release"
        })
        links({ "LLGL" })
    clearFilters()

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
    links(deps)
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
