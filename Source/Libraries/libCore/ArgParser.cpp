///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  ArgParser
//
//  Parses command line arguments.
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  Copyright (c) Project Elflord 2018
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "ArgParser.h"
#include "libCore.h"

#include <istream>
#include <sstream>

OPEN_NAMESPACE(Firestorm);

ArgParser::ArgParser(int ac, char** av)
{
	for(int i = 0; i < ac; ++i)
	{
		String arg(av[i]);
		Vector<String> splitString(SplitString(av[i], '='));
		if(splitString.size() == 1)
		{
			_args[splitString[0]] = "__FIRESTORM_TRUE_VALUE__";
		}
		else
		{
			_args[splitString[0]] = splitString[1];
		}
	}
}

bool ArgParser::Get(const String& key, bool def)
{
	if(_args.find(key) == _args.end())
		return def;

	if(_args[key] == "__FIRESTORM_TRUE_VALUE__")
		return true;

	return def;
}

String ArgParser::Get(const String& key, const String& def)
{
	if(_args.find(key) == _args.end())
		return def;
	return _args[key];
}

String ArgParser::Get(const String& key, const char* def)
{
	return Get(key, String(def));
}

bool ArgParser::Has(const String& key) const
{
	return _args.find(key) != _args.end();
}

CLOSE_NAMESPACE(Firestorm);