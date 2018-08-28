------------------------------------------------------------------------------------------------------------------------
--
--  ElflordPP/GlobalConfig.lua
--
--  This file is part of the Elflord Engine. This file includes some global utility functions to be used by all scripts.
--
------------------------------------------------------------------------------------------------------------------------
--  Copyright (c) 2018 Miki Ryan
------------------------------------------------------------------------------------------------------------------------

ENGINE_LIB_SOURCE_DIR  = "Source/Libraries"
ENGINE_APP_SOURCE_DIR  = "Source/Applications"
ENGINE_TST_SOURCE_DIR = "Source/Tests"
ENGINE_LIB_OUTPUT_DIR  = "Build/Libraries/%{cfg.architecture}/%{cfg.buildcfg}"
ENGINE_APP_OUTPUT_DIR = "Build/Applications/%{cfg.architecture}/%{cfg.buildcfg}"

ENGINE_LIBS = {}
ENGINE_GAME_LIBS = {}

local DIVIDER = 
"------------------------------------------------------------------------------------------------------------------------"


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

    print(DIVIDER)
    print("--", "Generating project for "..libName..".")
    print(DIVIDER)

    group("EngineLibs")
    project(libName)
    language("C++")
    cppdialect("C++17")
    kind("StaticLib")

    targetdir(ENGINE_LIB_OUTPUT_DIR)

    includedirs({ 
        ENGINE_LIB_SOURCE_DIR,
        "ThirdParty",
        "ThirdParty/rttr/src",
        "ThirdParty/glfw.git/include",
        "ThirdParty/angelscript/sdk/angelscript/include"
    })
    libdirs({ ENGINE_LIB_OUTPUT_DIR })

    pchheader("stdafx.h")
    pchsource(ENGINE_LIB_SOURCE_DIR.."/"..libName.."/stdafx.cpp")

    files({
        ENGINE_LIB_SOURCE_DIR.."/"..libName.."/**.h",
        ENGINE_LIB_SOURCE_DIR.."/"..libName.."/**.cpp"
    })
end

function configureApplication(appname)
end

function configureGame(gameName)
    clearFilters()

    print(DIVIDER)
    print("--", "Generating project for Game: "..gameName..".")
    print(DIVIDER)

    group("Games")
    project(gameName)
    language("C++")
    cppdialect("C++17")
    kind("ConsoleApp")

    targetdir(ENGINE_APP_OUTPUT_DIR)

    includedirs({ 
        ENGINE_APP_SOURCE_DIR,
        ENGINE_LIB_SOURCE_DIR,
        "ThirdParty",
        "ThirdParty/rttr/src",
        "ThirdParty/glfw.git/include",
        "ThirdParty/angelscript/sdk/angelscript/include"
    })
    libdirs({ ENGINE_APP_OUTPUT_DIR, ENGINE_LIB_OUTPUT_DIR })

    pchheader("stdafx.h")
    pchsource(ENGINE_APP_SOURCE_DIR.."/"..gameName.."/stdafx.cpp")

    addDependencies(ENGINE_GAME_LIBS)

    files({
        ENGINE_APP_SOURCE_DIR.."/"..gameName.."/**.h",
        ENGINE_APP_SOURCE_DIR.."/"..gameName.."/**.cpp"
    })
end

function configureUnitTest(libName)
    clearFilters()

    local tstsrcpath = ENGINE_TST_SOURCE_DIR.."/"..libName
    local libsrcpath = ENGINE_LIB_SOURCE_DIR

    local requiredDirectory = tstsrcpath
    print("--", ("Checking for required directory %q"):format(requiredDirectory))
    if not os.isdir(requiredDirectory) then
        error(libName.." is not a valid test. There is no source directory named "..requiredDirectory)
        return
    end

    local requiredFile = tstsrcpath.."/"..libName.."Tests.cpp"
    print("--", ("Checking for required unit test file %q"):format(requiredFile))
    if not os.isfile(requiredFile) then
        error(("%q is not a valid test. Missing file %q"):format(libName, requiredFile))
        return
    end

    group("UnitTests")
    project(libName.."_Test")

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
        libsrcpath.."/"..libName,
        "ThirdParty",
        "ThirdParty/rttr/src",
        "ThirdParty/glfw.git/include",
        "ThirdParty/angelscript/sdk/angelscript/include"
    })

    links({ 
        'libHarnessed', 
        'libCore',
        libName
    })
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
    return str:gsub('{'..k..'}', v)
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