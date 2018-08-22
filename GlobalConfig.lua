------------------------------------------------------------------------------------------------------------------------
--
--  ElflordPP/GlobalConfig.lua
--
--  This file is part of the Elflord Engine. This file includes some global utility functions to be used by all scripts.
--
------------------------------------------------------------------------------------------------------------------------
--  Copyright (c) 2018 Miki Ryan
------------------------------------------------------------------------------------------------------------------------

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
	print("------------------------------------------------------------------------------------------------------------------------")
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